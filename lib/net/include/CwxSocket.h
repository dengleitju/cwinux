#ifndef __CWX_SOCKET_H__
#define __CWX_SOCKET_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxSocket.h
@brief 基础socket API的封装类。
@author cwinux@gmail.com
@version 0.1
@date 2010-5-10
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxType.h"
#include "CwxTimeValue.h"
#include "CwxMsgHead.h"
#include "CwxMsgBlock.h"
#include "CwxNetMacro.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
CWINUX_BEGIN_NAMESPACE
/**
@class CwxSocket
@brief Socket API的封装类。
*/
class CWX_API CwxSocket
{
public:
    /**
    @brief 封装了setsockopt api，设置socket handle的参数。
    @param [in] handle socket 的handle。
    @param [in] level setsockopt的level
    @param [in] option setsockopt的option
    @param [in] optval setsockopt的optval
    @param [in] optlen setsockopt的optlen。
    @return -1：failure；0：成功。与setsockopt一致
    */
    static int setOption (CWX_HANDLE handle, int level,
        int option,
        void *optval,
        socklen_t optlen);

    /**
    @brief 打开、关闭连接的keep-alive开关。
    @param [in] handle socket 的handle。
    @param [in] bKeepalive 打开keep-alive
    @param [in] idle 开始首次KeepAlive探测前的TCP空闭时间(second）
    @param [in] inter 两次KeepAlive探测间的时间间隔(second)
    @param [in] count 判定断开前的KeepAlive探测次数。
    @return -1：failure；0：成功。与setsockopt一致
    */
    static int setKeepalive(CWX_HANDLE handle,
        bool bKeepalive,
        int idle,
        int inter,
        int count);

    /**
    @brief 封装了getsockopt api，获取socket handle的参数。
    @param [in] handle socket 的handle。
    @param [in] level setsockopt的level
    @param [in] option setsockopt的option
    @param [in] optval setsockopt的optval
    @param [in] optlen setsockopt的optlen。
    @return -1：failure；0：成功。与getsockopt一致
    */
    static int getOption (CWX_HANDLE handle, int level,
        int option,
        void *optval,
        socklen_t *optlen);

    /**
    @brief 监测读写事件是否发生。对select的封装。
    @param [in] width 最大的handle+1。
    @param [in] rfds 读的handle集合
    @param [in] wfds 写的handle集合
    @param [in] efds exeption的handle集合
    @param [in] timeout timeout的定时器，若为NULL表示阻塞。
    @param [in] bRestart 若在等待过程中被信号中断，是否继续等待。true：继续等待；false：不等待。
    @return -1：failure；0：超时；>0：ready的handle数量。与select一致。
    */
    static int select (int width,
        fd_set *rfds,
        fd_set *wfds = NULL,
        fd_set *efds = NULL,
        CwxTimeouter  *timeout = NULL,
        bool bRestart = true
        );
    /**
    @brief 从连接上接受数据。是对OS的read接口的封装
    @param [in] handle 读的句柄。
    @param [in] buf 接收数据的buf
    @param [in] len buf的长度
    @param [in] timeout timeout的值，若为NULL表示阻塞。
    @return -1：failure，包括超时；0：对端关闭；>0：读取的字节数量
    */
    static ssize_t read (CWX_HANDLE handle,
        void *buf,
        size_t len,
        CwxTimeouter *timeout = NULL);

    /**
    @brief 从连接上接受数据。是对OS的recv接口的封装
    @param [in] handle 读的句柄。
    @param [in] buf 接收数据的buf
    @param [in] len buf的长度
    @param [in] flags os的recv api的flag
    @param [in] timeout timeout的值，若为NULL表示阻塞。
    @return -1：failure，包括超时；0：对端关闭；>0：读取的字节数量
    */
    static ssize_t recv (CWX_HANDLE handle,
        void *buf,
        size_t len,
        int flags,
        CwxTimeouter *timeout = NULL);
    /**
    @brief 从连接上接受数据。是对OS的read接口的封装
    @param [in] handle 读的句柄。
    @param [in] buf 接收数据的buf
    @param [in] len buf的长度
    @param [in] timeout timeout的值，若为NULL表示阻塞。
    @return -1：failure，包括超时；0：对端关闭；>0：读取的字节数量
    */
    static ssize_t recv (CWX_HANDLE handle,
        void *buf,
        size_t len,
        CwxTimeouter *timeout = NULL);
    /**
    @brief 从连接上接受n个字节。是对OS的read接口的封装
    @param [in] handle 读的句柄。
    @param [in] buf 接收数据的buf
    @param [in] len buf的长度
    @param [in] timeout timeout的值，若为NULL表示阻塞。
    @param [out] accepted 失败时读取的字节的数量。
    @return -1：failure，包括超时；0：对端关闭；>0：读取的字节数量
    */
    static ssize_t read_n (CWX_HANDLE handle,
        void *buf,
        size_t len,
        CwxTimeouter *timeout = NULL,
        size_t* accepted=NULL);

    /**
    @brief 读len个字节。是对OS的recv接口的封装
    @param [in] handle 读的句柄。
    @param [in] buf 接收数据的buf
    @param [in] len 读取数据的长度
    @param [in] flags os的recv api的flag
    @param [in] timeout timeout的值，若为NULL表示阻塞。
    @param [out] accepted 失败时读取的字节的数量。
    @return -1：failure，包括超时；0：对端关闭；>0：读取的字节数量
    */
    static ssize_t recv_n (CWX_HANDLE handle,
        void *buf,
        size_t len,
        int flags,
        CwxTimeouter *timeout = NULL,
        size_t* accepted=NULL);

    /**
    @brief 读len个字节。是对OS的read接口的封装
    @param [in] handle 读的句柄。
    @param [in] buf 接收数据的buf
    @param [in] len 读取数据的长度
    @param [in] timeout timeout的值，若为NULL表示阻塞。
    @param [out] accepted 失败时读取的字节的数量。
    @return -1：failure，包括超时；0：对端关闭；>0：读取的字节数量
    */
    static ssize_t recv_n (CWX_HANDLE handle,
        void *buf,
        size_t len,
        CwxTimeouter *timeout = NULL,
        size_t* accepted=NULL);
    /**
    @brief 一个cwinux的数据包。
    @param [in] handle 读的句柄。
    @param [out] head 数据包的包头。
    @param [out] msg 接收的数据包，若失败，则返回NULL。
    @param [in] timeout timeout的值，若为NULL表示阻塞。
    @return -1：接收错误；0：连接关闭；>0：应该等于len，表示成功接收了len长度的字节流。
    */
    static ssize_t read (CWX_HANDLE handle,
        CwxMsgHead& head,
        CwxMsgBlock*& msg,
        CwxTimeouter  *timeout = 0) ;
    /**
    @brief 一个cwinux的数据包。
    @param [in] handle 读的句柄。
    @param [out] head 数据包的包头。
    @param [out] msg 接收的数据包，若失败，则返回NULL。
    @param [in] timeout timeout的值，若为NULL表示阻塞。
    @return -1：接收错误；0：连接关闭；>0：应该等于len，表示成功接收了len长度的字节流。
    */
    static ssize_t recv (CWX_HANDLE handle,
        CwxMsgHead& head,
        CwxMsgBlock*& msg,
        CwxTimeouter  *timeout = 0) ;
    /**
    @brief 从连接上接受一个msg，可为非面向连接的。是对OS的recvmsg接口的封装
    @param [in] handle 读的句柄。
    @param [in] msg 接收数据的buf
    @param [in] flags recvmsg的flag
    @param [in] timeout timeout的值，若为NULL表示阻塞。
    @return -1：failure，包括超时；0：对端关闭；>0：读取的字节数量
    */
    static ssize_t recvmsg(CWX_HANDLE handle,
        struct msghdr *msg,
        int flags,
        CwxTimeouter *timeout = NULL);

    /**
    @brief 从连接上接受一个msg，可为非面向连接的。是对OS的recvfrom接口的封装
    @param [in] handle 读的句柄。
    @param [in] buf 接收数据的buf
    @param [in] len buf的空间
    @param [in] flags recvfrom的flag
    @param [out] from 消息的来源地址
    @param [out] fromlen 来源地址的长度
    @param [in] timeout timeout的值，若为NULL表示阻塞。
    @return -1：failure，包括超时；0：对端关闭；>0：读取的字节数量
    */
    static ssize_t recvfrom (CWX_HANDLE handle,
        char *buf,
        int len,
        int flags,
        struct sockaddr *from,
        socklen_t *fromlen,
        CwxTimeouter *timeout = NULL);

    /**
    @brief 从连接上接受数据。是对OS的readv接口的封装
    @param [in] handle 读的句柄。
    @param [in] iov 接收数据的空间的数组
    @param [in] iovcnt 数组的大小
    @param [in] timeout timeout的值，若为NULL表示阻塞。
    @return -1：failure，包括超时；0：对端关闭；>0：读取的字节数量
    */
    static ssize_t recvv (CWX_HANDLE handle,
        iovec *iov,
        int iovcnt,
        CwxTimeouter *timeout = NULL);

    /**
    @brief 往连接上发送数据。是对OS的write接口的封装
    @param [in] handle 写的句柄。
    @param [in] buf 数据的buf
    @param [in] len 数据的长度
    @param [in] timeout timeout的值，若为NULL表示阻塞。
    @return -1：failure，包括超时；否则，为发送的字节数量
    */
    static ssize_t write (CWX_HANDLE handle,
        void const *buf,
        size_t len,
        CwxTimeouter *timeout = NULL);

    /**
    @brief 往连接上发送数据。是对OS的send接口的封装
    @param [in] handle 写的句柄。
    @param [in] buf 数据的buf
    @param [in] len 数据的长度
    @param [in] flags os的send api的flag
    @param [in] timeout timeout的值，若为NULL表示阻塞。
    @return -1：failure，包括超时；否则，为发送的字节数量
    */
    static ssize_t send (CWX_HANDLE handle,
        void const *buf,
        size_t len,
        int flags,
        CwxTimeouter *timeout = NULL);
    /**
    @brief 往连接上发送数据。是对OS的write接口的封装
    @param [in] handle 写的句柄。
    @param [in] buf 发送数据的buf
    @param [in] len 发送数据的长度
    @param [in] timeout timeout的值，若为NULL表示阻塞。
    @return -1：failure，包括超时；否则为写字节的数量
    */
    static ssize_t send (CWX_HANDLE handle,
        void const *buf,
        size_t len,
        CwxTimeouter *timeout = NULL);
    /**
    @brief 发送len个字节。是对OS的write接口的封装
    @param [in] handle 写的句柄。
    @param [in] buf 数据的buf
    @param [in] len 数据的长度
    @param [in] timeout timeout的值，若为NULL表示阻塞。
    @param [out] sent 失败时发送的字节的数量。
    @return -1：failure，包括超时；否则为发送的字节数量
    */
    static ssize_t write_n (CWX_HANDLE handle,
        void const*buf,
        size_t len,
        CwxTimeouter *timeout = NULL,
        size_t* sent=NULL);
    /**
    @brief 发送len个字节。是对OS的send接口的封装
    @param [in] handle 写的句柄。
    @param [in] buf 数据的buf
    @param [in] len 数据的长度
    @param [in] flags os的send api的flag
    @param [in] timeout timeout的值，若为NULL表示阻塞。
    @param [out] sent 失败时发送的字节的数量。
    @return -1：failure，包括超时；否则为发送的字节数量
    */
    static ssize_t send_n (CWX_HANDLE handle,
        void const*buf,
        size_t len,
        int flags,
        CwxTimeouter *timeout = NULL,
        size_t* sent=NULL);

    /**
    @brief 发送len个字节。是对OS的write接口的封装
    @param [in] handle 写的句柄。
    @param [in] buf 数据的buf
    @param [in] len 数据的长度
    @param [in] timeout timeout的值，若为NULL表示阻塞。
    @param [out] accepted 失败时发送的字节的数量。
    @return -1：failure，包括超时；否则为发送的字节数量
    */
    static ssize_t send_n (CWX_HANDLE handle,
        void const*buf,
        size_t len,
        CwxTimeouter *timeout = NULL,
        size_t* accepted=NULL);

    /**
    @brief 往连接上发送一个msg，可为非面向连接的。是对OS的sendmsg接口的封装
    @param [in] handle 写句柄。
    @param [in] msg 数据的buf
    @param [in] flags sendmsg的flag
    @param [in] timeout timeout的值，若为NULL表示阻塞。
    @return -1：failure，包括超时；否则为发送的字节数量
    */
    static ssize_t sendmsg(CWX_HANDLE handle,
        struct msghdr const *msg,
        int flags,
        CwxTimeouter *timeout = NULL);

    /**
    @brief 往连接上发送数据，可为非面向连接的。是对OS的sendto接口的封装
    @param [in] handle 写句柄。
    @param [in] buf 数据的buf
    @param [in] len 数据的len
    @param [in] flags sendto的flag
    @param [in] to sendto的地址
    @param [in] tolen sendto的地址的长度
    @param [in] timeout timeout的值，若为NULL表示阻塞。
    @return -1：failure，包括超时；否则为发送的字节数量
    */
    static ssize_t sendto (CWX_HANDLE handle,
        char const *buf,
        int len,
        int flags,
        struct sockaddr *to,
        socklen_t tolen,
        CwxTimeouter *timeout = NULL);

    /**
    @brief 往连接上发送数据。是对OS的writev接口的封装
    @param [in] handle 写句柄。
    @param [in] iov 数据的buf数组
    @param [in] iovcnt 数组的大小
    @param [in] timeout timeout的值，若为NULL表示阻塞。
    @return -1：failure，包括超时；否则为发送的字节数量
    */
    static ssize_t sendv (CWX_HANDLE handle,
        iovec const*iov,
        int iovcnt,
        CwxTimeouter *timeout = NULL);


    /**
    @brief 监测一个io handle是否读、写、例外ready，
    @param [in] handle 监测的句柄。
    @param [in] timeout timeout的值，若为NULL表示阻塞。
    @param [in] bRead 是否监测读
    @param [in] bWrite 是否监测写
    @param [in] bException 是否监测例外
    @param [in] bRestart 若在等待过程中被信号中断，是否继续等待。true：继续等待；false：不等待。
    @return -1：failure；0：超时；>0：handle ready的事件数量
    */
    static int handleReady (CWX_HANDLE handle,
        CwxTimeouter  *timeout = NULL,
        bool bRead = true,
        bool bWrite = false,
        bool bException = false,
        bool bRestart = true);



    ///是否支持IPV6
    static bool isEnableIpv6(void);
    ///是否支持IPV4
    static bool isEnableIpv4(void);
    ///获取服务的端口号, -1失败。
    static int getSvrPort(char const* szSvrName, char const* szProtocol);
private:
    //forbiden new instance
    CwxSocket();
    //检查是否支持指定的协议族
    static int ipvn_check(int &ipvn_enabled, int pf);
private:
    static int  m_enableIpv6; ///<ipv6支持的标志, -1:未检查；0：不支持；1:支持
    static int  m_enableIpv4; ///<ipv4支持的标志, -1:未检查；0：不支持；1：支持
};

CWINUX_END_NAMESPACE

#include "CwxSocket.inl"
#include "CwxPost.h"

#endif
