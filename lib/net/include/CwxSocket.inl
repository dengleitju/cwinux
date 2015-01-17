CWINUX_BEGIN_NAMESPACE

/// Wrapper around the @c setsockopt system call.
inline int CwxSocket::setOption (CWX_HANDLE handle, int level,
                      int option,
                      void *optval,
                      socklen_t optlen)
{
    return ::setsockopt(handle, level, option, optval, (socklen_t)optlen);
}



/// Wrapper around the @c getsockopt system call.
inline int CwxSocket::getOption (CWX_HANDLE handle,
                      int level,
                      int option,
                      void *optval,
                      socklen_t *optlen)
{
    return ::getsockopt(handle, level, option, optval, optlen);
}

inline int CwxSocket::select (int width,
                          fd_set *rfds,
                          fd_set *wfds,
                          fd_set *efds,
                          CwxTimeouter *timeout,
                          bool bRestart)
{
    timeval copy;
    timeval *timep = 0;
    int ret = 0;
    while(1)
    {
        if (timeout && timeout->isTimer()){
            if (timeout->timeout()) return 0;
            copy = *timeout->left();
            timep = &copy;
        }
        else
        {
            timep = NULL;
        }
        ret = ::select (width, rfds, wfds, efds, timep);
        if ( (-1 == ret) && (EINTR == errno) && bRestart ) continue;
        break;
    }
    return ret;
}

inline ssize_t CwxSocket::read (CWX_HANDLE handle,
                                void *buf,
                                size_t len,
                                CwxTimeouter *timeout )
{
    if (!timeout || !timeout->isTimer())
        return ::read(handle, buf, len);
    if (handleReady(handle, timeout, true, false, false) <= 0)
        return -1;
    return ::read (handle, buf, len);

}

inline ssize_t CwxSocket::recv (CWX_HANDLE handle,
                            void *buf,
                            size_t len,
                            int flags,
                            CwxTimeouter *timeout)
{
    if (!timeout || !timeout->isTimer())
        return ::recv (handle, buf, len, flags);
    if (handleReady(handle, timeout, true, false, false) <= 0)
        return -1;
    return ::recv (handle, buf, len, flags);
}

///读最多不超过len个字节，封装底层的read接口
inline ssize_t CwxSocket::recv (CWX_HANDLE handle,
                            void *buf,
                            size_t len,
                            CwxTimeouter *timeout)
{
    if (!timeout || !timeout->isTimer())
        return ::read (handle, buf, len);
    if (handleReady(handle, timeout, true, false, false) < 1)
        return -1;
    return ::read (handle, buf, len);
}

inline ssize_t CwxSocket::recvmsg (CWX_HANDLE handle,
                                struct msghdr *msg,
                                int flags,
                                CwxTimeouter *timeout)
{
    if (!timeout || !timeout->isTimer())
        return ::recvmsg (handle, msg, flags);
    if (handleReady(handle, timeout, true, false, false) < 1)
        return -1;
    return ::recvmsg (handle, msg, flags);
}

inline ssize_t CwxSocket::recvfrom (CWX_HANDLE handle,
        char *buf,
        int len,
        int flags,
        struct sockaddr *from,
        socklen_t *fromlen,
        CwxTimeouter *timeout)
{
    if (!timeout || !timeout->isTimer())
        return ::recvfrom(handle, buf, len, flags, from, fromlen);
    if (handleReady(handle, timeout, true, false, false) < 1)
        return -1;
    return ::recvfrom(handle, buf, len, flags, from, fromlen);
}

inline ssize_t CwxSocket::recvv (CWX_HANDLE handle,
            iovec *iov,
            int iovcnt,
            CwxTimeouter *timeout)
{
    if (!timeout || !timeout->isTimer())
        return ::readv(handle, iov, iovcnt);
    if (handleReady(handle, timeout, true, false, false) < 1)
        return -1;
    return ::readv(handle, iov, iovcnt);
}

inline ssize_t CwxSocket::write (CWX_HANDLE handle,
                      void const *buf,
                      size_t len,
                      CwxTimeouter *timeout)
{
    if (!timeout || !timeout->isTimer())
        return ::write(handle, buf, len);
    if (handleReady(handle, timeout, false, true, false) < 1)
        return -1;
    return ::write(handle, buf, len);
}


inline ssize_t CwxSocket::send (CWX_HANDLE handle,
                     void const*buf,
                     size_t len,
                     int flags,
                     CwxTimeouter *timeout)
{
    if (!timeout || !timeout->isTimer())
        return ::send(handle, buf, len, flags);
    if (handleReady(handle, timeout, false, true, false) < 1)
        return -1;
    return ::send (handle, buf, len, flags);
}

inline ssize_t CwxSocket::send (CWX_HANDLE handle,
                     void const *buf,
                     size_t len,
                     CwxTimeouter *timeout)
{
    if (!timeout || !timeout->isTimer())
        return ::write(handle, buf, len);
    if (handleReady(handle, timeout, false, true, false) < 1)
        return -1;
    return ::write(handle, buf, len);
}

inline ssize_t CwxSocket::sendmsg(CWX_HANDLE handle,
    struct msghdr const*msg,
    int flags,
    CwxTimeouter *timeout)
{
    if (!timeout || !timeout->isTimer())
        return ::sendmsg (handle, msg, flags);
    if (handleReady(handle, timeout, false, true, false) < 1)
        return -1;
    return ::sendmsg (handle, msg, flags);
}

inline ssize_t CwxSocket::sendto (CWX_HANDLE handle,
        char const*buf,
        int len,
        int flags,
        struct sockaddr *to,
        socklen_t tolen,
        CwxTimeouter *timeout)
{
    if (!timeout || !timeout->isTimer())
        return ::sendto (handle, buf, len, flags, to, tolen);
    if (handleReady(handle, timeout, false, true, false) < 1)
        return -1;
    return ::sendto (handle, buf, len, flags, to, tolen);
}

inline ssize_t CwxSocket::sendv (CWX_HANDLE handle,
        iovec const*iov,
        int iovcnt,
        CwxTimeouter *timeout)
{
    if (!timeout || !timeout->isTimer())
        return ::writev(handle, iov, iovcnt);
    if (handleReady(handle, timeout, false, true, false) < 1)
        return -1;
    return ::writev(handle, iov, iovcnt);

}


inline bool CwxSocket::isEnableIpv4(void){
  if (-1 == m_enableIpv4) ipvn_check(m_enableIpv4, AF_INET);
  return 1 == m_enableIpv4?true:false;
}

inline bool CwxSocket::isEnableIpv6(void){
  if (-1 == m_enableIpv6) ipvn_check (m_enableIpv6, AF_INET6);
  return 1==m_enableIpv6?true:false;
}


//检查是否支持指定的协议族
inline int CwxSocket::ipvn_check(int &ipvn_enabled, int pf){
    if (ipvn_enabled == -1)
    {
        // Determine if the kernel has IPv6 support by attempting to
        // create a AF_INET socket and see if it fails.
        CWX_HANDLE const s = socket (pf, SOCK_DGRAM, 0);
        if (s == CWX_INVALID_HANDLE)
        {
            ipvn_enabled = 0;
        }
        else
        {
            ipvn_enabled = 1;
            ::close(s);
        }
    }
    return ipvn_enabled;
}


CWINUX_END_NAMESPACE
