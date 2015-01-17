#include "CwxEchoChannelEventHandler.h"

/**
@brief 连接可读事件，返回-1，close()会被调用
@return -1：处理失败，会调用close()； 0：处理成功
*/
int CwxEchoChannelEventHandler::onInput()
{
    int ret = CwxAppHandler4Channel::recvPackage(getHandle(),
        m_uiRecvHeadLen,
        m_uiRecvDataLen,
        m_szHeadBuf,
        m_header,
        m_recvMsgData);
    if (1 != ret) return ret;
    replyMessage();
    if (m_recvMsgData) CwxMsgBlockAlloc::free(m_recvMsgData);
    this->m_recvMsgData = NULL;
    this->m_uiRecvHeadLen = 0;
    this->m_uiRecvDataLen = 0;
    return 0;
}
/**
@brief 通知连接关闭。
@return 对于主动连接，1：不从engine中移除注册；0：不从engine中移除注册但不删除handler；-1：从engine中将handle移除并删除。
*/
int CwxEchoChannelEventHandler::onConnClosed()
{
    return -1;
}

void CwxEchoChannelEventHandler::replyMessage()
{
    ///设置echo回复的消息类型，为请求的消息类型+1
    m_recvMsgData->event().getMsgHeader().setMsgType(m_recvMsgData->event().getMsgHeader().getMsgType() + 1);
    ///设置echo回复的数据包长度
    m_recvMsgData->event().getMsgHeader().setDataLen(m_recvMsgData->length());
    ///创建回复的数据包
    CwxMsgBlock* pBlock = CwxMsgBlockAlloc::malloc(m_recvMsgData->length() + CwxMsgHead::MSG_HEAD_LEN);
    ///拷贝数据包的包头
    memcpy(pBlock->wr_ptr(), m_recvMsgData->event().getMsgHeader().toNet(), CwxMsgHead::MSG_HEAD_LEN);
    ///滑动block的写指针
    pBlock->wr_ptr(CwxMsgHead::MSG_HEAD_LEN);
    ///拷贝数据包的数据
    memcpy(pBlock->wr_ptr(), m_recvMsgData->rd_ptr(), m_recvMsgData->length());
    ///滑动block的写指针
    pBlock->wr_ptr(m_recvMsgData->length());
    if (!putMsg(pBlock))
    {
        CWX_ERROR(("Failure to put message"));
        CwxMsgBlockAlloc::free(pBlock);
    }
    m_ullMsgNum ++;
    if (m_ullMsgNum && !(m_ullMsgNum%10000))
    {
        char szBuf[64];
        CwxCommon::toString(m_ullMsgNum, szBuf, 10);
        CWX_INFO(("Recv echo message num:%s", szBuf));
    }


}
