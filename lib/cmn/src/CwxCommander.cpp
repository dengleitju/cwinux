#include "CwxCommander.h"

CWINUX_BEGIN_NAMESPACE

int CwxCommander::onConnCreated(CwxCmdOp* pEventOp, CwxMsgBlock*& msg, CwxTss* pThrEnv)
{
    return pEventOp->onConnCreated(msg, pThrEnv);
}

int CwxCommander::onConnClosed(CwxCmdOp* pEventOp, CwxMsgBlock*& msg, CwxTss* pThrEnv)
{
    return pEventOp->onConnClosed(msg, pThrEnv);
}

int CwxCommander::onRecvMsg(CwxCmdOp* pEventOp, CwxMsgBlock*& msg, CwxTss* pThrEnv)
{
    return pEventOp->onRecvMsg(msg, pThrEnv);
}

int CwxCommander::onEndSendMsg(CwxCmdOp* pEventOp, CwxMsgBlock*& msg, CwxTss* pThrEnv)
{
    return pEventOp->onEndSendMsg(msg, pThrEnv);
}

int CwxCommander::onFailSendMsg(CwxCmdOp* pEventOp, CwxMsgBlock*& msg, CwxTss* pThrEnv)
{
    return pEventOp->onFailSendMsg(msg, pThrEnv);
}

int CwxCommander::onTimeoutCheck(CwxCmdOp* pEventOp, CwxMsgBlock*& msg, CwxTss* pThrEnv)
{
    return pEventOp->onTimeoutCheck(msg, pThrEnv);
}

int CwxCommander::onEvent4Handle(CwxCmdOp* pEventOp, CwxMsgBlock*& msg, CwxTss* pThrEnv)
{
    return pEventOp->onEvent4Handle(msg, pThrEnv);
}

int CwxCommander::onUserEvent(CwxCmdOp* pEventOp, CwxMsgBlock*& msg, CwxTss* pThrEnv)
{
    return pEventOp->onUserEvent(msg, pThrEnv);
}

CWINUX_END_NAMESPACE

