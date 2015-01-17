#include "CwxThreadPoolEx.h"

CWINUX_BEGIN_NAMESPACE
///构造函数
CwxThreadPoolEx::CwxThreadPoolEx(CWX_UINT16 unThreadNum,///<线程池中线程的数量
                 CwxCommander* commander,///<队列消息消费的缺省commander，若指定func可以不指定
                 CWX_TSS_THR_FUNC func, ///<用户的线程main函数
                 void*            arg ///<func的void*参数
                   ): CwxTpi(unThreadNum)
{
    m_commander = commander;
    m_func = func;
    m_arg = arg;
    m_threadArr =  NULL;
}

///析构函数
CwxThreadPoolEx::~CwxThreadPoolEx()
{
    if (m_threadArr){
        for (CWX_UINT16 i=0; i<getThreadNum(); i++) {
            if (m_threadArr[i]) delete m_threadArr[i];
        }
        delete [] m_threadArr;
    }
}

int CwxThreadPoolEx::start(CwxTss** pThrEnv, size_t stack_size)
{
    CWX_UINT16 i = 0;
    m_threadArr = new CwxThread*[getThreadNum()];
    for (i=0; i<getThreadNum(); i++) {
        m_threadArr[i] =  new CwxThread(i,
            m_commander,
            m_func,
            m_arg);
        if (0 != m_threadArr[i]->start(pThrEnv?pThrEnv[i]:NULL, stack_size)) {
            return -1;
        }
    }
    if (pThrEnv) delete [] pThrEnv;
    return 0;
}

void CwxThreadPoolEx::stop()
{
    for (CWX_UINT16 i=0; i<getThreadNum(); i++)
    {
        if (m_threadArr && m_threadArr[i]) m_threadArr[i]->stop();
    }
}

bool CwxThreadPoolEx::isStop()
{
    for (CWX_UINT16 i=0; i<getThreadNum(); i++) {
        if (m_threadArr && m_threadArr[i]) {
            if (m_threadArr[i]->isStop()) return true;
        }
    }
    return false;
}

CwxTss* CwxThreadPoolEx::getTss(CWX_UINT16 unThreadIndex) {
    if (m_threadArr && (unThreadIndex<getThreadNum()) && m_threadArr[unThreadIndex])
    {
        return m_threadArr[unThreadIndex]->getTss();
    }
    return NULL;
}
///锁住整个线程池。返回值0：成功；-1：失败
int CwxThreadPoolEx::lock()
{
    for (CWX_UINT16 i=0; i<getThreadNum(); i++)
    {
        if (m_threadArr && m_threadArr[i]) 
        {
            if (0 != m_threadArr[i]->lock())
            {
                for (CWX_UINT16 j=0; j<i; j++)
                {
                    if (m_threadArr[j]) m_threadArr[j]->unlock();
                }
                return -1;
            }
        }
    }
    return 0;
}
///解锁这个线程池。返回值0：成功；-1：失败
int CwxThreadPoolEx::unlock()
{
    for (CWX_UINT16 i=0; i<getThreadNum(); i++)
    {
        if (m_threadArr && m_threadArr[i]) 
        {
            m_threadArr[i]->unlock();
        }
    }
    return 0;
}

CWINUX_END_NAMESPACE
