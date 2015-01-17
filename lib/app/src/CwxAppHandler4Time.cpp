#include "CwxAppHandler4Time.h"
#include "CwxAppFramework.h"

CWINUX_BEGIN_NAMESPACE
///构造函数
CwxAppHandler4Time::CwxAppHandler4Time(CwxAppFramework* pApp,
                                       CwxAppReactor* reactor,
                                       CwxTimeValue const& internal)
:CwxAppHandler4Base(reactor),m_internal(internal)
{
    m_pApp = pApp;
}

CwxAppHandler4Time::~CwxAppHandler4Time()
{
}


int CwxAppHandler4Time::open (void *)
{
    if (0 != reactor()->scheduleTimer(this,m_internal))
    {
        CWX_ERROR(("Failure to register time click handle"));
        return -1;
    }
    return 0;
}

int CwxAppHandler4Time::handle_event(int, CWX_HANDLE )
{
    if (getApp()->isStopped()) return 0;
    getApp()->onTime(reactor()->getCurTime());
    if (0 != reactor()->scheduleTimer(this,m_internal))
    {
        CWX_ERROR(("Failure to re-register time click handle"));
        return -1;
    }
    return 0;
}

///handle close
int CwxAppHandler4Time::close(CWX_HANDLE)
{
    if (!getApp()->isStopped()) getApp()->stop();
    return 0;
}


CWINUX_END_NAMESPACE

