CWINUX_BEGIN_NAMESPACE

inline int CwxUnixConnector::connect(CwxUnixStream& stream,
                                     CwxAddr const& remoteAddr,
                                     CwxAddr const& localAddr,
                                     CwxTimeouter* timeout,
                                     int protocol,
                                     bool reuse_addr,
                                     CWX_NET_SOCKET_ATTR_FUNC fn,
                                     void* fnArg)
{
    return CwxSockConnector::connect(stream, remoteAddr, localAddr, timeout, protocol, reuse_addr, fn, fnArg);
}

CWINUX_END_NAMESPACE
