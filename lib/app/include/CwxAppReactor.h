#ifndef __CWX_APP_REACTOR_H__
#define __CWX_APP_REACTOR_H__

/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxAppReactor.h
@brief 架构的reactor对象
@author cwinux@gmail.com
@version 0.1
@date 2009-11-25
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

CWINUX_BEGIN_NAMESPACE


/**
@class CwxAppReactor
@brief 架构的reactor对象，实现事件的循环
*/

class CWX_API CwxAppReactor
{
public:
    enum{
        REG_TYPE_UNKNOWN=0,
        REG_TYPE_IO = 1,
        REG_TYPE_SIG = 2
    };
    typedef int (*REACTOR_EVENT_HOOK)(void *);
    ///构造函数
    CwxAppReactor(bool bEnableSig=true);
    ///析构函数
    ~CwxAppReactor();
public:
    /**
    @brief 打开reactor，调用线程为reactor的owner。
    @param [in] noticeHandler notice handler
    @return -1：失败；0：正常退出
    */
    int open(CwxAppHandler4Base* noticeHandler);
    /**
    @brief 关闭reactor，必须为owner thread。会释放所有的handler
    @return -1：失败；0：正常退出
    */
    int close();
    /**
    @brief 架构事件的循环处理API，实现消息的分发，调用线程为reactor的owner。
    @param [in] hook hook函数
    @param [in] arg hook函数的参数。
    @param [in] bOnce 是否只运行一次。
    @param [in] uiMiliTimeout 超时的毫秒数，0表示一直阻塞到事件发生。
    @return -1：失败；0：正常退出
    */
    int run(REACTOR_EVENT_HOOK hook=NULL,
        void* arg=NULL,
        bool  bOnce=false,
        CWX_UINT32 uiMiliTimeout=0);
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
    @param [in] uiConnId 连接ID，若连接ID不为CWX_APP_INVALID_CONN_ID，则会基于连接ID进行管理。
    @param [in] uiMillSecond 超时毫秒数，0表示不进行超时检测。
    @return -1：失败；
            0：成功；
    */
    int registerHandler (CWX_HANDLE io_handle,
        CwxAppHandler4Base *event_handler,
        int mask,
        CWX_UINT32 uiConnId=CWX_APP_INVALID_CONN_ID,
        CWX_UINT32 uiMillSecond = 0);
    /**
    @brief 删除io事件处理handle。多线程安全，任意线程都可以调用。
    @param [in] event_handler 移除的event-handler
    @param [in] bRemoveConnId 是否也从conn-id的map中删除
    @return -1：失败；
             0：成功；
    */
    int removeHandler (CwxAppHandler4Base *event_handler, bool bRemoveConnId=true);
    /**
    @brief suspend io事件处理handle。多线程安全，任意线程都可以调用。
    @param [in] event_handler suspend的event-handler
    @param [in] suspend_mask suspend的事件,为READ_MASK、WRITE_MASK组合
    @return -1：失败；
    0：成功；
    */
    int suspendHandler (CwxAppHandler4Base *event_handler,
        int suspend_mask);
    /**
    @brief resume io事件处理handle。多线程安全，任意线程都可以调用。
    @param [in] event_handler resume的event-handler
    @param [in] resume_mask resume的事件,为READ_MASK、WRITE_MASK组合
    @return -1：失败；
    0：成功；
    */
    int resumeHandler (CwxAppHandler4Base *event_handler,
        int resume_mask);

    /**
    @brief 删除io事件处理handle。多线程安全，任意线程都可以调用。
    @param [in] handle 移除的 io handle
    @param [in] bRemoveConnId 是否也从conn-id的map中删除
    @return NULL：不存在；否则：成功；
    */
    CwxAppHandler4Base* removeHandler (CWX_HANDLE handle, bool bRemoveConnId=true);
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
    @brief 删除io事件处理handle。多线程安全，任意线程都可以调用。
    @param [in] uiConnId 连接ID
    @param [in] bRemoveConnId 是否也从conn-id的map中删除
    @return NULL：失败；否则：成功；
    */
    CwxAppHandler4Base* removeHandlerByConnId (CWX_UINT32 uiConnId, bool bRemoveConnId=true);
    /**
    @brief suspend io事件处理handle。多线程安全，任意线程都可以调用。
    @param [in] uiConnId 连接ID
    @param [in] suspend_mask suspend的事件,为READ_MASK、WRITE_MASK组合
    @return -1：失败；
    0：成功；
    */
    int suspendHandlerByConnId (CWX_UINT32 uiConnId,
        int suspend_mask);
    /**
    @brief resume io事件处理handle。多线程安全，任意线程都可以调用。
    @param [in] uiConnId 连接ID
    @param [in] resume_mask resume的事件,为READ_MASK、WRITE_MASK组合
    @return -1：失败；
    0：成功；
    */
    int resumeHandlerByConnId (CWX_UINT32 uiConnId,
        int resume_mask);
    ///从Conn map删除指定的Handler，此时，连接必须没有注册。多线程安全，任意线程都可以调用。
    CwxAppHandler4Base* removeFromConnMap(CWX_UINT32 uiConnId);

    /**
    @brief 注册signal事件处理handle，信号具有PERSIST属性。多线程安全，任意线程都可以调用。
    @param [in] signum 信号
    @param [in] event_handler signal的event handler
    @return -1：失败；
    0：成功；
    */
    int registerSignal(int signum,
        CwxAppHandler4Base *event_handler);
    /**
    @brief 删除signal事件处理handle。多线程安全，任意线程都可以调用。
    @param [in] event_handler signal的event handler
    @return -1：失败；
    0：成功；
    */
    int removeSignal(CwxAppHandler4Base *event_handler);
    /**
    @brief 删除signal事件处理handle。多线程安全，任意线程都可以调用。
    @param [in] sig signal
    @return NULL：不存在；否则返回signal的handler
    */
    CwxAppHandler4Base* removeSignal(int sig);

    /**
    @brief 设置定时处理handle，timeout不具有persist特性。多线程安全，任意线程都可以调用。
    @param [in] event_handler timer的event handler
    @param [in] interval 定时的间隔
    @return -1：失败；
    0：成功；
    */
    int scheduleTimer (CwxAppHandler4Base *event_handler,
        CwxTimeValue const &interval);
    ///取消定时处理handle。多线程安全，任意线程都可以调用。
    int cancelTimer (CwxAppHandler4Base *event_handler);
    /**
    @brief notice reactor。多线程安全，任意线程都可以调用。
    @param [in] notice notice的对象，可以为空
    @return -1：失败；  0：成功；
    */
    int notice(CwxAppNotice* notice= NULL);
    /**
    @brief 获取notice的事件。多线程安全，任意线程都可以调用。
    @param [in] head notice对象链表的头
    @return void
    */
    void noticed(CwxAppNotice*& head);
    ///fork的re-init方法，返回值，0：成功；-1：失败
    int forkReinit();
public:
    ///获取下一个可用的连接Id。多线程安全，任意线程都可以调用。
    CWX_UINT32 getNextConnId();
    /**
    @brief 检查指定IO的handle是否已经注册。多线程安全，任意线程都可以调用。
    @return true：注册；false：没有注册
    */
    bool isRegIoHandle(CWX_HANDLE handle);
    ///根据conn id获取对应的handler，必须是owner thread
    CwxAppHandler4Base* getHandlerByConnId(CWX_UINT32 uiConnId);
    /**
    @brief 检查指定sig的handle是否已经注册。
    @return true：注册；false：没有注册
    */
    bool isRegSigHandle(int sig);
    /**
    @brief 获取指定sig对应的event handler。
    @return 返回handle对应的event handler；NULL表示不存在
    */
    CwxAppHandler4Base* getSigHandler(int sig);
    ///Return the ID of the "owner" thread.
    pthread_t getOwner() const;
    ///是否stop
    bool isStop();
    ///获取当前的时间
    CwxTimeValue const& getCurTime() const;
    ///io handle是否设置指定的mask
    bool isMask(CWX_HANDLE handle, int mask);
private:
    ///注册IO事件处理handle
    int _registerHandler (CWX_HANDLE io_handle,
        CwxAppHandler4Base *event_handler,
        int mask,
        CWX_UINT32 uiConnId=CWX_APP_INVALID_CONN_ID,
        CWX_UINT32 uiMillSecond = 0);
    ///删除io事件处理handle
    int _removeHandler (CwxAppHandler4Base *event_handler, bool bRemoveConnId=true);
    ///注册IO事件处理handle
    int _suspendHandler (CwxAppHandler4Base *event_handler,
        int suspend_mask);
    ///删除io事件处理handle
    int _resumeHandler (CwxAppHandler4Base *event_handler,
        int resume_mask);
    ///删除io事件处理handle。
    CwxAppHandler4Base* _removeHandler (CWX_HANDLE handle, bool bRemoveConnId=true);
    ///suspend io事件处理handle。
    int _suspendHandler (CWX_HANDLE handle,
        int suspend_mask);
    ///resume io事件处理handle。
    int _resumeHandler (CWX_HANDLE handle,
        int resume_mask);
    ///删除io事件处理handle。
    CwxAppHandler4Base* _removeHandlerByConnId (CWX_UINT32 uiConnId, bool bRemoveConnId=true);
    ///suspend io事件处理handle。
    int _suspendHandlerByConnId (CWX_UINT32 uiConnId,
        int suspend_mask);
    /// resume io事件处理handle。
    int _resumeHandlerByConnId (CWX_UINT32 uiConnId,
        int resume_mask);
    ///注册signal事件处理handle
    int _registerSignal(int signum,
        CwxAppHandler4Base *event_handler
        );
    ///删除signal事件处理handle
    int _removeSignal(CwxAppHandler4Base *event_handler);
    /**
    @brief 删除signal事件处理handle。
    @param [in] sig signal
    @return NULL：不存在；否则返回signal的handler
    */
    CwxAppHandler4Base* _removeSignal(int sig);

    ///设置定时处理handle
    int _scheduleTimer (CwxAppHandler4Base *event_handler,
        CwxTimeValue const &interval );
    ///取消定时处理handle
    int _cancelTimer (CwxAppHandler4Base *event_handler);
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
    bool _isRegIoHandle(CwxAppHandler4Base* handler);
    /**
    @brief 获取指定handle对应的event handler。
    @return 返回handle对应的event handler；NULL表示不存在
    */
    CwxAppHandler4Base* _getIoHandler(CWX_HANDLE handle);
    /**
    @brief 检查指定sig的handle是否已经注册。
    @return true：注册；false：没有注册
    */
    bool _isRegSigHandle(int sig);
    /**
    @brief 检查指定IO的handle是否已经注册。
    @return true：注册；false：没有注册
    */
    bool _isRegSigHandle(CwxAppHandler4Base* handler);
    /**
    @brief 获取指定sig对应的event handler。
    @return 返回handle对应的event handler；NULL表示不存在
    */
    CwxAppHandler4Base* _getSigHandler(int sig);

    ///检查handler对应的conn-id是否能加入conn-map中
    bool enableRegConnMap(CWX_UINT32 uiConnId, CwxAppHandler4Base* handler);
    void addRegConnMap(CWX_UINT32 uiConnId, CwxAppHandler4Base* handler);
    CwxAppHandler4Base* removeRegConnMap(CWX_UINT32 uiConnId);
    static void callback(CwxAppHandler4Base* handler, int mask, bool bPersist, void *arg);

private:
    bool                    m_bEnableSig;///<是否注册signal
    CwxMutexLock            m_lock; ///<全局锁
    CwxRwLock               m_rwLock; ///<读写锁
    pthread_t               m_owner; ///<reactor的owner 线程
    bool                    m_bStop; ///<reactor是否已经停止
    CWX_UINT32              m_connId[CWX_APP_MAX_IO_NUM]; ///<handler id到conn-id的映射
    CWX_UINT32             m_uiCurConnId; ///<上次分配的连接ID
    CwxMutexLock           m_connMapMutex;///<m_connMap的锁，本身m_lock就可以保护，但为了提高getNextConnId()，其受双锁保护
    hash_map<CWX_UINT32/*conn id*/, CwxAppHandler4Base*/*连接对象*/> m_connMap; ///<基于Conn id的连接Map
    CwxAppNoticePipe*       m_pNoticePipe;
    ///引擎的资源
    CwxAppEpoll*            m_engine; ///<epoll的engine


};


CWINUX_END_NAMESPACE
#include "CwxAppReactor.inl"
#include "CwxPost.h"

#endif
