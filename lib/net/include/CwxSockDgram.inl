CWINUX_BEGIN_NAMESPACE

inline ssize_t CwxSockDgram::send(const void *buf,  size_t len,  const CwxAddr &addr, int flags, CwxTimeouter  *timeout) const
{
    sockaddr *saddr = (sockaddr *) addr.getAddr();
    socklen_t addr_len = addr.getSize();
    return CwxSocket::sendto(this->getHandle(),
        (const char *) buf,
        len,
        flags,
        (struct sockaddr *) saddr,
        addr_len,
        timeout);
}

inline ssize_t CwxSockDgram::recv (void *buf,  size_t len,  CwxAddr &addr, int flags, CwxTimeouter  *timeout) const
{
    sockaddr *saddr = (sockaddr *) addr.getAddr();
    socklen_t addr_len = addr.getSize();
    ssize_t const status =  CwxSocket::recvfrom(this->getHandle(),
        (char *) buf,
        len,
        flags,
        (struct sockaddr *) saddr,
        &addr_len,
        timeout);
    addr.setSize (addr_len);
    addr.setType (saddr->sa_family);
    return status;
}



CWINUX_END_NAMESPACE
