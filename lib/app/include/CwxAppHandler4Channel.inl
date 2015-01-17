CWINUX_BEGIN_NAMESPACE


inline void CwxAppHandler4Channel::clear()
{
    m_uiSendByte = 0;
	if (m_curSndingMsg)
	{
		if (m_curSndingMsg->send_ctrl().isFailNotice())
		{
			onFailSendMsg(m_curSndingMsg);
		}
		if (m_curSndingMsg) CwxMsgBlockAlloc::free(m_curSndingMsg);
	}
	m_curSndingMsg = NULL;
	while(m_waitSendMsgHead){
		m_curSndingMsg = m_waitSendMsgHead->m_next;
		if (m_waitSendMsgHead->send_ctrl().isFailNotice())
		{
			onFailSendMsg(m_waitSendMsgHead);
		}
		if (m_waitSendMsgHead) CwxMsgBlockAlloc::free(m_waitSendMsgHead);
		m_waitSendMsgHead = m_curSndingMsg;
	}
	m_waitSendMsgTail = NULL;
}

///获取下一个待发送的消息，返回值：0，没有待发送信息；1,获得了一个待发送消息
inline int CwxAppHandler4Channel::getNextMsg()
{
    if (this->m_curSndingMsg) return 1;
    if (this->m_waitSendMsgHead == NULL) return 0;
    this->m_curSndingMsg = this->m_waitSendMsgHead;
    this->m_waitSendMsgHead = this->m_waitSendMsgHead->m_next;
    if (!this->m_waitSendMsgHead) this->m_waitSendMsgTail = NULL;
    m_curSndingMsg->m_next = NULL;
    return 1;
}

///对要发送的消息排队
inline bool CwxAppHandler4Channel::putMsg(CwxMsgBlock* msg)
{
    msg->m_next = NULL;
    if (this->m_waitSendMsgTail){
        this->m_waitSendMsgTail->m_next = msg;
        this->m_waitSendMsgTail = msg;
    }else{
        this->m_waitSendMsgTail = this->m_waitSendMsgHead = msg;
    }
    if (!isEmpty())
    {
        this->wakeUp();
    };
    return true;
}



inline bool CwxAppHandler4Channel::isEmpty() const
{
    return (!this->m_curSndingMsg) && (!this->m_waitSendMsgHead);

}

///获取channel
inline CwxAppChannel* CwxAppHandler4Channel::channel()
{
    return m_channel;
}

///以非阻塞的方式，发送消息。返回值,-1: failure; 0: not send all;1:send a msg
inline int CwxAppHandler4Channel::nonBlockSend()
{
    CWX_ASSERT(NULL != this->m_curSndingMsg);
    CwxMsgBlock* data = this->m_curSndingMsg;

    //get the leaving data length to send.
    ssize_t len = data->length () - this->m_uiSendByte;
    //send data
    ssize_t n = CwxSocket::write(getHandle(), data->rd_ptr () + this->m_uiSendByte, len);
    if (n <= 0){
        if ((errno != EWOULDBLOCK)&&(errno != EINTR)){ //error
            CWX_ERROR(("Failure to send msg to handle[%d], errno[%d].", 
                (int)getHandle(),
                errno));
            return -1;
        }
        errno = EWOULDBLOCK;
        n = 0;
    }else if (n < len)
    {
        // Re-adjust pointer to skip over the part we did send.
        this->m_uiSendByte += n;
        errno = EWOULDBLOCK;
    }else // if (n == length)
    {
        // The whole event is sent, we now decrement the reference count
        // (which deletes itself with it reaches 0).
        this->m_uiSendByte = 0;
        errno = 0;
    }
    return EWOULDBLOCK == errno? 0:1;
}



CWINUX_END_NAMESPACE

