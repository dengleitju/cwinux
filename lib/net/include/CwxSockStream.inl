CWINUX_BEGIN_NAMESPACE


inline ssize_t CwxSockStream::recv_n(void *buf, size_t len, int flags, CwxTimeouter  *timeout, size_t* bytes_transferred) const
{
    return CwxSocket::recv_n(getHandle(), buf, len, flags, timeout, bytes_transferred);
}

inline ssize_t CwxSockStream::recv_n(void *buf, size_t len, CwxTimeouter  *timeout, size_t *bytes_transferred) const
{
    return CwxSocket::recv_n(getHandle(), buf, len, timeout, bytes_transferred);
}

inline ssize_t CwxSockStream::read_n (void *buf, size_t len, CwxTimeouter  *timeout, size_t *bytes_transferred) const
{
    return CwxSocket::read_n(getHandle(), buf, len, timeout, bytes_transferred);
}

inline ssize_t CwxSockStream::read (CwxMsgHead& head, CwxMsgBlock*& msg, CwxTimeouter  *timeout) const
{
    return CwxSocket::read(getHandle(), head, msg, timeout);
}

inline ssize_t CwxSockStream::recv (CwxMsgHead& head, CwxMsgBlock*& msg, CwxTimeouter  *timeout) const
{
    return CwxSocket::recv(getHandle(), head, msg, timeout);
}

inline ssize_t CwxSockStream::write_n (const void *buf, size_t len, CwxTimeouter  *timeout, size_t *bytes_transferred) const
{
    return CwxSocket::write_n(getHandle(), buf, len, timeout, bytes_transferred);
}

inline ssize_t CwxSockStream::send_n (const void *buf, size_t len, int flags, CwxTimeouter  *timeout, size_t *bytes_transferred) const{
    return CwxSocket::send_n(getHandle(), buf, len, flags, timeout, bytes_transferred);
}

inline ssize_t CwxSockStream::send_n (const void *buf, size_t len, CwxTimeouter  *timeout,  size_t *bytes_transferred) const{
    return CwxSocket::send_n(getHandle(), buf, len ,timeout, bytes_transferred);
}


inline ssize_t CwxSockStream::send_urg (const void *ptr, size_t len, CwxTimeouter  *timeout) const{
    return CwxSocket::send(getHandle(), ptr, len, MSG_OOB, timeout);
}

inline ssize_t CwxSockStream::recv_urg (void *ptr, size_t len, CwxTimeouter  *timeout) const{
    return CwxSocket::recv(getHandle(), ptr, len, MSG_OOB, timeout);

}

inline int CwxSockStream::close_reader (void){
    return ::shutdown(getHandle(), SHUT_RD);
}

inline int CwxSockStream::close_writer (void){
    return ::shutdown(getHandle(), SHUT_WR);
}

CWINUX_END_NAMESPACE
