CWINUX_BEGIN_NAMESPACE

inline ssize_t CwxSockIo::recv (void *buf,
              size_t n,
              int flags,
              CwxTimeouter *timeout) const
{
    return CwxSocket::recv(getHandle(), buf, n, flags, timeout);
}

inline ssize_t CwxSockIo::recv (void *buf,
              size_t n,
              CwxTimeouter  *timeout) const
{
    return CwxSocket::recv(getHandle(), buf, n, timeout);
}

inline ssize_t CwxSockIo::recvv (iovec iov[],
               int n,
               CwxTimeouter* timeout) const
{
    return CwxSocket::recvv(getHandle(), iov, n, timeout);
}

inline ssize_t CwxSockIo::read (void *buf, size_t len, CwxTimeouter  *timeout) const
{
    return CwxSocket::read(getHandle(), buf, len, timeout);
}

inline ssize_t CwxSockIo::write (const void *buf, size_t len, CwxTimeouter  *timeout) const
{
    return CwxSocket::write(getHandle(), buf, len, timeout);
}


inline ssize_t CwxSockIo::send (const void *buf,
              size_t n,
              int flags,
              CwxTimeouter  *timeout) const
{
    return CwxSocket::send(getHandle(), buf, n, flags, timeout);
}

inline ssize_t CwxSockIo::send (const void *buf,
              size_t n,
              CwxTimeouter  *timeout) const
{
    return CwxSocket::send(getHandle(), buf, n, timeout);
}

inline ssize_t CwxSockIo::sendv (const iovec iov[],
               int n,
               CwxTimeouter  *timeout) const
{
    return CwxSocket::sendv(getHandle(), iov, n, timeout);
}

CWINUX_END_NAMESPACE
