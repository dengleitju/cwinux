#ifndef __CWX_SINGLETON_MGR_H__
#define __CWX_SINGLETON_MGR_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxSingletonMgr.h
@brief 单实例对象的管理类的定义，用于管理那些需要释放的单实例对象。
@author cwinux@gmail.com
@version 0.1
@date 2009-10-02
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxType.h"
#include "CwxLockGuard.h"
#include "CwxMutexLock.h"
#include "CwxSingleton.h"

CWINUX_BEGIN_NAMESPACE
/**
@class CwxSingletonMgr
@brief 单实例对象的管理类，用于管理那些需要释放的单实例对象。
       CwxSingletonMgr同样也是singleton对象。
*/

class CWX_API CwxSingletonMgr
{
public:
    ///获取CwxSingletonMgr对象的实例，若没有创建则创建
    static CwxSingletonMgr* instance();
    ///释放CwxSignletonMgr的Singleton对象。此API被注册在os的atexit。
    static void destroy();
public:
    ///注册需要释放的singleton对象
    void  reg(CwxSingleton* pObj);
private:
    ///构造函数，保证只能有instance() API创建。
    CwxSingletonMgr();
    ///析构函数，保证只能有destroy() API释放创建。
    ~CwxSingletonMgr();
private:
    static CwxSingletonMgr*    m_pInstance;///<Mgr的singleton对象
    static CwxMutexLock        m_lock;///<singleton对象的保护锁
    CwxSingleton*      m_pObjs;///<注册的singleton对象的链表
};

CWINUX_END_NAMESPACE


#include "CwxPost.h"

#endif
