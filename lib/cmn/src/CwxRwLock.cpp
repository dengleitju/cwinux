#include "CwxRwLock.h"

CWINUX_BEGIN_NAMESPACE

CwxRwLock::CwxRwLock(pthread_rwlockattr_t *arg )
{
    m_rwLock = new pthread_rwlock_t;
    pthread_rwlock_init(m_rwLock, arg);

}

CwxRwLock::~CwxRwLock()
{
    pthread_rwlock_destroy(m_rwLock);
    delete m_rwLock;
}


CWINUX_END_NAMESPACE
