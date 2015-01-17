#include "CwxEchoEventHandler.h"
#include "CwxEchoApp.h"
#include "../protobuf/echo.pb.h"
///echo请求的处理函数
int CwxEchoEventHandler::onRecvMsg(CwxMsgBlock*& msg, CwxTss* )
{
  string recv_str(msg->rd_ptr(), msg->length());
  cwinux_echo::echo echo;
  if (!echo.ParseFromString(recv_str)) {
    // 解析失败认为是通信错误，关闭连接
    CWX_ERROR(("Failure to unpack echo msg"));
    m_pApp->noticeCloseConn(msg->event().getConnId());
    return 1;
  }
  echo.SerializeToString(&recv_str);
  CwxMsgHead head(0,
    0,
    msg->event().getMsgHeader().getMsgType() + 1,
    msg->event().getMsgHeader().getTaskId(),
    recv_str.length());
  ///分配发送消息包的block
  CwxMsgBlock* block = CwxMsgBlockAlloc::pack(head, recv_str.c_str(), recv_str.length());
  if (!block) {
    CWX_ERROR(("Failure to pack package for no memory"));
    exit(0);
  }

  ///设置回复消息的发送控制信息
  block->send_ctrl().reset();
  ///设置回复消息对应连接的svr-id
  block->send_ctrl().setSvrId(msg->event().getSvrId());
  ///设置回复消息对应连接的host-id
  block->send_ctrl().setHostId(msg->event().getHostId());
  ///设置回复消息的连接id
  block->send_ctrl().setConnId(msg->event().getConnId());
  ///回复消息
  if (0 != this->m_pApp->sendMsgByConn(block)) {
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

