#include "CwxAddr.h"

CWINUX_BEGIN_NAMESPACE

// Note: this object requires static construction and destruction.
/* static */
const CwxAddr CwxAddr::sap_any (-1, -1);

CwxAddr::CwxAddr (CWX_INT32 iType, CWX_INT32 iSize) 
:m_iAddrType (iType),m_iAddrSize (iSize)
{
}

CwxAddr::~CwxAddr (void)
{
}

void * CwxAddr::getAddr (void) const
{
    return 0;
}

int CwxAddr::setAddr (void *, CWX_INT32 )
{
  return 0;
}


CWINUX_END_NAMESPACE
