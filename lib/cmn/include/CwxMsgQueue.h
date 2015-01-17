#ifndef  __CWX_MSG_QUEUE_H__
#define  __CWX_MSG_QUEUE_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
*@file CwxMsgQueue.h
*@brief CwxMsgQueue定义
*@author cwinux@gmail.com
*@version 1.0
*@date  2010-07-05
*@warning  nothing
*@bug    
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxCommon.h"
#include "CwxCondition.h"
#include "CwxTimeValue.h"
#include "CwxMsgBlock.h"

CWINUX_BEGIN_NAMESPACE

/**
*@class CwxMsgQueue
*@brief 生产者、消费者队列。
*/
class CWX_API CwxMsgQueue
{
public:
    enum
    {
        ///缺省的队列空间的上限，超过上限push消息时阻塞.
        DEFAULT_HWM = 1024 * 1024,
        ///缺省的队列空间的下限，超过上限后，只有低于下限才能push消息.
        DEFAULT_LWM = 1024 * 1024,
        ///活动状态
        ACTIVATED = 1,
        ///deactivated状态.
        DEACTIVATED = 2
    };
public:
    CwxMsgQueue(size_t hwm=DEFAULT_HWM, size_t lwm=DEFAULT_LWM);
    ~CwxMsgQueue();
public:
    ///关闭队列，同时释放所有消息。-1：失败；>=0：队列中的消息数量
    int close();
    ///只释放队列中的所有消息。-1：失败；>=0：队列中的消息数量
    int flush (void);
    ///将一个消息放到队列的头部。-1：失败；>=0：队列中的消息数量
    int enqueue (CwxMsgBlock *msg,
        CwxTimeValue *timeout = 0);
    ///将一个消息放到队列的尾部。-1：失败；>=0：队列中的消息数量
    int enqueue_tail (CwxMsgBlock * msg,
        CwxTimeValue *timeout = 0);
    ///从队列的头部获取一个消息。-1：失败；>=0：队列中的消息数量
    int dequeue (CwxMsgBlock *&msg,
        CwxTimeValue *timeout = 0);
    ///检查队列是否full
    bool isFull (void);
    ///检查队列是否空.
    bool isEmpty(void);
    ///获取所有消息的空间大小.
    size_t getMsgBytes(void);
    ///获取所有Msg的length.
    size_t getMsgLength(void);
    ///获取消息的数量.
    size_t getMsgCount(void);
    ///deactive 消息队列，-1：失败；否则返回先前的状态
    int deactivate (void);
    ///active消息队列，-1：失败；否则返回先前的状态
    int activate (void);
    ///获取消息队列的状态.
    int getState (void);
    ///检查队列是否deactive状态
    bool isDeactived(void);
    ///检查队列是否active状态
    bool isActivate(void);
    ///获取high water mark
    size_t getHwm(void) const;
    ///设置high water mark
    void setHwm(size_t hwm);
    ///获取low water mark
    size_t getLwm(void) const;
    ///设置low water mark
    void setLwm(size_t lwm);
    /// 获取锁.
    CwxMutexLock& lock();

private:
    ///deactive 消息队列，-1：失败；否则返回先前的状态
    int _deactivate(void);
    ///active消息队列，-1：失败；否则返回先前的状态
    int _activate(void);
    ///只释放队列中的所有消息。-1：失败；>=0：队列中的消息数量
    int _flush(void);
    ///检查队列是否full
    bool _isFull (void);
    ///检查队列是否空.
    bool _isEmpty(void);
    ///等待队列有空位置
    int _waitNotFullCond(CwxTimeValue *timeout = 0);
    ///等待队列消息
    int _waitNotEmptyCond(CwxTimeValue *timeout = 0);

    // = Disallow copying and assignment.
    CwxMsgQueue (const CwxMsgQueue &);
    CwxMsgQueue& operator= (const CwxMsgQueue &);

protected:
    /// 队列的状态.
    int m_state;
    /// 消息队列链表的头.
    CwxMsgBlock* m_head;
    /// 消息队列链表的尾.
    CwxMsgBlock* m_tail;
    /// low water mark.
    size_t m_lowWaterMark;
    /// High water mask.
    size_t m_highWaterMark;
    /// 当前队列中的msg block的空间字节总和.
    size_t m_curMsgBytes;
    /// 当前队列中的msg的长度总和.
    size_t m_curLength;
    /// 队列中msg的数量.
    size_t m_curCount;
    /// 同步保护的锁.
    CwxMutexLock m_lock;
    /// Used to make threads sleep until the queue is no longer empty.
    CwxCondition m_notEmptyCond;
    /// Used to make threads sleep until the queue is no longer full.
    CwxCondition m_notFullCond;
};


CWINUX_END_NAMESPACE

#include "CwxMsgQueue.inl"

#include "CwxPost.h"

#endif
