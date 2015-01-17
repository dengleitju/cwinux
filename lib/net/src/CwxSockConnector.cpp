#include "CwxSockConnector.h"
CWINUX_BEGIN_NAMESPACE
///默认构造函数
CwxSockConnector::CwxSockConnector()
{

}
///析构函数.
CwxSockConnector::~CwxSockConnector(void)
{

}

int CwxSockConnector::connect (CwxSockStream& stream,
                               CwxAddr const& remoteAddr,
                               CwxAddr const& localAddr,
                               CwxTimeouter* timeout,
                               int protocol,
                               bool reuse_addr,
                               CWX_NET_SOCKET_ATTR_FUNC fn,
                               void* fnArg)
{
    if ((stream.getHandle() == CWX_INVALID_HANDLE) &&
        (stream.open(remoteAddr.getType(),
        SOCK_STREAM,
        protocol,
        reuse_addr) == -1))
    {
        return -1;

    }

    if (localAddr != CwxAddr::sap_any)
    {
        sockaddr *laddr = reinterpret_cast<sockaddr *> (localAddr.getAddr());
        int size = localAddr.getSize ();
        if (::bind (stream.getHandle (),
            laddr,
            size) == -1)
        {
            CwxErrGuard guard;
            stream.close ();
            return -1;
        }
    }
    // Enable non-blocking, if required.
    if ((timeout != 0) && (stream.setNonblock(true) == -1))
    {
        CwxErrGuard guard;
        stream.close ();
        return -1;
    }
    if (fn)
    {
        if (0 != fn(stream.getHandle(), fnArg))
        {
            CwxErrGuard guard;
            stream.close();
            return -1;
        }
    }
    int result = ::connect (stream.getHandle (),
        reinterpret_cast<sockaddr *> (remoteAddr.getAddr ()),
        remoteAddr.getSize ());

    if (result == -1 && timeout != 0)
    {
        // Check whether the connection is in progress.
        if (errno == EINPROGRESS || errno == EWOULDBLOCK)
        {
            // This expression checks if we were polling.
            if (*timeout->getTimeout() == CwxTimeValue::ZERO)
            {
                errno = EWOULDBLOCK;
            }
            // Wait synchronously using timeout.
            else if (this->complete (stream,
                0,
                timeout) == -1)
            {
            }
            else
            {
                return 0;
            }
        }
    }

    // EISCONN is treated specially since this routine may be used to
    // check if we are already connected.
    if (result != -1 || errno == EISCONN)
    {
        // Start out with non-blocking disabled on the new_stream.
        result = stream.setNonblock(false);
        if (result == -1)
        {
            CwxErrGuard guard;
            stream.close ();
        }
    }
    else if (!(errno == EWOULDBLOCK || errno == ETIMEDOUT))
    {
        CwxErrGuard guard;
        stream.close ();
    }
    return result;
}

int CwxSockConnector::complete (CwxSockStream &stream,
              CwxAddr *remoteAddr ,
              CwxTimeouter* timeout)
{
    if (timeout && timeout->isTimer())
    {
        if (CwxSocket::handleReady(stream.getHandle(), timeout, false, true, false, true) < 1)
        {
            CwxErrGuard guard;
            stream.close();
            return -1;
        }
    }

    int sock_err = 0;
    socklen_t sock_err_len = sizeof (sock_err);
    int sockopt_ret = ::getsockopt (stream.getHandle(), SOL_SOCKET, SO_ERROR,
        &sock_err, &sock_err_len);
    if ((sockopt_ret < 0) || sock_err)
    {
        stream.close();
        errno = sock_err;
        return -1;
    }

    if (remoteAddr != 0)
    {
        socklen_t len = remoteAddr->getSize ();
        sockaddr *addr = reinterpret_cast<sockaddr *> (remoteAddr->getAddr ());
        if (::getpeername(stream.getHandle(),
            addr,
            &len) == -1)
        {
            // Save/restore errno.
            CwxErrGuard guard;
            stream.close ();
            return -1;
        }
    }

    stream.setNonblock(false);
    return 0;

}

CWINUX_END_NAMESPACE

