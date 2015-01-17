#include "CwxUnixAddr.h"

CWINUX_BEGIN_NAMESPACE
CwxUnixAddr::CwxUnixAddr()
:CwxAddr (AF_UNIX,sizeof this->unix_addr_)
{
    memset ((void *) &this->unix_addr_,
        0,
        sizeof this->unix_addr_);
    this->unix_addr_.sun_family = AF_UNIX;
}

CwxUnixAddr::CwxUnixAddr(char const* szPathFile)
{
    this->set(szPathFile);
}

CwxUnixAddr::CwxUnixAddr(sockaddr_un const* addr, int len)
{
    this->set(addr, len);
}

CwxUnixAddr::CwxUnixAddr(CwxUnixAddr const& addr)
:CwxAddr (AF_UNIX, addr.getSize())
{
    this->set (addr);
}


CwxUnixAddr::~CwxUnixAddr()
{

}

/// Return a pointer to the underlying network address.
void * CwxUnixAddr::getAddr (void) const
{
    return (void *) &this->unix_addr_;
}
/// Set a pointer to the underlying network address.
int CwxUnixAddr::setAddr (void *addr, int len)
{
    this->baseSet (AF_UNIX, len);
    memcpy ((void *) &this->unix_addr_,
        (void *) addr,
        len);
    return 0;
}


CWINUX_END_NAMESPACE

