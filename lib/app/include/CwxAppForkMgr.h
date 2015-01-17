#ifndef __CWX_APP_FORK_MGR_H__
#define __CWX_APP_FORK_MGR_H__

/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxAppForkMgr.h
@brief 异步fork管理对象的定义
@author cwinux@gmail.com
@version 0.1
@date 2009-11-25
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxStl.h"
#include "CwxLockGuard.h"
#include "CwxMutexLock.h"
#include "CwxAppConfig.h"
#include "CwxAppMacro.h"

CWINUX_BEGIN_NAMESPACE

class CwxAppFramework;
/**
@class CwxAppForkEnv
@brief 异步fork的消息对象，并定义了child进程的处理过程。在fork的时候，
       用户需要重载此类，完成数据的传递与child 进行的处理控制
*/
class CWX_API CwxAppForkEnv
{
public:
    ///构造函数
    CwxAppForkEnv()
    {
    }
    ///析构函数
    virtual ~CwxAppForkEnv()
    {
    }
public:
    /**
    @brief 通知上层应用在此点执行fork操作。
           用户在此API内，实现os的fork的操作，控制fork相关的操作。
    @return   对于父进程，成功则返回子进程的pid，失败的时候返回-1，失败时子进程没有创建。
    对于child进程，则返回0。
    */
    virtual int onFork()=0;
    /**
    @brief child进程的入口，完成从父进程clone的pApp对象中提取系统资源，如连接等。
           此API默认什么也不做
    @param [in] pApp 从父进程clone过来的app对象，离开此api后，pApp将被释放。
    @return void
    */
    virtual void onChildEntry(CwxAppFramework* pApp)
    {
        CWX_UNUSED_ARG(pApp);
    }
    /**
    @brief child进程的主体，完成child的工作。此API为纯虚函数，应用必须重载实现。
    @return void
    */
    virtual void onChildMain()=0;
};


/**
@class CwxAppForkMgr
@brief 异步fork的消息管理对象。
*/
class CWX_API CwxAppForkMgr
{
public:
    ///构造函数
    CwxAppForkMgr()
    {
        m_bEvent = false;
    }
    ///析构函数
    ~CwxAppForkMgr()
    {
        list<CwxAppForkEnv*>::iterator iter = m_forks.begin();
        while(iter != m_forks.end())
        {
            delete *iter;
            iter++;
        }
        m_forks.clear();
    }
public:
    ///添加一个fork的事件
    inline void append(CwxAppForkEnv* pForkEnv)
    {
        CwxMutexGuard<CwxMutexLock> lock(&m_lock);
        if (pForkEnv)
        {
            m_forks.push_back(pForkEnv);
            m_bEvent = true;
        }
    }
    ///获取所有的fork事件
    inline void getForkEnvs(list<CwxAppForkEnv*>& forks)
    {
        CwxMutexGuard<CwxMutexLock> lock(&m_lock);
        forks = m_forks;
        m_bEvent = false;
        m_forks.clear();
    }
    ///获取是否存在fork的事件，由于只是判断标记，因此为了效率不加锁
    inline bool isForkEvent() const
    {
        return m_bEvent;
    }
private:
    CwxMutexLock        m_lock;///<管理器的锁
    list<CwxAppForkEnv*> m_forks; ///<fork的事件
    bool                m_bEvent; ///<是否有fork事件，只是一个标志，不进行保护
};

CWINUX_END_NAMESPACE
#include "CwxPost.h"

#endif
