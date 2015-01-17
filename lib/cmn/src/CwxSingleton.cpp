#include "CwxSingleton.h"
#include "CwxSingletonMgr.h"

CWINUX_BEGIN_NAMESPACE
CwxSingleton::CwxSingleton(string const& strName)
:m_strName(strName)
{
    m_next = NULL;
    CwxSingletonMgr* pMgr = CwxSingletonMgr::instance();
    pMgr->reg(this);
}

CwxSingleton::~CwxSingleton()
{
}
CWINUX_END_NAMESPACE
