#include "cwx_stream.h"

CWX_UINT64 cwx_time_of_day(){
    return (CWX_UINT64)apr_time_now();
}

int cwx_stream_connect(CWX_CONNECT* conn, CWX_UINT32 millisecond){
    int fd=-1;
    fd_set write_fds;
    int flags=0;
    int error = 0;
    unsigned int len = sizeof(error);    
    struct sockaddr_in sin;
    struct timeval timeout;

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    memcpy(&sin.sin_addr, &conn->m_host->m_host, sizeof(struct in_addr));
    sin.sin_port = htons((short) conn->m_host->m_port);    
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        CWX_ERR(("%s:%d:cwx_stream_connect(): can't create socket by socket(). svr=%s ip=%s port=%d  errno=%d\n",
            __FILE__,
            __LINE__,
            conn->m_host->m_svr_name,
            conn->m_host->m_ip,
            conn->m_host->m_port,
            errno));
        return -1; /* bad socket */
    }

    flags = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, O_NONBLOCK|flags);
    FD_ZERO(&write_fds);
    FD_SET(fd, &write_fds);
    
    timeout.tv_sec = millisecond/1000;
    timeout.tv_usec = (millisecond%1000) * 1000;
    
    if (!connect(fd, (const struct sockaddr *) &sin, sizeof(sin))) {
        fcntl(fd, F_SETFL, flags);
        return fd;
    }

    while(1){
        if (select(fd + 1, 0, &write_fds, 0, &timeout) <= 0){
            if (EINTR == errno) continue;
            CWX_ERR(("%s:%d:cwx_stream_connect(): timeout. svr=%s ip=%s port=%d errno=%d\n",
                __FILE__,
                __LINE__,
                conn->m_host->m_svr_name,
                conn->m_host->m_ip,
                conn->m_host->m_port,
                errno));
            close(fd);
            return -1;
        }
        break;
    }
    if ((0 == FD_ISSET(fd, &write_fds)) ||
        (getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) ||
        error){
        CWX_ERR(("%s:%d:cwx_stream_connect(): connect failed. svr=%s ip=%s port=%d errno=%d\n",
            __FILE__,
            __LINE__,
            conn->m_host->m_svr_name,
            conn->m_host->m_ip,
            conn->m_host->m_port,
            errno));
        close(fd);
        return -1;
    }
    
    fcntl(fd, F_SETFL, flags);
    CWX_LOG(("%s:%d:cwx_stream_connect(): connected. svr=%s ip=%s port=%d socket=%d\n",
        __FILE__,
        __LINE__,
        conn->m_host->m_svr_name,
        conn->m_host->m_ip,
        conn->m_host->m_port,
        fd));
    return fd;
}

int cwx_stream_read(CWX_CONNECT* conn, void *buf, CWX_UINT32 length, CWX_UINT32 millisecond){
    int pos = 0;
    int ret=0;
    fd_set read_fds;
    struct timeval timeout;
    CWX_UINT64 cur_time=0;
    CWX_UINT64 end_time=0;
    CWX_UINT64 left_time=0;
    end_time = cwx_time_of_day() + millisecond * 1000;
    while(1){
        cur_time = cwx_time_of_day();
        left_time = end_time>cur_time?end_time-cur_time:0;
        if (0 == left_time){
            CWX_ERR(("%s:%d:cwx_stream_read(): timeout. svr=%s ip=%s port=%d socket=%d \n",
                __FILE__,
                __LINE__,
                conn->m_host->m_svr_name,
                conn->m_host->m_ip,
                conn->m_host->m_port,
                conn->m_socket->m_fd));
            return 0;
        }
        timeout.tv_sec = left_time/1000000;
        timeout.tv_usec = left_time%1000000;
        FD_ZERO(&read_fds);
        FD_SET(conn->m_socket->m_fd, &read_fds);
        ret = select(conn->m_socket->m_fd + 1, &read_fds, 0, 0, &timeout);
        if (0 == ret){//timeout 
            CWX_ERR(("%s:%d:cwx_stream_read(): timeout. svr=%s ip=%s port=%d socket=%d \n",
                __FILE__,
                __LINE__,
                conn->m_host->m_svr_name,
                conn->m_host->m_ip,
                conn->m_host->m_port,
                conn->m_socket->m_fd));
            return 0;
        }else if (-1 == ret){
            if (EINTR == errno) continue; //signal
            CWX_ERR(("%s:%d:cwx_stream_read(): inner error. svr=%s ip=%s port=%d socket=%d errno=%d \n",
                __FILE__,
                __LINE__,  
                conn->m_host->m_svr_name,
                conn->m_host->m_ip,
                conn->m_host->m_port,
                conn->m_socket->m_fd,
                errno));
            return -1;
        }
        //read from socket
        ret = read(conn->m_socket->m_fd, buf + pos, length - pos);
        if (0 == ret){//end file
            CWX_ERR(("%s:%d:cwx_stream_read(): socket closed. svr=%s ip=%s port=%d socket=%d errno=%d \n",
                __FILE__,
                __LINE__,  
                conn->m_host->m_svr_name,
                conn->m_host->m_ip,
                conn->m_host->m_port,
                conn->m_socket->m_fd,
                errno));
            return -1;
        }else if(-1 == ret){
            if (EINTR == errno) continue;//signal
            CWX_ERR(("%s:%d:cwx_stream_read(): inner error. svr=%s ip=%s port=%d socket=%d errno=%d \n",
                __FILE__,
                __LINE__,  
                conn->m_host->m_svr_name,
                conn->m_host->m_ip,
                conn->m_host->m_port,
                conn->m_socket->m_fd,
                errno));
            return -1;
        }
        pos += ret;
        if (pos == length) break;
    }
    return length;
}

int cwx_stream_write(CWX_CONNECT* conn, void *buf, CWX_UINT32 length, CWX_UINT32 millisecond){
    int pos = 0;
    int ret;
    fd_set write_fds, read_fds;
    struct timeval timeout;
    CWX_UINT64 cur_time=0;
    CWX_UINT64 end_time=0;
    CWX_UINT64 left_time=0;

    end_time = cwx_time_of_day() + millisecond * 1000;
    while(1){
        cur_time = cwx_time_of_day();
        left_time = end_time>cur_time?end_time-cur_time:0;
        if (0 == left_time){
            CWX_ERR(("%s:%d:cwx_stream_write(): timeout. svr=%s ip=%s port=%d socket=%d \n",
                __FILE__,
                __LINE__,
                conn->m_host->m_svr_name,
                conn->m_host->m_ip,
                conn->m_host->m_port,
                conn->m_socket->m_fd));
            return 0;
        }
        timeout.tv_sec = left_time/1000000;
        timeout.tv_usec = left_time%1000000;
        FD_ZERO(&write_fds);
        FD_SET(conn->m_socket->m_fd, &write_fds);
        FD_ZERO(&read_fds);
        FD_SET(conn->m_socket->m_fd, &read_fds);
        ret = select(conn->m_socket->m_fd + 1, &read_fds, &write_fds, 0, &timeout);
        if (0 == ret){//timeout 
            CWX_ERR(("%s:%d:stream_write(): timeout. svr=%s ip=%s port=%d socket=%d\n",
                __FILE__,
                __LINE__,
                conn->m_host->m_svr_name,
                conn->m_host->m_ip,
                conn->m_host->m_port,
                conn->m_socket->m_fd));
            return 0;
        }else if (-1 == ret){
            if (EINTR == errno) continue; //signal

            CWX_ERR(("%s:%d:stream_write(): inner error. svr=%s ip=%s port=%d socket=%d errno=%d\n",
                __FILE__,
                __LINE__,
                conn->m_host->m_svr_name,
                conn->m_host->m_ip,
                conn->m_host->m_port,
                conn->m_socket->m_fd,
                errno));
            return -1;
        }
        if (0 != FD_ISSET(conn->m_socket->m_fd, &read_fds)){
            CWX_ERR(("%s:%d:stream_write(): socket closed. svr=%s ip=%s port=%d socket=%d errno=%d\n",
                __FILE__,
                __LINE__,
                conn->m_host->m_svr_name,
                conn->m_host->m_ip,
                conn->m_host->m_port,
                conn->m_socket->m_fd,
                errno));
            return -1;
        }
        //write to socket
        ret = write(conn->m_socket->m_fd, buf + pos, length - pos);
        if (0 == ret){//end file
            CWX_ERR(("%s:%d:stream_write(): socket closed. svr=%s ip=%s port=%d socket=%d errno=%d\n",
                __FILE__,
                __LINE__,
                conn->m_host->m_svr_name,
                conn->m_host->m_ip,
                conn->m_host->m_port,
                conn->m_socket->m_fd,
                errno));
            return -1;
        }else if(-1 == ret){
            if (EINTR == errno) continue ;//signal
            CWX_ERR(("%s:%d:stream_write(): inner error. svr=%s ip=%s port=%d socket=%d errno=%d\n",
                __FILE__,
                __LINE__,
                conn->m_host->m_svr_name,
                conn->m_host->m_ip,
                conn->m_host->m_port,
                conn->m_socket->m_fd,
                errno));
            return -1;
        }
        pos += ret;
        if (pos == length) break;
    }
    return length;
}

int cwx_stream_close(int fd){
  return close(fd);
}

