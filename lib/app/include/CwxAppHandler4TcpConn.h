#ifndef __CWX_APP_HANDLER_4_TCP_CONN_H__
#define __CWX_APP_HANDLER_4_TCP_CONN_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxAppHandler4TcpConn.h
@brief 定义TCP，PIPE、及其他IO通信、管理的Handle对象：CwxAppHandler4TcpConn
@author cwinux@gmail.com
@version 0.1
@date 2009-07-20
@warning
@bug
*/

#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxMsgBlock.h"
#include "CwxMsgHead.h"
#include "CwxINetAddr.h"
#include "CwxAppConfig.h"
#include "CwxAppMacro.h"
#include "CwxLogger.h"
#include "CwxAppHandler4Base.h"
#include "CwxAppReactor.h"
#include "CwxAppHandler4Msg.h"

CWINUX_BEGIN_NAMESPACE
/**
@class CwxAppHandler4TcpConn
@brief TCP，PIPE、及其他IO通信、状态管理的Handle对象
*/

class CwxAppFramework;
class CwxAppHandlerCache;
class CwxAppTcpAcceptor;

class CWX_API CwxAppHandler4TcpConn:public CwxAppHandler4Msg
{
public:
    ///构造函数
    CwxAppHandler4TcpConn(CwxAppFramework* pApp, CwxAppReactor *reactor);
    ///析构函数
    ~CwxAppHandler4TcpConn();
public:
    /**
    @brief 初始化建立的连接，并往Reactor注册连接
    @param [in] arg 建立连接的acceptor或为NULL
    @return -1：放弃建立的连接； 0：连接建立成功
    */
    virtual int open (void * arg= 0);
    ///handle close
    virtual int close(CWX_HANDLE handle=CWX_INVALID_HANDLE);
    ///超时
    virtual void handle_timeout();
    /**
    @brief 获取连接的对端地址，只对STREAM_TYPE_TCP和STREAM_TYPE_UNIX有效
    @param [in,out] szBuf 返回地址的buf,获取成功后以\\0结束。
    @param [in] unSize szBuf的大小。
    @return 返回szBuf
    */
    virtual char* getRemoteAddr(char* szBuf, CWX_UINT16 unSize);
    /**
    @brief 获取连接的对端port，只对STREAM_TYPE_TCP有效
    @return 连接对端的port
    */
    virtual CWX_UINT16 getRemotePort();
    /**
    @brief 获取连接本端的地址，只对STREAM_TYPE_TCP和STREAM_TYPE_UNIX有效
    @param [in,out] szBuf 返回地址的buf,获取成功后以\\0结束。
    @param [in] unSize szBuf的大小。
    @return 返回szBuf
    */
    virtual char* getLocalAddr(char* szBuf, CWX_UINT16 unSize);
    /**
    @brief 获取连接的本端port，只对STREAM_TYPE_TCP有效
    @return 连接对端的port
    */
    virtual inline CWX_UINT16 getLocalPort();
public:
    ///设置主动连接的连接地址
    inline void setConnectAddr(char const* szAddr);
    ///获取主动连接的连接地址
    inline string const& getConnectAddr() const;
    ///设置主动连接的连接端口
    inline void setConnectPort(CWX_UINT16 unPort);
    ///获取主动连接的连接端口
    inline CWX_UINT16 getConnectPort() const;
public:
    CwxAppHandler4TcpConn*    m_next; ///<next connection
private:
    string                  m_strConnectAddr;///<ip addr
    CWX_UINT16               m_unConnectPort;//</port for connect
    CwxINetAddr              m_remoteAddr;
};
CWINUX_END_NAMESPACE

#include "CwxAppHandler4TcpConn.inl"

#include "CwxPost.h"

#endif
