#include "CwxEchoApp.h"
#include "CwxDate.h"

///构造函数
CwxEchoApp::CwxEchoApp() {
    m_eventHandler = NULL;
    m_threadPool = NULL;
    m_thriftServerThread = NULL; 
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
    ///创建线程池对象，此线程池中线程的group-id为2，线程池的线程数量为m_config.m_unThreadNum。
    m_threadPool = new CwxThreadPool(m_config.m_unThreadNum,
        &getCommander());
    ///启动线程，线程池中线程的线程栈大小为1M。
    if ( 0 != m_threadPool->start(NULL)){
        CWX_ERROR(("Failure to start thread pool"));
        return -1;
    }
    /// 创建thrift thread manager线程的tss对象
    CwxTss** pTss = new CwxTss*[m_config.m_unThreadNum];
    for (uint16_t i=0; i<m_config.m_unThreadNum; i++) {
      pTss[i] = new EchoTss();
      ((EchoTss*) pTss[i])->Init();
    }
    CwxThreadPoolThrift* threadPool = new CwxThreadPoolThrift(m_config.m_unThreadNum,
      pTss);
    m_threadManager = boost::shared_ptr<CwxThreadPoolThrift>(threadPool);
    ///启动server的主线程
    m_thriftServerThread = new CwxThreadPool(1, &getCommander(), ThreadMain, this);
    if ( 0 != m_thriftServerThread->start(NULL)){
      CWX_ERROR(("Failure to start thrift server pool"));
      return -1;
    }
    return 0;
}

void* CwxEchoApp::ThreadMain(CwxTss* , CwxMsgQueue* , void* arg) {
  CwxEchoApp* app = (CwxEchoApp*) arg;
  boost::shared_ptr<CwxEchoThriftIf> rpc_handler=boost::shared_ptr<CwxEchoThriftIf>(new CwxEchoThriftIf(app));
  /// 创建thrift的server
  boost::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());
  boost::shared_ptr<TProcessor> server_processor(new echo_thrift::EchoProcessor(rpc_handler));
  boost::shared_ptr<TServerTransport> serverTransport(
    new TServerSocket(app->m_config.m_listen.getPort()));
  boost::shared_ptr<TTransportFactory> transportFactory(
    new TBufferedTransportFactory());
  app->m_threadManager->start();
  app->m_server = new TThreadPoolServer(server_processor, serverTransport,
    transportFactory, protocolFactory, app->m_threadManager);
  app->m_server->serve();
  app->stop();
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

void CwxEchoApp::destroy() {
  if (m_server) m_server->stop();
  if (m_threadManager.get()) m_threadManager->stop();
  if (m_threadPool){
    m_threadPool->stop();
    delete m_threadPool;
    m_threadPool = NULL;
  }
  if (m_eventHandler){
    delete m_eventHandler;
    m_eventHandler = NULL;
  }
  if (m_server) delete m_server;
  CwxAppFramework::destroy();
}



