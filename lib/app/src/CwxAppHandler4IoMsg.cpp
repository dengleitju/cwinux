#include "CwxAppHandler4IoMsg.h"
#include "CwxAppFramework.h"

CWINUX_BEGIN_NAMESPACE


CwxAppHandler4IoMsg::CwxAppHandler4IoMsg(CwxAppFramework* pApp, CwxAppReactor *reactor)
:CwxAppHandler4Msg(pApp, reactor)
{
    m_next = NULL;
}

///析构函数
CwxAppHandler4IoMsg::~CwxAppHandler4IoMsg()
{
}

///handle close
int CwxAppHandler4IoMsg::close(CWX_HANDLE )
{
    int iCloseReturn=0;
    CWX_UINT16 ucLocState = m_conn.getState();
    CWX_DEBUG(("Handler close, svr_id[%u], host_id[%u), conn_id[%u], handle[%d]",
        m_conn.getSvrId(),
        m_conn.getHostId(),
        m_conn.getConnId(),
        (int)getHandle()
        ));
    ///从reactor中移除
    if (CWX_INVALID_HANDLE != getHandle())
        if (reactor()) reactor()->removeHandler(this);

    m_conn.setState(CwxAppConnInfo::FAILED);

    //reconnection or close
    if (CwxAppConnInfo::ESTABLISHED == ucLocState)
    {
        //re-dispatch all msg
        while(this->getNextMsg() == 1)
        {
            if (m_curSndingMsg && m_curSndingMsg->send_ctrl().isFailNotice())
            {
                this->getApp()->onFailSendMsg(m_curSndingMsg);
            }
            if (m_curSndingMsg) CwxMsgBlockAlloc::free(m_curSndingMsg);
            this->m_curSndingMsg = NULL;
        }
        iCloseReturn = this->getApp()->connClosed(*this);
        //remove recieved data.
        if (this->m_recvMsgData) CwxMsgBlockAlloc::free(this->m_recvMsgData);
        this->m_recvMsgData = NULL;
    }
    if (getHandle () != CWX_INVALID_HANDLE)
    {
        ::close(getHandle());
        setHandle(CWX_INVALID_HANDLE);
    }
    getApp()->getHandlerCache()->cacheIoMsgHandle(this);
    return 0;
}


CWINUX_END_NAMESPACE

