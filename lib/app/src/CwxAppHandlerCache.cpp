#include "CwxAppHandlerCache.h"

CWINUX_BEGIN_NAMESPACE
CwxAppHandlerCache::CwxAppHandlerCache()
{
    m_uiFreeTcpHandleNum = 0;
    m_uiFreeUnixHandleNum = 0;
    m_uiFreeIoMsgHandleNum = 0;
    m_uiFreeIoEventHandleNum = 0;
    m_freeTcpHandles = NULL;
    m_freeUnixHandles = NULL;
    m_freeIoMsgHandles = NULL;
    m_freeIoEventHandles = NULL;
}

CwxAppHandlerCache::~CwxAppHandlerCache()
{
    destroy();
}

void CwxAppHandlerCache::destroy()
{
    CwxAppHandler4TcpConn* pTcp;
    CwxAppHandler4UnixConn* pUnix;
    CwxAppHandler4IoMsg* pMsg;
    CwxAppHandler4IoEvent* pIo;
    while(m_freeTcpHandles)
    {
        pTcp = m_freeTcpHandles->m_next;
        delete m_freeTcpHandles;
        m_freeTcpHandles = pTcp;
    }
    m_freeTcpHandles = NULL;
    m_uiFreeTcpHandleNum = 0;
    while(m_freeUnixHandles)
    {
        pUnix = m_freeUnixHandles->m_next;
        delete m_freeUnixHandles;
        m_freeUnixHandles = pUnix;
    }
    m_freeUnixHandles = NULL;
    m_uiFreeUnixHandleNum = 0;

    while(m_freeIoMsgHandles)
    {
        pMsg = m_freeIoMsgHandles->m_next;
        delete m_freeIoMsgHandles;
        m_freeIoMsgHandles = pMsg;
    }
    m_freeIoMsgHandles = NULL;
    m_uiFreeIoMsgHandleNum = 0;

    while(m_freeIoEventHandles)
    {
        pIo = m_freeIoEventHandles->m_next;
        delete m_freeIoEventHandles;
        m_freeIoEventHandles = pIo;
    }
    m_freeIoEventHandles = NULL;
    m_uiFreeIoEventHandleNum = 0;
}


CWINUX_END_NAMESPACE


