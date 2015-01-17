#include "CwxUnixAcceptor.h"

CWINUX_BEGIN_NAMESPACE
///默认构造函数
CwxUnixAcceptor::CwxUnixAcceptor()
{

}
CwxUnixAcceptor::CwxUnixAcceptor(CwxAddr const& addr,
                bool reuse,
                int backlog,
                int domain,
                int protocol,
                CWX_NET_SOCKET_ATTR_FUNC fn,
                void* fnArg)
{
    listen(addr, reuse, backlog, domain, protocol, fn, fnArg);
}

///析构函数.
CwxUnixAcceptor::~CwxUnixAcceptor(void)
{

}

int CwxUnixAcceptor::accept (CwxUnixStream &stream,
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

