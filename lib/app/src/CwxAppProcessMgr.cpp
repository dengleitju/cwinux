#include "CwxAppProcessMgr.h"
#include "CwxFile.h"
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
CWINUX_BEGIN_NAMESPACE

CwxAppProcessInfo* CwxAppProcessMgr::m_pProcess = NULL;
string  CwxAppProcessMgr::m_strPorcfile;
string  CwxAppProcessMgr::m_strAppName;
string  CwxAppProcessMgr::m_strAppLockFile;
bool CwxAppProcessMgr::m_bExit = false;


int CwxAppProcessMgr::init(CwxAppFramework* pApp)
{
    if (m_pProcess)
    {
        delete m_pProcess;
    }
    m_pProcess = new CwxAppProcessInfo;
    m_pProcess->m_unProcId = 1;
    m_pProcess->m_pApp = pApp;
    m_pProcess->m_unState = CwxAppProcessInfo::PROC_STATE_STOPPED;
    m_bExit = false;
    return 0;
}

#ifndef WITH_CWINUX_GFLAGS
int CwxAppProcessMgr::start(int argc, char** argv, CWX_UINT16 unMaxHeartbeatInternal, CWX_UINT16 unDelaySec)
#else
int CwxAppProcessMgr::start(char const* app_name, CWX_UINT16 unMaxHeartbeatInternal, CWX_UINT16 unDelaySec)
#endif
{
    //chech the your command
    int ret = 0;
#ifndef WITH_CWINUX_GFLAGS
    ret = checkRunCmd(argc, argv);
#else
    ret = checkRunCmd(app_name);
#endif
    if (0 != ret) return 0;

    pid_t	pid1, pid2;
    char    szCmd[512];
    if ( (pid1 = fork()) < 0)
    {
        printf("fork error");
        exit(0);
    }
    else if (pid1 == 0)
    {		/* first child */
        if ( (pid2 = fork()) < 0)
        {
            printf("fork error");
            exit(0);
        }
        else if (pid2 == 0)
        { /* second child*/
            if (0 != regSigHandle()) return -1;
            //output process pid
            if (CwxFile::isFile(m_strPorcfile.c_str()))
            {
                int iRet = getProcId(m_strPorcfile.c_str());
                if (-1 == iRet) exit(0);
                iRet = isExistProc(m_strAppName.c_str(), iRet);
                if (-1 == iRet) exit(0);
                if (iRet)
                {
                    printf("process is start, if not, delete file:%s\n", m_strPorcfile.c_str());
                    exit(0);
                }
            }
            sprintf(szCmd, "echo %d > %s", getpid(), m_strPorcfile.c_str());
            system(szCmd);
            while(!m_bExit)
            {
                if (CwxAppProcessInfo::PROC_STATE_STOPPED == m_pProcess->m_unState)
                {
                    FILE* fd=lock();//let child wait parent 
                    if (!fd)
                    {
                        exit(0);
                    }
#ifndef WITH_CWINUX_GFLAGS
                    startProcess(argc, argv, unDelaySec);
#else
                    startProcess(app_name, unDelaySec);
#endif
                    sleep(1);
                    unlock(fd);//child can run now.
                    sleep(1);
                    continue;
                }
                if (time(NULL) - m_pProcess->m_ttLastChildHeat > unMaxHeartbeatInternal)
                {//restart child
                    kill(m_pProcess->m_pid, SIGKILL);
                }
                sleep(1);
            }
            int status;
            waitpid(m_pProcess->m_pid, &status, 1);
            //delete pid file
            CwxFile::rmFile(m_strPorcfile.c_str());
            exit(0);
        }
        printf("child-1 exist.");
        exit(0);//it's wait by parent
    }
    //first parent.
    waitpid(pid1, NULL, 0);
    exit(0);
}

int CwxAppProcessMgr::regSigHandle()
{
    struct sigaction sa;
    //set stop hander
    sa.sa_handler = 0;
    sa.sa_sigaction = CwxAppProcessMgr::stopHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags =SA_SIGINFO;
    if (-1 == sigaction(SIGQUIT, &sa, NULL))
    {
        printf("Failure register SIGQUIT signal handler.");
        return -1;
    }
    //set restart handler
    sa.sa_handler = 0;
    sa.sa_sigaction = CwxAppProcessMgr::restartHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags =SA_SIGINFO;
    if (-1 == sigaction( SIGINT, &sa, NULL))
    {
        printf("Failure register  SIGINT signal handler.");
        return -1;
    }
    //set child stop handler
    sa.sa_handler = 0;
    sa.sa_sigaction = CwxAppProcessMgr::childExitHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags =SA_SIGINFO;
    if (-1 == sigaction(SIGCHLD, &sa, NULL))
    {
        printf("Failure register SIGCHLD signal handler.");
        return -1;
    }
    //set child heat handler
    sa.sa_handler = 0;
    sa.sa_sigaction = CwxAppProcessMgr::childHeatHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags =SA_SIGINFO;
    if (-1 == sigaction(SIGHUP, &sa, NULL))
    {
        printf("Failure register SIGHUP signal handler.");
        return -1;
    }
    return 0;
}

#ifndef WITH_CWINUX_GFLAGS
int CwxAppProcessMgr::checkRunCmd(int argc, char** argv)
#else
int CwxAppProcessMgr::checkRunCmd(char const* app_name)
#endif
{
    pid_t pid;
    char szProcFile[256];

#ifndef WITH_CWINUX_GFLAGS
    CwxFile::getLastDirName(argv[0], m_strAppName);
#else
    CwxFile::getLastDirName(app_name, m_strAppName);
#endif
    m_strAppLockFile =".";
    m_strAppLockFile += m_strAppName + ".lock";

    CwxAppFramework app;
    int iRet = 0;
#ifndef WITH_CWINUX_GFLAGS
    iRet = app.parse(argc, argv);
#else
    iRet = app.parse();
#endif
    if (0 != iRet)
    {
        app.help();
        return 0==iRet?0:-1;
    }

    sprintf(szProcFile, "%s%s.pid", app.getWordDir(), m_strAppName.c_str());
    m_strPorcfile = szProcFile;

    if (app.isCmdRestart() || app.isCmdStop())
    {
        //get runing process's pid
        int iRet = getProcId(m_strPorcfile.c_str());
        if (-1 == iRet) return -1;
        pid = iRet;
        if (app.isCmdRestart())
        {
            kill(pid, SIGQUIT);
        }
        else
        {
            kill(pid, SIGQUIT);
        }
        return -1;
    }
    return 0;
}

//return new process's pid, -1: failure
#ifndef WITH_CWINUX_GFLAGS
int CwxAppProcessMgr::startProcess(int argc, char** argv, CWX_UINT16 unDelaySec)
#else
int CwxAppProcessMgr::startProcess(char const* app_name, CWX_UINT16 unDelaySec)
#endif
{
    pid_t	pid;
    if ( (pid = fork()) < 0)
    {//execute in parent.
        printf("fork error");
        return -1;
    }
    else if (pid > 0)
    { //parent
        m_pProcess->m_ttLastChildHeat = time(NULL) + unDelaySec;
        m_pProcess->m_pid = pid;
        m_pProcess->m_unState = CwxAppProcessInfo::PROC_STATE_RUNNING;
        return pid;
    }
    //the child;
    FILE* fd=lock();//wait parent run first
    if (!fd) exit(0);
    unlock(fd);//now i can run.
    m_pProcess->m_ttLastChildHeat = time(NULL) + unDelaySec;
    m_pProcess->m_pid = getpid();
    m_pProcess->m_unState = CwxAppProcessInfo::PROC_STATE_RUNNING;
#ifndef WITH_CWINUX_GFLAGS
    int ret = m_pProcess->m_pApp->init(argc, argv);
#else
    int ret = m_pProcess->m_pApp->init(app_name);
#endif
    if (-1 == ret)
    {
		pid_t ppid = getppid();
		if (1 != ppid) kill(ppid, SIGKILL);
        printf("Failure to invoke the work process's init().\n");
        exit(0);
    }
    m_pProcess->m_pApp->run();
    delete m_pProcess;
    exit(0);
}


void CwxAppProcessMgr::stopHandler(int , siginfo_t *info, void *)
{
    printf("Recv SIGQUIT. snd_pid=%d for exiting\n", info->si_pid);
    if (m_pProcess->m_pid && (m_pProcess->m_pid != getpid()))
    {
        m_bExit = true;
        m_pProcess->m_unState = CwxAppProcessInfo::PROC_STATE_STOPPING;
        if (info->si_pid != m_pProcess->m_pid) kill(m_pProcess->m_pid, SIGQUIT);
    }
}

void CwxAppProcessMgr::restartHandler(int , siginfo_t *info, void *)
{
    printf("Recv SIGINT. snd_pid=%d for restart\n", info->si_pid);
    if (m_pProcess->m_pid && (m_pProcess->m_pid != getpid()))
    {
        if (!m_bExit)
        {
            m_pProcess->m_unState = CwxAppProcessInfo::PROC_STATE_RESTARTING;
            kill(m_pProcess->m_pid, SIGQUIT);
        }
    }
}

void CwxAppProcessMgr::childExitHandler(int , siginfo_t *info, void *)
{
    printf("Recv SIGCHLD. snd_pid=%d \n", info->si_pid);
    waitpid(-1, NULL, 0);
    if (info->si_pid == m_pProcess->m_pid)
    {
        m_pProcess->m_unState = CwxAppProcessInfo::PROC_STATE_STOPPED;
        m_pProcess->m_pid = 0;
    }
}

void CwxAppProcessMgr::childHeatHandler(int , siginfo_t *info, void *)
{
    printf("Recv SIGHUP. snd_pid=%d\n", info->si_pid);
    if (info->si_pid == m_pProcess->m_pid)
    {
        m_pProcess->m_ttLastChildHeat = time(NULL);
    }
}

void CwxAppProcessMgr::blockSignal(int signal)
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, signal);
    sigprocmask(SIG_BLOCK, &mask, NULL);
}

void CwxAppProcessMgr::unblockSignal(int signal)
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, signal);
    sigprocmask(SIG_UNBLOCK, &mask, NULL);
}

FILE* CwxAppProcessMgr::lock()
{
    struct flock lock;
    FILE* fd = fopen(m_strAppLockFile.c_str(), "w+b");
    if (!fd){
        printf("Failure to open lock file:%s\n", m_strAppLockFile.c_str());
        return NULL;
    }
    lock.l_type = F_WRLCK;
    lock.l_whence =SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    while(1){
        if (-1 == fcntl(fileno(fd), F_SETLKW,  reinterpret_cast<long>(&lock)))
        {
            if (EINTR == errno)
            {
                printf("EINTR lock, pid=%d, Locking file:%s\n", getpid(), m_strAppLockFile.c_str());
                continue;
            }
            printf("Failure to lock proc-id file:%s, errno=%d\n", m_strAppLockFile.c_str(), errno);
            fclose(fd);
            return NULL;
        }
        break;
    }
    printf("pid=%d, Locking file:%s\n", getpid(), m_strAppLockFile.c_str());
    return fd;
}

void CwxAppProcessMgr::unlock(FILE* fd)
{
    struct flock lock;
    lock.l_type = F_UNLCK;
    lock.l_whence =SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    fcntl(fileno(fd), F_SETLKW,  reinterpret_cast<long>(&lock));
    printf("pid=%d, unLocking file:%s\n", getpid(), m_strAppLockFile.c_str());
    fclose(fd);
    unlink(m_strAppLockFile.c_str());
}

///判断进程是否存在。-1：失败；0：不存在；1：存在
int CwxAppProcessMgr::isExistProc(char const* szProcName, int pid)
{
    int iRet = 0;
    char szCmd[512];
    sprintf(szCmd,"ps -p %d| grep %s >/tmp/%s.%d", pid, szProcName, szProcName, pid);
    system(szCmd);
    sprintf(szCmd, "/tmp/%s.%d", szProcName, pid);
    off_t size = CwxFile::getFileSize(szCmd);
    if (size < 0)
    {
        printf("Failure to get file size, errno=%d, file:%s\n", errno, szCmd);
        iRet = -1;
    }
    else
    {
        iRet = 0==size?0:1;
    }
    sprintf(szCmd, "rm -f /tmp/%s.%d", szProcName, pid);
    system(szCmd);
    return iRet;
}

///获取pid文件中的进程id，-1：失败
int CwxAppProcessMgr::getProcId(char const* szPidFile)
{
    FILE* fd = fopen(szPidFile, "rb");
    string line;
    if (NULL == fd)
    {
        printf("Process file [%s] doesn't exist.\n", szPidFile);
        return -1;
    }
    if (false == CwxFile::readTxtLine(fd, line))
    {
        printf("Can't read process file:%s\n", szPidFile);
        return -1;
    }
    fclose(fd);
    return atoi(line.c_str());
}

CWINUX_END_NAMESPACE
