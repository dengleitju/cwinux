#ifndef __CWX_MUTEX_LOCK_H__
#define __CWX_MUTEX_LOCK_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxMutexLock.h
@brief 实现了排他锁的接口。
@author cwinux@gmail.com
@version 0.1
@date 2009-10-10
@warning
@bug
*/

#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxType.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <pthread.h>
CWINUX_BEGIN_NAMESPACE
/**
@class CwxMutexLock
@brief 排他锁对象，支持线程排他锁与进程排他锁。
*/
class CWX_API CwxMutexLock
{
public:
    ///构造函数，创建一个线程排他锁
    CwxMutexLock(pthread_mutexattr_t *arg = 0);
    ///构造函数，创建一个进程排他锁
//    CwxMutexLock(char const* path, pthread_mutexattr_t *arg = 0);
    ///析构函数
    ~CwxMutexLock();
public:
    ///获取排他锁，0：成功，-1：失败
    int acquire();
    ///检查排他锁是否空闲，0：空闲，-1：忙
    int tryacquire();
    ///释放锁，0：成功，-1：失败
    int release();
    ///获取排他锁，0：成功，-1：失败
    int acquire_read();
    ///获取排他锁，0：成功，-1：失败
    int acquire_write();
    ///检查排他锁是否空闲，0：空闲，-1：忙
    int tryacquire_read();
    ///检查排他锁是否空闲，0：空闲，-1：忙
    int tryacquire_write();
    ///获取锁句柄，为OS级的排他锁句柄
    pthread_mutex_t &lock() ;
private:
    pthread_mutex_t*  m_mutex;///<锁对象
    int              m_shm;///<共享锁的shm fd

};


CWINUX_END_NAMESPACE
#include "CwxMutexLock.inl"
#include "CwxPost.h"
#endif

