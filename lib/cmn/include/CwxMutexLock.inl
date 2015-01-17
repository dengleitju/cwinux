CWINUX_BEGIN_NAMESPACE

inline int CwxMutexLock::acquire()
{
    return pthread_mutex_lock(m_mutex)==0?0:-1;
}

inline int CwxMutexLock::tryacquire()
{
    return pthread_mutex_trylock(m_mutex)==0?0:-1;
}

inline int CwxMutexLock::release() 
{
    return pthread_mutex_unlock(m_mutex)==0?0:-1;
}

inline int CwxMutexLock::acquire_read()
{
    return acquire();
}

inline int CwxMutexLock::acquire_write()
{
    return acquire();
}

inline int CwxMutexLock::tryacquire_read() 
{
    return tryacquire();
}

inline int CwxMutexLock::tryacquire_write()
{
    return tryacquire();
}

inline pthread_mutex_t & CwxMutexLock::lock()
{
    return *m_mutex;
}


CWINUX_END_NAMESPACE

