#include "CwxAppReactor.h"

CWINUX_BEGIN_NAMESPACE

CwxAppReactor::CwxAppReactor(bool bEnableSig)
:m_connMap(CWX_APP_MAX_IO_NUM * 2)
{
    m_bEnableSig = bEnableSig;
    m_owner = CwxThread::self();
    m_bStop = true;
    ///初始化IO handler的数组
    memset(m_connId, 0x00, sizeof(m_connId));
    ///创建notice pipe对象
    m_pNoticePipe = NULL;
    //事件驱动
    m_engine= NULL;
}


CwxAppReactor::~CwxAppReactor()
{
    close();
}

///fork的re-init方法，返回值，0：成功；-1：失败
int CwxAppReactor::forkReinit()
{
    if (m_engine)
    {
        m_owner = CwxThread::self();
        int ret = m_engine->forkReinit();
        if (-1 == ret)
        {
            CWX_ERROR(("Failure to invoke libevent::event_reinit"));
            return -1;
        }
        return 0;
    }
    CWX_ERROR(("Epoll engine doesn't init"));
    return -1;
}

///打开reactor，return -1：失败；0：成功
int CwxAppReactor::open(CwxAppHandler4Base* noticeHandler)
{
    if (!m_bStop)
    {
        CWX_ERROR(("Can't re-open the openning reactor"));
        return -1;
    }
    m_bStop = false;
    ///设置reactor的owner
    m_owner = CwxThread::self();
    ///清理reactor
    this->close();
    ///创建notice pipe对象
    m_pNoticePipe = new CwxAppNoticePipe();
    ///创建engine
    if (m_engine)
    {
        delete m_engine;
        m_engine = NULL;
    }
    m_engine = new CwxAppEpoll(m_bEnableSig);
    if (0 != m_engine->init()) return -1;
    if (m_pNoticePipe->init() != 0)
    {
        CWX_ERROR(("Failure to invoke CwxAppNoticePipe::init()"));
        return -1;
    }
    ///注册notice handler
    noticeHandler->setHandle(m_pNoticePipe->getPipeReader());
    if (0 != this->registerHandler(m_pNoticePipe->getPipeReader(),
        noticeHandler,
        CwxAppHandler4Base::PREAD_MASK))
    {
        CWX_ERROR(("Failure to register CwxAppHandler4Notice notice handler"));
        return -1;
    }
    CWX_DEBUG(("Success to open CwxAppReactor"));
    return 0;
}

///关闭reactor，return -1：失败；0：成功
int CwxAppReactor::close()
{
    if (!CwxThread::equal(m_owner, CwxThread::self()))
    {
        CWX_ERROR(("CwxAppReactor::close must be invoked by owner thread"));
        return -1;
    }
    m_bStop = true;
    memset(m_connId, 0x00, sizeof(m_connId));
    if (m_pNoticePipe)
    {
        delete m_pNoticePipe;
        m_pNoticePipe = NULL;
    }
    m_uiCurConnId = 0;
    m_connMap.clear();
    if (m_engine) delete m_engine;
    m_engine = NULL;
    return 0;
}

/**
@brief 架构事件的循环处理API，实现消息的分发。
@return -1：失败；0：正常退出
*/
int CwxAppReactor::run(REACTOR_EVENT_HOOK hook,
                       void* arg,
                       bool  bOnce,
                       CWX_UINT32 uiMiliTimeout)
{

    int ret = 0;
    if (!bOnce)
    {
        if (!m_bStop || !m_engine)
        {
            CWX_ERROR(("CwxAppReactor::open() must be invoke before CwxAppReactor::run()"));
            return -1;
        }
    }
    m_bStop = false;

    do
    {
        {
            ///带锁执行event-loop
            CwxMutexGuard<CwxMutexLock> lock(&m_lock);
            ret = m_engine->poll(CwxAppReactor::callback, this, uiMiliTimeout);
        }
        if (m_bStop)
        {
            CWX_DEBUG(("Stop running for stop"));
            break;
        }
        if (0 != ret)
        {
            if ((-1 == ret) && (EINTR != errno))
            {
                CWX_ERROR(("Failure to running epoll with -1, errno=%d", errno));
                break;
            }
        }
        ///调用hook
        if (hook)
        {
            if (0 != hook(arg))
            {
                CWX_DEBUG(("Stop running for hook() without 0"));
                break;
            }
        }
        ///等待其他的线程执行各种操作。
        m_rwLock.acquire_write();
        m_rwLock.release();
    }
    while(!m_bStop && !bOnce);
    return ret;
}

/**
@brief 停止架构事件的循环处理。
@return -1：失败；0：正常退出
*/
int CwxAppReactor::stop()
{
    if (!CwxThread::equal(m_owner, CwxThread::self()))
    {
        m_rwLock.acquire_read();
        this->notice(NULL);
        {
            CwxMutexGuard<CwxMutexLock> lock(&m_lock);
            return _stop();
        }
        m_rwLock.release();
    }
    return _stop();
}

void CwxAppReactor::callback(CwxAppHandler4Base* handler, int mask, bool bPersist, void *arg)
{
    CwxAppReactor* reactor = (CwxAppReactor*)arg;
    if (!bPersist)
    {
        switch(handler->getRegType())
        {
        case REG_TYPE_IO:
            reactor->m_connId[handler->getHandle()] = CWX_APP_INVALID_CONN_ID;
            break;
        default:
            break;
        }
    }
    int ret = handler->handle_event(mask, handler->getHandle());
    if (-1 == ret)
    {
        handler->close(handler->getHandle());
    }
}

CWINUX_END_NAMESPACE
