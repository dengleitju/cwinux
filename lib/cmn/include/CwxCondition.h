#ifndef __CWX_CONDITION_H__
#define __CWX_CONDITION_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxCondition.h
@brief 实现了Condition的接口。
@author cwinux@gmail.com
@version 0.1
@date 2010-07-10
@warning
@bug
*/

#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxType.h"
#include "CwxMutexLock.h"
#include "CwxTimeValue.h"
CWINUX_BEGIN_NAMESPACE
/**
@class Condition
@brief Condition对象。
*/
class CWX_API CwxCondition
{
public:
    ///构造函数，创建一个线程排他锁
    CwxCondition(CwxMutexLock &lock, pthread_condattr_t * attr=NULL);
    ///析构函数
    ~CwxCondition();
public:
    /**
    @brief 阻塞直到从condition返回或者超时。
    @param [in] timeout 超时的时间，若为NULL表示持久的等待。
    @return -1：失败，若是超时，则errno为ETIMEDOUT； 0：成功
    */
    int wait (CwxTimeValue const *timeout = 0);
    /**
    @brief 通知一个thread。
    @return -1：失败； 0：成功
    */
    int signal (void);
    /**
    @brief 通知所有thread。
    @return -1：失败； 0：成功
    */
    int broadcast(void);
    ///获取锁
    CwxMutexLock  &lock();
    ///获取底层的condition
    pthread_cond_t* cond();
private:
    pthread_cond_t*  m_cond;///<condition对象
    CwxMutexLock&    m_lock;
};


CWINUX_END_NAMESPACE
#include "CwxCondition.inl"
#include "CwxPost.h"
#endif
