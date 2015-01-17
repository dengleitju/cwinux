#include "CwxAppHandler4TcpConn.h"
#include "CwxAppFramework.h"

CWINUX_BEGIN_NAMESPACE
CwxAppHandler4TcpConn::CwxAppHandler4TcpConn(CwxAppFramework* pApp,
                              CwxAppReactor *reactor)
                              :CwxAppHandler4Msg(pApp, reactor)
{
    m_next = NULL;
    m_unConnectPort = 0;
}

///析构函数
CwxAppHandler4TcpConn::~CwxAppHandler4TcpConn()
{
    if (getHandle() != CWX_INVALID_HANDLE)
    {
        ::close(getHandle());
        setHandle(CWX_INVALID_HANDLE);
    }
}

int CwxAppHandler4TcpConn::open (void * arg)
{
    if ((CwxAppConnInfo::ESTABLISHING == m_conn.getState())&&!getConnInfo().isActiveConn())
    {
        CwxSockStream stream(getHandle());
        stream.getRemoteAddr(m_remoteAddr);
    }
    return CwxAppHandler4Msg::open(arg);
}

///handle close
int CwxAppHandler4TcpConn::close(CWX_HANDLE )
{
    char szAddr[256];
    char szAddr1[256];
    CWX_UINT16 ucLocState = m_conn.getState();
    char const* szState;
    int iCloseReturn=0;

    if (getApp()->isStopped())
    {
		if (-1 != this->index()) reactor()->cancelTimer(this);
        delete this;
        return 0;
    }

    switch(ucLocState)
    {
        case CwxAppConnInfo::IDLE:
            szState = "IDLE"; ///一定没有在reactor注册
            break;
        case CwxAppConnInfo::CONNECTING:
            szState = "CONNECTING";///可能在重连的时候再reactor注册
            CWX_ERROR(("Failure to connect to host:%s, port=%d, errno=%d",
                m_strConnectAddr.c_str(),
                m_unConnectPort,
                getConnErrNo()));
            if (reactor()) reactor()->removeHandler(this);
            break;
        case CwxAppConnInfo::TIMEOUT:
            szState = "TIMEOUT"; ///可能注册了timeout
            if (-1 != this->index()) reactor()->cancelTimer(this);
            break;
        case CwxAppConnInfo::ESTABLISHING:
            szState = "ESTABLISHING";///可能注册了消息收发
            if (CWX_INVALID_HANDLE != getHandle())
                if (reactor()) reactor()->removeHandler(this, false);
            break;
        case CwxAppConnInfo::ESTABLISHED:
            szState = "ESTABLISHED";///一定注册了消息收发
            if (CWX_INVALID_HANDLE != getHandle())
                if (reactor()) reactor()->removeHandler(this, false);
            break;
        case CwxAppConnInfo::FAILED:
            szState = "FAILED";///一定没有注册消息收发
            break;
        default:
            szState = "Unknown";
            CWX_ASSERT(0);
            break;
    }
    if (CWX_INVALID_HANDLE != getHandle())
    {
        CWX_DEBUG(("Connection closed. State = %s, Local_Ip=%s, Local_port=%d, Remote_Ip=%s, Remote_port=%d, Active-Close=%s", 
            szState,
            getLocalAddr(szAddr, 255),
            getLocalPort(),
            m_conn.isActiveConn()? this->getConnectAddr().c_str():getRemoteAddr(szAddr1, 255),
            m_conn.isActiveConn()? m_unConnectPort:getRemotePort(),
            m_conn.isActiveClose()?"yes":"no"));
    }

    m_conn.setState(CwxAppConnInfo::FAILED);

    //reconnection or close
    if (CwxAppConnInfo::ESTABLISHED == ucLocState)
    {
        //re-dispatch all msg
        while(this->getNextMsg() == 1)
        {
            if (this->m_curSndingMsg && m_curSndingMsg->send_ctrl().isFailNotice())
            {
                this->getApp()->onFailSendMsg(m_curSndingMsg);
            }
            if (m_curSndingMsg) CwxMsgBlockAlloc::free(m_curSndingMsg);
            this->m_curSndingMsg = NULL;
        }
        iCloseReturn = this->getApp()->connClosed(*this);
        m_conn.setFailConnNum(1);
        //remove recieved data.
        if (this->m_recvMsgData) CwxMsgBlockAlloc::free(this->m_recvMsgData);
        this->m_recvMsgData = NULL;
    }
    else if (m_conn.isActiveConn())
    {
        m_conn.setFailConnNum(m_conn.getFailConnNum() + 1);
        iCloseReturn = this->getApp()->onFailConnect(*this);
    }

    if (getHandle () != CWX_INVALID_HANDLE)
    {
        ::close(getHandle());
        setHandle(CWX_INVALID_HANDLE);
    }
    if (m_conn.isNeedReconnect() && (iCloseReturn>=0))
    {
        CWX_UINT32 uiInternal = 0;
        if (m_conn.isReconn())
        {
            uiInternal = m_conn.getReconnDelay();
            m_conn.setReconn(false);
        }
        else
        {
            if (iCloseReturn > 0)
            {
                uiInternal = iCloseReturn;
            }
            else
            {
                uiInternal = 2 * m_conn.getMinRetryInternal() * getConnInfo().getFailConnNum();
                if (uiInternal > getConnInfo().getMaxRetryInternal()) uiInternal = getConnInfo().getMaxRetryInternal();
                uiInternal *= 1000;
            }
        }

        CWX_DEBUG(("Reconnect to address %s:%u after %d mili-second.", getConnectAddr().c_str(), getConnectPort(), uiInternal));
        if (uiInternal)
        {
            m_conn.setState(CwxAppConnInfo::TIMEOUT);
            if (this->reactor()->scheduleTimer(this, CwxTimeValue(uiInternal/1000, (uiInternal%1000) * 1000)) == -1)
            {
                CWX_ERROR(("Failure schedule_timer to noticeReconnect for conn id[%u]", m_conn.getConnId()));
                if (reactor()) reactor()->removeHandlerByConnId(m_conn.getConnId());
                delete this;
                return 0;
            }
        }
        else
        {
            if (-1 == getApp()->getTcpConnector()->connect(this,
                m_strConnectAddr.c_str(),
                m_unConnectPort))
            {
                CWX_ERROR(("Failure to reconnect ip:%s, port=%u, errno=%d",
                    m_strConnectAddr.c_str(),
                    m_unConnectPort,
                    errno));
                this->close();
            }
        }
    }
    else
    {
        reactor()->removeFromConnMap(m_conn.getConnId());
        getApp()->getHandlerCache()->cacheTcpHandle(this);
    }

    return 0;
}

///超时
void CwxAppHandler4TcpConn::handle_timeout()
{
    if (getApp()->isStopped()) return ;
    if (-1 == getApp()->getTcpConnector()->connect(this,
        m_strConnectAddr.c_str(),
        m_unConnectPort))
    {
        this->close();
    }
}


/**
@brief 获取连接的对端地址，只对STREAM_TYPE_TCP和STREAM_TYPE_UNIX有效
@param [in,out] szBuf 返回地址的buf,获取成功后以\0结束。
@param [in] unSize szBuf的大小。
@return 返回szBuf
*/
char* CwxAppHandler4TcpConn::getRemoteAddr(char* szBuf, CWX_UINT16 unSize)
{
    if (getConnInfo().isActiveConn())
    {
        CwxCommon::copyStr(szBuf, unSize, m_strConnectAddr.c_str(), m_strConnectAddr.length());
    }
    else
    {
        szBuf[0] = 0x00;
        char szHostName[256];
        memset(szHostName, 0x00, 255);
        if (m_remoteAddr.getHostIp(szHostName, 255)){
            CwxCommon::copyStr(szBuf, unSize, szHostName, strlen(szHostName));
        }
        else
        {
            CWX_ERROR(("Failure to get remote addr"));
        }
    }
    return szBuf;
}
/**
@brief 获取连接的对端port，只对STREAM_TYPE_TCP有效
@return 连接对端的port
*/
CWX_UINT16 CwxAppHandler4TcpConn::getRemotePort()
{
    if (getConnInfo().isActiveConn()) return m_unConnectPort;
    return m_remoteAddr.getPort();
}
/**
@brief 获取连接本端的地址，只对STREAM_TYPE_TCP和STREAM_TYPE_UNIX有效
@param [in,out] szBuf 返回地址的buf,获取成功后以\0结束。
@param [in] unSize szBuf的大小。
@return 返回szBuf
*/
char* CwxAppHandler4TcpConn::getLocalAddr(char* szBuf, CWX_UINT16 unSize)
{
    if (CWX_INVALID_HANDLE == getHandle())
    {
        strcpy(szBuf, "invalid handle");
        return szBuf;
    }
    szBuf[0] = 0x00;
    char szHostName[256];
    memset(szHostName, 0x00, 256);
    CwxINetAddr addr;
    CwxSockStream stream(getHandle());
    if (-1 != stream.getLocalAddr(addr)){
        if (addr.getHostIp(szHostName, 255)){
            CwxCommon::copyStr(szBuf, unSize, szHostName, strlen(szHostName));
        }
    }
    else
    {
        CWX_ERROR(("Failure to get local addr, errno=%d", errno));
    }
    return szBuf;
}
/**
@brief 获取连接的本端port，只对STREAM_TYPE_TCP有效
@return 连接对端的port
*/
CWX_UINT16 CwxAppHandler4TcpConn::getLocalPort()
{
    if (CWX_INVALID_HANDLE == getHandle()) return 0;
    CwxINetAddr addr;
    CwxSockStream stream(getHandle());
    if (-1 != stream.getLocalAddr(addr)){
        return addr.getPort();
    }
    else
    {
        CWX_ERROR(("Failure to get local port, errno=%d", errno));
    }
    return 0;
}




CWINUX_END_NAMESPACE

