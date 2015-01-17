#ifndef __CWX_APP_CHANNEL_H__
#define __CWX_APP_CHANNEL_H__

/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxAppChannel.h
@brief 事件channel对象
@author cwinux@gmail.com
@version 1.0
@date 2011-04-15
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxTimeValue.h"
#include "CwxRwLock.h"
#include "CwxMutexLock.h"
#include "CwxLockGuard.h"
#include "CwxAppConfig.h"
#include "CwxAppMacro.h"
#include "CwxAppHandler4Base.h"
#include "CwxAppNoticePipe.h"
#include "CwxThread.h"
#include "CwxLogger.h"
#include "CwxAppEpoll.h"
#include "CwxAppHandler4Channel.h"
#include "CwxStl.h"

CWINUX_BEGIN_NAMESPACE


/**
@class CwxAppChannel
@brief 架构的Channel对象，实现IO事件的循环
*/

class CWX_API CwxAppChannel
{
public:
    typedef int (*CHANNEL_EVENT_HOOK)(void *);
    ///构造函数
    CwxAppChannel();
    ///析构函数
    ~CwxAppChannel();
public:
    /**
    @brief 打开reactor，调用线程为reactor的owner。
    @param [in] noticeHandler notice handler
    @return -1：失败；0：正常退出
    */
    int open();
    /**
    @brief 关闭reactor，必须为owner thread。会释放所有的handler
    @return -1：失败；0：正常退出
    */
    int close();
    /**
    @brief 架构事件的循环处理API，实现消息的分发，调用线程为channel的owner。
    @param [in] uiMiliTimeout 超时的毫秒数，0表示一直阻塞到事件发生。
    @return -1：失败；0：正常退出
    */
    int dispatch(CWX_UINT32 uiMiliTimeout=0);
    /**
    @brief 停止架构事件的循环处理，多线程安全，任意线程都可以调用。
    @return -1：失败；0：正常退出
    */
    int stop();
    /**
    @brief 注册IO事件处理handle，多线程安全，任意线程都可以调用。
    @param [in] io_handle 监测的IO handle
    @param [in] event_handler io handle对应的event handler。
    @param [in] mask 注册的事件掩码，为READ_MASK、WRITE_MASK、PERSIST_MASK组合
    @param [in] uiMillSecond 超时毫秒数，0表示不进行超时检测。
    @return -1：失败；
    0：成功；
    */
    int registerHandler (CWX_HANDLE io_handle,
        CwxAppHandler4Channel *CwxAppHandler4Channel,
        int mask,
        CWX_UINT32 uiMillSecond = 0);
    /**
    @brief 删除io事件处理handle。多线程安全，任意线程都可以调用。
    @param [in] event_handler 移除的event-handler
    @return -1：失败；
    0：成功；
    */
    int removeHandler (CwxAppHandler4Channel *event_handler);
    /**
    @brief suspend io事件处理handle。多线程安全，任意线程都可以调用。
    @param [in] event_handler suspend的event-handler
    @param [in] suspend_mask suspend的事件,为READ_MASK、WRITE_MASK组合
    @return -1：失败；
    0：成功；
    */
    int suspendHandler (CwxAppHandler4Channel *event_handler,
        int suspend_mask);
    /**
    @brief resume io事件处理handle。多线程安全，任意线程都可以调用。
    @param [in] event_handler resume的event-handler
    @param [in] resume_mask resume的事件,为READ_MASK、WRITE_MASK组合
    @return -1：失败；
    0：成功；
    */
    int resumeHandler (CwxAppHandler4Channel *event_handler,
        int resume_mask);

    /**
    @brief 删除io事件处理handle。多线程安全，任意线程都可以调用。
    @param [in] handle 移除的 io handle
    @return NULL：不存在；否则：成功；
    */
    CwxAppHandler4Channel* removeHandler (CWX_HANDLE handle);
    /**
    @brief suspend io事件处理handle。多线程安全，任意线程都可以调用。
    @param [in] handle suspend io handle
    @param [in] suspend_mask suspend的事件,为READ_MASK、WRITE_MASK组合
    @return -1：失败；0：成功
    */
    int suspendHandler (CWX_HANDLE handle,
        int suspend_mask);
    /**
    @brief resume io事件处理handle。多线程安全，任意线程都可以调用。
    @param [in] handle resume io handle
    @param [in] resume_mask resume的事件,为READ_MASK、WRITE_MASK组合
    @return -1：失败；
    0：成功；
    */
    int resumeHandler (CWX_HANDLE handle,
        int resume_mask);
    /**
    @brief 设置定时处理handle，timeout不具有persist特性。多线程安全，任意线程都可以调用。
    @param [in] event_handler timer的event handler
    @param [in] interval 定时的间隔
    @return -1：失败；
    0：成功；
    */
    int scheduleTimer (CwxAppHandler4Channel *event_handler,
        CwxTimeValue const &interval);
    ///取消定时处理handle。多线程安全，任意线程都可以调用。
    int cancelTimer (CwxAppHandler4Channel *event_handler);
    /**
    @brief 注册redo handler，redo的handler在dispatch会被执行一次并移除。多线程安全，任意线程都可以调用。
    @param [in] event_handler redo的event handler
    @return true：成功；false：存在；
    */
    bool regRedoHander(CwxAppHandler4Channel* event_handler);
    /**
    @brief 删除redo handler。多线程安全，任意线程都可以调用。
    @param [in] event_handler redoe的event handler
    @return true：成功；false：不存在；
    */
    bool eraseRedoHander(CwxAppHandler4Channel* event_handler);
    /**
    @brief notice reactor。多线程安全，任意线程都可以调用。
    @return -1：失败；  0：成功；
    */
    int notice();
public:
    /**
    */
public:
    /**
    @brief 检查指定IO的handle是否已经注册。多线程安全，任意线程都可以调用。
    @return true：注册；false：没有注册
    */
    bool isRegIoHandle(CWX_HANDLE handle);
    ///Return the ID of the "owner" thread.
    pthread_t getOwner() const;
    ///是否stop
    bool isStop();
    ///获取当前的时间
    CwxTimeValue const& getCurTime() const;
    ///获取当前的cache时间
    ///io handle是否设置指定的mask
    bool isMask(CWX_HANDLE handle, int mask);
private:
    ///注册IO事件处理handle
    int _registerHandler (CWX_HANDLE io_handle,
        CwxAppHandler4Channel *event_handler,
        int mask,
        CWX_UINT32 uiMillSecond = 0);
    ///删除io事件处理handle
    int _removeHandler (CwxAppHandler4Channel *event_handler);
    ///注册IO事件处理handle
    int _suspendHandler (CwxAppHandler4Channel *event_handler,
        int suspend_mask);
    ///删除io事件处理handle
    int _resumeHandler (CwxAppHandler4Channel *event_handler,
        int resume_mask);
    ///删除io事件处理handle。
    CwxAppHandler4Channel* _removeHandler (CWX_HANDLE handle);
    ///suspend io事件处理handle。
    int _suspendHandler (CWX_HANDLE handle,
        int suspend_mask);
    ///resume io事件处理handle。
    int _resumeHandler (CWX_HANDLE handle,
        int resume_mask);
    ///设置定时处理handle
    int _scheduleTimer (CwxAppHandler4Channel *event_handler,
        CwxTimeValue const &interval );
    ///取消定时处理handle
    int _cancelTimer (CwxAppHandler4Channel *event_handler);
    ///添加redo handler
    bool _regRedoHander(CwxAppHandler4Channel* event_handler);
    ///删除redo handler。
    bool _eraseRedoHander(CwxAppHandler4Channel* event_handler);

    /**
    @brief 停止架构事件的循环处理。
    @return -1：失败；0：正常退出
    */
    int _stop();
    /**
    @brief 检查指定IO的handle是否已经注册。
    @return true：注册；false：没有注册
    */
    bool _isRegIoHandle(CWX_HANDLE handle);
    /**
    @brief 检查指定IO的handle是否已经注册。
    @return true：注册；false：没有注册
    */
    bool _isRegIoHandle(CwxAppHandler4Channel* handler);
    /**
    @brief 获取指定handle对应的event handler。
    @return 返回handle对应的event handler；NULL表示不存在
    */
    CwxAppHandler4Channel* _getIoHandler(CWX_HANDLE handle);
    static void callback(CwxAppHandler4Base* handler, int mask, bool bPersist, void *arg);
private:
    class NoticeHanlder:public CwxAppHandler4Base
    {
    public:
        NoticeHanlder():CwxAppHandler4Base(NULL)
        {
        }
        ~NoticeHanlder(){}
    public:
        virtual int open (void *){ return 0;}
        virtual int handle_event(int , CWX_HANDLE handle)
        {
            char sigBuf[64];
            read(handle, sigBuf, 63);
            return 0;
        }
        virtual int close(CWX_HANDLE ){return 0;}
    };

private:
    CwxMutexLock            m_lock; ///<全局锁
    CwxRwLock               m_rwLock; ///<读写锁
    pthread_t               m_owner; ///<reactor的owner 线程
    set<CwxAppHandler4Channel*>* m_pCurRedoSet;
    set<CwxAppHandler4Channel*>  m_redoHandlers_1; ///<此set中的handler，每次dispatch会执行一次,并移除。
    set<CwxAppHandler4Channel*>  m_redoHandlers_2; ///<此set中的handler，每次dispatch会执行一次,并移除。
    bool                    m_bStop; ///<reactor是否已经停止
    int                     m_noticeFd[2]; ///<notice的读写handle
    CwxAppEpoll*            m_engine; ///<epoll的engine
    NoticeHanlder*          m_pNoticeHandler; ///<notice的handler
};


CWINUX_END_NAMESPACE
#include "CwxAppChannel.inl"
#include "CwxPost.h"

#endif
