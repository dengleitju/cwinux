#include "CwxAppHandler4Msg.h"
#include "CwxAppFramework.h"

CWINUX_BEGIN_NAMESPACE


CwxAppHandler4Msg::CwxAppHandler4Msg(CwxAppFramework* pApp, CwxAppReactor *reactor)
:CwxAppHandler4Base(reactor)
{
    m_pApp = pApp;
    m_conn.setHandler(this);
    m_uiSendByte = 0;
    m_curSndingMsg = 0;
    m_waitSendMsgHead = NULL;
    m_waitSendMsgTail = NULL;
    memset(m_szHeadBuf, 0x00, CwxMsgHead::MSG_HEAD_LEN);
    m_uiRecvHeadLen = 0;
    m_uiRecvDataLen = 0;
    m_recvMsgData = NULL;
    m_bStopListen = false;
    m_connErrNo = 0;
}

///析构函数
CwxAppHandler4Msg::~CwxAppHandler4Msg()
{
    clear();
    if (getHandle() != CWX_INVALID_HANDLE)
    {
        ::close(getHandle());
        setHandle(CWX_INVALID_HANDLE);
    }
}


int CwxAppHandler4Msg::open (void * )
{
    if (getApp()->isStopped()) return -1;
    if (CwxAppConnInfo::CONNECTING == m_conn.getState())
    {
        if (0 != reactor()->registerHandler(getHandle(),
            this,
            CwxAppHandler4Base::WRITE_MASK,
			CWX_APP_INVALID_CONN_ID,
			getConnInfo().getConnectTimeout()
			))
        {
            CWX_ERROR(("Failure to register conn[%u] for waiting connecting",
                m_conn.getConnId()));
            return -1;
        }

    }
    else if (CwxAppConnInfo::ESTABLISHING == m_conn.getState())
    {
        //清空对象的变化数据
        this->clear();
        m_conn.setFailConnNum(0);
        // Turn on non-blocking I/O.
        if	(CwxSockIo::setNonblock(getHandle(), true) == -1)
        {
            m_conn.setState(CwxAppConnInfo::FAILED);
            CWX_ERROR(("Failure to set the connection for NONBLOCK.conn[%u]",
                m_conn.getConnId()));
            return -1;
        }
        int iRet = this->getApp()->openConn(*this, m_bStopListen);
        if (-1 == iRet) return -1;
        m_conn.setState(CwxAppConnInfo::ESTABLISHED);
    }
    else
    {
        CWX_ASSERT(0);
        CWX_ERROR(("Invalid conn state[%d]", m_conn.getState()));
        return -1;
    }
    return 0;
}

int CwxAppHandler4Msg::close(CWX_HANDLE)
{
    delete this;
    return 0;
}


/**
@brief 接受连接上的到达数据
@param [in] handle 连接的handle
@return -1：关闭的连接； 0：接收数据成功
*/
int CwxAppHandler4Msg::handle_event(int event, CWX_HANDLE)
{
    if (CwxAppConnInfo::ESTABLISHED == m_conn.getState())
    {///通信状态
        CWX_ASSERT((event&~CwxAppHandler4Base::RW_MASK)==0);
        if (CWX_CHECK_ATTR(event, CwxAppHandler4Base::WRITE_MASK))
        {
            if (0 != handle_output()) return -1;
        }
        if (CWX_CHECK_ATTR(event, CwxAppHandler4Base::READ_MASK))
        {
            if (0 != handle_input()) return -1;
        }
    }
    else if (CwxAppConnInfo::CONNECTING == m_conn.getState())
    {///等待连接状态
		if ((event&CwxAppHandler4Base::TIMEOUT_MASK)== CwxAppHandler4Base::TIMEOUT_MASK)
		{//连接超时
			return -1;
		}

        CWX_ASSERT((event&~CwxAppHandler4Base::WRITE_MASK)==0);
        int sock_err = 0;
        socklen_t sock_err_len = sizeof (sock_err);
        int sockopt_ret = ::getsockopt (getHandle(), SOL_SOCKET, SO_ERROR,
            &sock_err, &sock_err_len);
        if ((sockopt_ret < 0) || sock_err)
        {
            m_connErrNo = sock_err;
            return -1;
        }
        m_conn.setState(CwxAppConnInfo::ESTABLISHING);
        if (-1 == open()) return -1;
    }
    else if (CwxAppConnInfo::TIMEOUT == m_conn.getState())
    {
        CWX_ASSERT((event&~CwxAppHandler4Base::TIMEOUT_MASK)==0);
        if (CWX_CHECK_ATTR(event, CwxAppHandler4Base::TIMEOUT_MASK))
        {
            handle_timeout();
        }
    }
    else
    {
        CWX_ASSERT(0);
        return -1;
    }
    return 0;
}


int CwxAppHandler4Msg::handle_output ()
{
    if (getApp()->isStopped()) return 0;
	int result = 0;
    CWX_UINT32 uiNum = 0;
    bool bCloseConn = false;
    bool bReconn = false;
    CWX_UINT32 uiReconnDelay = 0;
	// The list had better not be empty, otherwise there's a bug!
    while(1)
    {
        if (NULL == this->m_curSndingMsg)
        {
            CWX_ASSERT(m_uiSendByte==0);
            while(1)
            {
                result = this->getNextMsg();
                if (0 == result) return this->cancelWakeup();
                if (CWX_CHECK_ATTR(this->m_curSndingMsg->send_ctrl().getMsgAttr(), CwxMsgSendCtrl::BEGIN_NOTICE))
                {
                    if (-1 == this->getApp()->onStartSendMsg(m_curSndingMsg, *this))
                    {
                        this->m_conn.setWaitingMsgNum(this->m_conn.getWaitingMsgNum()-1);
                        CwxMsgBlockAlloc::free(this->m_curSndingMsg);
                        this->m_curSndingMsg = NULL;
                        continue; //next msg;
                    }
                }//end if (this->m_curSndingMsg->IsBeginNotice())
                //it's a msg which need be sent.
                break; //break while
            }//end while
        }
        //has message
        result = this->nonBlockSend();
        if (0 == result)
        {// Partial send.
            CWX_ASSERT (errno == EWOULDBLOCK);
            break;
        }
        else if (1 == result)
        {//finish
            bCloseConn = CWX_CHECK_ATTR(this->m_curSndingMsg->send_ctrl().getMsgAttr(), CwxMsgSendCtrl::CLOSE_NOTICE);
            bReconn = CWX_CHECK_ATTR(this->m_curSndingMsg->send_ctrl().getMsgAttr(), CwxMsgSendCtrl::RECONN_NOTICE);
            if (bReconn) uiReconnDelay = m_curSndingMsg->send_ctrl().getReconnDelay();
            this->m_uiSendByte = 0;
            this->m_conn.setWaitingMsgNum(m_conn.getWaitingMsgNum()-1);
            this->m_conn.setLastSendMsgTime(time(NULL));
            if (CWX_CHECK_ATTR(this->m_curSndingMsg->send_ctrl().getMsgAttr(), CwxMsgSendCtrl::FINISH_NOTICE)){
                CWX_UINT32 uiResume = this->getApp()->onEndSendMsg(m_curSndingMsg, *this);
                if (CwxMsgSendCtrl::RESUME_CONN == uiResume)
                {
                    if (0 != reactor()->resumeHandler(this, CwxAppHandler4Base::READ_MASK))
                    {
                        CWX_ERROR(("Failure to resume handler with conn_id[%u]", m_conn.getConnId()));
                        return -1;
                    }
                }
                else if (CwxMsgSendCtrl::SUSPEND_CONN == uiResume)
                {
                    if (0 != reactor()->suspendHandler(this, CwxAppHandler4Base::READ_MASK))
                    {
                        CWX_ERROR(("Failure to suspend handler with conn_id[%u]", m_conn.getConnId()));
                        return -1;
                    }
                }
            }
            if (bCloseConn)
            {
                m_conn.setActiveClose(true);
                result = -1;
                break;
            }
            else if (bReconn)
            {
                m_conn.setReconn(true);
                m_conn.setReconnDelay(uiReconnDelay);
                result = -1;
                break;
            }
            if (m_curSndingMsg)
            {
                CwxMsgBlockAlloc::free(m_curSndingMsg);
                this->m_curSndingMsg = NULL;
            }
        }
        else
        {//failure
            if (m_curSndingMsg->send_ctrl().isFailNotice())
            {
                this->getApp()->onFailSendMsg(m_curSndingMsg);
            }
            if (m_curSndingMsg) CwxMsgBlockAlloc::free(m_curSndingMsg);
            this->m_curSndingMsg = NULL;
            break;
        }
        uiNum++;
        if (uiNum > m_conn.getContinueSendNum()) break;
    };
    return -1==result?-1:0;
}

/***
desc:
	recv data from socket.
param: 
	handle of the event.
return:
	-1 : failure, handle_close is invoked.
	0  : success.
***/
int CwxAppHandler4Msg::handle_input ()
{
    bool bSuspend = false;
    if (getApp()->isStopped()) return 0;
    ssize_t recv_size = 0;
    ssize_t need_size = 0;
    int result = 0;
    if (this->m_conn.isRawData())
    {//recv raw data
        bSuspend = false;
        result = this->getApp()->onRecvMsg(*this, bSuspend);
        if (bSuspend)
        {
            if (0 != reactor()->suspendHandler(this, CwxAppHandler4Base::READ_MASK))
            {
                CWX_ERROR(("Failure to suspend handler with conn_id[%u]", m_conn.getConnId()));
                return -1;
            }
        }
        return result>=0?0:-1;
    }
    //not recv raw data
    while(1)
    {
        need_size = CwxMsgHead::MSG_HEAD_LEN - this->m_uiRecvHeadLen;
        if (need_size > 0 )
        {//not get complete head
            recv_size = CwxSocket::recv(getHandle(), this->m_szHeadBuf + this->m_uiRecvHeadLen, need_size);
            if (recv_size <=0 )
            { //error or signal
                if ((0==recv_size) || ((errno != EWOULDBLOCK) && (errno != EINTR)))
                {
                    return -1; //error
                }
                else
                {//signal or no data
                    m_conn.setContinueRecvNum(0);
                    return 0;
                }
            }
            this->m_uiRecvHeadLen += recv_size;
            if (recv_size < need_size)
            {
                m_conn.setContinueRecvNum(0);
                return 0;
            }
            this->m_szHeadBuf[this->m_uiRecvHeadLen] = 0x00;
            if (!m_header.fromNet(this->m_szHeadBuf))
            {
                CWX_ERROR(("Msg header is error."));
                return -1;
            }
            this->m_recvMsgData = CwxMsgBlockAlloc::malloc(m_header.getDataLen());
            CWX_ASSERT(this->m_uiRecvDataLen==0);
        }//end  if (need_size > 0)
        //recv data
        need_size = m_header.getDataLen() - this->m_uiRecvDataLen;
        if (need_size > 0)
        {//not get complete data
            recv_size = CwxSocket::recv(getHandle(), this->m_recvMsgData->wr_ptr(), need_size);
            if (recv_size <=0 )
            { //error or signal
                if ((errno != EWOULDBLOCK)&&(errno != EINTR))
                {
                    return -1; //error
                }
                else
                {//signal or no data
                    m_conn.setContinueRecvNum(0);
                    return 0;
                }
            }
            //move write pointer
            this->m_recvMsgData->wr_ptr(recv_size);
            this->m_uiRecvDataLen += recv_size;
            if (recv_size < need_size)
            {
                m_conn.setContinueRecvNum(0);
                return 0;
            }
        }
        //notice recieving a msg.
        if (!this->m_recvMsgData) this->m_recvMsgData = CwxMsgBlockAlloc::malloc(0);
        bSuspend = false;
        result = this->getApp()->recvMessage(m_header, this->m_recvMsgData, *this, bSuspend);
        if (bSuspend)
        {
            if (0 != reactor()->suspendHandler(this, CwxAppHandler4Base::READ_MASK))
            {
                CWX_ERROR(("Failure to suspend handler with conn_id[%u]", m_conn.getConnId()));
                return -1;
            }
        }

        this->m_recvMsgData = NULL;
        this->m_uiRecvHeadLen = 0;
        this->m_uiRecvDataLen = 0;
        if (0 < result)
        {///recv next msg
            CWX_ASSERT(m_uiRecvDataLen==0);
            CWX_ASSERT(m_uiRecvHeadLen==0);
            m_conn.setContinueRecvNum(m_conn.getContinueRecvNum()+1);
            continue;
        }
        else
        {
            CWX_ASSERT(m_uiRecvDataLen==0);
            CWX_ASSERT(m_uiRecvHeadLen==0);
            m_conn.setContinueRecvNum(0);
            break;
        }
    }
    return result>=0?0:-1;
}

/***
desc:
	timeout evnet, it exec the noticeReconnect operation.
return:
	0  : success.
***/
void CwxAppHandler4Msg::handle_timeout()
{
    CWX_ASSERT(0);
}


/**
@brief 获取连接的对端地址
@param [in,out] szBuf 返回地址的buf,获取成功后以\\0结束。
@param [in] unSize szBuf的大小。
@return 返回szBuf
*/
char* CwxAppHandler4Msg::getRemoteAddr(char* szBuf, CWX_UINT16 )
{
    szBuf[0] = 0x00;
    return szBuf;
}
/**
@brief 获取连接的对端port
@return 连接对端的port
*/
CWX_UINT16 CwxAppHandler4Msg::getRemotePort()
{
    return 0;
}
/**
@brief 获取连接本端的地址
@param [in,out] szBuf 返回地址的buf,获取成功后以\\0结束。
@param [in] unSize szBuf的大小。
@return 返回szBuf
*/
char* CwxAppHandler4Msg::getLocalAddr(char* szBuf, CWX_UINT16 )
{
    szBuf[0]=0x00;
    return szBuf;
}
/**
@brief 获取连接的本端port
@return 连接对端的port
*/
CWX_UINT16 CwxAppHandler4Msg::getLocalPort()
{
    return 0;
}

void CwxAppHandler4Msg::clear()
{
	m_conn.reset();
	m_uiSendByte = 0;

	if (m_curSndingMsg)
	{
		if (m_curSndingMsg->send_ctrl().isFailNotice())
		{
			this->getApp()->onFailSendMsg(m_curSndingMsg);
		}
		if (m_curSndingMsg) CwxMsgBlockAlloc::free(m_curSndingMsg);
		this->m_curSndingMsg = NULL;
	}
	m_curSndingMsg = NULL;
	while(m_waitSendMsgHead)
	{
		m_curSndingMsg = m_waitSendMsgHead->m_next;
		if (m_waitSendMsgHead->send_ctrl().isFailNotice())
		{
			this->getApp()->onFailSendMsg(m_waitSendMsgHead);
		}
		if (m_waitSendMsgHead) CwxMsgBlockAlloc::free(m_waitSendMsgHead);
		m_waitSendMsgHead = m_curSndingMsg;
	}
	m_waitSendMsgTail = NULL;
	m_uiRecvHeadLen = 0;
	m_uiRecvDataLen = 0;
	if (m_recvMsgData) CwxMsgBlockAlloc::free(m_recvMsgData);
	m_recvMsgData = NULL;
	m_bStopListen = false;
	m_connErrNo = 0;
}


CWINUX_END_NAMESPACE

