#include "CwxEchoApp.h"
#include "CwxDate.h"

///构造函数
CwxEchoApp::CwxEchoApp()
{
    m_eventHandler = NULL;
    m_threadPool = NULL;
}

///析构函数
CwxEchoApp::~CwxEchoApp(){
}

///初始化
int CwxEchoApp::init(int argc, char** argv){
    string strErrMsg;
    ///首先调用架构的init api
    if (CwxAppFramework::init(argc, argv) == -1) return -1;
    ///检查是否通过-f指定了配置文件，若没有，则采用默认的配置文件
    if ((NULL == this->getConfFile()) || (strlen(this->getConfFile()) == 0)){
        this->setConfFile("svr_conf.cnf");
    }
    ///加载配置文件，若失败则退出
    if (0 != m_config.loadConfig(getConfFile())){
        CWX_ERROR((m_config.getError()));
        return -1;
    }
    ///设置运行日志的输出level
    setLogLevel(CwxLogger::LEVEL_ERROR|CwxLogger::LEVEL_INFO|CwxLogger::LEVEL_WARNING);
    return 0;
}

///配置运行环境信息
int CwxEchoApp::initRunEnv(){
    ///设置系统的时钟间隔，最小刻度为1ms，此为1s。
    this->setClick(1000);//1s
    ///设置工作目录
    this->setWorkDir(this->m_config.m_strWorkDir.c_str());
    ///设置循环运行日志的数量
    this->setLogFileNum(LOG_FILE_NUM);
    ///设置每个日志文件的大小
    this->setLogFileSize(LOG_FILE_SIZE*1024*1024);
    ///调用架构的initRunEnv，使以上设置的参数生效
    if (CwxAppFramework::initRunEnv() == -1 ) return -1;
    blockSignal(SIGPIPE);
    //set version
    this->setAppVersion(ECHO_APP_VERSION);
    //set last modify date
    this->setLastModifyDatetime(ECHO_MODIFY_DATE);
    //set compile date
    this->setLastCompileDatetime(CWX_COMPILE_DATE(_BUILD_DATE));

    ///将加载的配置文件信息输出到日志文件中，以供查看检查
    string strConfOut;
    m_config.outputConfig(strConfOut);
    CWX_INFO((strConfOut.c_str()));

    ///注册echo请求的处理handle，echo请求的svr-id为SVR_TYPE_ECHO
    m_eventHandler = new CwxEchoEventHandler(this);         
    this->getCommander().regHandle(SVR_TYPE_ECHO, m_eventHandler);

    ///监听TCP连接，其建立的连接的svr-id都为SVR_TYPE_ECHO，接收的消息的svr-id都为SVR_TYPE_ECHO。
    ///全部由m_eventHandler对象来处理
    if (0 > this->noticeTcpListen(SVR_TYPE_ECHO, 
        this->m_config.m_listen.getHostName().c_str(),
        this->m_config.m_listen.getPort(),
        false,
        CWX_APP_MSG_MODE,
        CwxEchoApp::setSockAttr,
        this))
    {
        CWX_ERROR(("Can't register the echo acceptor port: addr=%s, port=%d",
            this->m_config.m_listen.getHostName().c_str(),
            this->m_config.m_listen.getPort()));
        return -1;
    }
    ///监听UNIX DOMAIN连接，其建立的连接的svr-id都为SVR_TYPE_ECHO，接收的消息的svr-id都为SVR_TYPE_ECHO。
    ///全部由m_eventHandler对象来处理
    if (0 > this->noticeLsockListen(SVR_TYPE_ECHO, 
        this->m_config.m_strUnixPathFile.c_str()))
    {
        CWX_ERROR(("Can't register the echo unix acceptor port: path=%s",
            m_config.m_strUnixPathFile.c_str()));
        return -1;
    }
    ///创建线程池对象，此线程池中线程的group-id为2，线程池的线程数量为m_config.m_unThreadNum。
    m_threadPool = new CwxThreadPool(m_config.m_unThreadNum,
        &getCommander());
    ///启动线程，线程池中线程的线程栈大小为1M。
    if ( 0 != m_threadPool->start(NULL)){
        CWX_ERROR(("Failure to start thread pool"));
        return -1;
    }
    return 0;

}

///时钟函数，什么也没有做
void CwxEchoApp::onTime(CwxTimeValue const& current){
    CwxAppFramework::onTime(current);
}

///信号处理函数
void CwxEchoApp::onSignal(int signum){
    switch(signum){
    case SIGQUIT: 
        ///若监控进程通知退出，则推出
        CWX_INFO(("Recv exit signal , exit right now."));
        this->stop();
        break;
    default:
        ///其他信号，全部忽略
        CWX_INFO(("Recv signal=%d, ignore it.", signum));
        break;
    }

}

///echo请求的请求消息
int CwxEchoApp::onRecvMsg(CwxMsgBlock* msg, CwxAppHandler4Msg& conn, CwxMsgHead const& header, bool& bSuspendConn){

    msg->event().setSvrId(conn.getConnInfo().getSvrId());
    msg->event().setHostId(conn.getConnInfo().getHostId());
    msg->event().setConnId(conn.getConnInfo().getConnId());
    msg->event().setIoHandle(conn.getHandle());
    msg->event().setConnUserData(NULL);
    msg->event().setMsgHeader(header);
    msg->event().setEvent(CwxEventInfo::RECV_MSG);
    msg->event().setTimestamp(CwxDate::getTimestamp());
    ///不停止继续接受
    bSuspendConn = false;
    CWX_ASSERT (msg);
    ///将消息放到线程池队列中，有内部的线程调用其处理handle来处理
    m_threadPool->append(msg);
    return 0;

}

int CwxEchoApp::setSockAttr(CWX_HANDLE handle, void* arg)
{
    CwxEchoApp* app=(CwxEchoApp*)arg;
    int iSockBuf = 1024 * 1024;
    while (setsockopt(handle, SOL_SOCKET, SO_SNDBUF, (void*)&iSockBuf, sizeof(iSockBuf)) < 0)
    {
        iSockBuf -= 1024;
        if (iSockBuf <= 1024) break;
    }
    iSockBuf = 1024 * 1024;
    while(setsockopt(handle, SOL_SOCKET, SO_RCVBUF, (void *)&iSockBuf, sizeof(iSockBuf)) < 0)
    {
        iSockBuf -= 1024;
        if (iSockBuf <= 1024) break;
    }

    if (app->m_config.m_listen.isKeepAlive())
    {
        if (0 != CwxSocket::setKeepalive(handle,
            true,
            CWX_APP_DEF_KEEPALIVE_IDLE,
            CWX_APP_DEF_KEEPALIVE_INTERNAL,
            CWX_APP_DEF_KEEPALIVE_COUNT))
        {
            CWX_ERROR(("Failure to set listen addr:%s, port:%u to keep-alive, errno=%d",
                app->m_config.m_listen.getHostName().c_str(),
                app->m_config.m_listen.getPort(),
                errno));
            return -1;
        }
    }

    int flags= 1;
    if (setsockopt(handle, IPPROTO_TCP, TCP_NODELAY, (void *)&flags, sizeof(flags)) != 0)
    {
        CWX_ERROR(("Failure to set listen addr:%s, port:%u NODELAY, errno=%d",
            app->m_config.m_listen.getHostName().c_str(),
            app->m_config.m_listen.getPort(),
            errno));
        return -1;
    }
    struct linger ling= {0, 0};
    if (setsockopt(handle, SOL_SOCKET, SO_LINGER, (void *)&ling, sizeof(ling)) != 0)
    {
        CWX_ERROR(("Failure to set listen addr:%s, port:%u LINGER, errno=%d",
            app->m_config.m_listen.getHostName().c_str(),
            app->m_config.m_listen.getPort(),
            errno));
        return -1;
    }
    return 0;
}

void CwxEchoApp::destroy()
{
    if (m_threadPool){
        m_threadPool->stop();
        delete m_threadPool;
        m_threadPool = NULL;
    }
    if (m_eventHandler)
    {
        delete m_eventHandler;
        m_eventHandler = NULL;
    }
    CwxAppFramework::destroy();
}



