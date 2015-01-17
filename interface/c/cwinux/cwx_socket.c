#include "cwx_socket.h"
#include <stdarg.h>

static int cwx_snprintf(char *buf, size_t size, const char *format, ...)
{
    int ret = 0;
    va_list args;
    va_start(args, format);
    ret = vsnprintf(buf, size, format, args);
    va_end(args);
    return ret;
}

//0:success; -1:failure
static int cwx_inet_addr(struct sockaddr_in *sin, char const * szHost, char* szErr1K)
{
    struct in_addr tmp;
    struct hostent *host_entry;
    if (inet_aton(szHost, &tmp))
    {
        sin->sin_addr.s_addr = tmp.s_addr;
    }
    else
    {
        if (! (host_entry = gethostbyname(szHost)))
        {
            /* Note: < -10000 indicates a host lookup error */
            if (szErr1K)
            {
#ifdef WIN32
               cwx_snprintf(szErr1K, 1024, "Host lookup failed, host=%s, errno=%d", szHost, WSAGetLastError());
#else
                cwx_snprintf(szErr1K, 1024, "Host lookup failed, host=%s, errno=%d", szHost, errno);
#endif
            }
            return -1;
        }
        if (host_entry->h_addrtype != AF_INET)
        {
            cwx_snprintf(szErr1K, 1024, "Host lookup failed, Not AF_INET, host=%s", szHost);
            return -1;
        }
        memcpy(&(sin->sin_addr.s_addr), host_entry->h_addr_list[0], host_entry->h_length);
    }
    return 0;
}

///获取当前的时间，UINT64
uint64_t cwx_socket_timeofday()
{
    uint64_t  ttTime = 0;
#ifdef WIN32
    FILETIME timeStamp;
    GetSystemTimeAsFileTime(&timeStamp);
    ttTime = timeStamp.dwHighDateTime;
    ttTime <<=32;
    ttTime |= timeStamp.dwLowDateTime;
    ttTime /=10;
    ttTime -= INT64_C(11644473600000000);
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    ttTime = tv.tv_sec;
    ttTime *= 1000000;
    ttTime += tv.tv_usec;
#endif
    return ttTime;
}

int cwx_socket_init(char* szErr1K)
{
    int ret = 0;
    szErr1K[0]=0x00;
#ifdef WIN32
    WSADATA wsaData;
    ret = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (ret != NO_ERROR)
    {
        if (szErr1K) cwx_snprintf(szErr1K, 1024, "Failure to invoke WSAStartup");
        ret = -1;
    }
#endif
    return ret;
}

/**
*@brief tcp连接。
*@param [in] szIp  连接的IP地址。
*@param [in] unPort  连接的端口号。
*@param [in] millisecond 连接的超时ms数
*@return -1：失败；否则为连接的handle
*/
CWX_SOCKET cwx_socket_tcp_connect(char const* szHost, unsigned short unPort, unsigned int millisecond, char* szErr1K)
{
    CWX_SOCKET fd= CWX_INVALID_SOCKET;
    struct sockaddr_in sin;
    struct timeval timeout;

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port   = htons((unsigned short int)unPort);
    if (0 != cwx_inet_addr(&sin, szHost, szErr1K)) return CWX_INVALID_SOCKET;
#ifndef WIN32
    fd_set write_fds;
    int flags=0;
    int error = 0;
    unsigned int len = sizeof(error);

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        if (szErr1K) cwx_snprintf(szErr1K, 1024, "%s:%d:cwx_socket_tcp_connect(): can't create socket by socket(). host=%s port=%d  errno=%d",
            __FILE__,
            __LINE__,
            szHost,
            unPort,
            errno);
        return CWX_INVALID_SOCKET; /* bad socket */
    }
    flags = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, O_NONBLOCK|flags);
    FD_ZERO(&write_fds);
    FD_SET(fd, &write_fds);

    timeout.tv_sec = millisecond/1000;
    timeout.tv_usec = (millisecond%1000) * 1000;
    if (!connect(fd, (const struct sockaddr *) &sin, sizeof(sin)))
    {
        fcntl(fd, F_SETFL, flags);
        return fd;
    }
    while(1)
    {
        if (select(fd + 1, 0, &write_fds, 0, &timeout) <= 0)
        {
            if (EINTR == errno) continue;
            if (szErr1K) cwx_snprintf(szErr1K, 1024, "%s:%d:cwx_socket_tcp_connect(): timeout. host=%s port=%d  errno=%d",
                __FILE__,
                __LINE__,
                szHost,
                unPort,
                errno);
            close(fd);
            return CWX_INVALID_SOCKET;
        }
        break;
    }
    if ((0 == FD_ISSET(fd, &write_fds)) ||
        (getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) ||
        error)
    {
            if (szErr1K) cwx_snprintf(szErr1K, 1024, "%s:%d:cwx_socket_tcp_connect(): connect failed. host=%s port=%d  errno=%d",
                __FILE__,
                __LINE__,
                szHost,
                unPort,
                errno);
            close(fd);
            return CWX_INVALID_SOCKET;
    }    
    fcntl(fd, F_SETFL, flags);
#else
    unsigned long is_nonblock = 1;
    int result;
    WSANETWORKEVENTS networkResult;
    WSAEVENT event;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == CWX_INVALID_SOCKET)
    {
        if (szErr1K) cwx_snprintf(szErr1K, 1024, "%s:%d:cwx_socket_tcp_connect(): socket failed. host=%s port=%d  errno=%d",
            __FILE__,
            __LINE__,
            szHost,
            unPort,
            errno);
        return CWX_INVALID_SOCKET; /* bad socket */
    }
    ioctlsocket(fd, FIONBIO, &is_nonblock);
    if (connect(fd, (struct sockaddr *) sin, sizeof(struct sockaddr_in)))
    {
        event = WSACreateEvent();
        WSAEventSelect(fd, event, FD_CONNECT);
        result = WSAWaitForMultipleEvents(1, &event, 0, millisecond * 1000, 0);
        WSAEnumNetworkEvents(fd, event, &networkResult);
        WSAEventSelect(fd, 0, 0);
        WSACloseEvent(event);
        if (result != WSA_WAIT_EVENT_0 || networkResult.iErrorCode[FD_CONNECT_BIT] != NO_ERROR) {
            if (szErr1K) cwx_snprintf(szErr1K, 1024, "%s:%d:cwx_socket_tcp_connect(): connect failed. host=%s port=%d  errno=%d",
                __FILE__,
                __LINE__,
                szHost,
                unPort,
                errno);
            closesocket(fd);
            return CWX_INVALID_SOCKET;
        }
    }
    is_nonblock = 0;
    ioctlsocket(fd, FIONBIO, &is_nonblock);
#endif
    return fd;
}

CWX_SOCKET cwx_socket_unix_connect(char const* szPath, char* szErr1K)
{
    CWX_SOCKET fd= CWX_INVALID_SOCKET;
#ifdef WIN32
    if (szErr1K) cwx_snprintf(szErr1K, 1024, "%s:%d:cwx_socket_unix_connect(): not support in win32 . socket=%s ",
        __FILE__,
        __LINE__,
        szPath));
#else
    struct sockaddr_un	s_un;
    memset(&s_un, 0, sizeof(struct sockaddr_un));
    s_un.sun_family = PF_LOCAL;
    memcpy(s_un.sun_path, szPath, strlen(szPath));
    fd = socket(PF_UNIX, SOCK_STREAM, 0);
    if (fd < 0)
    {
        if (szErr1K) cwx_snprintf(szErr1K, 1024, "%s:%d:cwx_socket_unix_connect(): can't create socket by socket(). path=%s errno=%d",
            __FILE__,
            __LINE__,
            szPath,
            errno);
        return CWX_INVALID_SOCKET; /* bad socket */
    }
    if (0 == connect(fd, (const struct sockaddr *) &s_un, sizeof(s_un)))  return fd;
    if (szErr1K) cwx_snprintf(szErr1K, 1024, "%s:%d:cwx_socket_unix_connect(): connect failed. path=%s errno=%d",
           __FILE__,
           __LINE__,
           szPath,
           errno);
    close(fd);
#endif
    return CWX_INVALID_SOCKET;
}

int cwx_socket_read(CWX_SOCKET fd, void *buf, unsigned int length, unsigned int millisecond, char* szErr1K)
{
    int pos = 0;
    int ret=0;
    fd_set read_fds;
    struct timeval timeout;
    uint64_t cur_time=0;
    uint64_t end_time=0;
    uint64_t left_time=0;
    end_time = cwx_socket_timeofday() + millisecond * 1000;
    while(1)
    {
        cur_time = cwx_socket_timeofday();
        left_time = end_time>cur_time?end_time-cur_time:0;
        if (0 == left_time)
        {
            if(szErr1K) cwx_snprintf(szErr1K, 1024, "%s:%d:cwx_socket_read(): timeout. socket=%d",
                __FILE__,
                __LINE__,
                fd);
            return 0;
        }
        timeout.tv_sec = left_time/1000000;
        timeout.tv_usec = left_time%1000000;
        FD_ZERO(&read_fds);
        FD_SET(fd, &read_fds);
        ret = select(fd + 1, &read_fds, 0, 0, &timeout);
        if (0 == ret)
        {//timeout 
            if (szErr1K) cwx_snprintf(szErr1K, 1024, "%s:%d:cwx_socket_read(): timeout. socket=%d \n",
                __FILE__,
                __LINE__,
                fd);
            return 0;
        }else if (-1 == ret)
        {
#ifndef WIN32
            if (EINTR == errno) continue; //signal
#endif
            if (szErr1K) cwx_snprintf(szErr1K, 1024, "%s:%d:cwx_socket_read(): inner error. socket=%d errno=%d \n",
                __FILE__,
                __LINE__,
                fd,
                errno);
            return -1;
        }
        //read from socket
        ret = read(fd, buf + pos, length - pos);
        if (0 == ret)
        {//end file
            if (szErr1K) cwx_snprintf(szErr1K, 1024, "%s:%d:cwx_socket_read(): socket closed. socket=%d errno=%d \n",
                __FILE__,
                __LINE__,  
                fd,
                errno);
            return -1;
        }
        else if(-1 == ret)
        {
#ifndef WIN32
            if (EINTR == errno) continue;//signal
#endif
            if (szErr1K) cwx_snprintf(szErr1K, 1024, "%s:%d:cwx_socket_read(): inner error. socket=%d errno=%d ",
                __FILE__,
                __LINE__,  
                fd,
                errno);
            return -1;
        }
        pos += ret;
        if (pos == (int)length) break;
    }
    return length;
}

int cwx_socket_write(CWX_SOCKET fd, void *buf, unsigned int length, unsigned int millisecond, char* szErr1K)
{
    int pos = 0;
    int ret;
    fd_set write_fds;
    struct timeval timeout;
    uint64_t cur_time=0;
    uint64_t end_time=0;
    uint64_t left_time=0;

    end_time = cwx_socket_timeofday() + millisecond * 1000;
    while(1)
    {
        cur_time = cwx_socket_timeofday();
        left_time = end_time>cur_time?end_time-cur_time:0;
        if (0 == left_time)
        {
            if (szErr1K) cwx_snprintf(szErr1K, 1024, "%s:%d:cwx_socket_write(): timeout. socket=%d",
                __FILE__,
                __LINE__,
                fd);
            return 0;
        }
        timeout.tv_sec = left_time/1000000;
        timeout.tv_usec = left_time%1000000;
        FD_ZERO(&write_fds);
        FD_SET(fd, &write_fds);
        ret = select(fd + 1, 0, &write_fds, 0, &timeout);
        if (0 == ret)
        {//timeout 
            if (szErr1K) cwx_snprintf(szErr1K, 1024, "%s:%d:cwx_socket_write(): timeout. socket=%d",
                __FILE__,
                __LINE__,
                fd);
            return 0;
        }
        else if (-1 == ret)
        {
#ifndef WIN32
            if (EINTR == errno) continue; //signal
#endif
            if (szErr1K) cwx_snprintf(szErr1K, 1024, "%s:%d:cwx_socket_write(): inner error. socket=%d errno=%d",
                __FILE__,
                __LINE__,
                fd,
                errno);
            return -1;
        }
        //write to socket
        ret = write(fd, buf + pos, length - pos);
        if (0 == ret)
        {//end file
            if (szErr1K) cwx_snprintf(szErr1K, 1024, "%s:%d:cwx_socket_write(): socket closed. socket=%d errno=%d",
                __FILE__,
                __LINE__,
                fd,
                errno);
            return -1;
        }
        else if(-1 == ret)
        {
#ifndef WIN32
            if (EINTR == errno) continue ;//signal
#endif
            if (szErr1K) cwx_snprintf(szErr1K, 1024, "%s:%d:stream_write(): inner error. socket=%d errno=%d",
                __FILE__,
                __LINE__,
                fd,
                errno);
            return -1;
        }
        pos += ret;
        if (pos == (int)length) break;
    }
    return length;
}



int cwx_socket_close(CWX_SOCKET fd)
{
#ifdef WIN32
    return closesocket(fd);
#else
    return close(fd);
#endif
}

