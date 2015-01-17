#include "CwxAppHandler4Base.h"
#include "CwxAppReactor.h"
#include "CwxAppFramework.h"

CWINUX_BEGIN_NAMESPACE

/// Force CwxAppHandler4Base to be an abstract base class.
CwxAppHandler4Base::CwxAppHandler4Base (CwxAppReactor * reactor)
{
    m_reactor = reactor;
    m_handler = CWX_INVALID_HANDLE;
    m_type = CWX_APP_HANDLE_UNKNOW;
    m_regType = CwxAppReactor::REG_TYPE_UNKNOWN;
    m_ullTimeout = 0;
    m_index = -1;
}

CwxAppHandler4Base::~CwxAppHandler4Base (void)
{

}


CWINUX_END_NAMESPACE
