#ifndef __CWX_APP_LISTEN_MGR_H__
#define __CWX_APP_LISTEN_MGR_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/


/**
@file CwxAppListenMgr.h
@brief 架构的LISTEN的管理
@author cwinux@gmail.com
@version 0.1
@date 2009-07-20
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxLockGuard.h"
#include "CwxMutexLock.h"
#include "CwxAppConfig.h"
#include "CwxAppMacro.h"
#include "CwxLogger.h"
#include "CwxAppUnixAcceptor.h"
#include "CwxAppTcpAcceptor.h"

CWINUX_BEGIN_NAMESPACE
/**
@class CwxAppListenObj
@brief 实现TCP/UNIX-DOMAIN的Accept对象的统一封装。
*/
class CWX_API CwxAppListenObj
{
public:
    enum{
        TCP_LISTEN = 1,///<Tcp的Listen
        UNIX_LISTEN = 2///<Unix-domain的Listen
    };
    ///默认构造函数
    CwxAppListenObj()
    {
        m_unListenType = TCP_LISTEN;
        m_listenObject = NULL;
    }
public:
    CWX_UINT16  m_unListenType;///<listen对象的类型
    void*      m_listenObject;///<listen对象的地址指针
};

///定义Listen对象的MAP
typedef map<CWX_UINT32, CwxAppListenObj> CwxAppIdListenMap;

/**
@class CwxAppListenMgr
@brief Listen对象的管理器对象的定义。
*/
class CWX_API CwxAppListenMgr
{
public:
    ///listen id范围定义
    enum{
        MIN_LISTEN_ID = 1,///<最小的Listen ID
        MAX_LISTEN_ID = 0x3FFFFFFF///<最大的Listen id
    };
public:
    ///构造函数
    CwxAppListenMgr(CwxAppFramework* pApp);
    ///析构函数
    ~CwxAppListenMgr();
public:
    ///往管理器添加一个TCP的Listen对象，返回值：false，Listen-id的Listen对象已经存在
    bool add(CwxAppTcpAcceptor* acceptor);
    ///往管理器添加一个UNIX-DOMAIN的Listen对象，返回值：false，Listen-id的Listen对象已经存在
    bool add(CwxAppUnixAcceptor* acceptor);
    ///根据listen-id关闭一个Listen对象。返回值：false,listen id的监听对象不存在
    bool close(CWX_UINT32 uiListenId);
    ///resume一个listen对象的监听。返回值：false,listen id的监听对象不存在
    bool resume(CWX_UINT32 uiListenId);
    ///suspend一个listen对象的监听。返回值：false,listen id的监听对象不存在
    bool suspend(CWX_UINT32 uiListenId);
    ///获取下一个可用的Listen-id
    CWX_UINT32 getNextListenId();
    ///获取listen的id
    void getAllListens(vector<CWX_UINT32>& listens);
private:
    CwxMutexLock        m_lock;///<管理器的锁
    CWX_UINT32          m_uiListenId;///<从此listen-id开始遍历下一个可用ID
    CwxAppIdListenMap*  m_pListenMap;///<listen的map
    CwxAppFramework*    m_pApp;
};

CWINUX_END_NAMESPACE
#include "CwxPost.h"

#endif
