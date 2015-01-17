#include  "CwxMutexIdLocker.h"

CWINUX_BEGIN_NAMESPACE

CwxMutexIdLockMgr* CwxMutexIdLockMgr::m_pInstance = NULL;
CwxMutexLock  CwxMutexIdLockMgr::m_lock;

CwxMutexIdLockMgr::CwxMutexIdLockMgr()
:CwxSingleton("CwxMutexIdLockMgr")
{
    m_unFreeMutexNum = 0;
    m_pListLocks = NULL;
    m_pLockMap = NULL;
}

CwxMutexIdLockMgr::~CwxMutexIdLockMgr()
{
    m_unFreeMutexNum = 0;
    if (m_pListLocks)
    {
        list<CwxMutexObj*>::iterator iter = m_pListLocks->begin();
        while(iter != m_pListLocks->end())
        {
            delete *iter;
            iter++;
        }
        delete m_pListLocks;
    }
    m_pListLocks = NULL;
    if (m_pLockMap) delete m_pLockMap;
    m_pLockMap = NULL;
}

void CwxMutexIdLockMgr::init()
{
    m_unFreeMutexNum = 0;
    m_pListLocks = new list<CwxMutexObj*>();
    m_pLockMap = new hash_map<CWX_UINT64, CwxMutexObj* , CwxNumHash<CWX_UINT64> >(HASH_BUCKET_SIZE);
}


CWINUX_END_NAMESPACE


