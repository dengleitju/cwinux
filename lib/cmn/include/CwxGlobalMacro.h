#ifndef __CWX_GLOBAL_MACRO_H__
#define __CWX_GLOBAL_MACRO_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
*@file  CwxGlobalMacro.h
*@brief 系统的全局宏定义
*@author cwinux@gmail.com
*@version 0.1.0
*@date  2009-06-28
*@warning  无.
*/
#include "CwxPre.h"
#include <assert.h>

#define _CWX_COMPILE_DATE(x)                      #x
#define CWX_COMPILE_DATE(x)                      _CWX_COMPILE_DATE(x)


#if defined(CWX_WIN32) && defined(CWX_DLL)
#if defined(CWX_EXPORTS)
#define CWX_API __declspec(dllexport)
#else
#define CWX_API __declspec(dllimport)
#endif
#endif

#if !defined(CWX_API)
#define CWX_API
#endif


///define the namespace for cwinux
#define CWINUX_BEGIN_NAMESPACE namespace cwinux{
#define CWINUX_END_NAMESPACE }
#define CWINUX_USING_NAMESPACE using namespace cwinux;

///定义断言
#define CWX_ASSERT(a)  assert(a)

///属性的检查
# define CWX_CHECK_ATTR(WORD, ATTR) (((WORD) & (ATTR)) != 0)
///属性的设置
# define CWX_SET_ATTR(WORD, ATTR) (WORD |= (ATTR))
///属性的清空
# define CWX_CLR_ATTR(WORD, ATTR) (WORD &= ~(ATTR))

///位检查
# define CWX_CHECK_BIT(WORD, BIT) (((WORD) & (1<<(BIT))) != 0)
///位设置
# define CWX_SET_BIT(WORD, BIT) ((WORD) |= (1<<(BIT)))
///位清空
# define CWX_CLR_BIT(WORD, BIT) ((WORD) &= ~((1<<(BIT))))
///最小
# define CWX_MIN(a, b) ((b) > (a) ? (a) : (b))
///最大
# define CWX_MAX(a, b) ((b) > (a) ? (b) : (a))

#define CWX_UNUSED_ARG(a) (void)(a)

#define CWX_MSG_SIZE_MAX  1024 * 1024 * 64

#include "CwxPost.h"
#endif
