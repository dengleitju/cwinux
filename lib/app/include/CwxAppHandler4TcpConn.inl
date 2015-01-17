
CWINUX_BEGIN_NAMESPACE



///设置主动连接的连接地址
inline void CwxAppHandler4TcpConn::setConnectAddr(char const* szAddr)
{
    m_strConnectAddr = szAddr;
}

///获取主动连接的连接地址
inline string const& CwxAppHandler4TcpConn::getConnectAddr() const
{
    return m_strConnectAddr;
}

///设置主动连接的连接端口
inline void CwxAppHandler4TcpConn::setConnectPort(CWX_UINT16 unPort)
{
    m_unConnectPort = unPort;
}

///获取主动连接的连接端口
inline CWX_UINT16 CwxAppHandler4TcpConn::getConnectPort() const 
{
    return m_unConnectPort;
}




CWINUX_END_NAMESPACE

