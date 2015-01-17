#ifndef __CWX_SOCK_DGRAM_H__
#define __CWX_SOCK_DGRAM_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxSockDgram.h
@brief UDP包数据通信对象的定义。
@author cwinux@gmail.com
@version 0.1
@date 2009-10-11
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxType.h"
#include "CwxTimeValue.h"
#include "CwxNetMacro.h"
#include "CwxINetAddr.h"
#include "CwxSockBase.h"

CWINUX_BEGIN_NAMESPACE
/**
@class CwxSockDgram
@brief 面向包的UDP通信对象。
*/
class CWX_API CwxSockDgram : public CwxSockBase
{
public:
    ///默认构造函数
    CwxSockDgram();
    ///构造函数，创建对象并设定连接handle
    CwxSockDgram(CWX_HANDLE handle);
    ///析构函数.
    ~CwxSockDgram(void);
public:
    /**
    @brief 在限定的时间内，发送len字节的数据。
    @param [in] buf 发送数据的buf。
    @param [in] len 发送数据的字节数。
    @param [in] addr 发送的地址。
    @param [in] flags sendto()的系统OS api的flags参数。
    @param [in] timeout timeout的值，若为NULL表示阻塞。
    @return -1：发送错误或超时(errno==ETIME）；否则为发送的字节数，等于len
    */
    ssize_t send(const void *buf,  size_t len,  const CwxAddr &addr, int flags, CwxTimeouter  *timeout=0) const;
    /**
    @brief 在限定的时间内，接收一个数据包。
    @param [in] buf 接收数据的buf。
    @param [in] len 接收BUF的大小。
    @param [in] addr 数据的来源的地址。
    @param [in] flags recvfrom()的系统OS api的flags参数。
    @param [in] timeout timeout的值，若为NULL表示阻塞。
    @return -1：接收错误或超时(errno==ETIME）；否则为接收的字节数
    */
    ssize_t recv (void *buf,  size_t len,  CwxAddr &addr, int flags, CwxTimeouter  *timeout=0) const;
private:
    /**
    * Return the address of the remotely connected peer (if there is
    * one), in the referenced CwxAddr. Returns 0 if successful, else
    * -1.
    */
    int getRemoteAddr (CwxAddr &) const;
};

CWINUX_END_NAMESPACE

#include "CwxSockDgram.inl"
#include "CwxPost.h"

#endif
