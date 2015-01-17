#include "CwxMsgQueue.h"

CWINUX_BEGIN_NAMESPACE

CwxMsgQueue::CwxMsgQueue(size_t hwm, size_t lwm)
:m_notEmptyCond(m_lock),m_notFullCond(m_lock)
{
    m_state = ACTIVATED;
    m_head = NULL;
    m_tail = NULL;
    m_lowWaterMark = hwm;
    m_highWaterMark = lwm;
    m_curMsgBytes = 0;
    m_curLength = 0;
    m_curCount = 0; 
}
CwxMsgQueue::~CwxMsgQueue()
{
    if (m_head)
    {
        this->close();
    }
}

///将一个消息放到队列的头部。-1：失败；>=0：队列中的消息数量
int CwxMsgQueue::enqueue (CwxMsgBlock *msg,
                             CwxTimeValue *timeout)
{
    int queue_count = 0;
    {
        CwxMutexGuard<CwxMutexLock> guard(&this->m_lock);

        if (DEACTIVATED == this->m_state)
        {
            return -1;
        }
        if (this->_waitNotFullCond(timeout) == -1)
            return -1;
        if (!msg) return -1;
        m_curCount ++;
        m_curLength += msg->length();
        m_curMsgBytes += msg->capacity();
        msg->m_next = m_head;
        m_head = msg;
        if (!m_tail) m_tail = m_head;
        if (1 ==  m_curCount)
        {
            if (-1 == m_notEmptyCond.signal()) return -1;
        }
        queue_count = m_curCount;
    }
    return queue_count;
}


///将一个消息放到队列的尾部。-1：失败；>=0：队列中的消息数量
int CwxMsgQueue::enqueue_tail(CwxMsgBlock * msg,
                  CwxTimeValue *timeout)
{
    int queue_count = 0;
    {
        CwxMutexGuard<CwxMutexLock> guard(&this->m_lock);

        if (DEACTIVATED == this->m_state)
        {
            return -1;
        }
        if (this->_waitNotFullCond(timeout) == -1)
            return -1;
        if (!msg) return -1;
        msg->m_next = NULL;
        m_curCount ++;
        m_curLength += msg->length();
        m_curMsgBytes += msg->capacity();
        if (m_tail)
        {
            m_tail->m_next = msg;
        }
        else
        {
           m_head = msg;
        }
        m_tail = msg;
        if (1 ==  m_curCount)
        {
            if (-1 == m_notEmptyCond.signal()) return -1;
        }
        queue_count = m_curCount;
    }
    return queue_count;

}


///从队列的头部获取一个消息。-1：失败；>=0：队列中的消息数量
int CwxMsgQueue::dequeue (CwxMsgBlock *&msg,
                  CwxTimeValue *timeout)
{
    int queue_count = 0;
    bool bFull = false;
    {
        CwxMutexGuard<CwxMutexLock> guard(&this->m_lock);

        if (DEACTIVATED == this->m_state)
        {
            return -1;
        }
        if (this->_waitNotEmptyCond(timeout) == -1)
            return -1;
        bFull = _isFull();
        msg = m_head;
        m_head = msg->m_next;
        if (!m_head) m_tail = NULL;
        m_curCount --;
        m_curLength -= msg->length();
        m_curMsgBytes -= msg->capacity();
        if (bFull && !_isFull())
        {
            if (-1 == m_notFullCond.signal()) return -1;
        }
        queue_count = m_curCount;
    }
    return queue_count;

}


CWINUX_END_NAMESPACE
