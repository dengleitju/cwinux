
CWINUX_BEGIN_NAMESPACE

inline void CwxAppEpoll::timeout(CWX_UINT64& ullTime)
{
    CwxAppHandler4Base const* handler = m_timeHeap.top();
    if (!handler)
    {
        ullTime = 0;
    }
    else
    {
        ullTime = handler->getTimeout();
    }
}
///添加 epoll检测,mask为READ_MASK、WRITE_MASK的组合。
inline int CwxAppEpoll::addEvent(int fd, int mask)
{
    if (mask & CwxAppHandler4Base::RW_MASK)
    {
        struct epoll_event event;
        int op = !m_eHandler[fd].isReg()?EPOLL_CTL_ADD : EPOLL_CTL_MOD;
        event.events = 0;
        mask |= m_eHandler[fd].m_mask;
        if (mask & CwxAppHandler4Base::READ_MASK) event.events |= EPOLLIN;
        if (mask & CwxAppHandler4Base::WRITE_MASK) event.events |= EPOLLOUT;
        event.data.u64 = 0; /* avoid valgrind warning */
        event.data.fd = fd;
        if (epoll_ctl(m_epfd, op, fd, &event) == -1)
        {
            CWX_ERROR(("Failure to epoll_ctl, fd=%d, errno=%d", fd, errno));
            return -1;
        }
    }
    return 0;
}
///删除存在的mask，mask为READ_MASK、WRITE_MASK的组合。
inline int CwxAppEpoll::delEvent(int fd, int mask)
{
    if (mask & m_eHandler[fd].m_mask)
    {
        int ret = 0;
        struct epoll_event event;
        mask = m_eHandler[fd].m_mask & (~mask);
        event.events = 0;
        if (mask & CwxAppHandler4Base::READ_MASK) event.events |= EPOLLIN;
        if (mask & CwxAppHandler4Base::WRITE_MASK) event.events |= EPOLLOUT;
        event.data.u64 = 0; /* avoid valgrind warning */
        event.data.fd = fd;
        if (mask != 0) {
            ret = epoll_ctl(m_epfd,EPOLL_CTL_MOD,fd,&event);
        } else {
            /* Note, Kernel < 2.6.9 requires a non null event pointer even for
            * EPOLL_CTL_DEL. */
            ret = epoll_ctl(m_epfd,EPOLL_CTL_DEL,fd,&event);
        }
        if (-1 == ret)
        {
            CWX_ERROR(("Failure to epoll_ctl, fd=%d, errno=%d", fd, errno));
            return -1;
        }
    }
    return 0;
}

inline void CwxAppEpoll::stop()
{
    m_bStop = true;
}

///获取当前的时间
inline CwxTimeValue const& CwxAppEpoll::getCurTime() const
{
    return m_current;
}


CWINUX_END_NAMESPACE
