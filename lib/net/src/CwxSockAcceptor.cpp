#include "CwxSockAcceptor.h"

CWINUX_BEGIN_NAMESPACE
///默认构造函数
CwxSockAcceptor::CwxSockAcceptor()
{

}

CwxSockAcceptor::CwxSockAcceptor(CwxAddr const& addr,
                bool reuse,
                int backlog,
                int domain,
                int protocol,
                CWX_NET_SOCKET_ATTR_FUNC fn,
                void* fnArg)
{
    this->listen(addr, reuse, backlog, domain, protocol, fn, fnArg);
}

///析构函数.
CwxSockAcceptor::~CwxSockAcceptor(void)
{

}

int CwxSockAcceptor::listen(CwxAddr const& addr,
                                   bool reuse,
                                   int backlog,
                                   int domain,
                                   int protocol,
                                   CWX_NET_SOCKET_ATTR_FUNC fn,
                                   void* fnArg)
{
    int error = 0;

    if (CwxSockBase::open(domain,
        SOCK_STREAM,
        protocol,
        reuse) == -1)
    {
        return -1;
    }
    if (fn)
    {
        if (0 != fn(getHandle(), fnArg)) return -1;
    }
    if (domain == AF_INET6)
    {
        sockaddr_in6 local_inet6_addr;
        memset (reinterpret_cast<void *> (&local_inet6_addr),
            0,
            sizeof local_inet6_addr);

        if (CwxAddr::sap_any == addr)
        {
            local_inet6_addr.sin6_family = AF_INET6;
            local_inet6_addr.sin6_port = 0;
            local_inet6_addr.sin6_addr = in6addr_any;
        }
        else
            local_inet6_addr = *reinterpret_cast<sockaddr_in6 *> (addr.getAddr());
        if (::bind (this->getHandle(),
            reinterpret_cast<sockaddr *> (&local_inet6_addr),
            sizeof local_inet6_addr) == -1)
            error = 1;
    } else if (domain == AF_INET) {
        sockaddr_in local_inet_addr;
        memset (reinterpret_cast<void *> (&local_inet_addr),
            0,
            sizeof local_inet_addr);

        if (CwxAddr::sap_any == addr)
        {
            local_inet_addr.sin_port = 0;
        }
        else
        {
            local_inet_addr = *reinterpret_cast<sockaddr_in *> (addr.getAddr());
        }

        if (::bind (this->getHandle (),
            reinterpret_cast<sockaddr *> (&local_inet_addr),
            sizeof local_inet_addr) == -1)
            error = 1;
    }
    else if (::bind (this->getHandle (),
        (sockaddr *) addr.getAddr (),
        addr.getSize ()) == -1)
    {
        error = 1;
    }
    if ((error != 0) || (::listen (this->getHandle(), backlog) == -1))
    {
        error = 1;
        this->close();
    }
    return error ? -1 : 0;
}


int CwxSockAcceptor::accept(CwxSockStream &stream,
                            CwxAddr* remote_addr,
                            CwxTimeouter* timeout,
                            bool bRestart) const
{
    int noblocking = 1;
    int ret = 0;
    if (timeout && timeout->isTimer())
    {
        ret = CwxSocket::handleReady(getHandle(), timeout, true, false, false, bRestart);
        if (ret<1) return -1;
        noblocking = isNonBlock();
        if (-1 == noblocking) return -1;
        if (!noblocking && (setNonblock(true)==-1)) return -1;
    }
    {
        socklen_t *len_ptr = 0;
        sockaddr *addr = 0;
        socklen_t len = 0;
        if (remote_addr != 0)
        {
            len = remote_addr->getSize();
            len_ptr = &len;
            addr = (sockaddr *) remote_addr->getAddr();
        }
        do
        {
            stream.setHandle(::accept (this->getHandle(),
            addr,
            len_ptr));
        }
        while (stream.getHandle () == CWX_INVALID_HANDLE
            && bRestart
            && (errno == EINTR)
            && !timeout);

        if (stream.getHandle () != CWX_INVALID_HANDLE
            && remote_addr != 0)
        {
            remote_addr->setSize(len);
            if (addr) remote_addr->setType (addr->sa_family);
        }
    }

    if (!noblocking)
    {
        this->setNonblock(false);
        stream.setNonblock(false);
    }

    return stream.getHandle() == CWX_INVALID_HANDLE?-1:0;
}


CWINUX_END_NAMESPACE

