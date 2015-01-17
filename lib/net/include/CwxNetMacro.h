#ifndef __CWX_NET_MACRO_H__
#define __CWX_NET_MACRO_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/
/**
@file CwxNetMacro.h
@brief Net模块的公共宏定义文件。
@author cwinux@gmail.com
@version 0.1
@date 2009-10-10
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxType.h"

#include <netinet/in.h>
#include <arpa/inet.h>

# if !defined (INADDR_ANY)
#   define INADDR_ANY (CWX_UINT32)0x00000000
# endif /* INADDR_ANY */

#if !defined (INADDR_LOOPBACK)
#  define INADDR_LOOPBACK ((CWX_UINT32) 0x7f000001)
#endif /* INADDR_LOOPBACK */

#if !defined (AF_ANY)
#  define AF_ANY (-1)
#endif /* AF_ANY */

#define CWX_MAX_DEFAULT_PORT 65535

#define CWX_MAX_HOST_NAME_LEN  256


///消息socket handle的设置函数，0：成功；-1：失败
typedef int (*CWX_NET_SOCKET_ATTR_FUNC)(CWX_HANDLE handle, void* arg);

#include "CwxPost.h"

#endif
