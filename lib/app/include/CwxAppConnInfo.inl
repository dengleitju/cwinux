
CWINUX_BEGIN_NAMESPACE

inline CWX_UINT32 CwxAppConnInfo::getSvrId() const 
{
    return m_uiSvrId;
}

inline void CwxAppConnInfo::setSvrId(CWX_UINT32 uiSvrId)
{
    m_uiSvrId = uiSvrId;
}

///获取连接的HOST-ID
inline CWX_UINT32 CwxAppConnInfo::getHostId() const 
{
    return m_uiHostId;
}

inline void CwxAppConnInfo::setHostId(CWX_UINT32 uiHostId)
{
    m_uiHostId = uiHostId;
}
///获取连接的连接ID
inline CWX_UINT32 CwxAppConnInfo::getConnId() const 
{
    return m_uiConnId;
}
///设置连接的连接ID
inline void CwxAppConnInfo::setConnId(CWX_UINT32 uiConnId)
{
    m_uiConnId = uiConnId;
}
///设置被动连接的Listen ID
inline CWX_UINT32 CwxAppConnInfo::getListenId() const 
{
    return m_uiListenId;
}
///获取连接超时时间
inline CWX_UINT32 CwxAppConnInfo::getConnectTimeout() const
{
	return m_uiConnectTimeout;
}
///设置连接超时时间
inline void CwxAppConnInfo::setConnectTimeout(CWX_UINT32 uiTimeout)
{
	m_uiConnectTimeout = uiTimeout;
}

inline void CwxAppConnInfo::setListenId(CWX_UINT32 uiListenId)
{
    m_uiListenId = uiListenId;
}


///获取连接的状态
inline CWX_UINT16 CwxAppConnInfo::getState() const
{
    return m_unState;
}
inline void CwxAppConnInfo::setState(CWX_UINT16 unState)
{
    m_unState = unState;
}

///获取连接的创建时间
inline time_t CwxAppConnInfo::getCreateTime() const 
{
    return m_ttCreateTime;
}
inline void CwxAppConnInfo::setCreateTime(time_t ttTime)
{
    m_ttCreateTime = ttTime;
}

///获取连续连接失败的次数
inline CWX_UINT32 CwxAppConnInfo::getFailConnNum() const
{
    return m_uiFailConnNum;
}
///设置连接失败连接次数
inline void CwxAppConnInfo::setFailConnNum(CWX_UINT32 uiNum)
{
    m_uiFailConnNum = uiNum;
}
///增加连接失败连接次数
inline CWX_UINT32 CwxAppConnInfo::incFailConnNum()
{
    m_uiFailConnNum++;
    return m_uiFailConnNum;
}

///获取失效主动连接最小重连接时间间隔
inline CWX_UINT16 CwxAppConnInfo::getMinRetryInternal() const 
{
    return m_unMinRetryInternal;
}
inline void CwxAppConnInfo::setMinRetryInternal(CWX_UINT16 unInternal)
{
    m_unMinRetryInternal = unInternal;
}

///获取失效主动连接最大重连接时间间隔
inline CWX_UINT16 CwxAppConnInfo::getMaxRetryInternal() const 
{
    return m_unMaxRetryInternal;
}
inline void CwxAppConnInfo::setMaxRetryInternal(CWX_UINT16 unInternal)
{
    m_unMaxRetryInternal = unInternal;
}

///获取连接是否为主动连接
inline bool CwxAppConnInfo::isActiveConn() const 
{
    return m_bActiveConn;
}
inline void CwxAppConnInfo::setActiveConn(bool bActive)
{
    m_bActiveConn = bActive;
}

///获取连接是否主动关闭
inline bool CwxAppConnInfo::isActiveClose() const 
{
    return m_bActiveClose;
}
inline void CwxAppConnInfo::setActiveClose(bool bActive)
{
    m_bActiveClose = bActive;
}


///获取连接的数据包是否有包头
inline bool CwxAppConnInfo::isRawData() const 
{
    return m_bRawData;
}
inline void CwxAppConnInfo::setRawData(bool bRaw)
{
    m_bRawData = bRaw;
}



///获取连接最新收到消息的时间
inline time_t  CwxAppConnInfo::getLastRecvMsgTime() const 
{
    return m_ttLastRecvMsgTime;
}
inline void CwxAppConnInfo::setLastRecvMsgTime(time_t ttTime)
{
    m_ttLastRecvMsgTime = ttTime;
}

///获取连接最新发送消息的时间
inline time_t  CwxAppConnInfo::getLastSendMsgTime() const 
{
    return m_ttLastSendMsgTime;
}
inline void CwxAppConnInfo::setLastSendMsgTime(time_t ttTime)
{
    m_ttLastSendMsgTime = ttTime;
}


///获取连接的用户数据
inline void*  CwxAppConnInfo::getUserData() const
{
    return  m_pUserData;
}
///设置连接的用户数据
inline void CwxAppConnInfo::setUserData(void* userData)
{
    m_pUserData = userData;
}

///获取连接等待发送的最大消息的数量，0表示没有限制
inline CWX_UINT32 CwxAppConnInfo::getMaxWaitingMsgNum() const
{
    return m_uiMaxWaitingMsgNum;
}
///设置连接最大的等待发送的消息数量，默认0表示没有限制
inline void CwxAppConnInfo::setMaxWaitingMsgNum(CWX_UINT32 uiNum)
{
    m_uiMaxWaitingMsgNum = uiNum;
}
///判断是否连接待发送队列已满
inline bool CwxAppConnInfo::isWaitingMsgQueueFull() const
{
    return m_uiMaxWaitingMsgNum && (m_uiMaxWaitingMsgNum <= m_uiWaitingMsgNum);
}


///获取连接等待发送的消息的数量
inline CWX_UINT32 CwxAppConnInfo::getWaitingMsgNum() const
{
    return m_uiWaitingMsgNum;
}
///设置连接等待发送的消息的数量
inline void CwxAppConnInfo::setWaitingMsgNum(CWX_UINT32 uiNum)
{
    m_uiWaitingMsgNum = uiNum;
}
///增加连接等待发送的消息的数量
inline CWX_UINT32 CwxAppConnInfo::incWaitingMsgNum()
{
    m_uiWaitingMsgNum++;
    return m_uiWaitingMsgNum;
}
///减少连接等待发送的消息的数量
inline CWX_UINT32 CwxAppConnInfo::decWaitingMsgNum()
{
    if (m_uiWaitingMsgNum) m_uiWaitingMsgNum--;
    return m_uiWaitingMsgNum;
}

///获取连接已经连续接收到的消息包的数量
inline CWX_UINT32 CwxAppConnInfo::getContinueRecvNum() const
{
    return m_uiContinueRecvNum;
}
inline void CwxAppConnInfo::setContinueRecvNum(CWX_UINT32 uiNum)
{
    m_uiContinueRecvNum = uiNum;
}
///获取连续发送的消息数量
inline CWX_UINT32 CwxAppConnInfo::getContinueSendNum() const
{
    return m_uiContinueSendNum;
}
///设置连续发送的消息数量
inline void CwxAppConnInfo::setContinueSendNum(CWX_UINT32 uiNum)
{
    m_uiContinueSendNum = uiNum;
}


///判断断开的连接是否需要重连
inline bool CwxAppConnInfo::isNeedReconnect() const 
{
    return !m_bActiveClose && m_bActiveConn;
}


///是否调用CwxAppFramework::onCreate
inline bool CwxAppConnInfo::isInvokeCreate() const
{
    return m_bInvokeCreate;
}
///设置是否调用CwxAppFramework::onCreate
inline void CwxAppConnInfo::setInvokeCreate(bool bInvoke)
{
    m_bInvokeCreate = bInvoke;
}

///是否重新连接
inline bool CwxAppConnInfo::isReconn() const
{
    return m_bReconn;
}
///设置是否重连
inline void CwxAppConnInfo::setReconn(bool bReconnect)
{
    m_bReconn = bReconnect;
}
///获取重新连接延时的毫秒数
inline CWX_UINT32 CwxAppConnInfo::getReconnDelay() const
{
    return m_uiReconnDelay;
}
///设置重新连接延时的毫秒数
inline void CwxAppConnInfo::setReconnDelay(CWX_UINT32 uiDelay)
{
    m_uiReconnDelay = uiDelay;
}

///获取socket设置的function
inline CWX_NET_SOCKET_ATTR_FUNC CwxAppConnInfo::getSockFunc() const
{
    return m_fn;
}
///设置socket设置的function
inline void CwxAppConnInfo::setSockFunc(CWX_NET_SOCKET_ATTR_FUNC fn)
{
    m_fn = fn;
}

///获取socket设置function的arg
inline void* CwxAppConnInfo::getSockFuncArg() const
{
    return m_fnArg;
}
///设置socket设置function的arg
inline void CwxAppConnInfo::setSockFuncArg(void* arg)
{
    m_fnArg = arg;
}

///获取连接的family
inline CWX_INT32 CwxAppConnInfo::getFamily() const {
  return m_iFamily;
}
///设置连接的family
inline void CwxAppConnInfo::setFamily(CWX_INT32 family) {
  m_iFamily = family;
}

///获取连接对应的handler
inline CwxAppHandler4Msg* CwxAppConnInfo::getHandler()
{
    return m_pHandler;
}
///设置连接对应的handler
inline void CwxAppConnInfo::setHandler(CwxAppHandler4Msg*  pHandler)
{
    m_pHandler = pHandler;
}



inline void CwxAppConnInfo::reset()
{
    m_bActiveClose = false;
    m_uiWaitingMsgNum = 0;
    m_uiMaxWaitingMsgNum = 0;
    m_uiContinueRecvNum = 0;
    m_uiContinueSendNum = 0;
//    m_bInvokeCreate = true;
    m_bReconn = false;
    m_uiReconnDelay = 0;
}

CWINUX_END_NAMESPACE

