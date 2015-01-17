#include "CwxAppListenMgr.h"
#include "CwxAppFramework.h"

CWINUX_BEGIN_NAMESPACE
CwxAppListenMgr::CwxAppListenMgr(CwxAppFramework* pApp)
:m_pApp(pApp)
{
    m_pListenMap = new CwxAppIdListenMap();
    m_uiListenId = 1;
}

CwxAppListenMgr::~CwxAppListenMgr()
{
    if (m_pListenMap)
    {
        CwxAppIdListenMap::iterator iter = m_pListenMap->begin();
        while(iter != m_pListenMap->end())
        {
            if (CwxAppListenObj::TCP_LISTEN == iter->second.m_unListenType)
            {
                ((CwxAppTcpAcceptor*)iter->second.m_listenObject)->close();
            }else{
                ((CwxAppUnixAcceptor*)iter->second.m_listenObject)->close();
            }
            iter++;
        }
        delete m_pListenMap;
        m_pListenMap = NULL;
    }
}

bool CwxAppListenMgr::add(CwxAppTcpAcceptor* acceptor)
{
    CwxMutexGuard<CwxMutexLock> lock(&m_lock);
    if (m_pListenMap->find(acceptor->getListenId()) != m_pListenMap->end()) return false;
    CwxAppListenObj obj;
    obj.m_listenObject = acceptor;
    obj.m_unListenType = CwxAppListenObj::TCP_LISTEN;
    (*m_pListenMap)[acceptor->getListenId()] = obj;
    return true;
}

bool CwxAppListenMgr::add(CwxAppUnixAcceptor* acceptor)
{
    CwxMutexGuard<CwxMutexLock> lock(&m_lock);
    CwxAppListenObj obj;
    if (m_pListenMap->find(acceptor->getListenId()) != m_pListenMap->end()) return false;
    obj.m_listenObject = acceptor;
    obj.m_unListenType = CwxAppListenObj::UNIX_LISTEN;
    (*m_pListenMap)[acceptor->getListenId()] = obj;
    return true;
}

bool CwxAppListenMgr::close(CWX_UINT32 uiListenId)
{
    CwxAppIdListenMap::iterator iter;
    {
        CwxMutexGuard<CwxMutexLock> lock(&m_lock);
        CwxAppIdListenMap::iterator iter= m_pListenMap->find(uiListenId);
        if (iter == m_pListenMap->end())
        {
            CWX_ERROR(("Execute closing listen by listen_id[%u], it doesn't exist", uiListenId));
            return false;
        }
        m_pListenMap->erase(iter);
    }
    if (CwxAppListenObj::TCP_LISTEN == iter->second.m_unListenType)
    {//close tcp listen
        CwxAppTcpAcceptor* acceptor = (CwxAppTcpAcceptor*)iter->second.m_listenObject;
        m_pApp->reactor()->removeHandler(acceptor);
        acceptor->close();
    }
    else
    {//close unix listen
        CwxAppUnixAcceptor* acceptor = (CwxAppUnixAcceptor*)iter->second.m_listenObject;
        m_pApp->reactor()->removeHandler(acceptor);
        acceptor->close();
    }
    return true;
}

bool CwxAppListenMgr::resume(CWX_UINT32 uiListenId)
{
    {
        CwxMutexGuard<CwxMutexLock> lock(&m_lock);
        CwxAppIdListenMap::iterator iter= m_pListenMap->find(uiListenId);
        if (iter == m_pListenMap->end())
        {
            CWX_ERROR(("Execute resume listen by listen_id[%u], it doesn't exist", uiListenId));
            return false;
        }
        if (CwxAppListenObj::TCP_LISTEN == iter->second.m_unListenType)
        {//close tcp listen
            CwxAppTcpAcceptor* acceptor = (CwxAppTcpAcceptor*)iter->second.m_listenObject;
            acceptor->reactor()->resumeHandler(acceptor, CwxAppHandler4Base::READ_MASK);
        }
        else
        {//close unix listen
            CwxAppUnixAcceptor* acceptor = (CwxAppUnixAcceptor*)iter->second.m_listenObject;
            acceptor->reactor()->resumeHandler(acceptor, CwxAppHandler4Base::READ_MASK);
        }
    }
    return true;

}

bool CwxAppListenMgr::suspend(CWX_UINT32 uiListenId)
{
    {
        CwxMutexGuard<CwxMutexLock> lock(&m_lock);
        CwxAppIdListenMap::iterator iter= m_pListenMap->find(uiListenId);
        if (iter == m_pListenMap->end())
        {
            CWX_ERROR(("Execute resume listen by listen_id[%u], it doesn't exist", uiListenId));
            return false;
        }
        if (CwxAppListenObj::TCP_LISTEN == iter->second.m_unListenType)
        {//close tcp listen
            CwxAppTcpAcceptor* acceptor = (CwxAppTcpAcceptor*)iter->second.m_listenObject;
            acceptor->reactor()->suspendHandler(acceptor, CwxAppHandler4Base::READ_MASK);
        }
        else
        {//close unix listen
            CwxAppUnixAcceptor* acceptor = (CwxAppUnixAcceptor*)iter->second.m_listenObject;
            acceptor->reactor()->suspendHandler(acceptor, CwxAppHandler4Base::READ_MASK);
        }
    }
    return true;
}

CWX_UINT32 CwxAppListenMgr::getNextListenId()
{
    CwxMutexGuard<CwxMutexLock> lock(&m_lock);
    while(1)
    {
        m_uiListenId ++;
        if (m_uiListenId<MIN_LISTEN_ID) m_uiListenId = MIN_LISTEN_ID;
        if (m_uiListenId>MAX_LISTEN_ID)  m_uiListenId = MIN_LISTEN_ID;
        if (m_pListenMap->find(m_uiListenId) == m_pListenMap->end()) break;
    }
    return m_uiListenId;
}

void CwxAppListenMgr::getAllListens(vector<CWX_UINT32>& listens)
{
    listens.clear();
    CwxAppIdListenMap::iterator iter = m_pListenMap->begin();
    while(iter != m_pListenMap->end())
    {
        listens.push_back(iter->first);
        iter++;
    }
}


CWINUX_END_NAMESPACE
