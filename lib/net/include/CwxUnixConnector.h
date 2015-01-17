#ifndef __CWX_UNIX_CONNECTOR_H__
#define __CWX_UNIX_CONNECTOR_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxUnixConnector.h
@brief Unix domain主动连接的连接器对象的定义。
@author cwinux@gmail.com
@version 0.1
@date 2009-10-12
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxType.h"
#include "CwxNetMacro.h"
#include "CwxUnixAddr.h"
#include "CwxUnixStream.h"
#include "CwxSockConnector.h"

CWINUX_BEGIN_NAMESPACE
/**
@class CwxUnixConnector
@brief TCP主动连接的连接器对象。
*/
class CWX_API CwxUnixConnector:CwxSockConnector
{
public:
    ///默认构造函数
    CwxUnixConnector();
    ///析构函数.
    ~CwxUnixConnector(void);
public:
    /**
    @brief 建立TCP主动连接。
    @param [out] stream 主动连接。
    @param [in] remoteAddr 连接的远程地址。
    @param [in] localAddr 连接的本地地址，若为空，则有connect自己分配。
    @param [in] timeout 连接的超时时间，若NULL，表示没有超时限制。若值为0，则表示若没有连接上，立即返回。
    @param [in] protocol 连接的协议，就是socket()的protocol参数，由于一个协议家族的socket 类型，其protocol也是唯一的，为0就可以了。
    @param [in] reuse_addr 是否重用本地地址。
    @param [in] fn socket属性设置的function。
    @return -1：错误，errno记录错误的原因；0：成功。
    */
    int connect (CwxUnixStream& stream,
        CwxAddr const& remoteAddr,
        CwxAddr const& localAddr=CwxAddr::sap_any,
        CwxTimeouter* timeout=0,
        int protocol = 0,
        bool reuse_addr = false,
        CWX_NET_SOCKET_ATTR_FUNC fn=NULL, ///<socket设置的function
        void* fnArg = NULL
        );
private:
};

CWINUX_END_NAMESPACE

#include "CwxUnixConnector.inl"
#include "CwxPost.h"

#endif
