
CWINUX_BEGIN_NAMESPACE

inline size_t CwxThreadPoolEx::getQueuedMsgNum()
{
    size_t num = 0;
    for (CWX_UINT16 i=0; i<getThreadNum(); i++){
        if (m_threadArr && m_threadArr[i]) {
            num += m_threadArr[i]->getQueuedMsgNum();
        }
    }
    return num;
}
///获取线程的消息队列排队消息数
inline size_t CwxThreadPoolEx::getQueuedMsgNum(CWX_UINT16 unThreadIndex)
{
    if (!m_threadArr || (unThreadIndex >= getThreadNum())) return 0;
    return m_threadArr[unThreadIndex]->getQueuedMsgNum();
}

/**
@brief 往线程的消息队列添加一个新消息。
@param [in] pMsg append的消息
@param [in] uiThread 消息的线程队列，内部会根据Thread的数量，计算出所属的线程。
@return -1：失败；>=0为对应线程的队列中排队的消息数量
*/
inline int CwxThreadPoolEx::append(CwxMsgBlock* pMsg, CWX_UINT32 uiThread)
{
    CWX_UINT16 uiIndex = uiThread%getThreadNum();
    return m_threadArr[uiIndex]->append(pMsg);
}

inline int CwxThreadPoolEx::appendHead(CwxMsgBlock* pMsg, CWX_UINT32 uiThread)
{
    CWX_UINT16 uiIndex = uiThread%getThreadNum();
    return m_threadArr[uiIndex]->appendHead(pMsg);
}

CWINUX_END_NAMESPACE

