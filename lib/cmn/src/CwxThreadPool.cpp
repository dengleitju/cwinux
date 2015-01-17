#include "CwxThreadPool.h"
#include "CwxLogger.h"

CWINUX_BEGIN_NAMESPACE
///构造函数
CwxThreadPool::CwxThreadPool(CWX_UINT16 unThreadNum,///<线程池中线程的数量
                             CwxCommander* commander,///<队列消息消费的缺省commander，若指定func可以不指定
                             CWX_TSS_THR_FUNC func, ///<用户的线程main函数
                             void*            arg ///<func的void*参数
                             ):CwxTpi(unThreadNum)
{
  m_arrTssEnv = NULL;
  m_msgQueue = new CwxMsgQueue(1024*1024*200, 1024*1024*200);
  m_commander = commander;
  m_func = func;
  m_arg = arg;
  m_threadArr =  NULL;
}

///析构函数
CwxThreadPool::~CwxThreadPool(){
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

int CwxThreadPool::start(CwxTss** pThrEnv, size_t stack_size)
{
  CwxMutexGuard<CwxMutexLock> lock(&m_lock);
  if (pThrEnv){
    m_arrTssEnv = pThrEnv;
  }else{
    m_arrTssEnv = new CwxTss*[getThreadNum()];
    memset(m_arrTssEnv, 0x00, sizeof(CwxTss*) * getThreadNum());
  }
  ///创建线程
  CWX_UINT16 index= 0;
  m_threadArr = new CwxThread*[getThreadNum()];
  for (index=0; index<getThreadNum(); index++){
    m_threadArr[index] = new CwxThread(index,
      m_commander,
      m_func,
      m_arg,
      m_msgQueue);
    if (0 != m_threadArr[index]->start(m_arrTssEnv[index], stack_size)){
      _stop();
      return -1;
    }
  }
  return 0;
}

void CwxThreadPool::stop(){
  CwxMutexGuard<CwxMutexLock> lock(&m_lock);
  _stop();
}

bool CwxThreadPool::isStop() {
  return m_msgQueue->getState() == CwxMsgQueue::DEACTIVATED;
}

CwxTss* CwxThreadPool::getTss(CWX_UINT16 unThreadIndex)
{
  if (unThreadIndex >= getThreadNum()) return NULL;
  return m_arrTssEnv[unThreadIndex];
}

///锁住整个线程池。返回值0：成功；-1：失败
int CwxThreadPool::lock(){
  return m_msgQueue->lock().acquire();
}
///解锁这个线程池。返回值0：成功；-1：失败
int CwxThreadPool::unlock(){
  return m_msgQueue->lock().release();
}


void CwxThreadPool::_stop(){
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
