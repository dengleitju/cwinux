#include "CwxAppFramework.h"
#include "CwxFile.h"
#include "CwxGetOpt.h"

#ifdef WITH_CWINUX_GFLAGS
#include "gflags.h"

DEFINE_bool(stop, false, "Stop the daemon service.");
DEFINE_bool(restart, false, "Restart the daemon service.");
DEFINE_string(conf, "", "The config file.");
#endif

CWINUX_BEGIN_NAMESPACE

CwxAppFramework::CwxAppFramework(CWX_UINT16 unAppMode,
                  CWX_UINT32 uiMaxTaskNum)
                  :m_unAppMode(unAppMode),
                  m_taskBoard(uiMaxTaskNum)
{
    m_bAppRunValid = true;
    m_strAppRunFailReason = "";
    m_strVersion = "unknown";
    m_strLastModifyDatetime = "unknown";
    m_strLastCompileDatetime = "unknown";
    m_pStdIoHandler = NULL;
    m_pTimeHandler = NULL;
    m_pNoticeHandler = NULL;
    m_bCmdStop = false;
    m_bCmdRestart = false;
    m_bStopped = false;
    m_pReactor = NULL;
    m_uiTimeClick = 0;
    m_uiLogSize = CWX_APP_DEF_LOG_FILE_SIZE;
    m_unLogFileNum = CWX_APP_DEF_LOG_FILE_NUM;
    m_unDefKeepAliveSecond = DEF_KEEPALIVE_SECOND;
    m_unLogCheckSecond = DEF_LOG_CHECK_SECOND;
    m_bEnableHook = false;
    m_uiProcId = 0;
    m_pListenMgr = NULL;
    m_pTss = NULL;
    m_pHandleCache = NULL;
    m_pTcpConnector = NULL; ///<TCP的connector
    m_pUnixConnector = NULL; ///<unix的connector
    m_bDebug = false;
}

/**
desc:
     Destructor for class.
return
     void
*/
CwxAppFramework::~CwxAppFramework()
{
}

/**
desc:
    Init the object and parse the arg.
return:
    -1: failure.
    0 : success.
    1 : exit
*/
#ifndef WITH_CWINUX_GFLAGS
int CwxAppFramework::parse(int argc, char** argv)
{
    if (argc > 1) 
    {
        CwxGetOpt cmd_option(argc, argv, ":f:hsr");
        cmd_option.long_option("help", 'h', CwxGetOpt::NO_ARG);
        cmd_option.long_option("stop", 's', CwxGetOpt::NO_ARG);
        cmd_option.long_option("restart", 'r', CwxGetOpt::NO_ARG);        
        cmd_option.long_option("file", 'f', CwxGetOpt::ARG_REQUIRED);        
        int option;
        bool bHelp=false;
        while( (option = cmd_option.next()) != -1)
        {
            switch (option)
            {
            case 'f':
                if (!cmd_option.opt_arg() || (*cmd_option.opt_arg() == '-'))
                {
                    printf("-f requires an argument.\n");
                    return -1;
                }
                this->m_strConfFile = cmd_option.opt_arg();
                break;
            case 's':
                this->m_bCmdStop = true;
                break;
            case 'r':
                this->m_bCmdRestart = true;
                break;
            case 'h':
                bHelp = true;
                break;
            case ':':
                printf("%c requires an argument.\n", cmd_option.opt_opt ());
                return -1;
            case '?':
                break;
            default:
                printf("Invalid arg %s.\n", argv[cmd_option.opt_ind()-1]);
                return -1;
            }
            if (bHelp) break;
        }
        if (-1 == option)
        {
            if (cmd_option.opt_ind()  < argc)
            {
                printf("Invalid arg %s.\n", argv[cmd_option.opt_ind()]);
                return -1;
            }
        }
        if (bHelp) return 1;
    }
    return 0;
}
#else
int CwxAppFramework::parse() {
  this->m_strConfFile = FLAGS_conf;
  this->m_bCmdStop = FLAGS_stop;
  this->m_bCmdRestart = FLAGS_restart;
  return 0;
}

#endif
void CwxAppFramework::help()
{
    printf("commond option....\n");
    printf("-f:--file file_name : resource file name \n");
    printf("-s:--stop     : stop service.\n");
    printf("-r:--restart  : restart service.\n");
    printf("-h:--help     : help\n");
}

#ifndef WITH_CWINUX_GFLAGS
int CwxAppFramework::init(int argc, char** argv)
{
    int iRet = parse(argc, argv);
    if (0 != iRet)
    {
        help();
        return iRet;
    }
    if ( this->m_bCmdStop || this->m_bCmdRestart) return 0;

    this->destroy();

    if (0 != CwxTss::initTss())
    {
        CWX_ERROR(("Failure to init tss"));
        return -1;
    }
    //get app name
    CwxFile::getLastDirName(argv[0], m_strAppName);
    m_pTss = this->onTssEnv();
    CWX_ASSERT(m_pTss);
    m_pTss->getThreadInfo().setStartTime(time(NULL));
    m_pTss->getThreadInfo().setUpdateTime(time(NULL));
    m_pTss->getThreadInfo().setThreadNo(0);
    CwxTss::regTss(m_pTss);
 	return 0;
}
#else
int CwxAppFramework::init(char const* app_name)
{
  parse();
  if ( this->m_bCmdStop || this->m_bCmdRestart) return 0;
  this->destroy();
  if (0 != CwxTss::initTss())
  {
    CWX_ERROR(("Failure to init tss"));
    return -1;
  }
  //get app name
  CwxFile::getLastDirName(app_name, m_strAppName);
  m_pTss = this->onTssEnv();
  CWX_ASSERT(m_pTss);
  m_pTss->getThreadInfo().setStartTime(time(NULL));
  m_pTss->getThreadInfo().setUpdateTime(time(NULL));
  m_pTss->getThreadInfo().setThreadNo(0);
  CwxTss::regTss(m_pTss);
  return 0;
}
#endif

//register the stdin
int CwxAppFramework::noticeStdinListen()
{
    //stdio handler
    if (!m_pStdIoHandler)
    {
        this->m_pStdIoHandler = new CwxAppHandler4StdIo(this, m_pReactor);
        if ( 0 != m_pStdIoHandler->open(NULL))
        {
            delete m_pStdIoHandler;
            m_pStdIoHandler = NULL;
            CWX_ERROR (("Failure to register stdin_handler"));
            return -1;
        }
    }
	return 0;
}


int CwxAppFramework::noticeTcpListen(CWX_UINT32 uiSvrId,
                                  char const* szAddr,
                                  CWX_UINT16 unPort,
                                  bool bRawData,
                                  CWX_UINT16 unMode,
                                  CWX_NET_SOCKET_ATTR_FUNC fn,
                                  void* fnArg,
                                  CWX_INT32  iFamily)
{
    if (uiSvrId < SVR_TYPE_USER_START)
    {
        CWX_ERROR(("svr-id must not less than SVR_TYPE_USER_START"));
        return -1;
    }

    CWX_UINT32 uiListenId = 0;
    CwxINetAddr inetAddr;
    CwxAppTcpAcceptor* acceptor=NULL;
    uiListenId = m_pListenMgr->getNextListenId();
    acceptor = new CwxAppTcpAcceptor(this,
        reactor(),
        szAddr,
        unPort,
        uiSvrId,
        uiListenId,
        bRawData,
        unMode,
        fn,
        fnArg);
    int ret = 0;
    if (strcmp(szAddr, "*") == 0){
        if (AF_UNSPEC == iFamily) iFamily = AF_INET;
        ret = inetAddr.set(unPort, iFamily);
    } else {
        ret = inetAddr.set(unPort, szAddr, iFamily);
    }
    if (0 != ret) {
      CWX_ERROR(("Address is invalid, tcp listen for ip=%s, port=%u, errno=%d",
        szAddr,
        unPort,
        errno));
      return -1;
    }
    //register the acceptor
    if (acceptor->accept(inetAddr) != 0)
    {
        CWX_ERROR(("Can't open tcp listen for ip=%s, port=%u, errno=%d",
            szAddr,
            unPort,
            errno));
        acceptor->close();
        return -1;
    }
    if (0 != acceptor->open())
    {
        CWX_ERROR(("Failure to register tcp listen for ip=%s, port=%u", szAddr, unPort));
        acceptor->close();
        return -1;
    }
    //create and init the notice object.
    CwxAppNotice* notice = new CwxAppNotice();
    notice->m_unNoticeType = CwxAppNotice::TCP_LISTEN;
    notice->m_noticeArg = acceptor;
    if (0 != m_pReactor->notice(notice))
    {
        acceptor->close();
        delete notice;
        CWX_ERROR(("Failure to notice TCP listen by PIPE"));
        return -1;
    }
    return (int)uiListenId;
}


int CwxAppFramework::noticeLsockListen(CWX_UINT32 uiSvrId,
                   char const* szPathFile,
                   bool bRawData ,
                   CWX_UINT16 unMode,
                   CWX_NET_SOCKET_ATTR_FUNC fn,
                   void* fnArg)
{
    if (uiSvrId < SVR_TYPE_USER_START)
    {
        CWX_ERROR(("svr-id must not less than SVR_TYPE_USER_START"));
        return -1;
    }

    CWX_UINT32 uiListenId = 0;
    CwxAppUnixAcceptor* acceptor=NULL;
    CwxUnixAddr unixAddr;
    uiListenId = m_pListenMgr->getNextListenId();
    acceptor = new CwxAppUnixAcceptor(this,
        reactor(),
        szPathFile,
        uiSvrId,
        uiListenId,
        bRawData,
        unMode,
        fn,
        fnArg);
    unixAddr.set(szPathFile);
    CwxFile::rmFile(szPathFile);
    //register the acceptor
    if (acceptor->accept(unixAddr) != 0)
    {
        CWX_ERROR(("Can't open local unix listen for unix-file=%s, errno=%d",
            szPathFile,
            errno));
        acceptor->close();
        return -1;
    }
    if (0 != acceptor->open())
    {
        CWX_ERROR(("Failure to register local unix listen for unix-file=%s", szPathFile));
        acceptor->close();
        return -1;
    }

    //create and init the notice object.
    CwxAppNotice* notice = new CwxAppNotice();
    notice->m_unNoticeType = CwxAppNotice::UNIX_LISTEN;
    notice->m_noticeArg = acceptor;
    if (0 != m_pReactor->notice(notice))
    {
        acceptor->close();
        delete notice;
        CWX_ERROR(("Failure to notice local unix listen by PIPE"));
        return -1;
    }
    return (int)uiListenId;
}

int CwxAppFramework::noticeTcpConnect(CWX_UINT32 uiSvrId,
                  CWX_UINT32 uiHostId,
                  char const* szAddr,
                  CWX_UINT16 unPort,
                  bool bRawData,
                  CWX_UINT16 unMinRetryInternal,
                  CWX_UINT16 unMaxRetryInternal,
                  CWX_NET_SOCKET_ATTR_FUNC fn,
                  void* fnArg,
                  CWX_UINT32 uiMiliTimeout,
                  CWX_INT32  iFamily)
{
    if (uiSvrId < SVR_TYPE_USER_START)
    {
        CWX_ERROR(("svr-id must not less than SVR_TYPE_USER_START"));
        return -1;
    }

    CwxAppHandler4TcpConn* handle = m_pHandleCache->fetchTcpHandle();
    if (!handle) handle = new CwxAppHandler4TcpConn(this, reactor());
    CWX_UINT32 uiConnId = m_pReactor->getNextConnId();
    handle->getConnInfo().setSvrId(uiSvrId);
    handle->getConnInfo().setHostId(uiHostId);
    handle->setConnectAddr(szAddr);
    handle->setConnectPort(unPort);
    handle->getConnInfo().setRawData(bRawData);
    handle->getConnInfo().setMinRetryInternal(unMinRetryInternal);
    handle->getConnInfo().setMaxRetryInternal(unMaxRetryInternal);
    handle->getConnInfo().setConnId(uiConnId);
    handle->getConnInfo().setActiveConn(true);
    handle->getConnInfo().setSockFunc(fn);
    handle->getConnInfo().setSockFuncArg(fnArg);
    handle->getConnInfo().setConnectTimeout(uiMiliTimeout);
    handle->getConnInfo().setFamily(iFamily);

    CwxAppNotice* notice = new CwxAppNotice();
    notice->m_unNoticeType = CwxAppNotice::TCP_CONNECT;
    notice->m_noticeArg = handle;
    if (0 != m_pReactor->notice(notice))
    {
        delete handle;
        delete notice;
        CWX_ERROR(("Failure to notice tcp connection"));
        return -1;
    }
    return (int)uiConnId;
}

int CwxAppFramework::noticeLsockConnect(CWX_UINT32 uiSvrId,
                    CWX_UINT32 uiHostId,
                    char const* szPathFile,
                    bool bRawData,
                    CWX_UINT16 unMinRetryInternal,
                    CWX_UINT16 unMaxRetryInternal,
                    CWX_NET_SOCKET_ATTR_FUNC fn,
                    void* fnArg)
{
    if (uiSvrId < SVR_TYPE_USER_START)
    {
        CWX_ERROR(("svr-id must not less than SVR_TYPE_USER_START"));
        return -1;
    }
    CwxAppHandler4UnixConn* handle = m_pHandleCache->fetchUnixHandle();
    if (!handle) handle = new CwxAppHandler4UnixConn(this, reactor());
    CWX_UINT32 uiConnId = m_pReactor->getNextConnId();
    handle->getConnInfo().setSvrId(uiSvrId);
    handle->getConnInfo().setHostId(uiHostId);
    handle->setConnectPathFile(szPathFile);
    handle->getConnInfo().setRawData(bRawData);
    handle->getConnInfo().setMinRetryInternal(unMinRetryInternal);
    handle->getConnInfo().setMaxRetryInternal(unMaxRetryInternal);
    handle->getConnInfo().setConnId(uiConnId);
    handle->getConnInfo().setActiveConn(true);
    handle->getConnInfo().setSockFunc(fn);
    handle->getConnInfo().setSockFuncArg(fnArg);
    CwxAppNotice* notice = new CwxAppNotice();
    notice->m_unNoticeType = CwxAppNotice::UNIX_CONNECT;
    notice->m_noticeArg = handle;
    if (0 != m_pReactor->notice(notice))
    {
        delete handle;
        delete notice;
        CWX_ERROR(("Failure to notice unix connection"));
        return -1;
    }
    return (int)uiConnId;
}


int CwxAppFramework::noticeHandle4Msg(CWX_UINT32 uiSvrId,
                CWX_UINT32 uiHostId,
                CWX_HANDLE ioHandle,
                bool bRawData,
                void* userData
                )
{
    if (uiSvrId < SVR_TYPE_USER_START)
    {
        CWX_ERROR(("svr-id must not less than SVR_TYPE_USER_START"));
        return -1;
    }
    CwxAppHandler4IoMsg* handle = m_pHandleCache->fetchIoMsgHandle();
    if (!handle) handle = new CwxAppHandler4IoMsg(this, reactor());
    CWX_UINT32 uiConnId = m_pReactor->getNextConnId();
    handle->getConnInfo().setActiveConn(false);
    handle->getConnInfo().setState(CwxAppConnInfo::ESTABLISHING);
    handle->getConnInfo().setSvrId(uiSvrId);
    handle->getConnInfo().setHostId(uiHostId);
    handle->getConnInfo().setRawData(bRawData);
    handle->getConnInfo().setConnId(uiConnId);
    handle->setHandle(ioHandle);
    handle->getConnInfo().setInvokeCreate(false);
    handle->getConnInfo().setUserData(userData);
    CwxAppNotice* notice = new CwxAppNotice();
    notice->m_unNoticeType = CwxAppNotice::ADD_IO_HANDLE;
    notice->m_noticeArg = handle;
    if (0 != m_pReactor->notice(notice))
    {
        handle->setHandle(CWX_INVALID_HANDLE);
        handle->close();
        delete notice;
        CWX_ERROR(("Failure to notice io handle"));
        return -1;
    }
    return (int)uiConnId;
}

int CwxAppFramework::noticeHandle4Event(CWX_UINT32 uiSvrId,
                                 CWX_UINT32 uiHostId,
                                 CWX_HANDLE handle,
                                 void* userData,
                                 CWX_UINT16 unEventMask,
                                 CWX_UINT32 uiMillSecond)
{
    if (uiSvrId < SVR_TYPE_USER_START)
    {
        CWX_ERROR(("svr-id must not less than SVR_TYPE_USER_START"));
        return -1;
    }
    unEventMask &= CwxAppHandler4Base::RW_MASK;
    if (!unEventMask)
    {
        CWX_ERROR(("Not set handle's event mask, svr_id=%u, handle=%u", uiSvrId, (CWX_UINT32)handle));
        return -1;
    }
    CwxAppHandler4IoEvent* pHandle = m_pHandleCache->fetchIoEventHandle();
    if (!pHandle) pHandle = new CwxAppHandler4IoEvent(this, reactor());
    pHandle->setHandle(handle);
    pHandle->setSvrId(uiSvrId);
    pHandle->setHostId(uiHostId);
    pHandle->setIoEventMask(unEventMask);
    pHandle->setUserData(userData);
    int ret = reactor()->registerHandler(handle, pHandle, unEventMask,CWX_APP_INVALID_CONN_ID, uiMillSecond);
    if (0 != ret)
    {
        pHandle->setHandle(CWX_INVALID_HANDLE);
        m_pHandleCache->cacheIoEventHandle(pHandle);
        CWX_ERROR(("Failure to register io handler to reactor"));
        return -1;
    }
    return 0;
}

int CwxAppFramework::noticeCloseListen(CWX_UINT32 uiListenId)
{
    CwxAppNotice* notice = new CwxAppNotice();
    notice->m_unNoticeType = CwxAppNotice::CLOSE_LISTEN;
    notice->m_noticeArg = (void*)((size_t)(uiListenId));
    if (0 != m_pReactor->notice(notice))
    {
        delete notice;
        CWX_ERROR(("Failure to notice close accept"));
        return -1;
    }
    return 0;
}

int CwxAppFramework::noticeCloseConn(CWX_UINT32 uiConnId)
{
    CwxAppNotice* notice = new CwxAppNotice();
    notice->m_unNoticeType = CwxAppNotice::CLOSE_CONNECT;
    notice->m_noticeArg = (void*)((size_t)(uiConnId));
    if (0 != m_pReactor->notice(notice))
    {
        delete notice;
        CWX_ERROR(("Failure to notice close connection"));
        return -1;
    }
    return 0;
}

int CwxAppFramework::noticeResumeListen(CWX_UINT32 uiListenId)
{
    if (!m_pListenMgr->resume(uiListenId))
    {
        CWX_ERROR(("Failure to resume listen"));
        return -1;
    }
    return 0;
}

int CwxAppFramework::noticeSuspendListen(CWX_UINT32 uiListenId){
    if (!m_pListenMgr->suspend(uiListenId))
    {
        CWX_ERROR(("Failure to suspend listen"));
        return -1;
    }
    return 0;
}

int CwxAppFramework::noticeResumeConn(CWX_UINT32 uiConnId){
    if (!m_pReactor->resumeHandlerByConnId(uiConnId, CwxAppHandler4Base::READ_MASK))
    {
        CWX_ERROR(("Failure to resume conn[%u]", uiConnId));
        return -1;
    }
    return 0;
}

int CwxAppFramework::noticeSuspendConn(CWX_UINT32 uiConnId){
    if (!m_pReactor->suspendHandlerByConnId(uiConnId, CwxAppHandler4Base::READ_MASK))
    {
        CWX_ERROR(("Failure to suspend conn[%u]", uiConnId));
        return -1;
    }
    return 0;
}


int CwxAppFramework::noticeCancelHandle4Event(CWX_HANDLE handle){
    CwxAppHandler4IoEvent* handler = (CwxAppHandler4IoEvent*)m_pReactor->removeHandler(handle);
    if (!handler)
    {
        CWX_ERROR(("Failure to cancel handle[%d]", handle));
        return -1;
    }
    handler->setHandle(CWX_INVALID_HANDLE);
    handler->close();
    return 0;
}

int CwxAppFramework::noticeReconnect(CWX_UINT32 uiConnId, CWX_UINT32 uiDelay){
    CwxAppNotice* notice = new CwxAppNotice();
    notice->m_unNoticeType = CwxAppNotice::RECONNECTION;
    notice->m_noticeArg = (void*)((size_t)(uiConnId));
    notice->m_noticeArg1 = (void*)((size_t)(uiDelay));
    if (0 != m_pReactor->notice(notice))
    {
        delete notice;
        CWX_ERROR(("Failure to notice Reconnect connection"));
        return -1;
    }
    return 0;
}

int CwxAppFramework::noticeFork(CwxAppForkEnv* pForkEnv){
    CWX_ASSERT(pForkEnv);
    m_forkMgr.append(pForkEnv);
    return 0;
}


int CwxAppFramework::sendMsgByConn(CwxMsgBlock* msg)
{
    if (!msg || !msg->length())
    {
        CWX_ERROR(("sendMsgByConn's msg is null, conn_id:%u", msg?msg->send_ctrl().getConnId():0));
        return -1;
    }
    if (!msg->send_ctrl().getConnId())
    {
        CWX_ERROR(("CwxAppFramework::sendMsgByConn's conn-id must be more than 0"));
        return -1;
    }
    CwxAppNotice* notice = new CwxAppNotice();
    notice->m_unNoticeType = CwxAppNotice::SEND_MSG_BY_CONN;
    notice->m_noticeArg = (void*)msg;
    if (0 != m_pReactor->notice(notice))
    {
        delete notice;
        CWX_ERROR(("Failure to notice send msg event by connection"));
        return -1;
    }
    return 0;
}

/**
@brief 往CWX_APP_MSG_MODE模式的SVR分组发送消息，具体发送的连接在OnSendMsgBySvr()中选定。
@param [in] msg 要发送的消息，不能为空，此数据包有架构负责释放。
@return 0：成功； -1：失败。
*/
int CwxAppFramework::sendMsgBySvr(CwxMsgBlock* msg)
{
    if (!msg || !msg->length())
    {
        CWX_ERROR(("sendMsgBySvr's msg is null, svr_id:%u", msg?msg->send_ctrl().getSvrId():0));
        return -1;
    }
    if (!msg->send_ctrl().getSvrId())
    {
        CWX_ERROR(("CwxAppFramework::sendMsgBySvr's svr-id must be more than 0"));
        return -1;
    }
    CwxAppNotice* notice = new CwxAppNotice();
    notice->m_unNoticeType = CwxAppNotice::SEND_MSG_BY_SVR;
    notice->m_noticeArg = (void*)msg;
    if (0 != m_pReactor->notice(notice))
    {
        delete notice;
        CWX_ERROR(("Failure to notice send msg event by svr"));
        return -1;
    }
    return 0;
}



CwxTss* CwxAppFramework::onTssEnv()
{
    return new CwxTss();
}

void CwxAppFramework::onTime(CwxTimeValue const& current)
{
    CWX_UNUSED_ARG(current);
    if (isStopped()) return;
    static time_t lastLogTime=0;
    if (time(NULL) - lastLogTime > getLogCheckSecond())
    {//check log
        if (CwxLogger::instance()->getLogFileSize() > (int)this->getLogFileSize())
        {
            CwxLogger::instance()->nextLog(false);
        }
        lastLogTime = time(NULL);
    }


    static time_t ttLastStateCheck = time(NULL);
    if (time(NULL) - ttLastStateCheck > 1)
    {        
        ttLastStateCheck = time(NULL);
        CwxTss::instance()->getThreadInfo().setUpdateTime(ttLastStateCheck);
        if (APP_MODE_ALONE != m_unAppMode)
        {
            if (1 == ::getppid())
            {
                CWX_ERROR(("Parent proccess is stopped, existing..."));
                this->stop();
            }
        }
        if (APP_MODE_TWIN == m_unAppMode)
        {
            kill(::getppid(), SIGHUP);
        }
    }
}


void CwxAppFramework::onSignal(int )
{

}


int CwxAppFramework::onConnCreated(CWX_UINT32 uiSvrId,
                          CWX_UINT32 uiHostId,
                          CWX_HANDLE handle,
                          bool& bSuspend)
{
    CWX_UNUSED_ARG(uiSvrId);
    CWX_UNUSED_ARG(uiHostId);
    CWX_UNUSED_ARG(handle);
    CWX_UNUSED_ARG(bSuspend);
    return -1;
}


int CwxAppFramework::onConnCreated(CwxAppHandler4Msg& conn,
                                   bool& bSuspendConn,
                                   bool& bSuspendListen)
{
    CWX_UNUSED_ARG(conn);
    CWX_UNUSED_ARG(bSuspendConn);
    CWX_UNUSED_ARG(bSuspendListen);
    return 1;
}

int CwxAppFramework::onFailConnect(CwxAppHandler4Msg& conn)
{
    CWX_DEBUG(("Failure to connect, svr_id=%u, host_id=%u, conn_id=%u",
        conn.getConnInfo().getSvrId(),
        conn.getConnInfo().getHostId(),
        conn.getConnInfo().getConnId()));
    return 0;
}

int CwxAppFramework::onConnClosed(CwxAppHandler4Msg& conn)
{
    CWX_DEBUG(("connect closed, svr_id=%u, host_id=%u, conn_id=%u",
        conn.getConnInfo().getSvrId(),
        conn.getConnInfo().getHostId(),
        conn.getConnInfo().getConnId()));
    return 0;

}
int CwxAppFramework::onRecvMsg(CwxMsgBlock* msg,
                               CwxAppHandler4Msg& conn,
                               CwxMsgHead const& header,
                               bool& bSuspendConn)
{
    CWX_DEBUG(("recv msg, svr_id=%u, host_id=%u, conn_id=%u, msg_type=%u",
        conn.getConnInfo().getSvrId(),
        conn.getConnInfo().getHostId(),
        conn.getConnInfo().getConnId(),
        header.getMsgType()));
    if (msg) CwxMsgBlockAlloc::free(msg);
    bSuspendConn = false;
    return 0;
    
}

int CwxAppFramework::onRecvMsg(CwxAppHandler4Msg& conn,
                      bool& )
{
    CWX_DEBUG(("recv msg, svr_id=%u, host_id=%u, conn_id=%u",
        conn.getConnInfo().getSvrId(),
        conn.getConnInfo().getHostId(),
        conn.getConnInfo().getConnId()));
    char szBuf[4096];
    int ret = 0;
    while(1)
    {
        ret = CwxSocket::read(conn.getHandle(), szBuf, 4096);
        if (0>=ret)
        {
            if ((0 == ret) || (errno != EWOULDBLOCK))
            {
                return -1; //error
            }
            return 0;
        }
        if (ret < 4096) return 0;
    }
    return ret;
}
/**
@brief 通知sendMsgByMsg()发送消息，需要有用户自己选择发送的连接并发送。<br>
@param [in] msg 要发送的消息。
@return -1：无法发送。 0：成功发送消息。
*/
int CwxAppFramework::onSendMsgBySvr(CwxMsgBlock* )
{
    CWX_ERROR(("Not overload the CwxAppFramework::onSendMsgBySvr."));
    return -1;
}

int CwxAppFramework::onStartSendMsg(CwxMsgBlock* msg,
                                    CwxAppHandler4Msg& conn)
{
    CWX_UNUSED_ARG(msg);
    CWX_DEBUG(("begin send msg, svr_id=%u, host_id=%u, conn_id=%u",
        conn.getConnInfo().getSvrId(),
        conn.getConnInfo().getHostId(),
        conn.getConnInfo().getConnId()));
    return 0;
}

CWX_UINT32 CwxAppFramework::onEndSendMsg(CwxMsgBlock*& msg,
                                         CwxAppHandler4Msg& conn)
{
    CWX_UNUSED_ARG(msg);
    CWX_DEBUG(("finish send msg, svr_id=%u, host_id=%u, conn_id=%u",
        conn.getConnInfo().getSvrId(),
        conn.getConnInfo().getHostId(),
        conn.getConnInfo().getConnId()));
    return CwxMsgSendCtrl::UNDO_CONN;
}

void CwxAppFramework::onFailSendMsg(CwxMsgBlock*& msg)
{
    CWX_DEBUG(("finish send msg, svr_id=%u, host_id=%u, conn_id=%u",
        msg->send_ctrl().getSvrId(),
        msg->send_ctrl().getHostId(),
        msg->send_ctrl().getConnId()));
}
void CwxAppFramework::onEvent4Handle(CWX_UINT32 uiSvrId,
                                    CWX_UINT32 uiHostId,
                                    CWX_HANDLE handle,
                                    CWX_UINT16 unRegEventMask,
                                    CWX_UINT16 unEventMask,
                                    void* userData)
{
    CWX_UNUSED_ARG(userData);
    CWX_DEBUG(("Recv handle event without accepting, fd=%d, svr_id=%u, host_id=%u, reg-mask=%u, mask=%u",
        (int)handle,
        uiSvrId,
        uiHostId,
        unRegEventMask,
        unEventMask));
    ::close(handle);
}

void CwxAppFramework::onStdinInput(char* msg)
{
    CWX_UNUSED_ARG(msg);
    CWX_DEBUG(("recv msg from stdin"));
}

void CwxAppFramework::onStdinClosed()
{
    CWX_DEBUG(("stdin is closed"));
}

void CwxAppFramework::onStop()
{
    CWX_DEBUG(("framework is stopping"));
}

void CwxAppFramework::onHook()
{
    CWX_DEBUG(("onHook is invoking"));
}


int CwxAppFramework::run()
{
    if (this->initRunEnv() == -1)
    {
        this->destroy();
		pid_t pid=getppid();
		if (1 != pid){
			kill(pid, SIGQUIT);
		}
        return -1;
    }
    if (this->reactor())
    {
        m_bStopped = false;
        pid_t  pid=getpid();
        this->reactor()->run(CwxAppFramework::hook, this, false);
        if (pid != ::getpid())
        {//it's child
           return 0;
        }
        onStop();
    }
    this->destroy();
    return 0;
}

void CwxAppFramework::stop()
{
    m_bStopped = true;
    if (this->reactor()) this->reactor()->stop();
}


void CwxAppFramework::blockSignal(int signal)
{
    if (m_pReactor)
    {
        if (m_pReactor->isRegSigHandle(signal))
        {
            CwxAppHandler4Base* handler = m_pReactor->removeSignal(signal);
            if (handler) delete handler;
        }
    }
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, signal);
    sigprocmask(SIG_BLOCK, &mask, NULL);
}

void CwxAppFramework::unblockSignal(int signal)
{
    if (m_pReactor)
    {
        if (m_pReactor->getSigHandler(signal)) return;
        CwxAppHandler4Signal* handler = new CwxAppHandler4Signal(this, m_pReactor, signal);
        if (0 != m_pReactor->registerSignal(signal, handler))
        {
            CWX_ERROR(("Failure to unblock signal[%d]", signal));
            delete handler;
        }
    }
}


int CwxAppFramework::initRunEnv()
{
    if (!this->m_strWorkDir.length()){
        CWX_ERROR(("Must set work directory."));
        return -1;
    }

    string logPath = this->m_strWorkDir +"/log/";
    if (!CwxFile::isDir(logPath.c_str())){
        CwxFile::createDir(logPath.c_str());
    }

    //start log
    string logFile = this->m_strWorkDir +"/log/" + this->m_strAppName;
    if (0 != CwxLogger::instance()->init(logFile.c_str(),
        m_unLogFileNum,
        m_uiLogSize,
        true))
    {
        CWX_ERROR(("Failure to init logger."));
        return -1;
    }

    //change work directory
    ::chdir (this->m_strWorkDir.c_str());
    //create the listen map
    m_pListenMgr = new CwxAppListenMgr(this);
    //create the handler cache
    m_pHandleCache = new CwxAppHandlerCache();
    //register the notice function
    regNoticeFunc();
    //create the notice handle
    m_pNoticeHandler = new CwxAppHandler4Notice(this, m_pReactor);
    m_pReactor = new CwxAppReactor(true);
    if (0 != m_pReactor->open(m_pNoticeHandler))
    {
        CWX_ERROR(("Failure to invoke CwxAppReactor::open()"));
        return -1;
    }

    //create the tcp connector
    m_pTcpConnector = new CwxAppTcpConnector(this, m_pReactor);
    //create the unix connector
    m_pUnixConnector = new CwxAppUnixConnector(this, m_pReactor);


    //init the task board
    if (0 != m_taskBoard.init()){
        CWX_ERROR(("Failure to init task-board"));
        return -1;
    }


    //init signal
    if (!m_pReactor->isRegSigHandle(SIGINT))
    {
        m_pReactor->registerSignal(SIGINT,
            new CwxAppHandler4Signal(this, m_pReactor, SIGINT));
    }
    if (!m_pReactor->isRegSigHandle(SIGQUIT))
    {
        m_pReactor->registerSignal(SIGQUIT,
            new CwxAppHandler4Signal(this, m_pReactor, SIGQUIT));
    }
    if (!m_pReactor->isRegSigHandle(SIGTERM))
    {
        m_pReactor->registerSignal(SIGTERM,
            new CwxAppHandler4Signal(this, m_pReactor, SIGTERM));
    }
    if (!m_pReactor->isRegSigHandle(SIGUSR1))
    {
        m_pReactor->registerSignal(SIGUSR1,
            new CwxAppHandler4Signal(this, m_pReactor, SIGUSR1));
    }
    if (!m_pReactor->isRegSigHandle(SIGUSR2))
    {
        m_pReactor->registerSignal(SIGUSR2,
            new CwxAppHandler4Signal(this, m_pReactor, SIGUSR2));
    }
    if (!m_pReactor->isRegSigHandle(SIGCHLD))
    {
        m_pReactor->registerSignal(SIGCHLD,
            new CwxAppHandler4Signal(this, m_pReactor, SIGCHLD));
    }
    if (!m_pReactor->isRegSigHandle(SIGCLD))
    {
        m_pReactor->registerSignal(SIGCLD,
            new CwxAppHandler4Signal(this, m_pReactor, SIGCLD));
    }
    if (!m_pReactor->isRegSigHandle(SIGHUP))
    {
        m_pReactor->registerSignal(SIGHUP,
            new CwxAppHandler4Signal(this, m_pReactor, SIGHUP));
    }
    if (!m_pReactor->isRegSigHandle(SIGPIPE))
    {
        m_pReactor->registerSignal(SIGPIPE,
            new CwxAppHandler4Signal(this, m_pReactor, SIGPIPE));
    }
    //create the tcp connnector
    m_pTcpConnector = new CwxAppTcpConnector(this, m_pReactor);
    //create the unix connector
    this->m_pUnixConnector = new CwxAppUnixConnector(this, m_pReactor);

    //register the timer, 
    if (0 == m_uiTimeClick)  m_uiTimeClick = 1000;
    {
        m_pTimeHandler = new CwxAppHandler4Time(this, m_pReactor, CwxTimeValue(m_uiTimeClick/1000,(m_uiTimeClick%1000)*1000));
        if (0 != m_pTimeHandler->open(NULL))
        {
            delete m_pTimeHandler;
            m_pTimeHandler = NULL;
            CWX_ERROR(("Failure to register schedule_timer"));
            return -1;
        }
    }
    return 0;
}

void CwxAppFramework::regNoticeFunc(){
    m_arrNoticeApi[CwxAppNotice::DUMMY] = NULL;
    m_arrNoticeApi[CwxAppNotice::SEND_MSG_BY_CONN] = &CwxAppFramework::innerNoticeSendMsgByConn;
    m_arrNoticeApi[CwxAppNotice::SEND_MSG_BY_SVR] = &CwxAppFramework::innerNoticeSendMsgBySvr;
    m_arrNoticeApi[CwxAppNotice::TCP_CONNECT] = &CwxAppFramework::innerNoticeTcpConnect;
    m_arrNoticeApi[CwxAppNotice::UNIX_CONNECT] = &CwxAppFramework::innerNoticeUnixConnect;
    m_arrNoticeApi[CwxAppNotice::ADD_IO_HANDLE] = &CwxAppFramework::innerNoticeAddIoHandle;
    m_arrNoticeApi[CwxAppNotice::TCP_LISTEN] = &CwxAppFramework::innerNoticeTcpListen;
    m_arrNoticeApi[CwxAppNotice::UNIX_LISTEN] = &CwxAppFramework::innerNoticeUnixListen;
    m_arrNoticeApi[CwxAppNotice::CLOSE_LISTEN] = &CwxAppFramework::innerNoticeCloseListen;
    m_arrNoticeApi[CwxAppNotice::CLOSE_CONNECT] = &CwxAppFramework::innerNoticeCloseConnect;
    m_arrNoticeApi[CwxAppNotice::RECONNECTION] = &CwxAppFramework::innerNoticeReconnect;
}

int CwxAppFramework::hook(void* arg)
{
    CwxAppFramework* pApp = (CwxAppFramework*)arg;
    if (!pApp->isStopped()){
        ///分发事件
        pApp->noticeEvent();
        //invoke the hook
        if (pApp->m_bEnableHook) pApp->onHook();
        //dispatch the fork event
        if (pApp->m_forkMgr.isForkEvent()){
            list<CwxAppForkEnv*> forks;
            pid_t  src_pid = getpid();
            pid_t  curPid = 0;
            pApp->m_forkMgr.getForkEnvs(forks);
            list<CwxAppForkEnv*>::iterator iter = forks.begin();
            CwxAppForkEnv* pForkEnv = NULL;
            while(iter != forks.end()){
                pForkEnv = *iter;
                curPid = pForkEnv->onFork();
                if (-1 == curPid){
                    CWX_ERROR(("Failure to fork"));
                }else if (0 == curPid){//child
                    CWX_ASSERT(src_pid != getpid());
                    iter++;
                    //delete the fork event in child process
                    while(iter != forks.end()){
                        delete *iter;
                        iter++;
                    }
                    pApp->m_bStopped = true;
                    pApp->m_pReactor->forkReinit();
                    //invoke child
                    pForkEnv->onChildEntry(pApp);
                    pApp->destroy();
                    pForkEnv->onChildMain();
                    delete pForkEnv;
                    return -1;
                }
                CWX_ASSERT(src_pid == getpid());
                delete *iter;
                //it's parent
                iter++;
            }
        }
    }
    return 0;
}

void CwxAppFramework::destroy()
{
    m_bStopped = true;
    m_bAppRunValid = true;
    m_strAppRunFailReason = "";
    m_taskBoard.reset();
    m_commander.reset();

    if (m_pReactor)
    {
        m_pReactor->stop();
    }

    //delete  tcp connector
    if (this->m_pTcpConnector){
        delete this->m_pTcpConnector;
        this->m_pTcpConnector = NULL;
    }
    //delete  unix connector
    if (this->m_pUnixConnector){
        delete this->m_pUnixConnector;
        this->m_pUnixConnector = NULL;
    }

    if (m_pListenMgr){
        delete m_pListenMgr;
        m_pListenMgr = NULL;
    }

    if (this->m_pReactor)
    {
        m_pReactor->close();
        delete this->m_pReactor;
        this->m_pReactor = NULL;
    }
    //it's released by reactor
    m_pNoticeHandler=NULL;
    //it's released by reactor
    m_pStdIoHandler = NULL;

    //delete m_pHandleCache
    if (m_pHandleCache)
    {
        delete m_pHandleCache;
        m_pHandleCache = NULL;
    }
    if (m_pTss){
      delete m_pTss;
      m_pTss = NULL;
    }
    m_bStopped = false;
    m_bEnableHook = false;
    m_bDebug = false;
}

void CwxAppFramework::noticeEvent()
{
    CwxAppNotice* pTmp = NULL;
    CwxAppNotice* pHead = NULL;
    m_pReactor->noticed(pHead);
    while(pHead)
    {
        if (m_arrNoticeApi[pHead->m_unNoticeType]) m_arrNoticeApi[pHead->m_unNoticeType](this, pHead);
        pTmp = pHead->m_next;
        delete pHead;
        pHead = pTmp;
    }//end while
}

///notice send msg
void CwxAppFramework::innerNoticeSendMsgByConn(CwxAppFramework* pApp,
                                               CwxAppNotice* pNotice)
{
    CwxMsgBlock* msg = (CwxMsgBlock*)pNotice->m_noticeArg;
    CwxAppHandler4Msg* pConn = (CwxAppHandler4Msg*)pApp->m_pReactor->getHandlerByConnId(msg->send_ctrl().getConnId());
    if (!pConn || 
        (CwxAppConnInfo::ESTABLISHED != pConn->getConnInfo().getState()))
    {
        if (msg->send_ctrl().isFailNotice())
        {
            pApp->onFailSendMsg(msg);
        }
        if (msg) CwxMsgBlockAlloc::free(msg);
        pNotice->m_noticeArg = NULL;
        return;
    }
    if (!msg->send_ctrl().isUndoConn())
    {
        int ret = 0;
        if (msg->send_ctrl().isResumeConn())
        {
            ret = pConn->reactor()->resumeHandler(pConn, CwxAppHandler4Base::READ_MASK);
            if (0 != ret)
            {
                CWX_ERROR(("Failure to resume handler, conn_id=[%u]", msg->send_ctrl().getConnId()));
                pConn->close();
            }
        }
        else if (msg->send_ctrl().isSuspendConn())
        {
            ret = pConn->reactor()->suspendHandler(pConn, CwxAppHandler4Base::READ_MASK);
            if (0 != ret)
            {
                CWX_ERROR(("Failure to resume handler, conn_id=[%u]", msg->send_ctrl().getConnId()));
                pConn->close();
            }
        }
    }
    if (!pConn->putMsg(msg))
    {
        if (msg->send_ctrl().isFailNotice())
        {
            pApp->onFailSendMsg(msg);
        }
        if (msg) CwxMsgBlockAlloc::free(msg);
        pNotice->m_noticeArg = NULL;
    }
}


///notice send msg by svr
void CwxAppFramework::innerNoticeSendMsgBySvr(CwxAppFramework* pApp,
                                    CwxAppNotice* pNotice)
{
    CwxMsgBlock* msg = (CwxMsgBlock*)pNotice->m_noticeArg;
    if (0 != pApp->onSendMsgBySvr(msg))
    {
        if (msg->send_ctrl().isFailNotice())
        {
            pApp->onFailSendMsg(msg);
        }
        if (msg) CwxMsgBlockAlloc::free(msg);
    }
    pNotice->m_noticeArg = NULL;
}

///notice tcp connect
void CwxAppFramework::innerNoticeTcpConnect(CwxAppFramework* pApp,
                                            CwxAppNotice* pNotice)
{
    CwxAppHandler4TcpConn* handle = (CwxAppHandler4TcpConn*)pNotice->m_noticeArg;
    if (-1 == pApp->m_pTcpConnector->connect(handle,
        handle->getConnectAddr().c_str(),
        handle->getConnectPort(),
        handle->getConnInfo().getFamily(),
        handle->getConnInfo().getSockFunc(),
        handle->getConnInfo().getSockFuncArg()))
    {
        handle->close();
    }
}

///notice unix connect
void CwxAppFramework::innerNoticeUnixConnect(CwxAppFramework* pApp, CwxAppNotice* pNotice)
{
    CwxAppHandler4UnixConn* handle = (CwxAppHandler4UnixConn*)pNotice->m_noticeArg;
    if (-1 == pApp->m_pUnixConnector->connect(handle,
        handle->getConnectPathFile().c_str(),
        handle->getConnInfo().getSockFunc(),
        handle->getConnInfo().getSockFuncArg()))
    {
        handle->close();
    }
}

///notice add Io listen
void CwxAppFramework::innerNoticeAddIoHandle(CwxAppFramework* ,
                                             CwxAppNotice* pNotice)
{
    CwxAppHandler4IoMsg* handle = (CwxAppHandler4IoMsg*)pNotice->m_noticeArg;
    if (0 != handle->open())
    {
        CWX_ERROR(("Failure to open io msg handler ,conn_id[%u]", handle->getConnInfo().getConnId()));
        handle->close();
    }
}

///notice tcp accept
void CwxAppFramework::innerNoticeTcpListen(CwxAppFramework* pApp, CwxAppNotice* pNotice)
{
    CwxAppTcpAcceptor* acceptor = (CwxAppTcpAcceptor*)pNotice->m_noticeArg;
    pApp->m_pListenMgr->add(acceptor);
}
///notice unix accept
void CwxAppFramework::innerNoticeUnixListen(CwxAppFramework* pApp, CwxAppNotice* pNotice)
{
    CwxAppUnixAcceptor* acceptor = (CwxAppUnixAcceptor*)pNotice->m_noticeArg;
    // add to map
    pApp->m_pListenMgr->add(acceptor);
}

///notice close listen
void CwxAppFramework::innerNoticeCloseListen(CwxAppFramework* pApp, CwxAppNotice* pNotice)
{
    CWX_UINT32 uiListenId = (CWX_UINT32)((size_t)(pNotice->m_noticeArg));
    pApp->m_pListenMgr->close(uiListenId);
}

///notice close connect
void CwxAppFramework::innerNoticeCloseConnect(CwxAppFramework* pApp, CwxAppNotice* pNotice)
{
    CWX_UINT32 uiConnId = (CWX_UINT32)((size_t)(pNotice->m_noticeArg));
    CwxAppHandler4Msg* pConn = (CwxAppHandler4Msg*)pApp->m_pReactor->getHandlerByConnId(uiConnId);
    if (!pConn)
    {
        CWX_ERROR(("Execute closing connect by conn_id[%u], it doesn't exist", uiConnId));
        return ;
    }
    pConn->getConnInfo().setActiveClose(true);
    pConn->close();
}

///notice noticeReconnect 
void CwxAppFramework::innerNoticeReconnect(CwxAppFramework* pApp, CwxAppNotice* pNotice)
{
    CWX_UINT32 uiConnId = (CWX_UINT32)((size_t)(pNotice->m_noticeArg));
    CWX_UINT32 uiDelay = (CWX_UINT32)((size_t)(pNotice->m_noticeArg1));
    CwxAppHandler4Msg * pConn = (CwxAppHandler4Msg*)pApp->m_pReactor->getHandlerByConnId(uiConnId);
    if (!pConn)
    {
        CWX_ERROR(("Execute closing connect by conn_id[%u], it doesn't exist", uiConnId));
        return ;
    }
    if (!pConn->getConnInfo().isActiveConn())
    {
        CWX_ERROR(("Execute closing connect by conn_id[%u], it's not active conn", uiConnId));
        return ;
    }
    if (CwxAppConnInfo::CONNECTING == pConn->getConnInfo().getState())
    {
        CWX_DEBUG(("Conn[%s] is waiting connect, don't reconnect"));
        return;
    }
    pConn->getConnInfo().setActiveClose(false);
    pConn->getConnInfo().setReconn(true);
    pConn->getConnInfo().setReconnDelay(uiDelay);
    pConn->close();
}

CWINUX_END_NAMESPACE
