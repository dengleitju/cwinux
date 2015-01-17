#ifndef __CWX_SOCK_IO_H__
#define __CWX_SOCK_IO_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxSockIo.h
@brief CwxSockIo类的定义。
@author cwinux@gmail.com
@version 0.1
@date 2010-06-10
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxType.h"
#include "CwxTimeValue.h"
#include "CwxSockBase.h"
CWINUX_BEGIN_NAMESPACE

/**
* @class CwxSockIo 
*
* @brief  Defines the methods for the  socket wrapper I/O routines。
* (e.g., send/recv).
* 
*/
class CWX_API CwxSockIo : public CwxSockBase
{
public:

public:
    // = Initialization and termination methods.

    /// Constructor.
    CwxSockIo (void);

    /// Destructor.
    ~CwxSockIo (void);

    /**
    @brief 从连接上接受数据。是对OS的recv接口的封装
    @param [in] buf 接收数据的buf
    @param [in] n buf的长度
    @param [in] flags os的recv api的flag
    @param [in] timeout timeout的值，若为NULL表示阻塞。
    @return -1：failure，包括超时；0：对端关闭；>0：读取的字节数量
    */
    ssize_t recv (void *buf,
        size_t n,
        int flags,
        CwxTimeouter *timeout = 0) const;

    /**
    @brief 从连接上接受数据。是对OS的read接口的封装
    @param [in] buf 接收数据的buf
    @param [in] n buf的长度
    @param [in] timeout timeout的值，若为NULL表示阻塞。
    @return -1：failure，包括超时；0：对端关闭；>0：读取的字节数量
    */
    ssize_t recv (void *buf,
        size_t n,
        CwxTimeouter *timeout = 0) const;

    /**
    @brief 从连接上接受数据。是对OS的readv接口的封装
    @param [in] iov 接收数据的空间的数组
    @param [in] n 数组的大小
    @param [in] timeout timeout的值，若为NULL表示阻塞。
    @return -1：failure，包括超时；0：对端关闭；>0：读取的字节数量
    */
    ssize_t recvv (iovec iov[],
        int n,
        CwxTimeouter  *timeout = 0) const;
    /**
    @brief 最多接收len字节的数据。
    @param [out] buf 接收数据的buf。
    @param [in] len 接收数据的字节数。
    @param [in] timeout timeout的值，若为NULL表示阻塞。
    @return -1：failure，包括超时；0：对端关闭；>0：读取的字节数量
    */
    ssize_t read (void *buf, size_t len, CwxTimeouter  *timeout = 0) const;

    /**
    @brief 最多发送len字节的数据。
    @param [in] buf 发送数据的buf。
    @param [in] len 发送数据的字节数。
    @param [in] timeout timeout的值，若为NULL表示阻塞。
    @return -1：failure，包括超时；否则，为发送的字节数量
    */
    ssize_t write (const void *buf, size_t len, CwxTimeouter  *timeout = 0) const;

    /**
    @brief 往连接上发送数据。是对OS的send接口的封装
    @param [in] buf 数据的buf
    @param [in] n 数据的长度
    @param [in] flags os的send api的flag
    @param [in] timeout timeout的值，若为NULL表示阻塞。
    @return -1：failure，包括超时；否则，为发送的字节数量
    */
    ssize_t send (const void *buf,
        size_t n,
        int flags,
        CwxTimeouter  *timeout = 0) const;

    /**
    @brief 往连接上发送数据。是对OS的write接口的封装
    @param [in] buf 发送数据的buf
    @param [in] n 发送数据的长度
    @param [in] timeout timeout的值，若为NULL表示阻塞。
    @return -1：failure，包括超时；否则为写字节的数量
    */
    ssize_t send (const void *buf,
        size_t n,
        CwxTimeouter  *timeout = 0) const;

    /**
    @brief 往连接上发送数据。是对OS的writev接口的封装
    @param [in] iov 数据的buf数组
    @param [in] n 数组的大小
    @param [in] timeout timeout的值，若为NULL表示阻塞。
    @return -1：failure，包括超时；否则为发送的字节数量
    */
    ssize_t sendv (const iovec iov[],
        int n,
        CwxTimeouter  *timeout = 0) const;
};


CWINUX_END_NAMESPACE

#include "CwxSockIo.inl"
#include "CwxPost.h"

#endif
