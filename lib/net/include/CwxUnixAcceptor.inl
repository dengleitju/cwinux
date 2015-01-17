CWINUX_BEGIN_NAMESPACE

inline int CwxUnixAcceptor::listen(CwxAddr const& addr,
                            bool reuse,
                            int backlog,
                            int domain,
                            int protocol,
                            CWX_NET_SOCKET_ATTR_FUNC fn,
                            void* fnArg)
{
    this->local_addr_ = *((CwxUnixAddr *) &addr);
    return CwxSockAcceptor::listen(addr, reuse, backlog, domain, protocol, fn, fnArg);
}

inline int CwxUnixAcceptor::remove (void)
{
    int result = this->close ();
    if (-1 == result) return -1;
    return ::unlink (this->local_addr_.getPathFile ());
}

inline int CwxUnixAcceptor::getLocalAddr (CwxAddr &a) const
{
    CwxUnixAddr& target = dynamic_cast<CwxUnixAddr &> (a);

    target = this->local_addr_;
    return 0;
}

inline int CwxUnixAcceptor::getRemoteAddr(CwxAddr &) const
{
    return -1;
}


CWINUX_END_NAMESPACE
