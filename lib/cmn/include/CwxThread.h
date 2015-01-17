#ifndef __CWX_THREAD_H__
#define __CWX_THREAD_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxThread.h
@brief 线程对象定义
@author cwinux@gmail.com
@version 0.1
@date 2009-07-20
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxType.h"
#include "CwxLockGuard.h"
#include "CwxMutexLock.h"
#include "CwxMsgQueue.h"
#include "CwxMsgBlock.h"
#include "CwxCommander.h"
#include "CwxLogger.h"
#include "CwxTss.h"

CWINUX_BEGIN_NAMESPACE

typedef void* (*CWX_OS_THR_FUNC)(void *); ///os thread的function
typedef void* (*CWX_TSS_THR_FUNC)(CwxTss* tss, CwxMsgQueue* queue, void*); ///架构的函数

#define THREAD_BOUND               0x00000001
#define THREAD_NEW_LWP             0x00000002
#define THREAD_DETACHED            0x00000040
#define THREAD_SUSPENDED           0x00000080
#define THREAD_DAEMON              0x00000100
#define THREAD_SCHED_FIFO          0x00020000
#define THREAD_SCHED_RR            0x00040000
#define THREAD_SCHED_DEFAULT       0x00080000
#define THREAD_JOINABLE            0x00010000
#define THREAD_SCOPE_SYSTEM        THREAD_BOUND
#define THREAD_SCOPE_PROCESS       0x00200000
#define THREAD_INHERIT_SCHED       0x00400000
#define THREAD_EXPLICIT_SCHED      0x00800000
#define THREAD_SCHED_IO            0x01000000
#define CWX_DEFAULT_THREAD_PRIORITY (-0x7fffffffL - 1L)

class CWX_API CwxThread
{
public :
    ///构造函数
    CwxThread(CWX_UINT16 unThreadId,///<线程在线程group中的序号
        CwxCommander* commander,///<队列消息消费的缺省commander，若指定func可以不指定
        CWX_TSS_THR_FUNC func=NULL, ///<用户的线程main函数
        void*            arg=NULL, ///<func的void*参数
        CwxMsgQueue* queue = NULL///<线程池的队列，若不指定，则系统默认创建
        );
    ///析构函数
    virtual ~CwxThread();
public:
    /**
    @brief 启动线程
    @param [in] pThrEnv 线程的Tss，若不指定，可通过onThreadCreated创建。
    @param [in] stack_size 线程堆栈的大小，若为0，则采用系统默认大小。
    @return -1：失败； 0：成功
    */
    virtual int start(CwxTss* pThrEnv=NULL, size_t stack_size = 0);
    ///停止线程
    virtual void stop();
    /**
    @brief 线程的body，若需要改变线程的运行行为，则重载此API
    @param [out] pThrEnv Thread的Thread Env。
    @return void
    */
    virtual void threadMain();
    ///check thread 是否停止。若需要改变检查的规则，则重载此API
    virtual bool isStop();
    ///获取线程的TSS，及Thread env
    virtual CwxTss* getTss();
    ///锁住线程。返回值0：成功；-1：失败
    virtual int lock();
    ///解锁线程。返回值0：成功；-1：失败
    virtual int unlock();
public:
    ///获取线程的消息队列排队消息数
    size_t getQueuedMsgNum();
    /**
    @brief 往线程的消息队列添加一个新消息。
    @param [in] pMsg append的消息
    @return -1：失败；>=0队列中排队的消息数量
    */
    int  append(CwxMsgBlock* pMsg);
    /**
    @brief 往线程的消息队列头添加一个新消息，以便及时消费。
    @param [in] pMsg append的消息
    @return -1：失败；>=0队列中排队的消息数量
    */
    int  appendHead(CwxMsgBlock* pMsg);
    /**
    @brief 从线程的消息队列获取一个排队消息，若队列为空，则阻塞。
    @param [out] pMsg pop的消息。
    @return -1：失败；>=0队列中排队的消息数量
    */
    int  pop(CwxMsgBlock*& pMsg);
    ///获取线程的group id
    CWX_UINT16 getThreadId() const;
public:
    /**
    @brief 创建一个线程。
    @param [in] func 线程的main函数。
    @param [in] args 线程的main函数的参数。
    @param [in] flags 线程的属性标志。
    @param [out] thr_id 线程的id。
    @param [in] priority 线程的优先级。
    @param [in] stack 线程的堆栈。
    @param [in] stacksize 线程的堆栈的大小。
    @return -1：失败，错误原因在errno；0成功。
    */
    static int spawn(CWX_OS_THR_FUNC func,
        void *args=NULL,
        long flags=THREAD_NEW_LWP | THREAD_JOINABLE,
        pthread_t *thr_id=NULL,
        long priority = CWX_DEFAULT_THREAD_PRIORITY,
        void *stack = NULL,
        size_t stacksize = 0);
    ///对os的pthread_join的封装，0：成功；-1：失败，errno放失败的代码
    static int join(pthread_t thread, void **value_ptr);
    ///对os的pthread_kill的封装，0：成功；-1：失败，errno放失败的代码
    static int kill(pthread_t thread, int sig);
    ///对os的pthread_self的封装，返回thread id。
    static pthread_t self();
    ///对os的pthread_exit的封装
    static void exit(void *value_ptr);
    ///对os的pthread_equal的封装
    static bool equal(pthread_t t1, pthread_t t2);
    ///对os的pthread_cancel的封装，0：成功；-1：失败，errno放失败的代码
    static int cancel(pthread_t thread);
private:
    static void* threadFunc(void *);
protected:
    CwxTss*                 m_pTssEnv;///<线程的tss
    CWX_UINT16              m_unThreadId;///<线程在线程组中的序号
    CwxMsgQueue*            m_msgQueue; ///<队列
    bool                    m_bOwnQueue; ///<是否拥有队列
    CwxCommander*           m_commander; ///<commander
    CWX_TSS_THR_FUNC         m_func; ///<用户指定的thread main function
    void*                   m_arg; ///<线程的参数
    pthread_t                m_tid;
};

CWINUX_END_NAMESPACE

#include "CwxThread.inl"
#include "CwxPost.h"
#endif
