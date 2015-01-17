#ifndef __CWX_LOCK_GUARD_H__
#define __CWX_LOCK_GUARD_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
*@file  CwxLockGuard.h
*@brief CWINUX的排他、读写锁的模板策略锁对象CwxMutexGuard
*@author cwinux@gmail.com
*@version 0.1
*@date  2009-05-30
*@warning  无.
*/

#include "CwxPre.h"
#include "CwxGlobalMacro.h"

CWINUX_BEGIN_NAMESPACE

/**
* @class CwxMutexGuard
*
* @brief 策略锁对象，在构造函数中加锁，在析构函数中解锁
*/
template <class LOCK>
class CwxMutexGuard
{
public:
    ///构造函数，若lock不为空，则加锁
    CwxMutexGuard (LOCK * lock):m_pLock(lock)
    {
        if (m_pLock) m_pLock->acquire();
    }
    ///析构函数，若锁对象不为空，则解锁
    ~CwxMutexGuard ()
    {
        if (m_pLock) m_pLock->release();
    }
private:
    ///禁止不带锁对象的默认构造函数.
    CwxMutexGuard (): m_pLock (0) {}
    ///锁对象的指针
    LOCK*      m_pLock;

};


/**
* @class CwxReadLockGuard
*
* @brief This data structure is meant to be used within a method or
* function...  It performs automatic aquisition and release of
* a parameterized synchronization object \<LOCK\>.
*
* The \<LOCK\> class given as an actual parameter must provide at
* the very least the \<acquire_read\>,  \<release\> method
*/
template <class LOCK>
class CwxReadLockGuard
{
public:
    // = Initialization and termination methods.
    CwxReadLockGuard (LOCK * lock):m_pLock(lock){
        if (m_pLock) m_pLock->acquire_read();
    }
    /// Implicitly release the lock.
    ~CwxReadLockGuard (){
        if (m_pLock) m_pLock->release();
    }
private:
    /// Helper, meant for subclass only.
    CwxReadLockGuard (): m_pLock (0) {}
    /// Pointer to the LOCK we're guarding.
    LOCK*      m_pLock;

};

/**
* @class CwxWriteLockGuard
*
* @brief This data structure is meant to be used within a method or
* function...  It performs automatic aquisition and release of
* a parameterized synchronization object \<LOCK\>.
*
* The \<LOCK\> class given as an actual parameter must provide at
* the very least the \<acquire_write\>,  \<release\> method
*/
template <class LOCK>
class CwxWriteLockGuard
{
public:
    // = Initialization and termination methods.
    CwxWriteLockGuard (LOCK * lock):m_pLock(lock){
        if (m_pLock) m_pLock->acquire_write();
    }
    /// Implicitly release the lock.
    ~CwxWriteLockGuard (){
        if (m_pLock) m_pLock->release();
    }
private:
    /// Helper, meant for subclass only.
    CwxWriteLockGuard (): m_pLock (0) {}
    /// Pointer to the LOCK we're guarding.
    LOCK*      m_pLock;

};
CWINUX_END_NAMESPACE

#include "CwxPost.h"
#endif

