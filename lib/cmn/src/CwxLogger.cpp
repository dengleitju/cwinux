#include "CwxLogger.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

CWINUX_BEGIN_NAMESPACE

CwxLogger*  CwxLogger::m_pInstance = NULL;

CwxLogger* CwxLogger::instance()
{
    if (!m_pInstance)
    {
        if (!m_pInstance) m_pInstance = new CwxLogger();
    }
    return m_pInstance;
}

void CwxLogger::close()
{
    if (m_pInstance)
    {
        delete m_pInstance;
        m_pInstance = NULL;
    }
}

int CwxLogger::init(char const* szBaseName, CWX_UINT16 unFileNum, CWX_UINT32 uiMaxFileSize, bool bAppend)
{
    int ret = 0;
    this->m_strBaseName = szBaseName;
    this->m_unLogFileNum = unFileNum;
    if (m_unLogFileNum  > (CWX_UINT16)MAX_LOG_FILE_NUM) m_unLogFileNum = (CWX_UINT16) MAX_LOG_FILE_NUM ;
    if (m_unLogFileNum < (CWX_UINT16)MIN_LOG_FILE_NUM) m_unLogFileNum =  (CWX_UINT16) MIN_LOG_FILE_NUM ;
    this->m_uiMaxFileSize = uiMaxFileSize;
    int iSeq = _getStartLogNo();
    if (-1 == iSeq)
    {
        this->m_unCurLogFileNum = 0;
        ret =  this->_nextLog(bAppend);
    }
    else
    {
        this->m_unCurLogFileNum = iSeq;
        ret = this->_nextLog(true);
    }
    if (0 == ret) this->m_bInit = true;
    return ret;
}

int CwxLogger::nextLog(bool bAppend, CWX_UINT16 unFileNo)
{
    CwxMutexGuard<CwxMutexLock> lock(&this->m_mutex);
    return _nextLog(bAppend, unFileNo);
}

///获取msg header
int CwxLogger::_log_header(char const* szFile, int line, char* szBuf, CWX_UINT32 uiBufLen)
{
    struct tm tmNow;
    time_t tt=time(NULL);
    localtime_r(&tt, &tmNow);
    if (szFile)
    {
        return CwxCommon::snprintf(szBuf, uiBufLen, "[%.4d-%.2d-%.2d %.2d:%.2d:%.2d][pid=%d][tid=%u][%s:%d]",
            tmNow.tm_year + 1900,
            tmNow.tm_mon + 1,
            tmNow.tm_mday,
            tmNow.tm_hour,
            tmNow.tm_min,
            tmNow.tm_sec,
            getpid(),
            (CWX_UINT32)pthread_self(),
            szFile,
            line);
    }
    return CwxCommon::snprintf(szBuf, uiBufLen, "[%.4d-%.2d-%.2d %.2d:%.2d:%.2d][pid=%d][tid=%u]",
        tmNow.tm_year + 1900,
        tmNow.tm_mon + 1,
        tmNow.tm_mday,
        tmNow.tm_hour,
        tmNow.tm_min,
        tmNow.tm_sec,
        getpid(),
        (CWX_UINT32)pthread_self());
}

void CwxLogger::log(char const* szMsg)
{
    FILE* fd = this->m_curLogFd;
    if (NULL ==fd) fd= stderr;
    fprintf(fd, "%s\n", szMsg);
    fflush(fd);
}

void CwxLogger::info(char const* format, ...)
{
    if (isEnableLog(LEVEL_INFO))
    {
        CWX_UINT32 const LEN=CwxTss::TSS_ERR_BUF;
        char* szBuf = CwxTss::instance()->m_szErrMsg;
        int ret = _log_header(NULL, 0, szBuf, LEN);
        va_list args;
        va_start(args, format);
        vsnprintf(szBuf + ret, LEN - ret, format, args);
        va_end(args);
        log(szBuf);
    }
}
void CwxLogger::debug(char const* format, ...)
{
    if (isEnableLog(LEVEL_DEBUG))
    {
        CWX_UINT32 const LEN=CwxTss::TSS_ERR_BUF;
        char* szBuf = CwxTss::instance()->m_szErrMsg;
        char const* szFile = CWX_TSS_FILE_NAME;
        int line = CWX_TSS_FILE_NO;
        int ret = _log_header(szFile, line, szBuf, LEN);
        va_list args;
        va_start(args, format);
        vsnprintf(szBuf + ret, LEN - ret, format, args);
        va_end(args);
        log(szBuf);
    }
}

void CwxLogger::warning(char const* format, ...)
{
    if (isEnableLog(LEVEL_WARNING)){
        CWX_UINT32 const LEN=CwxTss::TSS_ERR_BUF;
        char* szBuf = CwxTss::instance()->m_szErrMsg;
        char const* szFile = CWX_TSS_FILE_NAME;
        int line = CWX_TSS_FILE_NO;
        int ret = _log_header(szFile, line, szBuf, LEN);
        va_list args;
        va_start(args, format);
        vsnprintf(szBuf + ret, LEN - ret, format, args);
        va_end(args);
        log(szBuf);
    }
}

void CwxLogger::error(char const* format, ...)
{
    if (isEnableLog(LEVEL_ERROR)){
        CWX_UINT32 const LEN=CwxTss::TSS_ERR_BUF;
        char* szBuf = CwxTss::instance()->m_szErrMsg;
        char const* szFile = CWX_TSS_FILE_NAME;
        int line = CWX_TSS_FILE_NO;
        int ret = _log_header(szFile, line, szBuf, LEN);
        va_list args;
        va_start(args, format);
        vsnprintf(szBuf + ret, LEN - ret, format, args);
        va_end(args);
        log(szBuf);
    }
}

int CwxLogger::_nextLog(bool append, CWX_UINT16 unFileNo)
{
    if (0 != unFileNo)
    {
        if (unFileNo > m_unLogFileNum) unFileNo = m_unLogFileNum;
        if (unFileNo != m_unCurLogFileNum)
        {
            if (1 == unFileNo)
            {
                m_unCurLogFileNum = m_unLogFileNum;
            }
            else
            {
                m_unCurLogFileNum = unFileNo - 1;
            }
        }
    }
    this->_nextLogFile();
    if (!this->m_curLogFd)
    {
        this->m_prevLogFd = NULL;
        this->m_curLogFd = fopen(this->m_strCurLogFileName.c_str(), append?"a+":"w+");
    }
    else
    {
        if (this->m_prevLogFd) fclose(this->m_prevLogFd);
        this->m_prevLogFd = this->m_curLogFd;
        this->m_curLogFd = fopen(this->m_strCurLogFileName.c_str(), append?"a+":"w+");
    }
    return 0;
}

int CwxLogger::closeLog()
{
    if (this->m_curLogFd) 
    {
        fclose(this->m_curLogFd);
        this->m_curLogFd = NULL;
    }

    if (this->m_prevLogFd)
    {
        fclose(this->m_prevLogFd);
        this->m_prevLogFd = NULL;
    }
    this->m_bInit = false;
    return 0;
}

void CwxLogger::_nextLogFile()
{
    if (this->m_unCurLogFileNum < this->m_unLogFileNum)
        this->m_unCurLogFileNum ++;
    else
        this->m_unCurLogFileNum = 1;
    _logFileName(this->m_unCurLogFileNum, this->m_strCurLogFileName);
}

void CwxLogger::_logFileName(int seq, std::string& strLog)
{
    char szSeq[8];
    sprintf(szSeq, "_%2.2d.log", seq);
    strLog = this->m_strBaseName + szSeq;
}

int CwxLogger::_getStartLogNo()
{
    int iSeq = -1;
    time_t ttLastTime = 0;
    time_t ttTmp;
    string strLogFile;
    struct stat fileStat;
    for (int i=1; i<=m_unLogFileNum; i++)
    {
        _logFileName(i, strLogFile);
        if (stat(strLogFile.c_str(), &fileStat)==0)
        {
            ttTmp = fileStat.st_mtime;
            if (ttTmp >= ttLastTime)
            {
                ttLastTime = ttTmp;
                iSeq = i - 1;
            }
            else
            {
                break;
            }
        }
        else
        {
            break;
        }
    }
    if (-1 != iSeq)
    {
        _logFileName(iSeq, strLogFile);
    }
    return iSeq;
}

CWINUX_END_NAMESPACE
