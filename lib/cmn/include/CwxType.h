#ifndef __CWX_TYPE_H__
#define __CWX_TYPE_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/
/**
*@file  CwxType.h
*@brief define the base data type and ntol serial macro.
*@author cwinux@gmail.com
*@version 0.1
*@date  2009-06-30
*@warning  none.
*/

#include "CwxPre.h"

#include "CwxConfig.h"
#include "CwxGlobalMacro.h"
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <strings.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <sys/time.h>
///基本数据类型定义
typedef int8_t             CWX_INT8;
typedef uint8_t            CWX_UINT8;
typedef int16_t            CWX_INT16;
typedef uint16_t           CWX_UINT16;
typedef int32_t            CWX_INT32;
typedef uint32_t           CWX_UINT32;
typedef int64_t            CWX_INT64;
typedef uint64_t           CWX_UINT64;

///网络字节序、本机字节序变换定义
#define CWX_NTOHS(x)  ntohs(x)
#define CWX_NTOHL(x)  ntohl(x)
#define CWX_HTONS(x)  htons(x)
#define CWX_HTONL(x)  htonl(x)

///IO句柄类型定义
typedef int CWX_HANDLE;
///无效IO句柄类型定义
#define CWX_INVALID_HANDLE  -1


#include "CwxPost.h"

#endif
