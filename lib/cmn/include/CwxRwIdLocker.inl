CWINUX_BEGIN_NAMESPACE

///创建signleton CwxRwIdLockMgr 对象
inline CwxRwIdLockMgr* CwxRwIdLockMgr::instance()
{
    if (!m_pInstance)
    {
        CwxMutexGuard<CwxMutexLock> lock(&m_lock);
        if (!m_pInstance)
        {
            m_pInstance = new CwxRwIdLockMgr();
            m_pInstance->init();
        }
    }
    return m_pInstance;
}

inline int CwxRwIdLockMgr::rdLock(CWX_UINT64  id)
{
    CwxRwObj* pLock = NULL;
    {
        CwxMutexGuard<CwxMutexLock> lock(&m_mutex);
        hash_map<CWX_UINT64, CwxRwObj* , CwxNumHash<CWX_UINT64> >::iterator iter = m_pLockMap->find(id);
        if (iter == m_pLockMap->end())
        {//not find
            if (m_unFreeMutexNum)
            {
                list<CwxRwObj*>::iterator lockIter= m_pListLocks->begin();
                CWX_ASSERT(lockIter != m_pListLocks->end());
                pLock = *lockIter;
                CWX_ASSERT(0 == pLock->m_unNum);
                m_pListLocks->pop_front();
                m_unFreeMutexNum--;
            }
            else
            {
                pLock = new CwxRwObj();
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
    pLock->m_lock.acquire_read();
}

inline int CwxRwIdLockMgr::wrLock(CWX_UINT64  id)
{
    CwxRwObj* pLock = NULL;
    {
        CwxMutexGuard<CwxMutexLock> lock(&m_mutex);
        hash_map<CWX_UINT64, CwxRwObj* , CwxNumHash<CWX_UINT64> >::iterator iter = m_pLockMap->find(id);
        if (iter == m_pLockMap->end())
        {//not find
            if (m_unFreeMutexNum)
            {
                list<CwxRwObj*>::iterator lockIter= m_pListLocks->begin();
                CWX_ASSERT(lockIter != m_pListLocks->end());
                pLock = *lockIter;
                CWX_ASSERT(0 == pLock->m_unNum);
                m_pListLocks->pop_front();
                m_unFreeMutexNum--;
            }
            else
            {
                pLock = new CwxRwObj();
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
    return pLock->m_lock.acquire_write();
}

inline int CwxRwIdLockMgr::unlock(CWX_UINT64 id)
{
    int ret = 0;
    //get global lock.
    CwxRwObj* pLock = NULL;
    CwxMutexGuard<CwxMutexLock> lock(&m_mutex);
    hash_map<CWX_UINT64, CwxRwObj* , CwxNumHash<CWX_UINT64> >::iterator iter = m_pLockMap->find(id);

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

