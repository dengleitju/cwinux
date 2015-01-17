#ifndef __CWX_ECHO_APP_H__
#define __CWX_ECHO_APP_H__
/*
版权声明：
    本软件遵循GNU GPL V3（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/
#include "CwxAppFramework.h"
#include "CwxEchoChannelConfig.h"
#include "CwxEchoChannelEventHandler.h"
#include "CwxAppChannel.h"

#define ECHO_APP_VERSION "1.0"
#define ECHO_MODIFY_DATE "2010-08-29"


///多线程的echo服务，支持TCP与UNIX-DOMAIN两种协议
class CwxEchoChannelApp : public CwxAppFramework{
public:
    enum{
        LOG_FILE_SIZE = 30, ///<每个可循环使用日志文件的MByte
        LOG_FILE_NUM = 7, ///<可循环使用日志文件的数量
        SVR_TYPE_ECHO = CwxAppFramework::SVR_TYPE_USER_START ///<echo服务的服务类型，及SVR-ID的数值
    };
    ///构造函数
	CwxEchoChannelApp();
    ///析构函数
	virtual ~CwxEchoChannelApp();
    ///重载初始化函数
    virtual int init(int argc, char** argv);
public:
    ///时钟响应函数
    virtual void onTime(CwxTimeValue const& current);
    ///signal响应函数
    virtual void onSignal(int signum);
    ///连接建立
    virtual int onConnCreated(CWX_UINT32 uiSvrId,
        CWX_UINT32 uiHostId,
        CWX_HANDLE handle,
        bool& bSuspendListen);
public:
    static void* ThreadMain(CwxTss* tss, CwxMsgQueue* queue, void*);
    ///0：正常；-1：队列停止
    static int ThreadDoQueue(CwxMsgQueue* queue, CwxAppChannel* channel);

    static int setSockAttr(CWX_HANDLE handle, void* arg);
protected:
    ///重载运行环境设置API
	virtual int initRunEnv();
    virtual void destroy();
private:
    CwxEchoChannelEventHandler*         m_eventHandler;///<echo请求处理的commander handle
    CwxThreadPool**                      m_threadPool;///<线程池对象
    CwxAppChannel**                     m_channel; ///<通信channel
    CwxEchoChannelConfig               m_config;///<配置文件对象
};
#endif

