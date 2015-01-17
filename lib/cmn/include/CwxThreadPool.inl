CWINUX_BEGIN_NAMESPACE

inline size_t CwxThreadPool::getQueuedMsgNum()
{ 
    return m_msgQueue->getMsgCount();
}

inline int CwxThreadPool::append(CwxMsgBlock* pMsg)
{
    return m_msgQueue->enqueue_tail(pMsg);
}

inline int CwxThreadPool::appendHead(CwxMsgBlock* pMsg)
{
    return m_msgQueue->enqueue(pMsg);
}

inline int CwxThreadPool::pop(CwxMsgBlock*& pMsg)
{
    return m_msgQueue->dequeue(pMsg);
}

CWINUX_END_NAMESPACE

