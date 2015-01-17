#include "CwxMsgHead.h"
#include "CwxMsgBlock.h"

CWINUX_BEGIN_NAMESPACE
CWX_UINT32 CwxMsgHead::m_uiMaxMsgSize = CWX_MSG_SIZE_MAX;
///设置package的最大大小
void CwxMsgHead::setMaxMsgSize(CWX_UINT32 uiSize){
    m_uiMaxMsgSize = uiSize;
    if (m_uiMaxMsgSize < 1024) m_uiMaxMsgSize = 1024;
}
///获取package的最大大小
CWX_UINT32 CwxMsgHead::getMaxMsgSize(){
    return m_uiMaxMsgSize;
}

CwxMsgBlock* CwxMsgHead::packKeepalive(bool bReply)
{
    CwxMsgBlock* msg = CwxMsgBlockAlloc::malloc(MSG_HEAD_LEN);
    m_ucVersion = 0;
    m_ucAttr = 0;
    setAttr(ATTR_SYS_MSG);
    m_unMsgType = bReply?MSG_TYPE_KEEPALIVE_REPLY:MSG_TYPE_KEEPALIVE;
    m_uiTaskId = 0;
    m_uiDataLen = 0;
    memcpy(msg->wr_ptr(), toNet(), MSG_HEAD_LEN);
    msg->wr_ptr(MSG_HEAD_LEN);
    return msg;
}

CwxMsgBlock* CwxMsgHead::packSysMsgReply(CwxMsgHead const& header, CwxMsgBlock* )
{
    if ((header.getMsgType() %2) == 0)
    {//it's reply 
        return NULL;
    }
    //need to reply, just msg_type + 1
    CwxMsgBlock* msg = CwxMsgBlockAlloc::malloc(MSG_HEAD_LEN);
    *this = header;
    m_unMsgType ++;
    memcpy(msg->wr_ptr(), toNet(), MSG_HEAD_LEN);
    msg->wr_ptr(MSG_HEAD_LEN);
    return msg;
}

CWINUX_END_NAMESPACE

