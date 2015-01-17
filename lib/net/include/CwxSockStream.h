#ifndef __CWX_SOCK_STREAM_H__
#define __CWX_SOCK_STREAM_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxSockStream.h
@brief TCP流数据通信连接对象的定义。
@author cwinux@gmail.com
@version 0.1
@date 2009-10-10
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxType.h"
#include "CwxMsgHead.h"
#include "CwxMsgBlock.h"
#include "CwxNetMacro.h"
#include "CwxINetAddr.h"
#include "CwxSockIo.h"

CWINUX_BEGIN_NAMESPACE
/**
@class CwxSockStream
@brief 面向流的TCP连接对象。
*/
class CWX_API CwxSockStream:public CwxSockIo
{
public:
    ///默认构造函数
    CwxSockStream();
    ///构造函数，创建对象并设定连接handle
    CwxSockStream(CWX_HANDLE handle);
    ///析构函数.
    ~CwxSockStream(void);
public:
    /**
    @brief 接收len字节的数据。
    @param [out] buf 接收数据的buf。
    @param [in] len 接收数据的字节数。
    @param [in] flags recv()的系统OS api的flags参数。
    @param [in] timeout timeout的值，若为NULL表示阻塞。
    @param [out] bytes_transferred 若不为0，则返回实际已经接收的数据数量，即便在失败的情况下。
    @return -1：接收错误；0：连接关闭；>0：应该等于len，表示成功接收了len长度的字节流。
    */
    ssize_t recv_n (void *buf, size_t len, int flags, CwxTimeouter  *timeout = 0, size_t* bytes_transferred = 0) const;
    /**
    @brief 接收len字节的数据。
    @param [out] buf 接收数据的buf。
    @param [in] len 接收数据的字节数。
    @param [in] timeout timeout的值，若为NULL表示阻塞。
    @param [out] bytes_transferred 若不为0，则返回实际已经接收的数据数量，即便在失败的情况下。
    @return -1：接收错误；0：连接关闭；>0：应该等于len，表示成功接收了len长度的字节流。
    */
    ssize_t recv_n (void *buf, size_t len, CwxTimeouter  *timeout = 0, size_t *bytes_transferred = 0) const;

    /**
    @brief 接收len字节的数据。
    @param [out] buf 接收数据的buf。
    @param [in] len 接收数据的字节数。
    @param [in] timeout timeout的值，若为NULL表示阻塞。
    @param [out] bytes_transferred 若不为0，则返回实际已经接收的数据数量，即便在失败的情况下。
    @return -1：接收错误；0：连接关闭；>0：应该等于len，表示成功接收了len长度的字节流。
    */
    ssize_t read_n (void *buf, size_t len, CwxTimeouter  *timeout = 0, size_t *bytes_transferred = 0) const;

    /**
    @brief 一个cwinux的数据包。
    @param [out] head 数据包的包头。
    @param [out] msg 接收的数据包，若失败，则返回NULL。
    @param [in] timeout timeout的值，若为NULL表示阻塞。
    @return -1：接收错误；0：连接关闭；>0：应该等于len，表示成功接收了len长度的字节流。
    */
    ssize_t read (CwxMsgHead& head, CwxMsgBlock*& msg, CwxTimeouter  *timeout = 0) const;
    /**
    @brief 一个cwinux的数据包。
    @param [out] head 数据包的包头。
    @param [out] msg 接收的数据包，若失败，则返回NULL。
    @param [in] timeout timeout的值，若为NULL表示阻塞。
    @return -1：接收错误；0：连接关闭；>0：应该等于len，表示成功接收了len长度的字节流。
    */
    ssize_t recv (CwxMsgHead& head, CwxMsgBlock*& msg, CwxTimeouter  *timeout = 0) const;


    /**
    @brief 发送len字节的数据。
    @param [in] buf 发送数据的buf。
    @param [in] len 发送数据的字节数。
    @param [in] timeout timeout的值，若为NULL表示阻塞。
    @param [out] bytes_transferred 若不为0，则返回实际已经发送的数据数量，即便在失败的情况下。
    @return -1：发送错误；>0：应该等于len，表示成功发送了len长度的字节流。
    */
    ssize_t write_n (const void *buf, size_t len, CwxTimeouter  *timeout = 0, size_t *bytes_transferred = 0) const;

    /**
    @brief 发送len字节的数据。
    @param [in] buf 发送数据的buf。
    @param [in] len 发送数据的字节数。
    @param [in] flags send()的系统OS api的flags参数。
    @param [in] timeout timeout的值，若为NULL表示阻塞。
    @param [out] bytes_transferred 若不为0，则返回实际已经发送的数据数量，即便在失败的情况下。
    @return -1：发送错误；>0：应该等于len，表示成功发送了len长度的字节流。
    */
    ssize_t send_n (const void *buf, size_t len, int flags, CwxTimeouter  *timeout = 0, size_t *bytes_transferred = 0) const;

    /**
    @brief 发送len字节的数据。
    @param [in] buf 发送数据的buf。
    @param [in] len 发送数据的字节数。
    @param [in] timeout timeout的值，若为NULL表示阻塞。
    @param [out] bytes_transferred 若不为0，则返回实际已经发送的数据数量，即便在失败的情况下。
    @return -1：发送错误；>0：应该等于len，表示成功发送了len长度的字节流。
    */
    ssize_t send_n (const void *buf, size_t len, CwxTimeouter  *timeout = 0,  size_t *bytes_transferred = 0) const;
    /**
    @brief 发送urgent数据。
    @param [out] ptr 要发送的urgent数据。
    @param [in] len urgent数据的长度。
    @param [in] timeout timeout的值，若为NULL表示阻塞。
    @return -1：发送错误；>0：等于len。
    */
    ssize_t send_urg (const void *ptr, size_t len = sizeof (char), CwxTimeouter  *timeout = 0) const;
    /**
    @brief 接收urgent数据。
    @param [out] ptr 接收urgent数据的buf。
    @param [in] len 接收urgent数据的长度。
    @param [in] timeout timeout的值，若为NULL表示阻塞。
    @return -1：接收错误；0：连接关闭；>0：等于len。
    */
    ssize_t recv_urg (void *ptr, size_t len = sizeof (char), CwxTimeouter  *timeout = 0) const;
    /// 关闭连接的读。返回值，0：成功；-1：失败。
    int close_reader (void);
    /// 关闭连接写。返回值，0：成功；-1：失败。
    int close_writer (void);
private:
};

CWINUX_END_NAMESPACE

#include "CwxSockStream.inl"
#include "CwxPost.h"

#endif
