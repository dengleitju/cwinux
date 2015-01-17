#include "CwxAppUnixAcceptor.h"
#include "CwxAppFramework.h"

CWINUX_BEGIN_NAMESPACE

CwxAppUnixAcceptor::CwxAppUnixAcceptor(CwxAppFramework* pApp,
                   CwxAppReactor *reactor,
                   char const* szPathFile, ///<listen file
                   CWX_UINT32 uiSvrId, ///<connect's svr-id
                   CWX_UINT32 uiListenId, ///<acceptor's listen id
                   bool      bRawData, ///<connect's msg having header
                   CWX_UINT16 unMode,
                   CWX_NET_SOCKET_ATTR_FUNC fn,
                   void* fnArg
                   )
                   :CwxAppHandler4Base(reactor)
{
    m_pApp = pApp;
    m_strPathFile = szPathFile;
    m_uiSvrId = uiSvrId;
    m_uiListenId = uiListenId;
    m_bRawData = bRawData;
    m_unMode = unMode;
    m_fn = fn;
    m_fnArg = fnArg;
    m_bCloseAll = false;
}

CwxAppUnixAcceptor::~CwxAppUnixAcceptor()
{
    if (CWX_INVALID_HANDLE != getHandle())
    {
        ::close(getHandle());
        setHandle(CWX_INVALID_HANDLE);
    }
}

int CwxAppUnixAcceptor::accept(CwxUnixAddr const& addr)
{
    if (0 != m_acceptor.listen(addr, true, CWX_APP_DEF_BACKLOG_NUM, AF_UNIX))
    {
        CWX_ERROR(("Failure to listen unix-file:%s, errno=%d",
            m_strPathFile.c_str(),
            errno));
        return -1;
    }
    if (0 != m_acceptor.setNonblock(true))
    {
        CWX_ERROR(("Failure to set listen unix-file:%s noblocked, errno=%d",
            m_strPathFile.c_str(),
            errno));
        return -1;
    }
    this->setHandle(m_acceptor.getHandle());
    return 0;
}

///handle open
int CwxAppUnixAcceptor::open (void * )
{
    if (0 != reactor()->registerHandler(getHandle(),
        this,
        CwxAppHandler4Base::PREAD_MASK))
    {
        CWX_ERROR(("Failure to reg listen unix-file:%s, errno=%d",
            m_strPathFile.c_str(),
            errno));
        return -1;
    }
    return 0;    

}
/// 事件的回调函数。返回-1：失败，自动释放handle；0：成功
int CwxAppUnixAcceptor::handle_event(int , CWX_HANDLE)
{
    fd_set conn_handle;
    struct timeval timeout;
    int select_width = int (getHandle()) + 1;
    int num = 0;
    int ret = 0;
    bool bSuspend = false;
    do
    {
        if (getApp()->isStopped()) break;
        m_stream.setHandle(CWX_INVALID_HANDLE);
        if (0 != m_acceptor.accept(m_stream))
        {
            CWX_ERROR(("Failure to CwxSockAcceptor::accept, errno=%d", errno));
            return 0;
        }
        if (CWX_APP_MSG_MODE == m_unMode)
        {
            CwxAppHandler4UnixConn *handler = makeHander();
            if (!handler){
                m_stream.close();
                CWX_ERROR(("Failure to CwxAppUnixAcceptor::makeHander"));
                return 0;
            }
            handler->setHandle(m_stream.getHandle());
            ret = handler->open(this);
            if (-1 == ret) handler->close();
            if (handler->isStopListen()){
                bSuspend = true;
                break;
            }
        }
        else
        {
            if (0 > getApp()->onConnCreated(m_uiSvrId, 0, m_stream.getHandle(), bSuspend))
            {
                m_stream.close();
            }
            if (bSuspend) break;
        }
        FD_ZERO(&conn_handle);
        FD_SET(getHandle(), &conn_handle);
        num++;
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;
    }
    while ((num<MAX_PER_RECIEVE_CONNECT)
        && (::select (select_width,
        &conn_handle,
        NULL,
        NULL,
        &timeout) == 1));
    m_stream.setHandle(CWX_INVALID_HANDLE);
    if (bSuspend)
    {
        if (0 != reactor()->suspendHandler(getHandle(), CwxAppHandler4Base::READ_MASK))
        {
            CWX_ERROR(("Failure to suspend unix listen addr:", m_strPathFile.c_str()));
            return -1;

        }
    }
    return 0;
}

///handle close
int CwxAppUnixAcceptor::close(CWX_HANDLE)
{
    if (CWX_INVALID_HANDLE != getHandle())
    {
        if (reactor()) reactor()->removeHandler(this);
        ::close(getHandle());
        setHandle(CWX_INVALID_HANDLE);
        m_acceptor.setHandle(CWX_INVALID_HANDLE);
    }
    delete this;
    return 0;
}

CwxAppHandler4UnixConn* CwxAppUnixAcceptor::makeHander()
{
    CwxAppHandler4UnixConn *ch = getApp()->getHandlerCache()->fetchUnixHandle();
    if (!ch) ch = new CwxAppHandler4UnixConn(getApp(), reactor());
    if (!ch) return NULL;
    ch->getConnInfo().setState(CwxAppConnInfo::ESTABLISHING);
    ch->getConnInfo().setSvrId(this->m_uiSvrId);
    ch->getConnInfo().setHostId(0);
    ch->getConnInfo().setConnId(reactor()->getNextConnId());
    ch->getConnInfo().setListenId(this->m_uiListenId);
    ch->getConnInfo().setActiveConn(false);
    ch->getConnInfo().setRawData(this->m_bRawData);
    ch->setConnectPathFile(m_strPathFile.c_str());
    return ch;
}


CWINUX_END_NAMESPACE

