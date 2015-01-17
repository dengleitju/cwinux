#ifndef __CWX_APP_HANDLER_4_IO_EVENT_H__
#define __CWX_APP_HANDLER_4_IO_EVENT_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxAppHandler4IoEvent.h
@brief IO事件监测Handle的定义
@author cwinux@gmail.com
@version 0.1
@date 2009-07-20
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxType.h"
#include "CwxMutexLock.h"
#include "CwxLockGuard.h"
#include "CwxAppMacro.h"
#include "CwxAppConfig.h"
#include "CwxAppHandler4Base.h"
#include "CwxAppReactor.h"

CWINUX_BEGIN_NAMESPACE

class CwxAppFramework;

/**
@class CwxAppHandler4IoEvent
@brief 处理IO HANDLE事件监测。
*/
class CWX_API CwxAppHandler4IoEvent : public CwxAppHandler4Base
{
public:
    ///构造函数
    CwxAppHandler4IoEvent(CwxAppFramework* app,
        CwxAppReactor *reactor);
    ///析构函数
    ~CwxAppHandler4IoEvent();
public:
    /**
    @brief handler open。往reactor的注册。
    @param [in] arg  无效参数
    @return -1：失败； 0：成功
    */
    virtual int open (void * arg= 0);
    /**
    @brief 连接上有事件发生。
    @param [in] event 连接上的事件
    @param [in] handle  发生的事件的handle。
    @return -1：处理失败； 0：处理成功
    */
    virtual int handle_event(int event, CWX_HANDLE handle=CWX_INVALID_HANDLE);
    ///handle close
    virtual int close(CWX_HANDLE handle=CWX_INVALID_HANDLE);
public:
    ///get svr id
    inline CWX_UINT32 getSvrId() const
    {
        return m_uiSvrId;
    };
    ///set svr id
    inline void setSvrId(CWX_UINT32 uiSvrId)
    {
        m_uiSvrId = uiSvrId;
    }
    ///get host id
    inline CWX_UINT32 getHostId() const
    {
        return m_uiHostId;
    }
    ///set host id
    inline void setHostId(CWX_UINT32 uiHostId)
    {
        m_uiHostId = uiHostId;
    }
    ///get io event mask
    inline int getIoEventMask() const
    {
        return m_ioEventMask;
    }
    ///set io event mask
    inline void setIoEventMask(int mask)
    {
        m_ioEventMask = mask&CwxAppHandler4Base::RW_MASK;
    }
    ///get user data
    inline void* getUserData() const
    {
        return m_userData;
    }
    ///set user data
    inline void setUserData(void* userData)
    {
        m_userData = userData;
    }
    ///获取app
    CwxAppFramework* getApp()
    {
        return m_pApp;
    }

public:
    CwxAppHandler4IoEvent* m_next;
private:
    CwxAppFramework* m_pApp;
    CWX_UINT32   m_uiSvrId;///<Handle的svr id
    CWX_UINT32   m_uiHostId;///<Handle的host id
    CWX_UINT16   m_ioEventMask;///<Handle的事件监测掩码
    void*       m_userData;///<Handle的用户数据
};

CWINUX_END_NAMESPACE
#include "CwxPost.h"

#endif
