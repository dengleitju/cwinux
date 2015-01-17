#include "CwxEchoEventHandler.h"
#include "CwxEchoApp.h"

///echo请求的处理函数
int CwxEchoEventHandler::onRecvMsg(CwxMsgBlock*& msg, CwxTss* )
{
    ///设置echo回复的消息类型，为请求的消息类型+1
    msg->event().getMsgHeader().setMsgType(msg->event().getMsgHeader().getMsgType() + 1);
    ///设置echo回复的数据包长度
    msg->event().getMsgHeader().setDataLen(msg->length());
    ///创建回复的数据包
    CwxMsgBlock* pBlock = CwxMsgBlockAlloc::malloc(msg->length() + CwxMsgHead::MSG_HEAD_LEN);
    ///拷贝数据包的包头
    memcpy(pBlock->wr_ptr(), msg->event().getMsgHeader().toNet(), CwxMsgHead::MSG_HEAD_LEN);
    ///滑动block的写指针
    pBlock->wr_ptr(CwxMsgHead::MSG_HEAD_LEN);
    ///拷贝数据包的数据
    memcpy(pBlock->wr_ptr(), msg->rd_ptr(), msg->length());
    ///滑动block的写指针
    pBlock->wr_ptr(msg->length());
    ///设置回复消息的发送控制信息
    pBlock->send_ctrl().reset();
    ///设置回复消息对应连接的svr-id
    pBlock->send_ctrl().setSvrId(msg->event().getSvrId());
    ///设置回复消息对应连接的host-id
    pBlock->send_ctrl().setHostId(msg->event().getHostId());
    ///设置回复消息的连接id
    pBlock->send_ctrl().setConnId(msg->event().getConnId());
    ///回复消息
    if (0 != this->m_pApp->sendMsgByConn(pBlock))
    {
        CWX_ERROR(("Failure to send msg"));
        return -1;
    }
    m_ullMsgNum ++;
    if (m_ullMsgNum && !(m_ullMsgNum%10000))
    {
        char szBuf[64];
        CwxCommon::toString(m_ullMsgNum, szBuf, 10);
        CWX_INFO(("Recv echo message num:%s", szBuf));
    }
    return 1;
}

