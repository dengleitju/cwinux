#include "CwxSockBase.h"
CWINUX_BEGIN_NAMESPACE

int CwxSockBase::getLocalAddr (CwxAddr &sa) const
{

    socklen_t len = sa.getSize ();
    sockaddr *addr = reinterpret_cast<sockaddr *> (sa.getAddr ());
    if (-1 == getsockname(this->getHandle(), addr, &len)) return -1;
    sa.setType (addr->sa_family);
    sa.setSize (len);
    return 0;
}

int CwxSockBase::getRemoteAddr (CwxAddr &sa) const
{
    socklen_t len = sa.getSize ();
    sockaddr *addr = reinterpret_cast<sockaddr *> (sa.getAddr ());
    if (-1 == getpeername(this->getHandle(), addr, &len)) return -1;
    sa.setType (addr->sa_family);
    sa.setSize (len);
    return 0;
}

int CwxSockBase::open (int domain,
          int type,
          int protocol,
          bool reuse_addr)
{
    int one = 1;
    this->setHandle (::socket(domain,
        type,
        protocol));
    if (this->getHandle () == CWX_INVALID_HANDLE)
        return -1;
    else if (domain != AF_UNIX
        && reuse_addr
        && CwxSocket::setOption(this->getHandle(),
        SOL_SOCKET,
        SO_REUSEADDR,
        &one,
        sizeof(one)) == -1)
    {
        this->close ();
        return -1;
    }
    return 0;
}
CwxSockBase::CwxSockBase (int domain,
            int type,
            int protocol,
            bool reuse_addr)
{
    open(domain, type, protocol, reuse_addr);
}

CwxSockBase::CwxSockBase (void)
{

}

CwxSockBase::~CwxSockBase()
{

}




CWINUX_END_NAMESPACE
