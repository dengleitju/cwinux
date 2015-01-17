#include "CwxDate.h"
#include "CwxAppHandler4Notice.h"
#include "CwxAppFramework.h"

CWINUX_BEGIN_NAMESPACE

///构造函数
CwxAppHandler4Notice::CwxAppHandler4Notice(CwxAppFramework* app, CwxAppReactor *reactor)
:CwxAppHandler4Base(reactor)
{
    m_pApp = app;
}
///析构函数
CwxAppHandler4Notice::~CwxAppHandler4Notice()
{
}


int CwxAppHandler4Notice::open (void * )
{
    if (0 != reactor()->registerHandler(getHandle(), this, CwxAppHandler4Base::PREAD_MASK))
    {
        CWX_ERROR(("Failure to register notice handler"));
        return -1;
    }
    return 0;
}

int CwxAppHandler4Notice::handle_event(int , CWX_HANDLE)
{
    if (getApp()->isStopped()) return 0;
    char szOneByte[1];
    if (1 != ::read(getHandle(), szOneByte, 1))
    {//pipe is closed 
        CWX_ERROR(("Failure to read the notice pipe, errno=%d", errno));
    }
    return 0;
}

int CwxAppHandler4Notice::close(CWX_HANDLE)
{
    if (reactor())reactor()->removeHandler(this);
    if (!getApp()->isStopped()) getApp()->stop();
    return 0;
}


CWINUX_END_NAMESPACE
