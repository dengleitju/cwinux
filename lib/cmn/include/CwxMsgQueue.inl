CWINUX_BEGIN_NAMESPACE

///关闭队列，同时释放所有消息。-1：失败；>=0：队列中的消息数量
inline int CwxMsgQueue::close()
{
    CwxMutexGuard<CwxMutexLock> guard(&this->m_lock);
    _deactivate();
    return _flush();
}
///只释放队列中的所有消息。-1：失败；>=0：队列中的消息数量
inline int CwxMsgQueue::flush (void)
{
    CwxMutexGuard<CwxMutexLock> guard(&this->m_lock);
    return _flush();
}

///检查队列是否full
inline bool CwxMsgQueue::isFull (void)
{
    CwxMutexGuard<CwxMutexLock> guard(&this->m_lock);
    return _isFull();
}
///检查队列是否空.
inline bool CwxMsgQueue::isEmpty(void)
{
    CwxMutexGuard<CwxMutexLock> guard(&this->m_lock);
    return _isEmpty();
}
///获取所有消息的空间大小.
inline size_t CwxMsgQueue::getMsgBytes(void)
{
    return m_curMsgBytes;
}
///获取所有Msg的length.
inline size_t CwxMsgQueue::getMsgLength(void)
{
    return m_curLength;
}
///获取消息的数量.
inline size_t CwxMsgQueue::getMsgCount(void)
{
    return m_curCount;
}


inline int CwxMsgQueue::deactivate (void)
{
    CwxMutexGuard<CwxMutexLock> guard(&this->m_lock);
    return _deactivate();
}

inline int CwxMsgQueue::activate (void)
{
    CwxMutexGuard<CwxMutexLock> guard(&this->m_lock);
    return _activate();
}

inline int CwxMsgQueue::getState (void)
{
    return m_state;
}

inline bool CwxMsgQueue::isDeactived(void)
{
    return m_state == DEACTIVATED;
}

inline bool CwxMsgQueue::isActivate(void)
{
    return m_state == ACTIVATED;
}

///获取high water mark
inline size_t CwxMsgQueue::getHwm(void) const
{
    return m_highWaterMark;
}
///设置high water mark
inline void CwxMsgQueue::setHwm(size_t hwm)
{
    m_highWaterMark = hwm;
}
///获取low water mark
inline size_t CwxMsgQueue::getLwm(void) const
{
    return m_lowWaterMark;
}
///设置low water mark
inline void CwxMsgQueue::setLwm(size_t lwm)
{
    m_lowWaterMark = lwm;
}

/// 获取锁.
inline CwxMutexLock& CwxMsgQueue::lock()
{
    return m_lock;
}


inline int CwxMsgQueue::_deactivate(void)
{
    int const previous_state = this->m_state;
    if (this->m_state != DEACTIVATED)
    {
        // Wakeup all waiters.
        this->m_notEmptyCond.broadcast();
        this->m_notFullCond.broadcast();
        this->m_state = DEACTIVATED;
    }
    return previous_state;
}

inline int CwxMsgQueue::_activate(void)
{
    int const previous_state = this->m_state;
    this->m_state = ACTIVATED;
    return previous_state;

}

inline int CwxMsgQueue::_flush(void)
{
    int number_flushed = 0;
    CwxMsgBlock* tmp = NULL;
    while(this->m_head)
    {
        tmp = m_head->m_next;
        CwxMsgBlockAlloc::free(m_head);
        m_head = tmp;
    }
    number_flushed = m_curCount;
    m_head = NULL;
    m_tail = NULL;
    m_curMsgBytes = 0;
    m_curLength = 0;
    m_curCount = 0;
    return number_flushed;
}

///检查队列是否full
inline bool CwxMsgQueue::_isFull (void)
{
    return m_curLength >= m_highWaterMark;
}
///检查队列是否空.
inline bool CwxMsgQueue::_isEmpty(void)
{
    return !m_head;
}

inline int CwxMsgQueue::_waitNotFullCond(CwxTimeValue *timeout)
{
    // Wait while the queue is full.
    while (this->_isFull())
    {
        if (this->m_notFullCond.wait(timeout) == -1)
        {
            return -1;
        }
        if (this->m_state != ACTIVATED)
        {
            return -1;
        }
    }
    return 0;

}

///等待队列消息
inline int CwxMsgQueue::_waitNotEmptyCond(CwxTimeValue *timeout)
{
    // Wait while the queue is full.
    while (this->_isEmpty())
    {
        if (this->m_notEmptyCond.wait(timeout) == -1)
        {
            return -1;
        }
        if (this->m_state != ACTIVATED)
        {
            return -1;
        }
    }
    return 0;
}


// = Disallow copying and assignment.
inline CwxMsgQueue::CwxMsgQueue (const CwxMsgQueue &)
:m_notEmptyCond(m_lock),m_notFullCond(m_lock)
{
}

inline CwxMsgQueue& CwxMsgQueue::operator= (const CwxMsgQueue &)
{
    return *this;
}


CWINUX_END_NAMESPACE
