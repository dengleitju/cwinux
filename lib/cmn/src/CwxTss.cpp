#include "CwxTss.h"
#include "CwxDate.h"

CWINUX_BEGIN_NAMESPACE


pthread_key_t CwxTss::m_tssKey;
bool CwxTss::m_bInit = false;

int CwxTss::initTss()
{
    if (!m_bInit){
        m_bInit = true;
        return pthread_key_create(&m_tssKey, NULL);
    }
    return 0;

}

int CwxTss::regTss(CwxTss* pThrEnv)
{
    if (!m_bInit)
    {
        if (0 != initTss()) return -1;
    }
    void* pObj = pthread_getspecific(m_tssKey);
    if (pObj) delete ((CwxTss*)pObj);
    return pthread_setspecific(m_tssKey, pThrEnv);
}

int CwxTss::unRegTss()
{
    if (m_bInit)  pthread_key_delete(m_tssKey);
    return 0;
}


CwxTss* CwxTss::instance()
{
    if (!m_bInit)
    {
        if (0 != initTss()) return NULL;
    }
    void * pObj = NULL;
    pObj = pthread_getspecific(m_tssKey);
    if (!pObj)
    {
        pObj = new CwxTss();
        regTss((CwxTss*)pObj);
    }
    return (CwxTss*)pObj;
}


CWINUX_END_NAMESPACE

