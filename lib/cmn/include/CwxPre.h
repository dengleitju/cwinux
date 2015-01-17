#ifndef __CWX_PRE_H__
#define __CWX_PRE_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/
/**
*@file  CwxPre.h
*@brief 位于所有头文件的开始，设置统一的编译规则
*@author cwinux@gmail.com
*@version 0.1
*@date  2009-06-28
*@warning  无.
*/

#define __STDC_FORMAT_MACROS

#undef CWX_LIKELY
#undef CWX_UNLIKELY

#if defined(__GNUC__) && __GNUC__ >= 4
#define CWX_LIKELY(x)   (__builtin_expect((x), 1))
#define CWX_UNLIKELY(x) (__builtin_expect((x), 0))
#else
#define CWX_LIKELY(x)   (x)
#define CWX_UNLIKELY(x) (x)
#endif

#endif
