#ifndef __CWX_APP_PROCESS_MGR_H__
#define __CWX_APP_PROCESS_MGR_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxAppProcessMgr.h
@brief 双进程模式的进程管理器对象，其会创建双进程，一个是工作进程，一个是管理进程
@author cwinux@gmail.com
@version 0.1
@date 2009-07-20
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxType.h"
#include "CwxAppMacro.h"
#include "CwxAppConfig.h"
#include "CwxStl.h"
#include "CwxAppFramework.h"

CWINUX_BEGIN_NAMESPACE

/**
@class CwxAppProcessInfo
@brief 工作进行信息对象
*/
class CWX_API CwxAppProcessInfo
{
public:
    ///工作进程状态
    enum{
        PROC_STATE_RUNNING = 1,///<运行状态
        PROC_STATE_RESTARTING = 2,///<正在重启
        PROC_STATE_STOPPING = 3,///<正在停止
        PROC_STATE_STOPPED = 4///<停止
    };
public:
    ///构造函数
    CwxAppProcessInfo()
    {
        m_pApp = NULL;
        m_unProcId = 0;
        m_pid = 0;
        m_ttLastChildHeat = 0;
        m_unState = PROC_STATE_STOPPED;
    }
    ///析构函数
    ~CwxAppProcessInfo()
    {
        if (m_pApp) delete m_pApp;
    }
public:
    CwxAppFramework*   m_pApp;///<工作进程的对象
    CWX_UINT16         m_unProcId;///<工作进程的ID
    pid_t              m_pid;///<工作进程的pid
    time_t             m_ttLastChildHeat;///<工作进程上次发送心跳的事件
    CWX_UINT8          m_unState;///<工作进程的状态
};

/**
@class CwxAppProcessMgr
@brief 双进程模式的进程管理器对象，其会创建双进程，
一个是工作进程，一个是管理进程。工作进程会定期的给监控进程发送HUP的心跳信号
*/

class CWX_API CwxAppProcessMgr
{
public:
    /**
    @brief 初始化进程管理器
    @param [in] pApp 工作进程的APP
    @return -1：失败。 0：成功
    */
    static int init(CwxAppFramework* pApp);
    /**
    @brief 启动工作进程及监控进程
    @param [in] argc 服务启动时的参数个数
    @param [in] argv 服务启动时的参数
    @param [in] unMaxHeartbeatInternal 工作进程发送心跳的最大间隔（s）
    @param [in] unDelaySec 启动的初期，延时发送心跳的时间（s）
    @return -1：启动失败。 0：启动成功。
    */
#ifndef WITH_CWINUX_GFLAGS
    static int start(int argc, char** argv, CWX_UINT16 unMaxHeartbeatInternal, CWX_UINT16 unDelaySec);
#else
    static int start(char const* app_name, CWX_UINT16 unMaxHeartbeatInternal, CWX_UINT16 unDelaySec);
#endif
private:
    ///注册信号处理handle
    static int regSigHandle();
#ifndef WITH_CWINUX_GFLAGS
    ///检查服务启动的命令行参数
    static int checkRunCmd(int argc, char** argv);
    ///启动工作进程
    static int startProcess(int argc, char** argv, CWX_UINT16 unDelaySec);
#else
    ///检查服务启动的命令行参数
    static int checkRunCmd(char const* app_name);
    ///启动工作进程
    static int startProcess(char const* app_name, CWX_UINT16 unDelaySec);
#endif
    ///SIGQUIT 信号句柄
    static void stopHandler(int , siginfo_t *info, void *);
    ///SIGINT 信号句柄
    static void restartHandler(int , siginfo_t *info, void *);
    ///SIGCHLD 信号句柄，此为工作进程退出的信号
    static void childExitHandler(int , siginfo_t *info, void *);
    ///SIGHUP的心跳信号句柄
    static void childHeatHandler(int , siginfo_t *info, void *);
    ///阻塞指定的信号
    static void blockSignal(int signal);
    ///取消阻塞指定的信号
    static void unblockSignal(int signal);
    ///锁住进程文件。返回进程文件的FILE
    static FILE* lock();
    ///解锁进程文件。同时关闭文件
    static void unlock(FILE* fd);
    ///判断进程是否存在,-1：失败；0：不存在；1：存在
    static int isExistProc(char const* szProcName, int pid);
    ///获取pid文件中的进程id，-1：失败
    static int getProcId(char const* szPidFile);

private:
    ///禁止创建对象实例
    CwxAppProcessMgr()
    {

    }
private:
    static CwxAppProcessInfo*  m_pProcess;///<工作进程实例
    static string      m_strPorcfile;///<进程文件名
    static string      m_strAppName;///<服务名
    static string      m_strAppLockFile;///<进程锁文件名
    static bool        m_bExit;///<是否停止服务
};


CWINUX_END_NAMESPACE
#include "CwxPost.h"

#endif
