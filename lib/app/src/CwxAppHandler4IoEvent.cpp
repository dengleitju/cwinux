#include "CwxAppHandler4IoEvent.h"
#include "CwxAppFramework.h"

CWINUX_BEGIN_NAMESPACE

///构造函数
CwxAppHandler4IoEvent::CwxAppHandler4IoEvent(CwxAppFramework* app, CwxAppReactor *reactor)
:CwxAppHandler4Base(reactor)
{
    m_pApp = app;
    m_uiSvrId = 0;
    m_uiHostId = 0;
    m_ioEventMask = 0;
    m_userData = NULL;
    m_next = NULL;
}
///析构函数
CwxAppHandler4IoEvent::~CwxAppHandler4IoEvent()
{
    if (getHandle() != CWX_INVALID_HANDLE)
    {
        ::close(getHandle());
        setHandle(CWX_INVALID_HANDLE);
    }
}

///handle open
int CwxAppHandler4IoEvent::open (void * )
{
    if (0 != reactor()->registerHandler(getHandle(),
        this,
        m_ioEventMask))
    {
        CWX_ERROR(("Failure to register io handle[%d], mast=%d",
            getHandle(),
            m_ioEventMask));
        return -1;
    }
    return 0;
}

int CwxAppHandler4IoEvent::handle_event(int event, CWX_HANDLE)
{
    getApp()->onEvent4Handle(m_uiSvrId,
        m_uiHostId,
        getHandle(),
        m_ioEventMask,
        event,
        m_userData);
    setHandle(CWX_INVALID_HANDLE);
    getApp()->getHandlerCache()->cacheIoEventHandle(this);
    return 0;
}

///handle close
int CwxAppHandler4IoEvent::close(CWX_HANDLE)
{
    if (getHandle() != CWX_INVALID_HANDLE)
    {
        getApp()->reactor()->removeHandler(this);
        CWX_ERROR(("Handle is closed, handle[%d]", getHandle()));
        ::close(getHandle());
        setHandle(CWX_INVALID_HANDLE);
    }
    if (!getApp()->isStopped())
        getApp()->getHandlerCache()->cacheIoEventHandle(this);
    else
        delete this;
    return 0;
}



CWINUX_END_NAMESPACE
