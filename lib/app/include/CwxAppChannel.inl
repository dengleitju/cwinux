
CWINUX_BEGIN_NAMESPACE

///注册IO事件处理handle
inline int CwxAppChannel::registerHandler (CWX_HANDLE io_handle,
                                           CwxAppHandler4Channel *event_handler,
                                           int mask,
                                           CWX_UINT32 uiMillSecond)
{
    if (!CwxThread::equal(m_owner, CwxThread::self()))
    {
        int ret = 0;
        m_rwLock.acquire_read();
        this->notice();
        {
            CwxMutexGuard<CwxMutexLock> lock(&m_lock);
            ret =  _registerHandler(io_handle, event_handler, mask, uiMillSecond);
        }
        m_rwLock.release();
        return ret;
    }
    return _registerHandler(io_handle, event_handler, mask, uiMillSecond);
}
///删除io事件处理handle
inline int CwxAppChannel::removeHandler (CwxAppHandler4Channel *event_handler)
{
    if (!CwxThread::equal(m_owner, CwxThread::self()))
    {
        int ret = 0;
        m_rwLock.acquire_read();
        this->notice();
        {
            CwxMutexGuard<CwxMutexLock> lock(&m_lock);
            ret = _removeHandler(event_handler);
        }
        m_rwLock.release();
        return ret;
    }
    return _removeHandler(event_handler);
}

///注册IO事件处理handle
inline int CwxAppChannel::suspendHandler (CwxAppHandler4Channel *event_handler, int suspend_mask)
{
    if (!CwxThread::equal(m_owner, CwxThread::self()))
    {
        int ret = 0;
        m_rwLock.acquire_read();
        this->notice();
        {
            CwxMutexGuard<CwxMutexLock> lock(&m_lock);
            ret = _suspendHandler(event_handler, suspend_mask);
        }
        m_rwLock.release();
        return ret;
    }
    return _suspendHandler(event_handler, suspend_mask);
}
///删除io事件处理handle
inline int CwxAppChannel::resumeHandler (CwxAppHandler4Channel *event_handler, int resume_mask)
{
    if (!CwxThread::equal(m_owner, CwxThread::self()))
    {
        int ret = 0;
        m_rwLock.acquire_read();
        this->notice();
        {
            CwxMutexGuard<CwxMutexLock> lock(&m_lock);
            ret = _resumeHandler(event_handler, resume_mask);
        }
        m_rwLock.release();
        return ret;
    }
    return _resumeHandler(event_handler, resume_mask);
}


inline CwxAppHandler4Channel* CwxAppChannel::removeHandler (CWX_HANDLE handle)
{
    if (!CwxThread::equal(m_owner, CwxThread::self()))
    {
        CwxAppHandler4Channel* ret = NULL;
        m_rwLock.acquire_read();
        this->notice();
        {
            CwxMutexGuard<CwxMutexLock> lock(&m_lock);
            ret = _removeHandler(handle);
        }
        m_rwLock.release();
        return ret;
    }
    return _removeHandler(handle);
}

inline int CwxAppChannel::suspendHandler (CWX_HANDLE handle,
                                          int suspend_mask)
{
    if (!CwxThread::equal(m_owner, CwxThread::self()))
    {
        int ret = 0;
        m_rwLock.acquire_read();
        this->notice();
        {
            CwxMutexGuard<CwxMutexLock> lock(&m_lock);
            ret =  _suspendHandler(handle, suspend_mask);
        }
        m_rwLock.release();
        return ret;
    }
    return _suspendHandler(handle, suspend_mask);
}

inline int CwxAppChannel::resumeHandler (CWX_HANDLE handle,
                                         int resume_mask)
{
    if (!CwxThread::equal(m_owner, CwxThread::self()))
    {
        int ret = 0;
        m_rwLock.acquire_read();
        this->notice();
        {
            CwxMutexGuard<CwxMutexLock> lock(&m_lock);
            ret = _resumeHandler(handle, resume_mask);
        }
        m_rwLock.release();
        return ret;
    }
    return _resumeHandler(handle, resume_mask);

}

///设置定时处理handle
inline int CwxAppChannel::scheduleTimer (CwxAppHandler4Channel *event_handler,
                                         CwxTimeValue const&interval)
{
    if (!CwxThread::equal(m_owner, CwxThread::self()))
    {
        int ret = 0;
        m_rwLock.acquire_read();
        this->notice();
        {
            CwxMutexGuard<CwxMutexLock> lock(&m_lock);
            ret = _scheduleTimer(event_handler, interval);
        }
        m_rwLock.release();
        return ret;
    }
    return _scheduleTimer(event_handler, interval);
}

///取消定时处理handle
inline int CwxAppChannel::cancelTimer (CwxAppHandler4Channel *event_handler)
{
    if (!CwxThread::equal(m_owner, CwxThread::self()))
    {
        int ret = 0;
        m_rwLock.acquire_read();
        this->notice();
        {
            CwxMutexGuard<CwxMutexLock> lock(&m_lock);
            ret = _cancelTimer(event_handler);
        }
        m_rwLock.release();
        return ret;
    }
    return _cancelTimer(event_handler);
}

inline bool CwxAppChannel::regRedoHander(CwxAppHandler4Channel* event_handler)
{
    if (!CwxThread::equal(m_owner, CwxThread::self()))
    {
        int ret = 0;
        m_rwLock.acquire_read();
        this->notice();
        {
            CwxMutexGuard<CwxMutexLock> lock(&m_lock);
            ret = _regRedoHander(event_handler);
        }
        m_rwLock.release();
        return ret;
    }
    return _regRedoHander(event_handler);
}
/**
@brief 删除redo handler。多线程安全，任意线程都可以调用。
@param [in] event_handler redo的event handler
@return true：成功；false：不存在；
*/
inline bool CwxAppChannel::eraseRedoHander(CwxAppHandler4Channel* event_handler)
{
    if (!CwxThread::equal(m_owner, CwxThread::self()))
    {
        int ret = 0;
        m_rwLock.acquire_read();
        this->notice();
        {
            CwxMutexGuard<CwxMutexLock> lock(&m_lock);
            ret = _eraseRedoHander(event_handler);
        }
        m_rwLock.release();
        return ret;
    }
    return _eraseRedoHander(event_handler);

}


inline int CwxAppChannel::notice()
{
    write(m_noticeFd[1], "1", 1);
    return 0;
}


/**
@brief 检查指定IO的handle是否已经注册。
@return true：注册；false：没有注册
*/
inline bool CwxAppChannel::isRegIoHandle(CWX_HANDLE handle)
{
    return _isRegIoHandle(handle);
}


///Return the ID of the "owner" thread.
inline pthread_t CwxAppChannel::getOwner() const
{
    return m_owner;
}
///是否stop
inline bool CwxAppChannel::isStop()
{
    return m_bStop;
}

///获取当前的时间
inline CwxTimeValue const& CwxAppChannel::getCurTime() const
{
    return m_engine->getCurTime();
}

///io handle是否设置指定的mask
inline bool CwxAppChannel::isMask(CWX_HANDLE handle, int mask)
{
    return CWX_CHECK_ATTR(m_engine->m_eHandler[handle].m_mask, mask);
}


///注册IO事件处理handle
inline int CwxAppChannel::_registerHandler (CWX_HANDLE io_handle,
                                            CwxAppHandler4Channel *event_handler,
                                            int mask,
                                            CWX_UINT32 uiMillSecond)
{
    int ret = 0;
    if (_isRegIoHandle(io_handle))
    {
        CWX_ERROR(("Handle[%d] exist", (int)io_handle));
        return -1;
    }
    event_handler->setHandle(io_handle);
    mask &=CwxAppHandler4Base::IO_MASK; ///只支持READ、WRITE、PERSIST、TIMEOUT四种掩码
    ret = m_engine->registerHandler(io_handle,
        event_handler,
        mask,
        uiMillSecond);
    if (0 != ret)
    {
        CWX_ERROR(("Failure to add event handler to event-base, handle[%d], errno=%d",
            (int)io_handle, 
            errno));
    }
    return ret==0?0:-1;
}

///删除io事件处理handle
inline int CwxAppChannel::_removeHandler (CwxAppHandler4Channel *event_handler)
{
    return _removeHandler(event_handler->getHandle())?0:-1;
}

///注册IO事件处理handle
inline int CwxAppChannel::_suspendHandler (CwxAppHandler4Channel *event_handler, int suspend_mask)
{
    int ret=0;
    if (!_isRegIoHandle(event_handler))
    {
        CWX_ERROR(("event handle[%d] doesn't exist", (int)event_handler->getHandle()));
        return -1;
    }
    ret = m_engine->suspendHandler(event_handler->getHandle(), suspend_mask);
    if (0 != ret)
    {
        CWX_ERROR(("Failure to suspend handle[%d], errno=%d",
            (int)event_handler->getHandle(),
            errno));
        return -1;
    }
    return 0;
}

///删除io事件处理handle
inline int CwxAppChannel::_resumeHandler (CwxAppHandler4Channel *event_handler, int resume_mask)
{
    int ret = 0;
    if (!_isRegIoHandle(event_handler))
    {
        CWX_ERROR(("event handle[%d] doesn't exist", (int)event_handler->getHandle()));
        return -1;
    }
    ret = m_engine->resumeHandler(event_handler->getHandle(), resume_mask);
    if (0 != ret)
    {
        CWX_ERROR(("Failure to resume handle[%d] from event base, errno=%d",
            (int)event_handler->getHandle(),
            errno));
        return -1;
    }
    return 0;
}


///删除io事件处理handle。
inline CwxAppHandler4Channel* CwxAppChannel::_removeHandler (CWX_HANDLE handle)
{
    if (handle >= CWX_APP_MAX_IO_NUM)
    {
        CWX_ERROR(("Handle[%d] exceed the max handle-no[%d]",
            (int)handle,
            CWX_APP_MAX_IO_NUM));
        return NULL;
    }
    CwxAppHandler4Channel* handler= (CwxAppHandler4Channel*)m_engine->removeHandler(handle);
    if (!handler)
    {
        CWX_DEBUG(("Handle[%d] doesn't exist", (int)handle));
        return NULL;
    }
    return handler;
}

///suspend io事件处理handle。
inline int CwxAppChannel::_suspendHandler (CWX_HANDLE handle,
                                           int suspend_mask)
{
    if (handle >= CWX_APP_MAX_IO_NUM)
    {
        CWX_ERROR(("Handle[%d] exceed the max handle-no[%d]",
            (int)handle,
            CWX_APP_MAX_IO_NUM));
        return -1;
    }
    return m_engine->suspendHandler(handle, suspend_mask);
}

///resume io事件处理handle。
inline int CwxAppChannel::_resumeHandler (CWX_HANDLE handle,
                                          int resume_mask)
{
    if (handle >= CWX_APP_MAX_IO_NUM)
    {
        CWX_ERROR(("Handle[%d] exceed the max handle-no[%d]",
            (int)handle,
            CWX_APP_MAX_IO_NUM));
        return -1;
    }
    return m_engine->resumeHandler(handle, resume_mask);
}



///设置定时处理handle
inline int CwxAppChannel::_scheduleTimer (CwxAppHandler4Channel *event_handler,
                                          CwxTimeValue const&interval)
{
    int ret = m_engine->scheduleTimer(event_handler, interval);
    if (0 != ret)
    {
        CWX_ERROR(("Failure to register timer handler to event base, errno=%d",
            errno));
        return -1;
    }
    return 0;

}
///取消定时处理handle
inline int CwxAppChannel::_cancelTimer (CwxAppHandler4Channel *event_handler)
{

    int ret = m_engine->cancelTimer(event_handler);
    if (-1 == ret)
    {
        CWX_ERROR(("Failure to remvoe timer handle from event-base, errno=%d", 
            errno));
        return -1;
    }
    return 0;
}

///添加redo handler
inline bool CwxAppChannel::_regRedoHander(CwxAppHandler4Channel* event_handler)
{
    if (!event_handler->m_bRedo)
    {
        m_pCurRedoSet->insert(event_handler);
        event_handler->m_bRedo = true;
        return true;
    }
    return false;
}
///删除redo handler。
inline bool CwxAppChannel::_eraseRedoHander(CwxAppHandler4Channel* event_handler)
{
    if (event_handler->m_bRedo)
    {
        set<CwxAppHandler4Channel*>::iterator iter = m_pCurRedoSet->find(event_handler);
        if (iter != m_pCurRedoSet->end())
        {
            event_handler->m_bRedo = false;
            m_pCurRedoSet->erase(iter);
            return true;
        }
    }
    return false;
}


inline bool CwxAppChannel::_isRegIoHandle(CWX_HANDLE handle)
{
    if (handle >= CWX_APP_MAX_IO_NUM) return true;
    return m_engine->m_eHandler[handle].m_handler != NULL;
}

inline bool CwxAppChannel::_isRegIoHandle(CwxAppHandler4Channel* handler)
{
    if (handler->getHandle() >= CWX_APP_MAX_IO_NUM) return true;
    return m_engine->m_eHandler[handler->getHandle()].m_handler == handler;
}


/**
@brief 停止架构事件的循环处理。
@return -1：失败；0：正常退出
*/
inline int CwxAppChannel::_stop()
{
    m_bStop = true;
    return 0;
}


/**
@brief 获取指定handle对应的event handler。
@param bLock  true：api内部lock；false：外部加锁
@return 返回handle对应的event handler；NULL表示不存在
*/
inline CwxAppHandler4Channel* CwxAppChannel::_getIoHandler(CWX_HANDLE handle)
{
    if (handle >= CWX_APP_MAX_IO_NUM) return NULL;
    return (CwxAppHandler4Channel*)m_engine->m_eHandler[handle].m_handler;
}


CWINUX_END_NAMESPACE
