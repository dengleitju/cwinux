#ifndef __CWX_THREAD_POOL_THRIFT_H__
#define __CWX_THREAD_POOL_THRIFT_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

#ifdef WITH_CWINUX_THRIFT
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxType.h"
#include "CwxLockGuard.h"
#include "CwxMutexLock.h"
#include "CwxMsgBlock.h"
#include "CwxTss.h"
#include "CwxTpi.h"
#include "CwxThread.h"
#include "concurrency/ThreadManager.h"
CWINUX_BEGIN_NAMESPACE

using namespace apache::thrift::concurrency;

/**
*@class CwxMsgQueueThrift
*@brief 生产者、消费者队列。
*/
class CWX_API CwxMsgQueueThrift
{
public:
  enum
  {
    ///活动状态
    ACTIVATED = 1,
    ///deactivated状态.
    DEACTIVATED = 2
  };
public:
  CwxMsgQueueThrift(size_t max_count);
  ~CwxMsgQueueThrift();
public:
  ///关闭队列，同时释放所有消息。-1：失败；>=0：队列中的消息数量
  int close();
  ///只释放队列中的所有消息。-1：失败；>=0：队列中的消息数量
  int flush (void);
  ///将一个消息放到队列的头部。-1：失败；>=0：队列中的消息数量
  int enqueue (boost::shared_ptr<Runnable>task,
    CwxTimeValue *timeout = 0);
  ///将一个消息放到队列的尾部。-1：失败；>=0：队列中的消息数量
  int enqueue_tail (boost::shared_ptr<Runnable>task,
    CwxTimeValue *timeout = 0);
  ///从队列的头部获取一个消息。-1：失败；>=0：队列中的消息数量
  int dequeue (boost::shared_ptr<Runnable> &task,
    CwxTimeValue *timeout = 0);
  ///检查队列是否full
  bool isFull (void);
  ///检查队列是否空.
  bool isEmpty(void);
  ///获取消息的数量.
  size_t getMsgCount(void);
  ///deactive 消息队列，-1：失败；否则返回先前的状态
  int deactivate (void);
  ///active消息队列，-1：失败；否则返回先前的状态
  int activate (void);
  ///获取消息队列的状态.
  int getState (void);
  ///检查队列是否deactive状态
  bool isDeactived(void);
  ///检查队列是否active状态
  bool isActivate(void);
  /// 获取锁.
  CwxMutexLock& lock();
private:
  ///deactive 消息队列，-1：失败；否则返回先前的状态
  int _deactivate(void);
  ///active消息队列，-1：失败；否则返回先前的状态
  int _activate(void);
  ///只释放队列中的所有消息。-1：失败；>=0：队列中的消息数量
  int _flush(void);
  ///检查队列是否full
  bool _isFull (void);
  ///检查队列是否空.
  bool _isEmpty(void);
  ///等待队列有空位置
  int _waitNotFullCond(CwxTimeValue *timeout = 0);
  ///等待队列消息
  int _waitNotEmptyCond(CwxTimeValue *timeout = 0);
  // = Disallow copying and assignment.
  CwxMsgQueueThrift (const CwxMsgQueueThrift &);
  CwxMsgQueueThrift& operator= (const CwxMsgQueueThrift &);

protected:
  /// 队列的状态.
  int m_state;
  /// 消息队列.
  list<boost::shared_ptr<Runnable> >  m_tasks;
  /// 最大的消息数量
  size_t m_maxCount;
  /// 队列中msg的数量.
  size_t m_curCount;
  /// 同步保护的锁.
  CwxMutexLock m_lock;
  /// Used to make threads sleep until the queue is no longer empty.
  CwxCondition m_notEmptyCond;
  /// Used to make threads sleep until the queue is no longer full.
  CwxCondition m_notFullCond;
};

class CWX_API CwxThreadThrift
{
public :
    ///构造函数
    CwxThreadThrift(CWX_UINT16 unThreadId,///<线程在线程group中的序号
        CwxMsgQueueThrift* queue=NULL ///<线程池的队列，若不指定，则系统默认创建
        );
    ///析构函数
    virtual ~CwxThreadThrift();
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
    int  append(boost::shared_ptr<Runnable> task);
    /**
    @brief 往线程的消息队列头添加一个新消息，以便及时消费。
    @param [in] pMsg append的消息
    @return -1：失败；>=0队列中排队的消息数量
    */
    int  appendHead(boost::shared_ptr<Runnable> task);
    /**
    @brief 从线程的消息队列获取一个排队消息，若队列为空，则阻塞。
    @param [out] pMsg pop的消息。
    @return -1：失败；>=0队列中排队的消息数量
    */
    int  pop(boost::shared_ptr<Runnable> &task);
    ///获取线程的group id
    CWX_UINT16 getThreadId() const;
private:
    static void* threadFunc(void *);
protected:
    CwxTss*                 m_pTssEnv;///<线程的tss
    CWX_UINT16              m_unThreadId;///<线程在线程组中的序号
    CwxMsgQueueThrift*      m_msgQueue; ///<队列
    bool                    m_bOwnQueue; ///<是否拥有队列
    pthread_t               m_tid; ///<线程的id
};

/**
@class CwxThreadPoolThrift
@brief 与thrift集成的线程池。
*/
class CWX_API CwxThreadPoolThrift:public ThreadManager {
public :
    ///构造函数
    CwxThreadPoolThrift(CWX_UINT16 unThreadNum,///<线程池中线程的数量
        CwxTss** pThrEnv=NULL, ///<tss
        size_t stack_size= 0 ///<线程的栈大小
        );
    ///析构函数
    ~CwxThreadPoolThrift();
public:
  //启动线程池
  virtual void start();
  ///停止线程池
  virtual void stop();
  /// join 线程
  virtual void join();
  /// 线程池状态
  virtual ThreadManager::STATE state() const;
  /// 获取线程的创建类工场，当前忽视
  virtual boost::shared_ptr<ThreadFactory> threadFactory() const;
  /// 设置线程的创建类工场，当前忽视
  virtual void threadFactory(boost::shared_ptr<ThreadFactory> value);
  /// 增加一个线程，当前忽视
  virtual void addWorker(size_t value=1);
  /// 去掉一个线程，当前忽视
  virtual void removeWorker(size_t value=1);
  /// 空闲的线程数量，当前忽视
  virtual size_t idleWorkerCount() const;
  /// 工作的线程数量，当前忽视
  virtual size_t workerCount() const;
  /// 等待处理的task数量
  virtual size_t pendingTaskCount() const;
  /// 总共的任务数量
  virtual size_t totalTaskCount() const;
  /// 任务的最大值
  virtual size_t pendingTaskCountMax() const;
  /// 超时任务的数量，当前忽视
  virtual size_t expiredTaskCount();
  /// 添加任务
  virtual void add(boost::shared_ptr<Runnable>task,
                   int64_t timeout=0LL,
                   int64_t expiration=0LL);
  /// 删除任务
  virtual void remove(boost::shared_ptr<Runnable> task);
  /// 删除下一个任务
  virtual boost::shared_ptr<Runnable> removeNextPending();
  /// 删除超时的任务
  virtual void removeExpiredTasks();
  /// 设置任务超时的回调函数
  virtual void setExpireCallback(ExpireCallback expireCallback);
  ///check thread 是否停止。若需要改变检查的规则，则重载此API
  virtual bool isStop();
  ///获取线程的TSS，及Thread env
  virtual CwxTss* getTss(CWX_UINT16 unThreadIndex);
  ///锁住整个线程池。返回值0：成功；-1：失败
  virtual int lock();
  ///解锁这个线程池。返回值0：成功；-1：失败
  virtual int unlock();
public:
    ///获取线程的消息队列排队消息数
    inline size_t getQueuedMsgNum();
    /**
    @brief 往线程的消息队列添加一个新消息。
    @param [in] pMsg append的消息
    @return -1：失败；>=0队列中排队的消息数量
    */
    inline int  append(boost::shared_ptr<Runnable> task);
    /**
    @brief 往线程的消息队列头添加一个新消息，以便及时消费。
    @param [in] pMsg append的消息
    @return -1：失败；>=0队列中排队的消息数量
    */
    int  appendHead(boost::shared_ptr<Runnable> task);

    /**
    @brief 从线程的消息队列获取一个排队消息，若队列为空，则阻塞。
    @param [out] pMsg pop的消息。
    @return -1：失败；>=0队列中排队的消息数量
    */
    inline int  pop(boost::shared_ptr<Runnable>& task);

    inline CWX_UINT16 getThreadNum() const { return m_threadNum;}
private:
    ///停止线程池
    void _stop();
protected:
    CwxMutexLock           m_lock;
    CwxTss**               m_arrTssEnv;///<线程的tss
    CwxMsgQueueThrift*     m_msgQueue; ///<队列
    size_t                 m_stackSize;
    CwxThreadThrift**      m_threadArr;  ///<thead的数组
    boost::shared_ptr<ThreadFactory>  m_factory;
    CWX_UINT16             m_threadNum;
};

CWINUX_END_NAMESPACE

#include "CwxThreadPoolThrift.inl"
#include "CwxPost.h"
#endif
#endif

