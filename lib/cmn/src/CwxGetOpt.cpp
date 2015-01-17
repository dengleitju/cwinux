#include "CwxGetOpt.h"
CWINUX_BEGIN_NAMESPACE
///构造函数
CwxGetOpt::CwxGetOpt (int argc,
           char **argv,
           char const* optstring)
{
    struct option  empty={0,0,0,0};
    m_argc = argc;
    m_argv = argv;
    m_optString = optstring;
    m_longOptionsNum = 0;
    m_longOptionsSize = 16;
    m_longOptions = new option[m_longOptionsSize];
    for (int i=0; i<m_longOptionsSize; i++)
    {
        memcpy(&m_longOptions[i], &empty, sizeof empty);
    }
    m_opt = 0;
    m_longindex = 0;
    optarg = 0;
    optopt = 0;
    opterr   =  0;
    optind   =  1;

}

CwxGetOpt::~CwxGetOpt (void)
{
    if (m_longOptions)
    {
        for (int i=0; i<m_longOptionsNum; i++)
        {
           if(m_longOptions[i].name) delete m_longOptions[i].name;
        }
        delete [] m_longOptions;
        m_longOptions = NULL;
    }
}

int CwxGetOpt::long_option (char const *name, int short_option, int has_arg)
{
    int i;
    if (m_longOptionsNum + 2 >= m_longOptionsSize)
    {
        m_longOptionsSize *= 2;
        struct option* tmp = new option[m_longOptionsSize];
        for (i=0; i<m_longOptionsNum; i++)
        {
            memcpy(&tmp[i], &m_longOptions[i], sizeof m_longOptions[i]);
        }
        struct option  empty={0,0,0,0};
        for (; i<m_longOptionsSize; i++)
        {
            memcpy(&m_longOptions[i], &empty, sizeof empty);
        }
    }
    char* szName = (char*)malloc(strlen(name) + 1);
    m_longOptions[m_longOptionsNum].name = szName;
    strcpy(szName, name);
    m_longOptions[m_longOptionsNum].has_arg = has_arg;
    m_longOptions[m_longOptionsNum].flag = NULL;
    m_longOptions[m_longOptionsNum].val = short_option;
    m_longOptionsNum++;
    return 0;
}

int CwxGetOpt::next()
{
    m_longindex = 0;
    m_opt = getopt_long (m_argc, m_argv, m_optString, m_longOptions, &m_longindex);
    return m_opt;
}

///获取option的参数
char* CwxGetOpt::opt_arg (void) const
{
    return optarg;
}
///返回当前的option
int CwxGetOpt::opt_opt (void) const
{   
    return m_opt;
}
///返回当前参数的index
int CwxGetOpt::opt_ind (void) const
{
    return optind;
}
///返回当前的long option名
char const* CwxGetOpt::long_option() const
{
    if (m_longindex < m_longOptionsNum) return m_longOptions[m_longindex].name;
    return NULL;
}



CWINUX_END_NAMESPACE


