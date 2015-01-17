#include <stdlib.h>
#include "CwxSingletonMgr.h"
CWINUX_BEGIN_NAMESPACE
CwxSingletonMgr* CwxSingletonMgr::m_pInstance = NULL;
CwxMutexLock CwxSingletonMgr::m_lock;


CwxSingletonMgr::CwxSingletonMgr()
{
    m_pObjs = NULL;
    atexit (CwxSingletonMgr::destroy);
}
CwxSingletonMgr::~CwxSingletonMgr()
{
    CwxSingleton* pObj = NULL;
    while(m_pObjs)
    {
        pObj = m_pObjs->m_next;
        delete m_pObjs;
        m_pObjs = pObj;
    }
    m_pObjs = NULL;
}


CwxSingletonMgr* CwxSingletonMgr::instance()
{
    if (!m_pInstance)
    {
       CwxMutexGuard<CwxMutexLock> lock(&m_lock);
       if (!m_pInstance)
       {
           m_pInstance = new CwxSingletonMgr();
       }
    }
    return m_pInstance;
}

void CwxSingletonMgr::destroy()
{
    if (m_pInstance) delete m_pInstance;
    m_pInstance = NULL;
}

void CwxSingletonMgr::reg(CwxSingleton* pObj)
{
    CwxMutexGuard<CwxMutexLock> lock(&m_lock);
    pObj->m_next = m_pObjs;
    m_pObjs = pObj;
}
CWINUX_END_NAMESPACE
