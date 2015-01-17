#ifndef __CWX_APP_FRAMEWORK_H__
#define __CWX_APP_FRAMEWORK_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxAppFramework.h
@brief 定义架构基类CwxAppFramework
@author cwinux@gmail.com
@version 0.1
@date 2009-07-20
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxLockGuard.h"
#include "CwxMutexLock.h"
#include "CwxDate.h"
#include "CwxAppConfig.h"
#include "CwxAppMacro.h"
#include "CwxLogger.h"
#include "CwxAppTcpAcceptor.h"
#include "CwxAppTcpConnector.h"
#include "CwxAppUnixAcceptor.h"
#include "CwxAppUnixConnector.h"
#include "CwxAppHandler4TcpConn.h"
#include "CwxAppHandler4UnixConn.h"
#include "CwxAppHandler4IoEvent.h"
#include "CwxAppHandler4Signal.h"
#include "CwxAppHandler4Time.h"
#include "CwxAppHandler4Notice.h"
#include "CwxAppHandler4Stdio.h"
#include "CwxAppHandlerCache.h"
#include "CwxAppConnInfo.h"
#include "CwxTss.h"
#include "CwxThreadPool.h"
#include "CwxCommander.h"
#include "CwxTaskBoard.h"
#include "CwxAppListenMgr.h"
#include "CwxAppForkMgr.h"
#include "CwxAppReactor.h"

CWINUX_BEGIN_NAMESPACE
/**
@class CwxAppFramework
@brief 架构对象的基类，实现了通信、信号、时钟、stdio、日志等的管理。
*/
class CWX_API CwxAppFramework
{
private:
    ///Service与host对象的hash的KEY对象
    class HostMapKey
    {
    public:
        HostMapKey(CWX_UINT32 uiSvrId, CWX_UINT32 uiHostId)
            :m_uiSvrId(uiSvrId),m_uiHostId(uiHostId)
        {
        }
        bool operator==(HostMapKey const& item) const
        {
            return (m_uiSvrId == item.m_uiSvrId) &&
                (m_uiHostId == item.m_uiHostId); 
        }
        size_t hash() const
        {
            return (m_uiSvrId<<3) + m_uiHostId;
        }
    private:
        CWX_UINT32 m_uiSvrId;
        CWX_UINT32 m_uiHostId;
    };
    ///IO handle的hash定义
    typedef hash_set<CWX_HANDLE, char, CwxNumHash<CWX_UINT32> > HandleHash;

    ///消息处理函数的类型定义
    typedef void (*fnNoticeApi)(CwxAppFramework* pApp, CwxAppNotice* pNotice);

public:
    ///APP模式的枚举类型定义
    enum{
        APP_MODE_ALONE=1,///<单进程模式
        APP_MODE_TWIN=2,///<双进程模式
        APP_MODE_PIPE=3///<多进程模式
    };
    ///系统参数常量定义
    enum{
        DEF_LOG_CHECK_SECOND = 600,///<缺省的运行日志检查间隔
        MIN_LOG_CHECK_SECOND = 10,///<最小的运行日志检查间隔
        MAX_LOG_CHECK_SECOND = 1200,///<最大的运行日志检查间隔
        DEF_KEEPALIVE_SECOND = 30,///<缺省的连接KEEP-ALIVE检查间隔
        MIN_KEEPALIVE_SECOND = 1,///<最小的连接KEEP-ALIVE检查间隔
        MAX_KEEPALIVE_SECOND = 300,///<最大的连接KEEP-ALIVE检查间隔
        DEF_KEEPALIVE_REPLY_SECOND = 10///<KEEP-ALIVE回复时延
    };
    ///服务类型与线程组定义
    enum{
        SVR_TYPE_USER_START  = 2///用户服务类型的最小值
    };
    /**
    @brief 构造函数
    @param [in] unAppMode APP的模式，缺省为双进程模式
    @param [in] uiMaxTaskNum Taskboard中管理的TASK的最大值
    */
    CwxAppFramework(CWX_UINT16 unAppMode=CwxAppFramework::APP_MODE_TWIN,
        CWX_UINT32 uiMaxTaskNum=CWX_APP_MAX_TASK_NUM);
    ///析构函数
    virtual ~CwxAppFramework();
    /**
    @brief 架构的初始化函数，派生类需要重载此API实现自己对象的初始化，<br>
    但需要首先调用父类的init()以实现父类环境的初始化。
    @param [in] argc main的argc，为输入参数的数量
    @param [in] argv main的argv，为输入参数的列表
    @return -1：失败； 0：成功； 1：help
    */
#ifndef WITH_CWINUX_GFLAGS
    virtual int init(int argc, char** argv);
#else
    virtual int init(char const* app_name);
#endif
public:
    /**
    @brief 注册接受标准输入的信息。若注册成功，则标准输入的输入将通过OnStdinInput通知。
    @return -1：注册失败；0：注册成功
    */
    int noticeStdinListen();
    /**
    @brief 往架构注册一个监听的TCP 地址+PORT，并设置各种属性
    @param [in] uiSvrId 由此监听所建立的连接的SVR ID。
    @param [in] szAddr 监听的IP地址, *表示监听所有的本地地址
    @param [in] unPort 监听的端口号
    @param [in] bRawData 此监听地址所建立的连接的上接受的数据，是否具有包头<br>
    true：具有包头；false：没有包头
    @param [in] unMode Framework对连接的管理方式，<br>
            CWX_APP_MSG_MODE表示有架构负责消息的收发；<br>
            CWX_APP_EVENT_MODE表示架构只负责连接上读写事件的通知。
    @param [in] fn listen socket的熟悉设置函数，若为空，则不设置。
    @param [in] fnArg fn函数的arg参数。
    @param [in] iFamily AF_INET或AF_INET6。
    @return >0：此监听的Listen ID；-1：失败。
    */
    int noticeTcpListen(CWX_UINT32 uiSvrId,
        char const* szAddr,
        CWX_UINT16 unPort,
        bool bRawData = false,
        CWX_UINT16 unMode=CWX_APP_MSG_MODE,
        CWX_NET_SOCKET_ATTR_FUNC fn=NULL,
        void* fnArg = NULL,
        CWX_INT32  iFamily = AF_INET
        );
    /**
    @brief 往架构注册一个local ipc监听，并设置各种属性
    @param [in] uiSvrId 由此监听所建立的连接的SVR ID。
    @param [in] szPathFile local ipc对应的文件
    @param [in] bRawData 此监听地址所建立的连接的上接受的数据，是否具有包头<br>
    true：具有包头；false：没有包头
    @param [in] unMode Framework对连接的管理方式，<br>
            CWX_APP_MSG_MODE表示有架构负责消息的收发；<br>
            CWX_APP_EVENT_MODE表示架构只负责连接上读写事件的通知。
    @param [in] fn listen socket的熟悉设置函数，若为空，则不设置。
    @param [in] fnArg fn函数的arg参数。
    @return >0：此监听的Listen ID；-1：失败。
    */
    int noticeLsockListen(CWX_UINT32 uiSvrId,
        char const* szPathFile,
        bool bRawData = false,
        CWX_UINT16 unMode=CWX_APP_MSG_MODE,
        CWX_NET_SOCKET_ATTR_FUNC fn=NULL,
        void* arg = NULL);
    /**
    @brief 往架构注册一个主动的TCP连接
    @param [in] uiSvrId 设定连接的SVR ID。
    @param [in] uiHostId 设定连接的Host ID。
    @param [in] szAddr 连接的IP地址。
    @param [in] unPort 连接的端口号。
    @param [in] bRawData 此连接上接受的数据，是否具有包头.true：具有包头；false：没有包头
    @param [in] unMinRetryInternal 连接失败时的最小连接间隔.
    @param [in] unMaxRetryInternal 连接失败时的最大连接间隔.
    @param [in] fn socket的熟悉设置函数，若为空，则不设置。
    @param [in] fnArg fn函数的arg参数。
	@param [in] uiMiliTimeout 连接超时时间，缺省0表示不进行超时控制，单位为毫秒。
    @return  >0：此连接的CONN_ID；-1：注册失败。
    */
    int noticeTcpConnect(CWX_UINT32 uiSvrId,
        CWX_UINT32 uiHostId,
        char const* szAddr,
        CWX_UINT16 unPort,
        bool bRawData = false,
        CWX_UINT16 unMinRetryInternal = 1,
        CWX_UINT16 unMaxRetryInternal = 60,
        CWX_NET_SOCKET_ATTR_FUNC fn=NULL,
        void* fnArg=NULL,
        CWX_UINT32 uiMiliTimeout=0,
        CWX_INT32  iFamily = AF_UNSPEC);
    /**
    @brief 往架构注册一个主动的Local IPC连接
    @param [in] uiSvrId 设定连接的SVR ID。
    @param [in] uiHostId 设定连接的Host ID。
    @param [in] szPathFile local IPC连接对应的本地文件。
    @param [in] bRawData 此连接上接受的数据，是否具有包头. true：具有包头；false：没有包头
    @param [in] unMinRetryInternal 连接失败时的最小连接间隔.
    @param [in] unMaxRetryInternal 连接失败时的最大连接间隔.
    @param [in] fn socket的熟悉设置函数，若为空，则不设置。
    @param [in] fnArg fn函数的arg参数。
    @return  >0：此连接的CONN_ID；-1：注册失败。
    */
    int noticeLsockConnect(CWX_UINT32 uiSvrId,
        CWX_UINT32 uiHostId,
        char const* szPathFile,
        bool bRawData = false,
        CWX_UINT16 unMinRetryInternal = 1,
        CWX_UINT16 unMaxRetryInternal = 60,
        CWX_NET_SOCKET_ATTR_FUNC fn=NULL,
        void* fnArg = NULL);
    /**
    @brief 往架构注册一个IO handle连接,此链接一定是CWX_APP_MSG_MODE模式。
    @param [in] uiSvrId 设定连接的SVR ID。
    @param [in] uiHostId 设定连接的Host ID。
    @param [in] handle IO handle。
    @param [in] bRawData 此连接上接受的数据，是否具有包头. true：具有包头；false：没有包头.
    @param [in] userData 与此链接相关的用户数据
    @param [in] pPai 通信数据适配器对象
    @return  >0：此连接的CONN_ID；-1：注册失败。
    */
    int noticeHandle4Msg(CWX_UINT32 uiSvrId,
        CWX_UINT32 uiHostId,
        CWX_HANDLE handle,
        bool bRawData = false,
        void* userData = NULL
        );
    /**
    @brief 往架构注册一个IO handle的事件监听
    @param [in] uiSvrId  Svr id。
    @param [in] uiHostId  host id
    @param [in] handle IO handle。
    @param [in] userData 与此链接相关的用户数据
    @param [in] unEventMask 检测的事件掩码.
    @param [in] uiMillSecond 超时毫秒数，0表示不进行超时检测。
    @return 0：成功;-1：注册失败。
    */
    int noticeHandle4Event(CWX_UINT32 uiSvrId,
        CWX_UINT32 uiHostId,
        CWX_HANDLE handle,
        void* userData,
        CWX_UINT16 unEventMask= CwxAppHandler4Base::RW_MASK,
        CWX_UINT32 uiMillSecond=0
        );
    /**
    @brief 关闭Socket的listen监听
    @param [in] uiListenId Listen的id。
    @return  0：成功； -1：失败。
    */
    int noticeCloseListen(CWX_UINT32 uiListenId);
    /**
    @brief 关闭一个有架构管理的Socket 连接，为CWX_APP_MSG_MODE
    @param [in] uiConnId socket的连接ID。
    @return 0：成功； -1：失败。
    */
    int noticeCloseConn(CWX_UINT32 uiConnId);
    /**
    @brief resume 一个监听端口的监听
    @param [in] uiListenId 监听的Listen id.
    @return 0：成功； -1：失败。
    */
    int noticeResumeListen(CWX_UINT32 uiListenId);
    /**
    @brief suspend 一个监听端口的监听
    @param [in] uiListenId 监听的Listen id.
    @return  0：成功； -1：失败。
    */
    int noticeSuspendListen(CWX_UINT32 uiListenId);

    /**
    @brief resume 一个连接的数据接收
    @param [in] uiConnId 连接的CONN ID.
    @return 0：成功； -1：失败。
    */
    int noticeResumeConn(CWX_UINT32 uiConnId);
    /**
    @brief suspend 一个连接的数据接收
    @param [in] uiConnId 连接的CONN ID.
    @return  0：成功； -1：失败。
    */
    int noticeSuspendConn(CWX_UINT32 uiConnId);

    /**
    @brief 取消一个handle的事件监听
    @param [in] handle 监听事件的 handle。
    @return 0：成功； -1：失败。
    */
    int noticeCancelHandle4Event(CWX_HANDLE handle);
    /**
    @brief 重新建立指定的socket的连接
    @param [in] conn_id 连接的connection id。
    @param [in] uiDelay 重连延时的毫秒数，0表示立即连接。
    @return 0：成功； -1：失败。
    */
    int noticeReconnect(CWX_UINT32 conn_id, CWX_UINT32 uiDelay=0);
    /**
    @brief 通知事件通知，pForkEnv保存fork相关的环境信息及fork后的child进程的操作
    @param [in] pForkEnv fork相关的环境信息及fork后的child进程的操作。
    @return -1：当前的framework的环境禁止直行fork，0：success。
    */
    int noticeFork(CwxAppForkEnv* pForkEnv);
public:
    /**
    @brief 往CWX_APP_MSG_MODE模式的一条连接发送数据。
    @param [in] msg 要发送的消息，不能为空，此数据包有架构负责释放。
    @return 0：成功； -1：失败。
    */
    int sendMsgByConn(CwxMsgBlock* msg);
    /**
    @brief 往CWX_APP_MSG_MODE模式的SVR分组发送消息，具体发送的连接在OnSendMsgBySvr()中选定。
    @param [in] msg 要发送的消息，不能为空，此数据包有架构负责释放。
    @return 0：成功； -1：失败。
    */
    int sendMsgBySvr(CwxMsgBlock* msg);

public:
    /**
    @brief 创建通信线程的thread self store。
    @return 通信线程的tss对象指针
    */
    virtual CwxTss* onTssEnv();
    /**
    @brief 时钟通知，只要设置了时钟间隔，则会定时调用此API。
    @param [in] current 当前的时间。
    @return void
    */
    virtual void onTime(CwxTimeValue const& current);
    /**
    @brief 信号通知，若收到了一个没有屏蔽的信号，则会定时调用此API。
    @param [in] signum 收到的信号。
    @return void
    */
    virtual void onSignal(int signum);
    /**
    @brief 通知建立建立一个CWX_APP_EVENT_MODE连接。建立的连接是非阻塞模式。<br>
    @param [in] uiSvrId 连接的svr id。
    @param [in] uiHostId 连接的host id。
    @param [in] handle 连接的handle。
    @param [out] bSuspendListen 对于被动连接，若为true,则停止继续监听，否则继续监听
    @return <0：关闭连接； >=0连接有效。
    */
    virtual int onConnCreated(CWX_UINT32 uiSvrId,
        CWX_UINT32 uiHostId,
        CWX_HANDLE handle,
        bool& bSuspendListen);

    /**
    @brief 通知建立CWX_APP_MSG_MODE一个连接。建立的连接全部是非阻塞模式。<br>
    @param [in] conn 连接兑现，只能在此API中时候，上层不能缓存。
    @param [out] bSuspendConn 若为true,则暂停消息接收；false，接收连接上的消息
    @param [out] bSuspendListen 对于被动连接，若为true,则停止继续监听，否则继续监听
    @return <0：关闭连接； >=0连接有效。
    */
    virtual int onConnCreated(CwxAppHandler4Msg& conn,
        bool& bSuspendConn,
        bool& bSuspendListen);
    /**
    @brief 主动连接失败。
    @param [in] conn 失败连接的信息。
    @return <0 停止连接；0：默认方式； >0：下次连接重试的时间间隔，单位为ms。
    */
    virtual int onFailConnect(CwxAppHandler4Msg& conn);
    /**
    @brief 通知CWX_APP_MSG_MODE模式的连接关闭。
    @param [in] conn 关闭的连接。
    @return 对于主动连接，-1：表示不连接，0：默认方式， >0：下次重连的时间间隔，单位为ms；被动连接忽略。
    */
    virtual int onConnClosed(CwxAppHandler4Msg & conn);

    /**
    @brief 通知从CWX_APP_MSG_MODE模式的、非raw类型连接收到一个消息
    @param [in] msg 收到的消息，空表示没有消息体。
    @param [in] conn 收到消息的连接。
    @param [in] header 收到消息的消息头。
    @param [out] bSuspendConn 若为true,则暂停消息接收；false，接收连接上的消息。
    @return -1：消息无效，关闭连接。 0：不连续接受消息； >0：连续从此连接上接受消息。
    */
    virtual int onRecvMsg(CwxMsgBlock* msg,
        CwxAppHandler4Msg& conn,
        CwxMsgHead const& header,
        bool& bSuspendConn);
    /**
    @brief 通知CWX_APP_MSG_MODE模式的、raw类型连接有数据到达，数据需要用户自己读取
    @param [in] conn 有消息的连接。
    @param [out] bSuspendConn 若为true,则暂停消息接收；false，接收连接上的消息。
    @return -1：消息无效，关闭连接。 0：成功。
    */
    virtual int onRecvMsg(CwxAppHandler4Msg& conn,
        bool& bSuspendConn);
    /**
    @brief 通知sendMsgByMsg()发送消息，需要有用户自己选择发送的连接并发送。<br>
    @param [in] msg 要发送的消息。
    @return -1：无法发送。 0：成功发送消息。
    */
    virtual int onSendMsgBySvr(CwxMsgBlock* msg);
    /**
    @brief 通知CWX_APP_MSG_MODE模式的连接开始发送一个消息。<br>
    只有在sendMsg()的时候指定BEGIN_NOTICE的时候才调用.
    @param [in] msg 要发送的消息。
    @param [in] conn 发送消息的连接。
    @return -1：取消消息的发送。 0：发送消息。
    */
    virtual int onStartSendMsg(CwxMsgBlock* msg,
        CwxAppHandler4Msg& conn);
    /**
    @brief 通知CWX_APP_MSG_MODE模式的连接完成一个消息的发送。<br>
    只有在sendMsg()的时候指定FINISH_NOTICE的时候才调用.
    @param [in,out] msg 传入发送完毕的消息，若返回NULL，则msg有上层释放，否则底层释放。
    @param [in] conn 发送消息的连接。
    @return 
            CwxMsgSendCtrl::UNDO_CONN：不修改连接的接收状态
            CwxMsgSendCtrl::RESUME_CONN：让连接从suspend状态变为数据接收状态。
            CwxMsgSendCtrl::SUSPEND_CONN：让连接从数据接收状态变为suspend状态
    */
    virtual CWX_UINT32 onEndSendMsg(CwxMsgBlock*& msg,
        CwxAppHandler4Msg& conn);

    /**
    @brief 通知CWX_APP_MSG_MODE模式的连接上，一个消息发送失败。<br>
    只有在sendMsg()的时候指定FAIL_NOTICE的时候才调用.
    @param [in,out] msg 发送失败的消息，若返回NULL，则msg有上层释放，否则底层释放。
    @return void。
    */
    virtual void onFailSendMsg(CwxMsgBlock*& msg);
    /**
    @brief 对一个IO HANDLE 的READ/WRITE READY的时候通知。
    @param [in] uiSvrId handle的svr id。
    @param [in] uiHostId handle的host id。
    @param [in] handle 可读或可写的IO HANDLE。
    @param [in] unRegEventMask 监控的事件。
    @param [in] unEventMask  ready的事件，若超时的话，会有CwxAppHandler4Base::TIMEOUT_MASK。
    @param [in] userData 注册时的userData
    @return void。
    */
    virtual void onEvent4Handle(CWX_UINT32 uiSvrId,
        CWX_UINT32 uiHostId,
        CWX_HANDLE handle,
        CWX_UINT16 unRegEventMask,
        CWX_UINT16 unEventMask,
        void* userData);
    /**
    @brief STDIN的输入时间。
    @param [in] msg 收到的消息
    @return void。
    */
    virtual void onStdinInput(char* msg);

    ///STDIN关闭事件，当终端关闭的时候，就会收到此消息，此消息只通知一遍。
    virtual void onStdinClosed();
    ///通知进程退出，需要停止上层的线程
    virtual void onStop();
    ///架构的挂钩，用于与其他的消息型架构集成
    virtual void onHook();
public:
    ///启动架构, 0:成功，-1：失败。推出前，会自己释放自己。
    int run();
    ///停止架构
    void stop();
    /**
    @brief 解析服务运行参数
    @param [in] argc main的argc，为输入参数的数量
    @param [in] argv main的argv，为输入参数的列表
    @return -1：失败； 0：成功； 1：help
    */
#ifndef WITH_CWINUX_GFLAGS
    int parse(int argc, char** argv);
#else
    int parse();
#endif
    ///output help information
    void help();
    ///屏蔽指定的信号
    void blockSignal(int signal);
    ///解除对指定信号的屏蔽
    void unblockSignal(int signal);
    ///事件引擎的hook api,-1：退出，0：继续
    static int hook(void* arg);
public:
    ///返回进程运行状态是否正常
    bool isAppRunValid() const;
    ///设置进程运行状态是否正常
    void setAppRunValid(bool bValid);
    ///获取进程运行不正常的原因
    string const& getAppRunFailReason() const;
    ///设置进程运行不正常的原因
    void setAppRunFailReason(string const& strReason);
    ///设置程序的版本号
    void setAppVersion(string const& strVersion);
    ///获取程序的版本号
    string const& getAppVersion() const;
    ///设置程序的最新修改时间
    void setLastModifyDatetime(string const& strDatetime);
    ///获取程序的最新修改时间
    string const& getLastModifyDatetime() const;
    ///设置程序的编译时间
    void setLastCompileDatetime(string const& strDatetime);
    ///获取程序的编译时间
    string const& getLastCompileDatetime() const;
    ///服务的启动参数是否要停止服务。
    bool isCmdStop() const;
    ///服务的启动参数是否要重新启动服务。
    bool isCmdRestart() const;
    ///服务是否正在退出
    bool isStopped() const;
    ///获取工作目录
    char const*  getWordDir() const;
    ///设置工作目录
    void setWorkDir(char const* szWorkDir);
    ///获取配置文件的名字
    char const* getConfFile() const;
    ///设置配置文件
    void setConfFile(char const* szConfFile);
    ///获取系统的时钟间隔
    CWX_UINT32 getClick() const ;
    ///设置系统的时钟间隔,单位为毫秒，0表示没有时钟。
    void setClick(CWX_UINT32 uiInternal);
    ///获取循环日志文件的大小
    CWX_UINT32 getLogFileSize() const;
    ///设置循环日志文件的大小
    void setLogFileSize(CWX_UINT32 uiSize);
    ///获取日志级别
    CWX_UINT32 getLogLevel() const;
    ///设置日志级别
    void setLogLevel(CWX_UINT32 unLevel);
    ///获取循环日志的数量
    CWX_UINT16 getLogFileNum() const;
    ///设置循环日志的数量
    void setLogFileNum(CWX_UINT16 unLogFileNum);
    ///设置循环日志的日志文件
    void toLogFileNo(CWX_UINT16 unLogFileNo);
    ///获取keep-alive的时间间隔
    CWX_UINT16 getKeepAliveSecond() const;
    ///设置keep-alive的时间间隔
    void setKeepAliveSecond(CWX_UINT16 unSecond);
    ///获取log的check间隔
    CWX_UINT16 getLogCheckSecond() const ;
    ///设置Log的check间隔
    void setLogCheckSecond(CWX_UINT16 unSecond) ;
    ///获取是否在debug模式
    bool isDebug() const;
    ///设置debug模式
    void setDebug(bool bDebug);
    ///check enable onHook
    bool isEnableHook();
    ///set enable onHook
    void enableHook(bool bEnable);
    ///设置进程ID
    CWX_UINT16 getProcId() const;
    ///获取进程ID
    void setProcId(CWX_UINT16 uiProcId);
    ///获取架构的commander
    CwxCommander& getCommander();
    ///获取架构的taskboard对象
    CwxTaskBoard& getTaskBoard();
    ///获取通信线程的tss信息
    CwxTss*  getAppTss();
    ///获取handle cache
    CwxAppHandlerCache*  getHandlerCache();
    ///获取TCP connector
    CwxAppTcpConnector* getTcpConnector();
    ///获取UNIX connector
    CwxAppUnixConnector* getUnixConnector();
    ///return the reactor.
    CwxAppReactor* reactor();
public:
    /*
    一下接口是由底层的msg handler调用的，子类或其他对象不能够使用。
    */ 

    ///通知一条连接建立，与onConnCreated()对应
    int openConn(CwxAppHandler4Msg& conn,
        bool& bStopListen);
    ///通知收到了一条消息，与onRecvMsg()对应。
    int recvMessage(CwxMsgHead& header,
        CwxMsgBlock* msg,
        CwxAppHandler4Msg& conn,
        bool& bSuspendConn);
    ///通知连接关闭，与onConnClosed()对应。
    int connClosed(CwxAppHandler4Msg& conn);
    ///accept notice event
    void noticeEvent();
protected:
    ///配置架构的运行设置，返回值：0:success, -1:failure.
    virtual int initRunEnv();
    ///释放架构的所有资源，每个重载类必须重载此API释放自己的资源。并在最后调用基类的destroy。
    ///此API必须是可重入的，也就是说，可以反复调用
    ///若app中有线程池，则线程池的stop必须在此API中。
    virtual void destroy();

private:
    ///注册架构事件接收的事件处理方法
    void  regNoticeFunc();
    ///notice send msg by conn
    static void innerNoticeSendMsgByConn(CwxAppFramework* pApp,
        CwxAppNotice* pNotice);
    ///notice send msg by svr
    static void innerNoticeSendMsgBySvr(CwxAppFramework* pApp,
        CwxAppNotice* pNotice);
    ///notice tcp connect
    static void innerNoticeTcpConnect(CwxAppFramework* pApp,
        CwxAppNotice* pNotice);
    ///notice unix connect
    static void innerNoticeUnixConnect(CwxAppFramework* pApp,
        CwxAppNotice* pNotice);
    ///notice add Io listen
    static void innerNoticeAddIoHandle(CwxAppFramework* pApp,
        CwxAppNotice* pNotice);
    ///notice tcp accept
    static void innerNoticeTcpListen(CwxAppFramework* pApp,
        CwxAppNotice* pNotice);
    ///notice unix accept
    static void innerNoticeUnixListen(CwxAppFramework* pApp,
        CwxAppNotice* pNotice);
    ///notice close listen
    static void innerNoticeCloseListen(CwxAppFramework* pApp,
        CwxAppNotice* pNotice);
    ///notice close connect
    static void innerNoticeCloseConnect(CwxAppFramework* pApp,
        CwxAppNotice* pNotice);
    ///notice noticeReconnect 
    static void innerNoticeReconnect(CwxAppFramework* pApp,
        CwxAppNotice* pNotice);
    ///通知收到了一条系统消息。
    int recvSysMessage(CwxMsgBlock* msg,
        CwxAppHandler4Msg& conn,
        CwxMsgHead const& header);

private:
    bool                       m_bAppRunValid;///<进程运行正常标记
    string                     m_strAppRunFailReason;///<进程运行不正常的原因
    string                     m_strVersion;///<程序的版本号
    string                     m_strLastModifyDatetime;///<程序的最后修改时间
    string                     m_strLastCompileDatetime;///<程序的编译时间
    CwxAppHandler4StdIo*        m_pStdIoHandler;///<接收标准输入的Handle
    CwxAppHandler4Time*         m_pTimeHandler; ///<时钟事件的handler
    CwxAppHandler4Notice*       m_pNoticeHandler; ///<处理通知事件的Handle
    CWX_UINT16                 m_unAppMode;///<APP 模式
    bool                       m_bCmdStop;///<启动命令是否要停止服务
    bool                       m_bCmdRestart;///<启动命令是否要重启服务
    bool                       m_bStopped;///<服务是否在退出状态
    CwxAppReactor*	            m_pReactor; ///<the reactor for event loop.
    CWX_UINT32                 m_uiTimeClick; ///<时钟刻度，单位为ms
    string                      m_strWorkDir; ///<工作目录.
    string                      m_strAppName; //<服务名字
    string                      m_strConfFile; ///<服务的配置文件
    CWX_UINT32                  m_uiLogSize; ///<日志文件的大小.
    CWX_UINT16                  m_unLogFileNum; ///<日志文件的数量
    CWX_UINT16                  m_unDefKeepAliveSecond; ///<连接KEEP-ALIVE的检查间隔
    CWX_UINT16                  m_unLogCheckSecond; ///<运行log文件的检查间隔
    bool                        m_bEnableHook;///<是否开发架构的HOOK
    CWX_UINT16                  m_uiProcId; ///<多进程模式的进程序号
    CwxCommander              m_commander;///<commander对象
    CwxTaskBoard              m_taskBoard;///<taskboard对象
    CwxAppListenMgr*             m_pListenMgr;///<监听管理器对象
    CwxTss*                  m_pTss;///<通信线程的TSS
    CwxAppHandlerCache*          m_pHandleCache;///<释放连接句柄cache
    CwxAppTcpConnector*         m_pTcpConnector; ///<TCP的connector
    CwxAppUnixConnector*        m_pUnixConnector; ///<unix的connector
    //not lock for communite thread only access
    fnNoticeApi                m_arrNoticeApi[CwxAppNotice::ALL_NOTICE_NUM + 1];///<notcie的消息映射
    //调试控制
    bool                      m_bDebug; ///<是否在调试中
    //following is fork's info
    CwxAppForkMgr              m_forkMgr; ///fork的事件管理器
};
CWINUX_END_NAMESPACE

#include "CwxAppFramework.inl"

#include "CwxPost.h"


#endif
