#ifndef  __CWX_STL_H__
#define  __CWX_STL_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
*@file  CwxStl.h
*@brief sgi header
*@author cwinux@gmail.com
*@version 0.1
*@date  2009-06-28
*@warning  无.
*/
#include "CwxPre.h"

#include <utility>
#include <algorithm>
#include <string>
#include <vector>
#include <queue>
#include <list>
#include <set>
#include <map>
#include "CwxGlobalMacro.h"
#ifdef __GNUC__
#define GCC_VERSION (__GNUC__ * 10000  + __GNUC_MINOR__ * 100)
#if GCC_VERSION >= 40300
#include <unordered_set>
#include <unordered_map>

CWINUX_BEGIN_NAMESPACE

inline size_t __stl_hash_string(const char* __s)
{
    unsigned long __h = 0;
    for ( ; *__s; ++__s)
        __h = 5*__h + *__s;
    return size_t(__h);
}
CWINUX_END_NAMESPACE
#define hash_map  unordered_map
#define hash_set  unordered_set
#else
#include <ext/hash_set>
#include <ext/hash_map>
#endif
#undef  GCC_VERSION
using namespace __gnu_cxx;
#define HAVE_STL_HASH  1
#else
#undef HAVE_STL_HASH
#endif

using namespace std;

#include "CwxPost.h"
#endif

