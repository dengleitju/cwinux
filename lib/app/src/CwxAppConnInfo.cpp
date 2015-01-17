#include "CwxAppConnInfo.h"
CWINUX_BEGIN_NAMESPACE
CwxAppConnInfo::CwxAppConnInfo()
{
    m_uiSvrId = 0;
    m_uiHostId = 0;
    m_uiConnId = 0;
    m_uiListenId = 0;
    m_uiConnectTimeout = 0;
    m_unState = IDLE;
    m_ttCreateTime = time(NULL);
    m_uiFailConnNum = 0;
    m_unMinRetryInternal = 1;
    m_unMaxRetryInternal = 60;
    m_bActiveConn = false;
    m_bActiveClose = false;
    m_bRawData = false;
    m_ttLastRecvMsgTime = 0;
    m_ttLastSendMsgTime = 0;
    m_pUserData = NULL;
    m_uiContinueRecvNum = 0;
    m_uiContinueSendNum = 0;
    m_uiMaxWaitingMsgNum = 0;
    m_uiWaitingMsgNum = 0;
    m_bInvokeCreate = true;
    m_bReconn = false;
    m_uiReconnDelay = 0;
    m_iFamily = AF_INET;
    m_pHandler = NULL;
}

CwxAppConnInfo::~CwxAppConnInfo()
{
}




CWINUX_END_NAMESPACE

