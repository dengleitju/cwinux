#include "CwxAppHandler4Stdio.h"
#include "CwxAppFramework.h"

CWINUX_BEGIN_NAMESPACE
///构造函数
CwxAppHandler4StdIo::CwxAppHandler4StdIo(CwxAppFramework* pApp,
                                         CwxAppReactor* reactor)
:CwxAppHandler4Base(reactor)
{
    m_pApp = pApp;
}
CwxAppHandler4StdIo::~CwxAppHandler4StdIo()
{
}

int CwxAppHandler4StdIo::open (void *)
{
    setHandle(fileno(stdin));
    if (0 != reactor()->registerHandler(getHandle(), this, CwxAppHandler4Base::PREAD_MASK))
    {
        CWX_ERROR(("Failure to register stdio handle, stdio[%d]", (int)getHandle()));
        return -1;
    }
    return 0;
}

int CwxAppHandler4StdIo::handle_event(int , CWX_HANDLE)
{
    if (getApp()->isStopped()) return 0;
    memset(m_szLine, 0x00, MAX_LINE_LENGTH + 1);
    fgets(m_szLine, MAX_LINE_LENGTH, stdin);
    unsigned len = strlen(m_szLine);
    m_szLine[len] = 0x00;
    if (0 == len)
    {
        CWX_ERROR(("Stdio is closed."));
        this->getApp()->onStdinClosed();
        return -1;
    }
    this->getApp()->onStdinInput(m_szLine);
    return 0;
}

///handle close
int CwxAppHandler4StdIo::close(CWX_HANDLE)
{
    if (getApp()->isStopped()) return 0;
    if (reactor()) reactor()->removeHandler(this);
    this->setHandle(CWX_INVALID_HANDLE);
    delete this;
    return 0;
}



CWINUX_END_NAMESPACE

