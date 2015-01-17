CWINUX_BEGIN_NAMESPACE
///创建signleton CwxRwIdLockMgr 对象
inline CwxMutexIdLockMgr* CwxMutexIdLockMgr::instance()
{
    if (!m_pInstance)
    {
        CwxMutexGuard<CwxMutexLock> lock(&m_lock);
        if (!m_pInstance)
        {
            m_pInstance = new CwxMutexIdLockMgr();
            m_pInstance->init();
        }
    }
    return m_pInstance;
}

inline int CwxMutexIdLockMgr::lock(CWX_UINT64  id)
{
    CwxMutexObj* pLock = NULL;
    {
        CwxMutexGuard<CwxMutexLock> lock(&m_mutex);
        hash_map<CWX_UINT64, CwxMutexObj* , CwxNumHash<CWX_UINT64> >::iterator iter = m_pLockMap->find(id);
        if (iter == m_pLockMap->end())
        {//not find
            if (m_unFreeMutexNum)
            {
                list<CwxMutexObj*>::iterator lockIter= m_pListLocks->begin();
                CWX_ASSERT(lockIter != m_pListLocks->end());
                pLock = *lockIter;
                CWX_ASSERT(0 == pLock->m_unNum);
                m_pListLocks->pop_front();
                m_unFreeMutexNum--;
            }
            else
            {
                pLock = new CwxMutexObj();
            }
            pLock->m_unNum = 1;
            (*m_pLockMap)[id] = pLock;
        }
        else
        {
            pLock = iter->second;
            pLock->m_unNum++;
        }
    }
    pLock->m_lock.acquire();
}


inline int CwxMutexIdLockMgr::unlock(CWX_UINT64 id)
{
    int ret = 0;
    //get global lock.
    CwxMutexObj* pLock = NULL;
    CwxMutexGuard<CwxMutexLock> lock(&m_mutex);
    hash_map<CWX_UINT64, CwxMutexObj* , CwxNumHash<CWX_UINT64> >::iterator iter = m_pLockMap->find(id);

    if (iter == m_pLockMap->end()) return -1;

    pLock = (*iter).second;
    CWX_ASSERT(pLock->m_unNum>0);
    //find
    ret = pLock->m_lock.release();
    if (0 == ret)
    {
        pLock->m_unNum--;
        if (0 == pLock->m_unNum)
        {//not used
            m_pLockMap->erase(iter);
            if (m_unFreeMutexNum >= MAX_FREE_MUTEX_NUM)
            {
                delete pLock;
            }
            else
            {
                m_pListLocks->push_front(pLock);
                m_unFreeMutexNum++;
            }
        }
    }
    return ret;
}

CWINUX_END_NAMESPACE

