#include "CwxAppEpoll.h"



CWINUX_BEGIN_NAMESPACE

int CwxAppEpoll::m_signalFd[2]={CWX_INVALID_HANDLE,CWX_INVALID_HANDLE}; ///<信号的读写handle
sig_atomic_t CwxAppEpoll::m_arrSignals[CWX_APP_MAX_SIGNAL_ID + 1]={0};///<signal的数组
volatile sig_atomic_t  CwxAppEpoll::m_bSignal=0; ///<是否有信号

CwxAppEpoll::CwxAppEpoll(bool bEnableSignal):m_timeHeap(4096)
{
    m_epfd = CWX_INVALID_HANDLE;
    m_bEnableSignal = bEnableSignal;
    memset(m_events, 0x00, sizeof(m_events));
    if (bEnableSignal)
    {
        memset(m_arrSignals, 0x00, sizeof(m_arrSignals));
        memset(m_sHandler, 0x00, sizeof(m_sHandler));
        m_sigHandler = new SignalHanlder();
    }
    else
    {
        m_sigHandler = NULL;
    }
    m_bStop = false;
    m_current.now();
}

CwxAppEpoll::~CwxAppEpoll()
{
    //删除各个handler
    int i=0;
    for (i=0; i<CWX_APP_MAX_IO_NUM; i++)
    {
        if (m_eHandler[i].m_handler)
        {
//            removeHandler(i);
            m_eHandler[i].m_handler->close(m_eHandler[i].m_handler->getHandle());
        }
    }
    if (m_bEnableSignal)
    {
        //删除signal handler
        for (i=0; i<CWX_APP_MAX_SIGNAL_ID; i++)
        {
            if (m_sHandler[i]) m_sHandler[i]->close(m_sHandler[i]->getHandle());
        }
    }
    //删除timeout handler
    CwxAppHandler4Base* handler;
    while((handler=m_timeHeap.pop()))
    {
        handler->close();
    }
    if (m_epfd != CWX_INVALID_HANDLE)
    {
        ::close(m_epfd);
    }
    if (m_bEnableSignal)
    {
        if (m_signalFd[0] != CWX_INVALID_HANDLE)
        {
            ::close(m_signalFd[0]);
        }
        if (m_signalFd[1] != CWX_INVALID_HANDLE)
        {
            ::close(m_signalFd[1]);
        }
    }
}

///初始化epoll
int CwxAppEpoll::init()
{
    if (CWX_INVALID_HANDLE != m_epfd)
    {
        CWX_ERROR(("epoll handle is created, file_id=%d", m_epfd));
        return -1;
    }
    m_epfd = epoll_create(CWX_EPOLL_INIT_HANDLE); 
    if (m_epfd == -1)
    {
        CWX_ERROR(("Failure to invoke epoll_create, errno=%d", errno));
        return -1;
    }
    if (0 != CwxIpcSap::setCloexec(m_epfd, true))
    {
        CWX_ERROR(("Failure to set handle's cloexec sign, errno=%d", errno));
        return -1;
    }
    if (0 != m_timeHeap.init())
    {
        CWX_ERROR(("Failure to init time heap"));
        return -1;
    }

    if (m_bEnableSignal)
    {
        //创建signal的handle
        if (0 != pipe(m_signalFd))
        {
            CWX_ERROR(("Failure to invokde pipe to create signal fd, errno=%d", errno));
            return -1;
        }
        if (0 != CwxIpcSap::setCloexec(m_signalFd[0], true))
        {
            CWX_ERROR(("Failure to set signal handle[0]'s cloexec sign, errno=%d", errno));
            return -1;
        }
        if (0 != CwxIpcSap::setCloexec(m_signalFd[1], true))
        {
            CWX_ERROR(("Failure to set signal handle[1]'s cloexec sign, errno=%d", errno));
            return -1;
        }
        if (0 != CwxIpcSap::setNonblock(m_signalFd[0], true))
        {
            CWX_ERROR(("Failure to set signal handle[0]'s noblock sign, errno=%d", errno));
            return -1;
        }
        if (0 != CwxIpcSap::setNonblock(m_signalFd[1], true))
        {
            CWX_ERROR(("Failure to set signal handle[1]'s noblock sign, errno=%d", errno));
            return -1;
        }
        //注册信号fd的读
        m_sigHandler->setHandle(m_signalFd[0]);
        if (0 != registerHandler(m_signalFd[0], m_sigHandler, CwxAppHandler4Base::PREAD_MASK))
        {
            CWX_ERROR(("Failure to register signal handle to engine"));
            return -1;
        }
    }
    return 0;
}

int CwxAppEpoll::registerHandler(CWX_HANDLE handle,
                    CwxAppHandler4Base *event_handler,
                    int mask,
                    CWX_UINT32 uiMillSecond)
{
    if ((handle < 0) || (handle >= CWX_APP_MAX_IO_NUM))
    {
        CWX_ERROR(("Invalid io handle id[%d], range[0,%d]", handle, CWX_APP_MAX_IO_NUM));
        return -1;
    }
    ///若handle相等，是fork后的重新添加
    if (m_eHandler[handle].m_handler)
    {
        CWX_ERROR(("handler is registered, handle[%d]", (int)handle));
        return -1;
    }

    mask &=CwxAppHandler4Base::IO_MASK; ///只支持READ、WRITE、PERSIST、TIMEOUT四种掩码
    if (uiMillSecond) mask |= CwxAppHandler4Base::TIMEOUT_MASK;

    if (uiMillSecond)
    {
        CWX_ASSERT(-1 == event_handler->index());
        event_handler->m_ullTimeout = CwxDate::getTimestamp();
        event_handler->m_ullTimeout += uiMillSecond * 1000;
        if (!m_timeHeap.push(event_handler))
        {
            CWX_ERROR(("Failure to add handler to time-heap, io[%d]", handle));
            return -1;
        }
    }

    if (mask&CwxAppHandler4Base::RW_MASK) ///如果存在READ、WRITE的掩码，则注册
    {
        if (0 != addEvent(handle, mask))
        {
            if (uiMillSecond) 
                m_timeHeap.erase(event_handler); ///删除timeout
            return -1;
        }
    }
    m_eHandler[handle].m_handler = event_handler;
    m_eHandler[handle].m_mask = mask;
    return 0;
}


CwxAppHandler4Base* CwxAppEpoll::removeHandler (CWX_HANDLE handle)
{
    if ((handle < 0) || (handle >= CWX_APP_MAX_IO_NUM))
    {
        CWX_ERROR(("Invalid io handle id[%d], range[0,%d]", handle, CWX_APP_MAX_IO_NUM));
        return NULL;
    }
    CwxAppHandler4Base* event_handler = m_eHandler[handle].m_handler;
    if (!event_handler) return NULL;
    if (event_handler->index() >=0)
    {//timeout
        m_timeHeap.erase(event_handler);
        event_handler->setTimeout(0);
    }
    delEvent(handle, m_eHandler[handle].m_mask);
    m_eHandler[handle].m_mask = 0;
    m_eHandler[handle].m_handler = NULL;
    return event_handler;
}

int CwxAppEpoll::suspendHandler (CWX_HANDLE handle,
                    int suspend_mask)
{
    if ((handle < 0) || (handle >= CWX_APP_MAX_IO_NUM))
    {
        CWX_ERROR(("Invalid io handle id[%d], range[0,%d]", handle, CWX_APP_MAX_IO_NUM));
        return -1;
    }
    CwxAppHandler4Base* event_handler = m_eHandler[handle].m_handler;
    if (!event_handler)
    {
        CWX_ERROR(("Handler[%d] doesn't exist.", handle));
        return -1;
    }
    suspend_mask &=(CwxAppHandler4Base::RW_MASK&m_eHandler[handle].m_mask);
    if(!suspend_mask) return 0;
    if (0 != delEvent(handle, suspend_mask))
    {
        CWX_ERROR(("Failure to suspend handler[%d]", handle));
        return -1;
    }
    m_eHandler[handle].m_mask &=~suspend_mask;
    return 0;
}

int CwxAppEpoll::resumeHandler (CWX_HANDLE handle,
                   int resume_mask)
{
    if ((handle < 0) || (handle >= CWX_APP_MAX_IO_NUM))
    {
        CWX_ERROR(("Invalid io handle id[%d], range[0,%d]", handle, CWX_APP_MAX_IO_NUM));
        return -1;
    }
    CwxAppHandler4Base* event_handler = m_eHandler[handle].m_handler;
    if (!event_handler)
    {
        CWX_ERROR(("Handler[%d] doesn't exist.", handle));
        return -1;
    }
    resume_mask &=CwxAppHandler4Base::RW_MASK;
    resume_mask &=~m_eHandler[handle].m_mask;
    if(!resume_mask) return 0;
    if (0 != addEvent(handle, resume_mask))
    {
        CWX_ERROR(("Failure to resume handler[%d]", handle));
        return -1;
    }
    m_eHandler[handle].m_mask |=resume_mask;
    return 0;

}

int CwxAppEpoll::registerSignal(int signum,
                   CwxAppHandler4Base *event_handler)
{
    if (!m_bEnableSignal)
    {
        CWX_ASSERT(0);
        CWX_ERROR(("Epoll engine not support signal"));
        return -1;
    }
    if ((signum < 0) || (signum >= CWX_APP_MAX_SIGNAL_ID))
    {
        CWX_ERROR(("Invalid signal id[%d], range[0,%d]", signum, CWX_APP_MAX_SIGNAL_ID));
        return -1;
    }
    if (m_sHandler[signum])
    {
        CWX_ERROR(("Signal[%d]  exist.", signum));
        return -1;
    }
    struct sigaction sa;
    //set stop hander
    sa.sa_handler = 0;
    sa.sa_sigaction = CwxAppEpoll::sigAction;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags =SA_SIGINFO;
    if (-1 == sigaction(signum, &sa, NULL))
    {
        CWX_ERROR(("Failure register signal[%d] handler.", signum));
        return -1;
    }
    event_handler->setHandle(signum);
    m_sHandler[signum] = event_handler;
    return 0;

}

int CwxAppEpoll::removeSignal(CwxAppHandler4Base *event_handler)
{
    if (!m_bEnableSignal)
    {
        CWX_ASSERT(0);
        CWX_ERROR(("Epoll engine not support signal"));
        return -1;
    }
    removeSignal(event_handler->getHandle());
    return 0;
}

CwxAppHandler4Base* CwxAppEpoll::removeSignal(int signum)
{
    if (!m_bEnableSignal)
    {
        CWX_ASSERT(0);
        CWX_ERROR(("Epoll engine not support signal"));
        return NULL;
    }
    if ((signum < 0) || (signum >= CWX_APP_MAX_SIGNAL_ID))
    {
        CWX_ERROR(("Invalid signal id[%d], range[0,%d]", signum, CWX_APP_MAX_SIGNAL_ID));
        return NULL;
    }
    if (!m_sHandler[signum])
    {
        CWX_ERROR(("Signal[%d] doesn't exist.", signum));
        return NULL;
    }
    CwxAppHandler4Base* handler = m_sHandler[signum];
    struct sigaction sa;
    //set stop hander
    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags =0x0;
    sigaction(signum, &sa, NULL);
    m_sHandler[signum] = NULL;
    return handler;
}


int CwxAppEpoll::scheduleTimer (CwxAppHandler4Base *event_handler,
                   CwxTimeValue const &interval)
{
    CWX_UINT64 ullTime = CwxDate::getTimestamp();
    if (event_handler->index()>=0)
    {
        m_timeHeap.erase(event_handler);
        event_handler->setTimeout(0);
    }
    ullTime += interval.to_usec();
    event_handler->setTimeout(ullTime);
    m_timeHeap.push(event_handler);
    return 0;
}

int CwxAppEpoll::cancelTimer (CwxAppHandler4Base *event_handler)
{
    if (-1 == event_handler->index())
    {
        CWX_ERROR(("No timer for handler."));
        return -1;
    }
    m_timeHeap.erase(event_handler);
    event_handler->setTimeout(0);
    return 0;
}

int CwxAppEpoll::forkReinit()
{
    int i = 0;
    int old_sigFd = m_signalFd[0];
    m_epfd = epoll_create(CWX_EPOLL_INIT_HANDLE); 
    if (m_epfd == -1)
    {
        CWX_ERROR(("Failure to invoke epoll_create, errno=%d", errno));
        return -1;
    }
    if (0 != CwxIpcSap::setCloexec(m_epfd, true))
    {
        CWX_ERROR(("Failure to set handle's cloexec sign, errno=%d", errno));
        return -1;
    }
    if (m_bEnableSignal)
    {
        //创建signal的handle
        if (0 != pipe(m_signalFd))
        {
            CWX_ERROR(("Failure to invokde socketpair to create signal fd, errno=%d", errno));
            return -1;
        }
        if (0 != CwxIpcSap::setCloexec(m_signalFd[0], true))
        {
            CWX_ERROR(("Failure to set signal handle[0]'s cloexec sign, errno=%d", errno));
            return -1;
        }
        if (0 != CwxIpcSap::setCloexec(m_signalFd[1], true))
        {
            CWX_ERROR(("Failure to set signal handle[1]'s cloexec sign, errno=%d", errno));
            return -1;
        }
        if (0 != CwxIpcSap::setNonblock(m_signalFd[0], true))
        {
            CWX_ERROR(("Failure to set signal handle[0]'s noblock sign, errno=%d", errno));
            return -1;
        }
        if (0 != CwxIpcSap::setNonblock(m_signalFd[1], true))
        {
            CWX_ERROR(("Failure to set signal handle[1]'s noblock sign, errno=%d", errno));
            return -1;
        }
    }
    //恢复handle
    {
        EventHandle eHandler[CWX_APP_MAX_IO_NUM];
        memcpy(eHandler, m_eHandler, sizeof(m_eHandler));
        for (i=0; i<CWX_APP_MAX_IO_NUM; i++)
        {
            m_eHandler[i].m_mask = 0;
            m_eHandler[i].m_handler = NULL;
        }
        //清除signal的handler
        eHandler[old_sigFd].m_mask = 0;
        eHandler[old_sigFd].m_handler = 0;
        //添加handler
        for (i=0; i<CWX_APP_MAX_IO_NUM; i++)
        {
            if (eHandler[i].m_handler)
            {
                if (eHandler[i].m_mask&CwxAppHandler4Base::RW_MASK) ///如果存在READ、WRITE的掩码，则注册
                {
                    if (0 != addEvent(eHandler[i].m_handler->getHandle(), eHandler[i].m_mask))
                    {
                        return -1;
                    }
                }
                m_eHandler[i].m_handler = eHandler[i].m_handler;
                m_eHandler[i].m_mask = eHandler[i].m_mask;
            }
        }
        if (m_bEnableSignal)
        {
            m_sigHandler->setHandle(m_signalFd[0]);
            if (0 != registerHandler(m_signalFd[0], m_sigHandler, CwxAppHandler4Base::PREAD_MASK))
            {
                CWX_ERROR(("Failure to register handle to engine"));
                return -1;
            }
        }
    }
    if (m_bEnableSignal)
    {
        //回复信号,signal的屏蔽状态会从父进程传递
        for (i=0; i<CWX_APP_MAX_SIGNAL_ID; i++)
        {
            m_arrSignals[i] = 0;
        }
        m_bSignal = 0;
    }

    return 0;
}


int CwxAppEpoll::poll(REACTOR_CALLBACK callback, void* arg, CWX_UINT32 uiMiliTimeout)
{
    static CWX_UINT64 ullLastTime=0;
    bool bClockChange = false;
    int i = 0;
    int num = 0;
    CWX_UINT64 ullNow = 0;
    CWX_UINT64 ullTimeout = 0;
    int tv=-1;
    struct epoll_event *event=NULL;
    CwxAppHandler4Base* handler = NULL;
    m_current.now();
    ullNow = m_current.to_usec();
    
    if (ullLastTime > ullNow + 1000000){ ///多余1秒
        ullLastTime = ullNow;
        bClockChange = true;
    }else{
        bClockChange = false;
        ullLastTime = ullNow;
    }

    ///计算超时时间
    timeout(ullTimeout);
    if (ullTimeout){
        if (ullTimeout < ullNow){
            tv =1;
        }else{
            ullTimeout -= ullNow;
            tv = ullTimeout/1000;
            if (tv < 1) tv = 1;
        }
        if (uiMiliTimeout && (tv > (int)uiMiliTimeout)) tv = uiMiliTimeout;
    }else if (uiMiliTimeout){
        tv = uiMiliTimeout;
    }

    m_bStop = false;
    num = epoll_wait(m_epfd, m_events, CWX_APP_MAX_IO_NUM, tv);
    if (num > 0){
        for (i = 0; i < num; i++){
            int mask = 0;
            if (m_bStop) return 0;
            event = &m_events[i];
/*            if (m_signalFd[0] == event->data.fd)
            {
                CWX_ASSERT(event->events == EPOLLIN);
                continue;
            }*/
            CWX_ASSERT(m_eHandler[event->data.fd].m_handler);
            if (event->events & EPOLLIN) mask |= CwxAppHandler4Base::READ_MASK;
            if (event->events & EPOLLOUT) mask |= CwxAppHandler4Base::WRITE_MASK;
            handler = m_eHandler[event->data.fd].m_handler;
            //检测timeout
            if (m_eHandler[event->data.fd].m_handler->index()>=0)
            {//从heap中删除
                if (0 != cancelTimer(m_eHandler[event->data.fd].m_handler))
                {
                    CWX_ERROR(("Failure to cancel hander's timer, fd[%d]", event->data.fd));
                }
            }
            if (!CWX_CHECK_ATTR(m_eHandler[event->data.fd].m_mask, CwxAppHandler4Base::PERSIST_MASK))
            {
                if (removeHandler(event->data.fd) != handler)
                {
                    CWX_ASSERT(0);
                    CWX_ERROR(("handler[%d] is not right", event->data.fd));
                    return -1;
                }
            }
            callback(handler,
                mask,
                CWX_CHECK_ATTR(m_eHandler[event->data.fd].m_mask, CwxAppHandler4Base::PERSIST_MASK),
                arg);
        }
    }
    //检测signal
    if (m_bSignal && m_bEnableSignal)
    {
        if (m_bStop) return 0;
        m_bSignal = 0;
        for (i=0; i<CWX_APP_MAX_SIGNAL_ID; i++)
        {
            if (m_arrSignals[i])
            {
                m_arrSignals[i] = 0;
                CWX_ASSERT(m_sHandler[i]);
                callback(m_sHandler[i],
                    CwxAppHandler4Base::SIGNAL_MASK,
                    true,
                    arg);
            }
        }
    }
    //检测超时
    if (bClockChange){
        list<CwxAppHandler4Base*> handles;
        while(!m_timeHeap.isEmpty()){
            if (m_bStop) return 0;
            if (m_timeHeap.top()->getHandle() != CWX_INVALID_HANDLE){
                handler = removeHandler(m_timeHeap.top()->getHandle());
                if (!handler){
                    CWX_ERROR(("Failure to remove timeout handler, fd[%d]", event->data.fd));
                }
            }else{
                handler = m_timeHeap.pop();
            }
            handles.push_back(handler);
        }
        list<CwxAppHandler4Base*>::iterator iter = handles.begin();
        while(iter != handles.end()){
            callback(*iter,
                CwxAppHandler4Base::TIMEOUT_MASK,
                false,
                arg);
            iter++;
        }
    }else{
        while (!m_timeHeap.isEmpty() && (m_timeHeap.top()->getTimeout() < ullNow)){
            if (m_bStop) return 0;
            if (m_timeHeap.top()->getHandle() != CWX_INVALID_HANDLE){
                handler = removeHandler(m_timeHeap.top()->getHandle());
                if (!handler){
                    CWX_ERROR(("Failure to remove timeout handler, fd[%d]", event->data.fd));
                }
            }else{
                handler = m_timeHeap.pop();
            }
            callback(handler,
                CwxAppHandler4Base::TIMEOUT_MASK,
                false,
                arg);
        }
    }
    return 0;
}

void CwxAppEpoll::sigAction(int sig, siginfo_t *, void *)
{
    if (!m_arrSignals[sig]) m_arrSignals[sig] = 1;
    m_bSignal = 1;
    write(m_signalFd[1], "1", 1);
}



CWINUX_END_NAMESPACE
