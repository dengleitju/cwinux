#ifndef  __CWX_GET_OPT_H__
#define  __CWX_GET_OPT_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
*@file CwxGetOpt.h
*@brief CwxGetOpt定义
*@author cwinux@gmail.com
*@version 1.0
*@date  2009-06-05
*@warning  nothing
*@bug    
*/

#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxType.h"
#include <getopt.h>
#include <stdlib.h>

CWINUX_BEGIN_NAMESPACE

/**
* @class CwxGetOpt
*
* @brief 对os的getopt_long的封装.
*/
class CWX_API CwxGetOpt
{
public:
    enum {
        /// Doesn't take an argument.
        NO_ARG = 0,
        /// Requires an argument, same as passing ":" after a short option
        /// character in optstring.
        ARG_REQUIRED = 1,
        /// Argument is optional, same as passing "::" after a short
        /// option character in optstring.
        ARG_OPTIONAL = 2
    };
    ///构造函数
    CwxGetOpt (int argc,
        char **argv,
        char const* optstring = "");
    ///析构函数
    ~CwxGetOpt (void);
public:
    ///设置long option对应的short option
    int long_option (char const *name, int short_option, int has_arg = NO_ARG);
    /**
    @brief 获取下一个参数
    @return 与getopt_long()相同
    */
    int next();
    ///获取option的参数
    char *opt_arg (void) const;
    ///返回当前的option
    int opt_opt (void) const;
    ///返回当前参数的index
    int opt_ind (void) const;
    ///返回当前的long option名
    char const* long_option() const;
private:
    int             m_argc;
    char **         m_argv;
    char const*     m_optString;
    struct option*   m_longOptions;
    int             m_longindex;
    int             m_longOptionsNum;
    int             m_longOptionsSize;
    int             m_opt;
};



CWINUX_END_NAMESPACE

#include "CwxPost.h"

#endif
