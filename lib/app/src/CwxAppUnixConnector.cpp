#include "CwxAppFramework.h"
CWINUX_BEGIN_NAMESPACE


///构造函数
CwxAppUnixConnector::CwxAppUnixConnector (CwxAppFramework* pApp, CwxAppReactor *reactor)
:m_pApp(pApp), m_reactor(reactor)
{

}
///析构函数
CwxAppUnixConnector::~CwxAppUnixConnector()
{

}

int CwxAppUnixConnector::connect(CwxAppHandler4UnixConn* pHandler,
                             char const* szPathFile,
                             CWX_NET_SOCKET_ATTR_FUNC fn,
                             void* fnArg
                             )
{
    CwxTimeValue timeout;
    CwxTimeouter timer(&timeout);
    CwxUnixConnector connector;
    CwxUnixStream stream;
    CwxUnixAddr addr(szPathFile);
    int ret = connector.connect(stream,
        addr,
        CwxAddr::sap_any,
        &timer,
        0,
        true,
        fn,
        fnArg);
    if (0 == ret)
    {
        pHandler->setHandle(stream.getHandle());
        pHandler->getConnInfo().setState(CwxAppConnInfo::ESTABLISHING);
        if (-1 == pHandler->open(NULL)) return -1;
        return 1;
    }
    if (EWOULDBLOCK == errno)
    {
        pHandler->setHandle(stream.getHandle());
        pHandler->getConnInfo().setState(CwxAppConnInfo::CONNECTING);
        if (-1 == pHandler->open(NULL)) return -1;
        return 0;
    }
    CWX_ERROR(("Failure to connect to:unix file=%s errno=%d",
        szPathFile,
        errno));
    stream.close();
    return -1;

}


CWINUX_END_NAMESPACE

