#ifndef __CWX_APP_EPOLL_H__
#define __CWX_APP_EPOLL_H__

/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxAppEpoll.h
@brief 架构的epoll事件引擎对象
@author cwinux@gmail.com
@version 0.1
@date 2011-04-13
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxTimeValue.h"
#include "CwxAppConfig.h"
#include "CwxAppMacro.h"
#include "CwxAppHandler4Base.h"
#include "CwxThread.h"
#include "CwxLogger.h"
#include "CwxMinHeap.h"
#include "CwxIpcSap.h"
#include "CwxDate.h"
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>

CWINUX_BEGIN_NAMESPACE

class CwxAppReactor;
class CwxAppChannel;

/**
@class CwxAppEpoll
@brief 架构的epoll事件引擎
*/
typedef void (*REACTOR_CALLBACK)(CwxAppHandler4Base* handler, int mask, bool bPersist, void *arg);

class CWX_API CwxAppEpoll
{
public:
    enum
    {
        CWX_EPOLL_INIT_HANDLE = 81920
    };
public:
    ///构造函数
    CwxAppEpoll(bool bEnableSignal=true);
    ///析构函数
    ~CwxAppEpoll();
public:
    /**
    @brief 初始化epoll引擎。
    @return -1：失败；0：成功
    */
    int init();
    /**
    @brief 注册IO事件处理handle。
    @param [in] handle 监测的IO handle
    @param [in] event_handler io handle对应的event handler。
    @param [in] mask 注册的事件掩码，为READ_MASK、WRITE_MASK、PERSIST_MASK、TIMEOUT_MASK组合
    @param [in] uiMillSecond 多少毫秒超时。0表示没有超时设置。
    @param [in] bForkAdd 是否是fork后重新添加。
    @return -1：失败；0：成功；
    */
    int registerHandler(CWX_HANDLE handle,
        CwxAppHandler4Base *event_handler,
        int mask,
        CWX_UINT32 uiMillSecond = 0);
    /**
    @brief 删除io事件处理handle。
    @param [in] handle 移除的 io handle
    @return NULL：不存在；否则：成功；
    */
    CwxAppHandler4Base* removeHandler (CWX_HANDLE handle);
    /**
    @brief suspend io事件处理handle。
    @param [in] handle suspend io handle
    @param [in] suspend_mask suspend的事件,为READ_MASK、WRITE_MASK组合
    @return -1：失败；0：成功
    */
    int suspendHandler (CWX_HANDLE handle,
        int suspend_mask);
    /**
    @brief resume io事件处理handle。
    @param [in] handle resume io handle
    @param [in] resume_mask resume的事件,为READ_MASK、WRITE_MASK组合
    @return -1：失败；0：成功；
    */
    int resumeHandler (CWX_HANDLE handle,
        int resume_mask);
    /**
    @brief 注册signal事件处理handle，信号具有PERSIST属性。
    @param [in] signum 信号
    @param [in] event_handler signal的event handler
    @return -1：失败； 0：成功；
    */
    int registerSignal(int signum,
        CwxAppHandler4Base *event_handler);
    /**
    @brief 删除signal事件处理handle。
    @param [in] event_handler signal的event handler
    @return -1：失败；0：成功；
    */
    int removeSignal(CwxAppHandler4Base *event_handler);
    /**
    @brief 删除signal事件处理handle。多线程安全，任意线程都可以调用。
    @param [in] sig signal
    @return NULL：不存在；否则返回signal的handler
    */
    CwxAppHandler4Base* removeSignal(int sig);

    /**
    @brief 设置定时处理handle，timeout不具有persist特性。
    @param [in] event_handler timer的event handler
    @param [in] interval 定时的间隔
    @return -1：失败；0：成功；
    */
    int scheduleTimer (CwxAppHandler4Base *event_handler,
        CwxTimeValue const &interval);
    ///取消定时处理handle。
    int cancelTimer (CwxAppHandler4Base *event_handler);
    ///fork的re-init方法，返回值，0：成功；-1：失败
    int forkReinit();
    /**
    @brief 检测事件。
    @param [in] callback 事件的回调函数
    @param [in] arg 回调函数的参数
    @param [in] uiMiliTimeout 超时的毫秒数，0表示一直阻塞到事件发生。
    @return -1：失败；0：成功
    */
    int poll(REACTOR_CALLBACK callback, void* arg, CWX_UINT32 uiMiliTimeout=0);
    ///停止运行
    void stop();
    ///获取当前的时间
    CwxTimeValue const& getCurTime() const;
private:
    ///获取下一个的epoll的超时时间
    void timeout(CWX_UINT64& ullTime);
    ///添加 epoll检测,mask为READ_MASK、WRITE_MASK的组合。
    int addEvent(int fd, int mask);
    ///删除存在的mask，mask为READ_MASK、WRITE_MASK的组合。
    int delEvent(int fd, int mask);
    ///信号handle
    static void sigAction(int , siginfo_t *info, void *);
private:
    class EventHandle
    {
    public:
        EventHandle()
        {
            m_mask = 0;
            m_handler = NULL;
        }
        inline bool isReg() { return (m_mask&CwxAppHandler4Base::RW_MASK) != 0;}
    public:
        int         m_mask;
        CwxAppHandler4Base* m_handler;
    };
    class SignalHanlder:public CwxAppHandler4Base
    {
    public:
        SignalHanlder():CwxAppHandler4Base(NULL)
        {
        }
        ~SignalHanlder(){}
    public:
        virtual int open (void *){ return 0;}
        virtual int handle_event(int , CWX_HANDLE handle)
        {
            char sigBuf[64];
            ssize_t num = read(handle, sigBuf, 64);
            return num==0?0:0;
        }
        virtual int close(CWX_HANDLE ){return 0;}
    };
private:
    friend class CwxAppReactor;
    friend class CwxAppChannel;
private:
    int                             m_epfd;     ///<epoll的fd
    struct epoll_event              m_events[CWX_APP_MAX_IO_NUM]; ///<epoll的event 数组
    EventHandle                     m_eHandler[CWX_APP_MAX_IO_NUM]; ///<epoll的event handler
    static int                      m_signalFd[2]; ///<信号的读写handle
    static sig_atomic_t             m_arrSignals[CWX_APP_MAX_SIGNAL_ID + 1];///<signal的数组
    static volatile sig_atomic_t    m_bSignal; ///<是否有信号
    bool                            m_bEnableSignal; ///<是否支持signal
    CwxAppHandler4Base*             m_sHandler[CWX_APP_MAX_SIGNAL_ID + 1];///<signal handler的数组
    CwxMinHeap<CwxAppHandler4Base>  m_timeHeap; ///<时间堆
    SignalHanlder*                  m_sigHandler; ///<读取signal事件的handle
    bool                            m_bStop; ///<是否停止
    CwxTimeValue                    m_current; ///<当前的时间
};


CWINUX_END_NAMESPACE
#include "CwxAppEpoll.inl"
#include "CwxPost.h"

#endif
