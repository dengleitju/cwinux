#ifndef __CWX_APP_HANDLER_4_UNIX_CONN_H__
#define __CWX_APP_HANDLER_4_UNIX_CONN_H__

/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxAppHandler4UnixConn.h
@brief 定义UNIX-DOMAIN连接通信的Handle对象：CwxAppHandler4UnixConn
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
#include "CwxAppConfig.h"
#include "CwxAppMacro.h"
#include "CwxLogger.h"
#include "CwxAppHandler4Msg.h"
#include "CwxAppReactor.h"

CWINUX_BEGIN_NAMESPACE

class CwxAppFramework;
class CwxAppHandlerCache;
/**
@class CwxAppHandler4UnixConn
@brief UNIX-DOMAIN连接的Handle对象，负责UNIX-DOMAIN连接的对象
*/
class CWX_API CwxAppHandler4UnixConn:public CwxAppHandler4Msg
{
public:
    ///构造函数
    CwxAppHandler4UnixConn(CwxAppFramework* pApp, CwxAppReactor *reactor);
    ///析构函数
    ~CwxAppHandler4UnixConn();
public:
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
    void setConnectPathFile(char const* szPathFile);
    string const& getConnectPathFile() const;

public:
    CwxAppHandler4UnixConn*    m_next; ///<next connection
private:
    string                m_strConnectPathFile;///path file for connect
};
CWINUX_END_NAMESPACE

#include "CwxAppHandler4UnixConn.inl"
#include "CwxPost.h"

#endif
