CWINUX_BEGIN_NAMESPACE

inline int CwxSockAcceptor::close (void)
{
    return CwxSockBase::close();
}

inline int CwxSockAcceptor::getRemoteAddr(CwxAddr &) const
{
    return -1;
}

CWINUX_END_NAMESPACE
