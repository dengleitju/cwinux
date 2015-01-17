CWINUX_BEGIN_NAMESPACE

///构造函数，创建一个线程排他锁
inline CwxCondition::CwxCondition(CwxMutexLock &lock, pthread_condattr_t * attr):m_lock(lock)
{
    m_cond = (pthread_cond_t*)malloc(sizeof (pthread_cond_t));
    pthread_cond_init(this->m_cond, attr);
}
///析构函数
inline CwxCondition::~CwxCondition()
{
    pthread_cond_destroy(this->m_cond);
    free(m_cond);
}

inline int CwxCondition::wait (CwxTimeValue const *timeout)
{
    int ret = 0;
    if (timeout)
    {
        struct timespec out;
        out.tv_sec = timeout->sec();
        // Convert microseconds into nanoseconds.
        out.tv_nsec = timeout->usec() * 1000;
        ret = pthread_cond_timedwait(m_cond, &m_lock.lock(), &out);
    }
    else
    {
        ret = pthread_cond_wait(m_cond, &m_lock.lock());
    }
    if (0 != ret)
    {
        errno = ret;
        ret = -1;
    }
    return ret;
}

inline int CwxCondition::signal (void)
{
    int ret =  pthread_cond_signal(m_cond);
    if (0 != ret)
    {
        errno = ret;
        ret = -1;
    }
    return ret;
}

inline int CwxCondition::broadcast(void)
{
    int ret =  pthread_cond_broadcast(m_cond);
    if (0 != ret)
    {
        errno = ret;
        ret = -1;
    }
    return ret;
}
///获取锁句柄
inline CwxMutexLock  &CwxCondition::lock() 
{
    return m_lock;
}

inline pthread_cond_t* CwxCondition::cond()
{
    return m_cond;
}

CWINUX_END_NAMESPACE

