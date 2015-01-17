#ifndef __CWX_SOCK_ACCEPTOR_H__
#define __CWX_SOCK_ACCEPTOR_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/
/**
@file CwxSockAcceptor.h
@brief TCP被动连接接收对象的定义。
@author cwinux@gmail.com
@version 0.1
@date 2009-10-11
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxType.h"
#include "CwxErrGuard.h"
#include "CwxNetMacro.h"
#include "CwxINetAddr.h"

#include "CwxSockStream.h"

CWINUX_BEGIN_NAMESPACE
/**
@class CwxSockAcceptor
@brief TCP被动连接接收对象。
*/
class CWX_API CwxSockAcceptor:public CwxSockBase
{
public:
    enum{
        DEF_BACK_LOG = 64
    };
    ///默认构造函数
    CwxSockAcceptor();
    ///打开监听端口的构造函数
    CwxSockAcceptor(CwxAddr const& addr,
        bool reuse= 0,
        int backlog = DEF_BACK_LOG,
        int domain = AF_INET,
        int protocol = 0,
        CWX_NET_SOCKET_ATTR_FUNC fn=NULL,
        void* fnArg = NULL);
    ///析构函数.
    ~CwxSockAcceptor(void);
public:
    /**
    @brief 建立TCP主动连接。
    @param [in] addr Accept的本地地址。
    @param [in] reuse 当监听地址没被释放的话，是否re-use。true：是；false：不是。
    @param [in] backlog listen最大排队的数量。
    @param [in] domain 协议族，就是socket()中的domain，PF_UNSPEC表示采用addr中的协议族。
    @param [in] protocol 连接的协议，就是socket()的protocol参数，由于一个协议家族的socket 类型，其protocol也是唯一的，为0就可以了。
    @param [in] fn socket 设置的function。
    @return -1：错误，errno记录错误的原因；0：成功。
    */
    int listen(CwxAddr const& addr,
        bool reuse= 0,
        int backlog = DEF_BACK_LOG,
        int domain = AF_INET,
        int protocol = 0,
        CWX_NET_SOCKET_ATTR_FUNC fn=NULL,
        void* fnArg=NULL);
    /**
    @brief 接收一个被动TCP连接。
    @param [in] stream 返回接收到的被动连接。
    @param [in] remote_addr 若不为空，则返回接收到的对端host的地址。
    @param [in] timeout accept的超时时间，若为NULL，则无限等待。
    @param [in] bRestart 若在等待过程中被信号中断，是否继续等待。true：继续等待；false：不等待。
    @return -1：错误，errno记录错误的原因；0：成功。
    */
    int accept (CwxSockStream &stream,
        CwxAddr* remote_addr = 0,
        CwxTimeouter* timeout = NULL,
        bool bRestart = true) const;
public:
    int close (void);
private:
    int getRemoteAddr(CwxAddr &) const;
};

CWINUX_END_NAMESPACE

#include "CwxSockAcceptor.inl"
#include "CwxPost.h"

#endif
