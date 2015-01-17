#ifndef __CWX_LOGGER_H__
#define __CWX_LOGGER_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxLogger.h
@brief 架构的运行日志对象
@author cwinux@gmail.com
@version 0.1
@date 2009-07-20
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxType.h"
#include "CwxStl.h"
#include <stdio.h>
#include "CwxTss.h"
#include "CwxLockGuard.h"
#include "CwxMutexLock.h"
#include "CwxFile.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "CwxCommon.h"
CWINUX_BEGIN_NAMESPACE
/**
@class CwxLogger
@brief 架构日志对象的定义，其具有如下的特点：
1、其日志分为info、debug、warning、error四个级别。可以设置输出全部或某几个级别的日志。
2、可以设计运行日志的大小，当超过此大小后，会切换下一个日志文件
3、可以设置日志文件的数量，实现日志文件的循环使用
*/
class CWX_API CwxLogger
{
public:
    ///日志类型定义
    enum{
        LEVEL_NONE = 0x00, ///<不输入任何日志
        LEVEL_INFO = 0x01,///<info日志
        LEVEL_DEBUG = 0x02,///<debug日志
        LEVEL_WARNING = 0x04,///<warning日志
        LEVEL_ERROR = 0x08,///<error 日志
        LEVEL_ALL=0xFFFFFFFF ///<输出全部日志
    };
    ///日志文件数量定义
    enum{
       MAX_LOG_FILE_NUM = 99,///<最多的日志文件数量
       MIN_LOG_FILE_NUM = 4///<最少的日志文件数量
    };
    ///析构函数
    ~CwxLogger()
    {
        closeLog();
    }
public:
    ///单实例获取
    static CwxLogger* instance();
    ///关闭日志
    static void close();
    /**
    @brief 日志对象初始化
    @param [in] base_name 日志的主文件名字，包括路径名
    @param [in] num 日志文件的数量
    @param [in] max_size 每个日志文件的大小，单位为字节
    @param [in] bAppend 是否清空当前使用的日志文件
    @return -1：失败； 0：成功
    */
    int  init(const char* base_name,
        CWX_UINT16 num,
        CWX_UINT32 max_size,
        bool bAppend);
    /**
    @brief 切换下一个日志文件
    @param [in] bAppend true：不清空下一个日志文件，采用append方式记录日志；否则记录日志前清空
    @param [in] unFileNo 下一个日志的文件号，从0开始。若为0，则为当前日志系统的下一个日志文件
    @return -1：失败； 0：成功
    */
    int  nextLog(bool bAppend, CWX_UINT16 unFileNo=0);
    /**
    @brief 记录info类型的日志，此类型的日志不记录文件名及行号
    @param [in] format 记录的日志内容
    @return void
    */
    void info(char const* format, ...);
    /**
    @brief 记录debug类型的日志，此类型的日志记录文件名及行号
    @param [in] format 记录的日志内容
    @return void
    */
    void debug(char const* format, ...);
    /**
    @brief 记录warning类型的日志，此类型的日志记录文件名及行号
    @param [in] format 记录的日志内容
    @return void
    */
    void warning(char const* format, ...);
    /**
    @brief 记录error类型的日志，此类型的日志记录文件名及行号
    @param [in] format 记录的日志内容
    @return void
    */
    void error(char const* format, ...);
    ///将szMsg的数据log到文件中
    void log(char const* szMsg);
    /**
    @brief 打开uiLevel类型的日志输出，可以为多个类型的或
    @param [in] uiLevel 打开的日志类型
    @return void
    */
    inline void enableLog(CWX_UINT32 uiLevel)
    {
        m_uiLevel |= uiLevel;
    }
    /**
    @brief 关闭uiLevel类型的日志输出，可以为多个类型的或
    @param [in] uiLevel 关闭的日志类型
    @return void
    */
    inline void disableLog(CWX_UINT32 uiLevel)
    {
        m_uiLevel &=~uiLevel;
    }
    /**
    @brief 检查uiLevel类型的日志输出是否打开，可以为多个类型的或
    @param [in] uiLevel 检查的日志类型
    @return true：打开；false：没有打开
    */
    inline bool isEnableLog(CWX_UINT32 uiLevel) const
    {
        return (m_uiLevel&(uiLevel))!=0;
    }
    ///获取当期的日志输出级别
    inline CWX_UINT32 getLevel() const 
    {
        return m_uiLevel;
    }
    ///设置当前的日志输出级别
    inline void setLevel(CWX_UINT32 uiLevel)
    {
        m_uiLevel = uiLevel;
    }
    ///获取当前使用的日志文件的大小， -1:表示获取失败
    inline off_t getLogFileSize() const
    {
        return m_curLogFd?CwxFile::getFileSize(m_strCurLogFileName.c_str()):0;
    }
    ///获取设置的日志文件的数量
    inline CWX_UINT16 getLogFileNum() const
    {
        return this->m_unLogFileNum;
    }
    ///获取当前日志文件的序号
    inline CWX_UINT16 getCurLogFileNum() const 
    {
        return this->m_unCurLogFileNum;
    }
    ///获取设置的日志文件的大小
    inline CWX_UINT32 getMaxLogFileSize() const
    {
        return this->m_uiMaxFileSize;
    }
    ///获取当前日志文件的FILE handle
    inline FILE* getCurLogFileHandle() const
    {
        return this->m_curLogFd;
    }
private:
    ///构造函数，只有instance()才能创建日志对象，保证了单实例。
    CwxLogger()
    {
        this->m_uiLevel = 0xFFFFFFFF;
        this->m_unLogFileNum = 0;
        this->m_unCurLogFileNum = 0;
        this->m_uiMaxFileSize = 0;
        this->m_curLogFd = NULL;
        this->m_prevLogFd = NULL;
        this->m_bInit = false;
    }
    ///关闭日志对象，返回值：0:success, -1:failure
    int closeLog();
    ///设置下一个日志文件的环境信息
    void _nextLogFile();
    ///获取序号为seq的日志文件的名字
    void _logFileName(int seq, std::string& strLog);
    ///启动时，根据日志文件的日志，获取开始使用的日志文件序号。-1:从0开始, otherwize, log's id
    int  _getStartLogNo();
    ///切换到下一个日志文件。返回值：0:success, -1:failure
    int _nextLog(bool append=true, CWX_UINT16 unFileNo=0);
    ///获取msg header
    int _log_header(char const* szFile, int line, char* szBuf, CWX_UINT32 uiBufLen);

private:
    CWX_UINT32         m_uiLevel;///<日志输出的LEVEL
    CWX_UINT32         m_uiMaxFileSize;///<设置的日志文件尺寸
    CWX_UINT16         m_unLogFileNum;///<设置的日志文件数量
    CWX_UINT16         m_unCurLogFileNum;///<当前使用的日志文件号
    CwxMutexLock     m_mutex;///<保护日志文件切换的锁
    string             m_strBaseName;///<日志文件的主文件名
    string             m_strCurLogFileName;///<当期日志文件的文件名
    FILE *             m_curLogFd;///<当前日志文件的fd
    FILE *             m_prevLogFd;///<起一个日志文件的fd
    bool               m_bInit;///<日志对象是否初始化成功
    static CwxLogger* m_pInstance;///<日志对象的单实例对象。
};

CWINUX_END_NAMESPACE
///输出一个错误消息
#define CWX_ERROR(msg) do {\
    CWX_TSS_FILE_NO = __LINE__;\
    CWX_TSS_FILE_NAME = __FILE__;\
    CwxLogger::instance()->error msg;} while(0)
///输出一个警告信息
#define CWX_WARNING(msg) do {\
    CWX_TSS_FILE_NO = __LINE__;\
    CWX_TSS_FILE_NAME = __FILE__;\
    CwxLogger::instance()->warning msg;} while(0)
///输出一个调试信息
#define CWX_DEBUG(msg) do {\
    CWX_TSS_FILE_NO = __LINE__;\
    CWX_TSS_FILE_NAME = __FILE__;\
    CwxLogger::instance()->debug msg;} while(0)
///输出一个info信息
#define CWX_INFO(msg) do {\
    CWX_TSS_FILE_NO = 0;\
    CWX_TSS_FILE_NAME = NULL;\
    CwxLogger::instance()->info msg;} while(0)
///输出一个info信息
#define CWX_LOG(msg) do {\
    CwxLogger::instance()->log(msg);} while(0)
#include "CwxPost.h"
#endif
