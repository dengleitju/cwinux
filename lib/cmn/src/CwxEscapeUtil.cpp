#include "CwxEscapeUtil.h"

CWINUX_BEGIN_NAMESPACE
CwxEscapeUtil* CwxEscapeUtil::m_pInstance=NULL;
CwxMutexLock CwxEscapeUtil::m_lock;

CwxEscapeUtil::CwxEscapeUtil():CwxSingleton("CwxEscapeUtil")
{
};
CwxEscapeUtil::~CwxEscapeUtil()
{

}

void CwxEscapeUtil::init()
{
    CWX_UINT16 unFlag = 0;
    for (CWX_UINT16 c=0; c<256; c++)
    {
        unFlag = 0;
        if (!CwxChrType::isAlnum(c) && !strchr("$-_.+!*'(),:@&=/~", c)) 
        {
            unFlag |= URL_ESCAPE;
        }
        m_chrMap[c] = unFlag;
    }
}


CWINUX_END_NAMESPACE
