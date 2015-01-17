#include  "CwxRwIdLocker.h"

CWINUX_BEGIN_NAMESPACE

CwxRwIdLockMgr* CwxRwIdLockMgr::m_pInstance = NULL;
CwxMutexLock  CwxRwIdLockMgr::m_lock;

CwxRwIdLockMgr::CwxRwIdLockMgr()
:CwxSingleton("CwxRwIdLockMgr")
{
    m_unFreeMutexNum = 0;
    m_pListLocks = NULL;
    m_pLockMap = NULL;
}

CwxRwIdLockMgr::~CwxRwIdLockMgr()
{
    m_unFreeMutexNum = 0;
    if (m_pListLocks)
    {
        list<CwxRwObj*>::iterator iter = m_pListLocks->begin();
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

void CwxRwIdLockMgr::init()
{
    m_unFreeMutexNum = 0;
    m_pListLocks = new list<CwxRwObj*>();
    m_pLockMap = new hash_map<CWX_UINT64, CwxRwObj* , CwxNumHash<CWX_UINT64> >(HASH_BUCKET_SIZE);
}



CWINUX_END_NAMESPACE


