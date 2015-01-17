#include "CwxEchoChannelEventHandler.h"
#include "../protobuf/echo.pb.h"

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
  string recv_str(m_recvMsgData->rd_ptr(), m_recvMsgData->length());
  cwinux_echo::echo echo;
  if (!echo.ParseFromString(recv_str)) {
    // 解析失败认为是通信错误，关闭连接
    CWX_ERROR(("Failure to unpack echo msg"));
  }
  echo.SerializeToString(&recv_str);
  CwxMsgHead head(0,
    0,
    m_header.getMsgType() + 1,
    m_header.getTaskId(),
    recv_str.length());
  ///分配发送消息包的block
  CwxMsgBlock* block = CwxMsgBlockAlloc::pack(head, recv_str.c_str(), recv_str.length());
  if (!block) {
    CWX_ERROR(("Failure to pack package for no memory"));
    exit(0);
  }
  if (!putMsg(block)) {
    CWX_ERROR(("Failure to put message"));
    CwxMsgBlockAlloc::free(block);
  }
  m_ullMsgNum ++;
  if (m_ullMsgNum && !(m_ullMsgNum%10000)){
    char szBuf[64];
    CwxCommon::toString(m_ullMsgNum, szBuf, 10);
    CWX_INFO(("Recv echo message num:%s", szBuf));
  }
}
