CWINUX_BEGIN_NAMESPACE

inline CwxAppConnInfo& CwxAppHandler4Msg::getConnInfo()
{
    return m_conn;
}

inline CwxAppConnInfo const& CwxAppHandler4Msg::getConnInfo() const
{
    return m_conn;
}



///获取下一个待发送的消息，返回值：0，没有待发送信息；1,获得了一个待发送消息
inline int CwxAppHandler4Msg::getNextMsg()
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
inline bool CwxAppHandler4Msg::putMsg(CwxMsgBlock* msg)
{
    if (m_conn.isWaitingMsgQueueFull())
    {
        CWX_DEBUG(("Connection is full, svr_id=%u, host_id=%u, conn_id=%u, msg_num=%u", 
            m_conn.getSvrId(),
            m_conn.getHostId(),
            m_conn.getConnId(),
            m_conn.getWaitingMsgNum()));
        return false;
    }
    msg->m_next = NULL;
    if (this->m_waitSendMsgTail){
        this->m_waitSendMsgTail->m_next = msg;
        this->m_waitSendMsgTail = msg;
        this->m_conn.setWaitingMsgNum(this->m_conn.getWaitingMsgNum()+1);
    }else{
        this->m_waitSendMsgTail = this->m_waitSendMsgHead = msg;
        this->m_conn.setWaitingMsgNum(1);
        this->wakeUp();
    }
    return true;
}

///由于没有消息发送，使连接的发送监测休眠.返回值， -1: failure, 0: success
inline int CwxAppHandler4Msg::cancelWakeup()
{
    if(-1 == this->reactor()->suspendHandler(this, CwxAppHandler4Base::WRITE_MASK)){
        CWX_ERROR(("Failure to cancel wakeup a connection. conn[%u]", m_conn.getConnId()));
        return -1;
    }
    return 0;
}

///唤醒连接的可写监控，以发送未发送完毕的数据.返回值， -1:failure； 0:success。
inline int CwxAppHandler4Msg::wakeUp()
{
    if(-1 == this->reactor()->resumeHandler(this, CwxAppHandler4Base::WRITE_MASK)){
        CWX_ERROR(("Failure to wakeup a connection. conn[%u]", m_conn.getConnId()));
        return -1;
    }
    return 0;
}

///检查是否suspend连接的可读、可写监测
inline bool CwxAppHandler4Msg::isStopListen() const
{
    return m_bStopListen;
}

///设置stop listen
inline void CwxAppHandler4Msg::setStopListen(bool bStop)
{
    m_bStopListen = bStop;
}

inline bool CwxAppHandler4Msg::isEmpty() const
{
    return (!this->m_curSndingMsg) && (!this->m_waitSendMsgHead);

}

///获取连接时的错误代码
inline int CwxAppHandler4Msg::getConnErrNo() const
{
    return m_connErrNo;
}


///以非阻塞的方式，发送消息。返回值,-1: failure; 0: not send all;1:send a msg
inline int CwxAppHandler4Msg::nonBlockSend()
{
    CWX_ASSERT(NULL != this->m_curSndingMsg);
    char szAddr[256];
    CwxMsgBlock* data = this->m_curSndingMsg;

    //get the leaving data length to send.
    ssize_t len = data->length () - this->m_uiSendByte;
    //send data
    ssize_t n = CwxSocket::write(getHandle(), data->rd_ptr () + this->m_uiSendByte, len);
    if (n <= 0){
        if ((errno != EWOULDBLOCK)&&(errno != EINTR)){ //error
            CWX_ERROR(("Failure to send msg to IP[%s], port[%d], handle[%d], errno[%d].", 
                this->getRemoteAddr(szAddr, 255),
                this->getRemotePort(),
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

