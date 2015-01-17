
CWINUX_BEGIN_NAMESPACE

inline int CwxRwLock::acquire()
{
    return pthread_rwlock_wrlock(m_rwLock)==0?0:-1;
}

inline int CwxRwLock::tryacquire() 
{
    return pthread_rwlock_trywrlock(m_rwLock)==0?0:-1;
}

inline int CwxRwLock::release() 
{
    return pthread_rwlock_unlock(m_rwLock)==0?0:-1;
}

inline int CwxRwLock::acquire_read()
{
    return pthread_rwlock_rdlock(m_rwLock)==0?0:-1;
}

inline int CwxRwLock::acquire_write()
{
    return acquire();
}

inline int CwxRwLock::tryacquire_read() 
{
    return pthread_rwlock_tryrdlock(m_rwLock)==0?0:-1;
}

inline int CwxRwLock::tryacquire_write()
{
    return tryacquire();
}

inline pthread_rwlock_t const &CwxRwLock::lock() const
{
    return *m_rwLock;
}


CWINUX_END_NAMESPACE
