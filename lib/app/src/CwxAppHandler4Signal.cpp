#include "CwxAppHandler4Signal.h"
#include "CwxAppFramework.h"

CWINUX_BEGIN_NAMESPACE
///构造函数
CwxAppHandler4Signal::CwxAppHandler4Signal(CwxAppFramework* pApp,
                                           CwxAppReactor* reactor,
                                           int sig)
:CwxAppHandler4Base(reactor)
{
    m_pApp = pApp;
    setHandle(sig);
}
CwxAppHandler4Signal::~CwxAppHandler4Signal()
{
}

/**
@brief 连接建立的处理函数，否则连接建立的初始化
@param [in] parent 建立连接的acceptor或connector
@return -1：放弃建立的连接； 0：连接建立成功
*/
int CwxAppHandler4Signal::open (void *)
{
    if (0 != reactor()->registerSignal(getHandle(), this))
    {
        CWX_ERROR(("Failure to register signal handle, sig[%d]", (int)getHandle()));
        return -1;
    }
    return 0;

}
/**
@brief 接受连接上的事件
@param [in] handle 连接的handle
@return -1：处理失败； 0：处理成功
*/
int CwxAppHandler4Signal::handle_event(int, CWX_HANDLE )
{
    if (getApp()->isStopped()) return 0;
    this->getApp()->onSignal(getHandle());
    return 0;
}

int CwxAppHandler4Signal::close(CWX_HANDLE)
{
    reactor()->removeSignal(this);
    delete this;
    return 0;
}



CWINUX_END_NAMESPACE

