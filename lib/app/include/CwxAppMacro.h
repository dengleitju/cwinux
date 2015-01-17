#ifndef __CWX_APP_MACRO_H__
#define __CWX_APP_MACRO_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
*@file  CwxAppMacro.h
*@brief app的全局宏定义
*@author cwinux@gmail.com
*@version 0.1.0
*@date  2009-06-28
*@warning  无.
*/
#include "CwxPre.h"
#include <signal.h>


#define CWX_APP_MAX_IO_NUM_BIT 19
#define CWX_APP_MAX_IO_NUM  (1<<19)
#define CWX_APP_MAX_IO_ID (CWX_APP_MAX_IO_NUM - 1)

#define CWX_APP_MAX_SIGNAL_ID _NSIG

///连接模式定义
#define CWX_APP_MSG_MODE    1 ///<基于msg的连接模式
#define CWX_APP_EVENT_MODE  2 ///<基于event的连接模式

///无效链接ID定义
#define CWX_APP_INVALID_CONN_ID 0 ///<无效的连接ID
#define CWX_APP_MIN_CONN_ID     1 ///<最小的连接ID
#define CWX_APP_MAX_CONN_ID     0x7FFFFFFF ///<最大的连接ID

///定义Handle 类型
#define CWX_APP_HANDLE_UNKNOW  0
#define CWX_APP_HANDLE_SIGNAL  1
#define CWX_APP_HANDLE_TIMEOUT 2


#define CWX_APP_MAX_TASK_NUM  4096 ///<Taskboard中最大的Task数量
#define CWX_APP_DEF_LOG_FILE_SIZE  1024 * 1024 * 20 ///<缺省的日志文件大小
#define CWX_APP_DEF_LOG_FILE_NUM   7 ///<缺省的日志文件数量

#define CWX_APP_DEF_BACKLOG_NUM   8192


#define CWX_APP_DEF_KEEPALIVE_IDLE 10//开始首次KeepAlive探测前的TCP空闭时间
#define CWX_APP_DEF_KEEPALIVE_INTERNAL 5//两次KeepAlive探测间的时间间隔
#define CWX_APP_DEF_KEEPALIVE_COUNT   2//判定断开前的KeepAlive探测次数

#include "CwxPost.h"
#endif
