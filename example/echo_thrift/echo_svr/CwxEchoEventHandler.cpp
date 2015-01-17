#include "CwxEchoEventHandler.h"
#include "CwxEchoApp.h"

///echo请求的处理函数
int CwxEchoEventHandler::onRecvMsg(CwxMsgBlock*& msg, CwxTss* )
{
    CwxMsgQueue* queue = (CwxMsgQueue*)msg->event().getConnUserData();
    CwxMsgBlock* block = CwxMsgBlockAlloc::malloc(msg->length());
    memcpy(block->wr_ptr(), msg->rd_ptr(), msg->length());
    block->wr_ptr(msg->length());
    block->event().m_ullArg = msg->event().m_ullArg;
    queue->enqueue(block);
    return 1;
}

