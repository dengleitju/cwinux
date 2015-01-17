#include "CwxMutexLock.h"
CWINUX_BEGIN_NAMESPACE

CwxMutexLock::CwxMutexLock(pthread_mutexattr_t *arg)
{
    m_mutex = new pthread_mutex_t;
    pthread_mutex_init(m_mutex, arg);
}

CwxMutexLock::~CwxMutexLock()
{
    pthread_mutex_destroy(m_mutex);
    delete m_mutex;
};

CWINUX_END_NAMESPACE

