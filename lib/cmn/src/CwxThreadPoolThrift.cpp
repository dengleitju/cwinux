#ifdef WITH_CWINUX_THRIFT
#include "CwxThreadPoolThrift.h"
#include "CwxLogger.h"

CWINUX_BEGIN_NAMESPACE
CwxMsgQueueThrift::CwxMsgQueueThrift(size_t max_count)
  :m_notEmptyCond(m_lock),m_notFullCond(m_lock)
{
  m_state = ACTIVATED;
  m_maxCount = max_count;
  m_curCount = 0; 
}
CwxMsgQueueThrift::~CwxMsgQueueThrift()
{
  if (m_tasks.begin() != m_tasks.end()) {
    this->close();
  }
}

///将一个消息放到队列的头部。-1：失败；>=0：队列中的消息数量
int CwxMsgQueueThrift::enqueue (boost::shared_ptr<Runnable>task,
  CwxTimeValue *timeout)
{
  int queue_count = 0;
  {
    CwxMutexGuard<CwxMutexLock> guard(&this->m_lock);

    if (DEACTIVATED == this->m_state) return -1;
    if (this->_waitNotFullCond(timeout) == -1) return -1;
    m_curCount ++;
    m_tasks.push_front(task);
    if (1 ==  m_curCount){
      if (-1 == m_notEmptyCond.signal()) return -1;
    }
    queue_count = m_curCount;
  }
  return queue_count;
}


///将一个消息放到队列的尾部。-1：失败；>=0：队列中的消息数量
int CwxMsgQueueThrift::enqueue_tail(boost::shared_ptr<Runnable>task,
  CwxTimeValue *timeout)
{
  int queue_count = 0;
  {
    CwxMutexGuard<CwxMutexLock> guard(&this->m_lock);
    if (DEACTIVATED == this->m_state) return -1;
    if (this->_waitNotFullCond(timeout) == -1) return -1;
    m_tasks.push_back(task);
    m_curCount ++;
    if (1 ==  m_curCount) {
      if (-1 == m_notEmptyCond.signal()) return -1;
    }
    queue_count = m_curCount;
  }
  return queue_count;

}


///从队列的头部获取一个消息。-1：失败；>=0：队列中的消息数量
int CwxMsgQueueThrift::dequeue (boost::shared_ptr<Runnable> &task,
  CwxTimeValue *timeout)
{
  int queue_count = 0;
  bool bFull = false;
  {
    CwxMutexGuard<CwxMutexLock> guard(&this->m_lock);
    if (DEACTIVATED == this->m_state)  return -1;
    if (this->_waitNotEmptyCond(timeout) == -1) return -1;
    bFull = _isFull();
    task = *m_tasks.begin();
    m_tasks.pop_front();
    m_curCount --;
    if (bFull && !_isFull()) {
      if (-1 == m_notFullCond.signal()) return -1;
    }
    queue_count = m_curCount;
  }
  return queue_count;
}


///构造函数
CwxThreadThrift::CwxThreadThrift(CWX_UINT16 unThreadId,///<线程在线程group中的序号
  CwxMsgQueueThrift* queue///<线程池的队列，若不指定，则系统默认创建
  )
{
  m_pTssEnv = NULL;
  m_unThreadId = unThreadId;
  if (!queue){
    m_msgQueue = new CwxMsgQueueThrift(1024*1024);
    m_bOwnQueue = true;
  }else{
    m_msgQueue = queue;
    m_bOwnQueue = false;
  }
}

CwxThreadThrift::~CwxThreadThrift(){
  if (m_bOwnQueue && m_msgQueue){
    delete m_msgQueue;
    m_msgQueue = NULL;
  }
  if (m_pTssEnv){
    delete m_pTssEnv;
    m_pTssEnv = NULL;
  }
}

int CwxThreadThrift::start(CwxTss* pThrEnv, size_t stack_size){
  m_pTssEnv = pThrEnv;
  if (-1 ==  CwxThread::spawn(
    threadFunc,
    this,
    THREAD_NEW_LWP | THREAD_JOINABLE | THREAD_INHERIT_SCHED,
    &m_tid,
    CWX_DEFAULT_THREAD_PRIORITY,
    NULL,
    stack_size        
    ))
  {
    CWX_ERROR(("Failure to spawn thread[id=%u], errno=%d", m_unThreadId, errno));
    return -1;
  }
  return 0;
}

void CwxThreadThrift::stop()
{
  if (m_msgQueue->isActivate()) {
    m_msgQueue->deactivate();
  }
  CwxThread::join(m_tid, NULL);
  if (m_bOwnQueue)
    m_msgQueue->flush();
}

void CwxThreadThrift::threadMain() {
  time_t ttTime = time(NULL);
  {//注册tss
    if (!m_pTssEnv){
      m_pTssEnv = new CwxTss;
    }
    m_pTssEnv->getThreadInfo().setThreadNo(m_unThreadId);
  }
  m_pTssEnv->getThreadInfo().setStopped(false);
  CwxTss::regTss(m_pTssEnv);
  m_pTssEnv->getThreadInfo().setThreadId(CwxThread::self());
  m_pTssEnv->getThreadInfo().setStartTime(ttTime);
  m_pTssEnv->getThreadInfo().setUpdateTime(ttTime);
  m_pTssEnv->getThreadInfo().setRecvMsgNum(0);
  m_pTssEnv->getThreadInfo().setQueuedMsgNum(0);
  int iRet = 0;
  boost::shared_ptr<Runnable> task;
  while((iRet = this->pop(task)) != -1) {//block until has query message
    m_pTssEnv->getThreadInfo().setBlocked(true);
    m_pTssEnv->getThreadInfo().setBlocked(false);
    ttTime = time(NULL);
    m_pTssEnv->getThreadInfo().setUpdateTime(ttTime);
    m_pTssEnv->getThreadInfo().setQueuedMsgNum(iRet);
    m_pTssEnv->getThreadInfo().incRecvMsgNum();
    task->run();
    m_pTssEnv->getThreadInfo().setBlocked(true);
  };
  m_pTssEnv->getThreadInfo().setStopped(true);
}

bool CwxThreadThrift::isStop() {
  return m_msgQueue->getState() == CwxMsgQueue::DEACTIVATED;
}

CwxTss* CwxThreadThrift::getTss() {
  return m_pTssEnv;
}

///锁住线程。返回值0：成功；-1：失败
int CwxThreadThrift::lock() {
  return m_msgQueue->lock().acquire();
}
///解锁这个线程。返回值0：成功；-1：失败
int CwxThreadThrift::unlock() {
  return m_msgQueue->lock().release();
}

void* CwxThreadThrift::threadFunc(void * thread)
{
  CwxThreadThrift* pThread = (CwxThreadThrift*)thread;
  pThread->threadMain();
  return thread;
}

///构造函数
CwxThreadPoolThrift::CwxThreadPoolThrift(CWX_UINT16 unThreadNum,///<线程池中线程的数量
  CwxTss** pThrEnv, ///<tss
  size_t stack_size ///<线程的栈大小
  )
{
  m_arrTssEnv = pThrEnv;
  m_msgQueue = new CwxMsgQueueThrift(1024*1024);
  m_stackSize = stack_size;
  m_threadArr =  NULL;
  m_threadNum = unThreadNum;
}

///析构函数
CwxThreadPoolThrift::~CwxThreadPoolThrift(){
  if (m_arrTssEnv){
    delete []m_arrTssEnv;
  }
  if (m_threadArr) {
    for (CWX_UINT16 i=0; i<getThreadNum(); i++) {
      if (m_threadArr[i]) delete m_threadArr[i];
    }
    delete [] m_threadArr;
  }
  if (m_msgQueue) delete m_msgQueue;
}

void CwxThreadPoolThrift::start() {
  CwxMutexGuard<CwxMutexLock> lock(&m_lock);
  if (!m_arrTssEnv) {
    m_arrTssEnv = new CwxTss*[getThreadNum()];
    memset(m_arrTssEnv, 0x00, sizeof(CwxTss*) * getThreadNum());
  }
  ///创建线程
  CWX_UINT16 index= 0;
  m_threadArr = new CwxThreadThrift*[getThreadNum()];
  for (index=0; index<getThreadNum(); index++){
    m_threadArr[index] = new CwxThreadThrift(index, m_msgQueue);
    if (0 != m_threadArr[index]->start(m_arrTssEnv[index], m_stackSize)){
      _stop();
      return;
    }
  }
}

void CwxThreadPoolThrift::stop() {
  CwxMutexGuard<CwxMutexLock> lock(&m_lock);
  _stop();
}

/// join 线程
void CwxThreadPoolThrift::join() {
  stop();
}

/// 线程池状态
ThreadManager::STATE CwxThreadPoolThrift::state() const {
  return ThreadManager::STARTED;
}

boost::shared_ptr<ThreadFactory> CwxThreadPoolThrift::threadFactory() const {
  return m_factory;
}
void CwxThreadPoolThrift::threadFactory(boost::shared_ptr<ThreadFactory> value) {
  m_factory = value;
}

void CwxThreadPoolThrift::addWorker(size_t) {
}

void CwxThreadPoolThrift::removeWorker(size_t) {
}
size_t CwxThreadPoolThrift::idleWorkerCount() const {
  return 0;
}
size_t CwxThreadPoolThrift::workerCount() const {
  return getThreadNum();
}
size_t CwxThreadPoolThrift::pendingTaskCount() const {
  return m_msgQueue->getMsgCount();
}
size_t CwxThreadPoolThrift::totalTaskCount() const {
  return m_msgQueue->getMsgCount();
}
size_t CwxThreadPoolThrift::pendingTaskCountMax() const  {
  return 1024*1024;
}
size_t CwxThreadPoolThrift::expiredTaskCount() {
  return 0;
}
void CwxThreadPoolThrift::add(boost::shared_ptr<Runnable>task,
  int64_t ,
  int64_t ) {
    m_msgQueue->enqueue(task);
}
void CwxThreadPoolThrift::remove(boost::shared_ptr<Runnable> ) {
  return;
}
boost::shared_ptr<Runnable> CwxThreadPoolThrift::removeNextPending() {
  return boost::shared_ptr<Runnable>();
}
void CwxThreadPoolThrift::removeExpiredTasks() {

}
void CwxThreadPoolThrift::setExpireCallback(ExpireCallback) {

}

bool CwxThreadPoolThrift::isStop(){
  return m_msgQueue->getState() == CwxMsgQueue::DEACTIVATED;
}

CwxTss* CwxThreadPoolThrift::getTss(CWX_UINT16 unThreadIndex)
{
  if (unThreadIndex >= getThreadNum()) return NULL;
  return m_arrTssEnv[unThreadIndex];
}

///锁住整个线程池。返回值0：成功；-1：失败
int CwxThreadPoolThrift::lock(){
  return m_msgQueue->lock().acquire();
}
///解锁这个线程池。返回值0：成功；-1：失败
int CwxThreadPoolThrift::unlock(){
  return m_msgQueue->lock().release();
}

void CwxThreadPoolThrift::_stop(){
  if (m_msgQueue->isActivate()){
    m_msgQueue->deactivate();
    if (m_threadArr){
      for(CWX_UINT16 i=0; i<getThreadNum(); i++){
        if (m_threadArr[i]){
          m_threadArr[i]->stop();
        }
      }
    }
  }
  m_msgQueue->flush();
}


CWINUX_END_NAMESPACE
#endif