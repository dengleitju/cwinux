#ifndef __CWX_APP_HANDLER_CACHE_H__
#define __CWX_APP_HANDLER_CACHE_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxAppHandlerCache.h
@brief 关闭的TCP/Unix-domain的连接句柄的CACHE对象，实现句柄的重用
@author cwinux@gmail.com
@version 0.1
@date 2009-07-20
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxAppConfig.h"
#include "CwxAppMacro.h"
#include "CwxAppHandler4TcpConn.h"
#include "CwxAppHandler4UnixConn.h"
#include "CwxAppHandler4IoMsg.h"
#include "CwxAppHandler4IoEvent.h"
CWINUX_BEGIN_NAMESPACE

/**
@class CwxAppHandlerCache
@brief 关闭的TCP/Unix-domain的连接对象的CACHE，实现连接对象的重用
*/
class CWX_API CwxAppHandlerCache
{
public:
    enum{
        MAX_FREE_HANDLE_NUM = 64///<cache的关闭连接对象的数量
    };
public:
    ///构造函数
    CwxAppHandlerCache();
    ///析构函数
    ~CwxAppHandlerCache();
public:
    ///获取一个TCP类型的连接对象，返回NULL表示没有CACHE的连接对象
    inline CwxAppHandler4TcpConn* fetchTcpHandle();
    ///Cache一个关闭的TCP类型的连接对象
    inline void cacheTcpHandle(CwxAppHandler4TcpConn* pHandle);
    ///获取一个UNIX-domain类型的连接对象，返回NULL表示没有CACHE的连接对象
    inline CwxAppHandler4UnixConn* fetchUnixHandle();
    ///Cache一个关闭的UNIX-domain类型的连接对象
    inline void cacheUnixHandle(CwxAppHandler4UnixConn* pHandle);
    ///获取一个IO MSG类型的连接对象，返回NULL表示没有CACHE的连接对象
    inline CwxAppHandler4IoMsg* fetchIoMsgHandle();
    ///Cache一个关闭的Io msg类型的连接对象
    inline void cacheIoMsgHandle(CwxAppHandler4IoMsg* pHandle);
    ///获取一个io event handler类型的连接对象，返回NULL表示没有CACHE的连接对象
    inline CwxAppHandler4IoEvent* fetchIoEventHandle();
    ///Cache一个关闭的io event handler类型的连接对象
    inline void cacheIoEventHandle(CwxAppHandler4IoEvent* pHandle);
private:
    ///释放所有cache的连接对象
    void destroy();
private:
    CwxMutexLock                 m_lock;///<Cache的锁
    CWX_UINT32                   m_uiFreeTcpHandleNum;///<cache的TCP连接对象数量
    CWX_UINT32                   m_uiFreeUnixHandleNum;///<cache的UNIX-DOMAIN连接对象数量
    CWX_UINT32                   m_uiFreeIoMsgHandleNum; ///<cache msg handle连接对象数量
    CWX_UINT32                   m_uiFreeIoEventHandleNum; ///<cache io event连接对象数量
    CwxAppHandler4TcpConn*        m_freeTcpHandles;///<cache的TCP连接对象的链表
    CwxAppHandler4UnixConn*       m_freeUnixHandles;///<cache的UNIX-DOMAIN连接对象链表
    CwxAppHandler4IoMsg*          m_freeIoMsgHandles; ///<cache msg handle连接对象的链表
    CwxAppHandler4IoEvent*        m_freeIoEventHandles;///<cache的Io event连接对象的链表
};

CWINUX_END_NAMESPACE

#include "CwxAppHandlerCache.inl"

#include "CwxPost.h"

#endif
