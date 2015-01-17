#include "CwxSocket.h"
#include <stdlib.h>
#include <netdb.h>
CWINUX_BEGIN_NAMESPACE
int  CwxSocket::m_enableIpv6 = -1;
int  CwxSocket::m_enableIpv4 = -1;

int CwxSocket::setKeepalive(CWX_HANDLE handle,
                                   bool bKeepalive,
                                   int idle,
                                   int inter,
                                   int count)
{
    int keepAlive = bKeepalive?1:0;
    if(::setsockopt(handle,
        SOL_SOCKET,
        SO_KEEPALIVE,
        (void*)&keepAlive,
        sizeof(keepAlive)) == -1)
    {
        return -1;
    }
    if (!bKeepalive) return 0;

    if(::setsockopt(handle,SOL_TCP,TCP_KEEPIDLE,(void *)&idle,sizeof(idle)) == -1)
    {
        return -1;
    }
    if(::setsockopt(handle,SOL_TCP,TCP_KEEPINTVL,(void *)&inter,sizeof(inter)) == -1)
    {
        return -1;
    }
    if(::setsockopt(handle,SOL_TCP,TCP_KEEPCNT,(void *)&count,sizeof(count)) == -1)
    {
        return -1;
    }
    return 0;
}


ssize_t CwxSocket::read_n (CWX_HANDLE handle,
                       void *buf,
                       size_t len,
                       CwxTimeouter *timeout,
                       size_t* accepted)
{
    size_t recv_byte = 0;
    ssize_t ret = 0;
    while(recv_byte < len)
    {
        if (timeout && timeout->isTimer())
        {
            ret = handleReady(handle, timeout, true, false, false);
            if (ret <= 0)
            {
                ret = -1;
                break;
            }
        }
        ret = ::read(handle, ((char*)buf) + recv_byte, len - recv_byte);
        if ( (-1 == ret) && (EINTR == errno) ) continue;
        if (0 >= ret)
            break;
        recv_byte += ret;
    }
    if (accepted) *accepted = recv_byte;
    return ret>0?recv_byte:ret;
}

ssize_t CwxSocket::recv_n (CWX_HANDLE handle,
                              void *buf,
                              size_t len,
                              int flags,
                              CwxTimeouter *timeout,
                              size_t* accepted)
{
    size_t recv_byte = 0;
    ssize_t ret = 0;
    while(recv_byte < len)
    {
        if (timeout && timeout->isTimer())
        {
            ret = handleReady(handle, timeout, true, false, false);
            if (ret <= 0)
            {
                ret = -1;
                break;
            }
        }
        ret = ::recv(handle, ((char*)buf) + recv_byte, len - recv_byte, flags);
        if ( (-1 == ret) && (EINTR == errno) ) continue;
        if (0 >= ret)
            break;
        recv_byte += ret;
    }
    if (accepted) *accepted = recv_byte;
    return ret>0?recv_byte:ret;
}

ssize_t CwxSocket::recv_n (CWX_HANDLE handle,
                void *buf,
                size_t len,
                CwxTimeouter *timeout,
                size_t* accepted)
{
    size_t recv_byte = 0;
    ssize_t ret = 0;
    while(recv_byte < len)
    {
        if (timeout && timeout->isTimer())
        {
            ret = handleReady(handle, timeout, true, false, false);
            if (ret <= 0)
            {
                ret = -1;
                break;
            }
        }
        ret = ::read(handle, ((char*)buf) + recv_byte, len - recv_byte);
        if ( (-1 == ret) && (EINTR == errno) ) continue;
        if (0 >= ret)
            break;
        recv_byte += ret;
    }
    if (accepted) *accepted = recv_byte;
    return ret>0?recv_byte:ret;
}

ssize_t CwxSocket::read (CWX_HANDLE handle, CwxMsgHead& head, CwxMsgBlock*& msg, CwxTimeouter  *timeout) 
{
    char szHead[CwxMsgHead::MSG_HEAD_LEN];
    ssize_t  ret = read_n(handle, szHead, CwxMsgHead::MSG_HEAD_LEN, timeout, NULL);
    if (ret != CwxMsgHead::MSG_HEAD_LEN)
        return ret;
    if (!head.fromNet(szHead))
    {
        errno = EBADMSG;
        return -1;
    }
    msg = CwxMsgBlockAlloc::malloc(head.getDataLen());
    if (!msg)
    {
        errno = ENOMEM;
        return -1;
    }
    ret = read_n(handle, msg->wr_ptr(), head.getDataLen(), timeout, NULL);
    if (ret != (ssize_t)head.getDataLen())
    {
        CwxMsgBlockAlloc::free(msg);
        msg = NULL;
        return ret;
    }
    msg->wr_ptr(head.getDataLen());
    return CwxMsgHead::MSG_HEAD_LEN + head.getDataLen();
}

ssize_t CwxSocket::recv (CWX_HANDLE handle, CwxMsgHead& head, CwxMsgBlock*& msg, CwxTimeouter  *timeout) 
{
    char szHead[CwxMsgHead::MSG_HEAD_LEN];
    ssize_t  ret = read_n(handle, szHead, CwxMsgHead::MSG_HEAD_LEN, timeout, NULL);
    if (ret != CwxMsgHead::MSG_HEAD_LEN)
        return ret;
    if (!head.fromNet(szHead))
    {
        errno = EBADMSG;
        return -1;
    }
    msg = CwxMsgBlockAlloc::malloc(head.getDataLen());
    if (!msg)
    {
        errno = ENOMEM;
        return -1;
    }
    ret = recv_n(handle, msg->wr_ptr(), head.getDataLen(), timeout, NULL);
    if (ret != (ssize_t)head.getDataLen())
    {
        CwxMsgBlockAlloc::free(msg);
        msg = NULL;
        return ret;
    }
    msg->wr_ptr(head.getDataLen());
    return CwxMsgHead::MSG_HEAD_LEN + head.getDataLen();

}


ssize_t CwxSocket::write_n (CWX_HANDLE handle,
                        void const*buf,
                        size_t len,
                        CwxTimeouter *timeout,
                        size_t* sent)
{
    size_t send_byte = 0;
    ssize_t ret = 0;
    while(send_byte < len)
    {
        if (timeout && timeout->isTimer())
        {
            ret = handleReady(handle, timeout, false, true, false);
            if (ret <= 0)
            {
                ret = -1;
                break;
            }
        }
        ret = ::write(handle, ((char*)buf) + send_byte, len - send_byte);
        if ( (-1 == ret) && (EINTR == errno) ) continue;
        if (0 >= ret)
            break;
        send_byte += ret;
    }
    if (sent) *sent = send_byte;
    return ret>0?send_byte:ret;
}

ssize_t CwxSocket::send_n (CWX_HANDLE handle,
                       void const*buf,
                       size_t len,
                       int flags,
                       CwxTimeouter *timeout,
                       size_t* sent)
{
    size_t send_byte = 0;
    ssize_t ret = 0;
    while(send_byte < len)
    {
        if (timeout && timeout->isTimer())
        {
            ret = handleReady(handle, timeout, false, true, false);
            if (ret <= 0)
            {
                ret = -1;
                break;
            }
        }
        ret = ::send(handle, ((char*)buf) + send_byte, len - send_byte, flags);
        if ( (-1 == ret) && (EINTR == errno) ) continue;
        if (0 >= ret)
            break;
        send_byte += ret;
    }
    if (sent) *sent = send_byte;
    return ret>0?send_byte:ret;

}

ssize_t CwxSocket::send_n (CWX_HANDLE handle,
                       void const*buf,
                       size_t len,
                       CwxTimeouter *timeout,
                       size_t* accepted)
{
    size_t send_byte = 0;
    ssize_t ret = 0;
    while(send_byte < len)
    {
        if (timeout && timeout->isTimer())
        {
            ret = handleReady(handle, timeout, false, true, false);
            if (ret <= 0)
            {
                ret = -1;
                break;
            }
        }
        ret = ::write(handle, ((char*)buf) + send_byte, len - send_byte);
        if ( (-1 == ret) && (EINTR == errno) ) continue;
        if (0 >= ret)
            break;
        send_byte += ret;
    }
    if (accepted) *accepted = send_byte;
    return ret>0?send_byte:ret;
}



int CwxSocket::handleReady (CWX_HANDLE handle,
                               CwxTimeouter *timeout,
                               bool bRead,
                               bool bWrite,
                               bool bException,
                               bool bRestart)
{
    fd_set handle_set;
    FD_ZERO(&handle_set);
    FD_SET(handle, &handle_set);
    // Wait for data or for the timeout to elapse.
    int select_width = int(handle) + 1;
    int result = 0;
    result = select (select_width,
        bRead ? &handle_set:NULL, // read_fds.
        bWrite ? &handle_set:NULL, // write_fds.
        bException ? &handle_set:NULL, // exception_fds.
        timeout,
        bRestart);

    if (result < 0) 
    {
        return -1;
    }
    else if (0 == result)
    {
        errno = ETIME;
        return 0;
    }
    return result;
}



int CwxSocket::getSvrPort(char const* szSvrName, char const* szProtocol)
{
    // Maybe port_name is directly a port number?
    char *endp = 0;
    long port = strtol (szSvrName, &endp, 10);
    if (*endp == '\0')
    {
        // port_name was really a number, and nothing else.
        // Check for overflow.
        if (port < 0 || port > CWX_MAX_DEFAULT_PORT)
            return -1;
        return port;
    }
    // We try to resolve port number from its name.
    port = -1;
    struct servent * sp = getservbyname(szSvrName, szProtocol);
    
    if (sp != 0)
    {
        CWX_UINT16 unPort = sp->s_port;
        port = CWX_NTOHS(unPort);
    }
    return port;
}


CWINUX_END_NAMESPACE

