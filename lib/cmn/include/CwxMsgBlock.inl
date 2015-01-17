
CWINUX_BEGIN_NAMESPACE
inline bool CwxMsgSendCtrl::isBeginNotice() const 
{ 
    return (m_uiMsgAttr&BEGIN_NOTICE)!=0;
}

inline bool CwxMsgSendCtrl::isFinishNotice() const
{
    return (m_uiMsgAttr&FINISH_NOTICE)!=0;
}

inline bool CwxMsgSendCtrl::isFailNotice() const
{
    return (m_uiMsgAttr&FAIL_NOTICE)!=0;
}

inline void CwxMsgSendCtrl::setConnCtrl(CWX_UINT32 uiSvrId,
                                           CWX_UINT32 uiHostId,
                                           CWX_UINT32 uiConnId,
                                           CWX_UINT32 uiMsgAttr,
                                           void* userData,
                                           CWX_UINT32 uiConnState)
{
    m_uiSvrId = uiSvrId;
    m_uiHostId = uiHostId;
    m_uiConnId = uiConnId;
    m_uiMsgAttr = uiMsgAttr;
    m_userData = userData;
    m_uiConnResumeState = uiConnState;
}

///获取SVR ID
inline CWX_UINT32 CwxMsgSendCtrl::getSvrId() const
{
    return m_uiSvrId;
}
///设置SVR ID
inline void CwxMsgSendCtrl::setSvrId(CWX_UINT32 uiSvrId)
{
    m_uiSvrId = uiSvrId;
}

///获取HOST ID
inline CWX_UINT32 CwxMsgSendCtrl::getHostId() const
{
    return m_uiHostId;
}
///设置HOST ID
inline void CwxMsgSendCtrl::setHostId(CWX_UINT32 uiHostId)
{
    m_uiHostId = uiHostId;
}

inline CWX_UINT32 CwxMsgSendCtrl::getConnId() const 
{ 
    return m_uiConnId;
}

inline void CwxMsgSendCtrl::setConnId(CWX_UINT32 uiConnId) 
{ 
    m_uiConnId = uiConnId;
}

inline CWX_UINT32 CwxMsgSendCtrl::getDefHostId() const
{
    return m_uiDefHostId;
}

inline void CwxMsgSendCtrl::setDefHostId(CWX_UINT32 uiDefHostId)
{
    m_uiDefHostId = uiDefHostId;
}

inline CWX_UINT32 CwxMsgSendCtrl::getMsgAttr() const 
{ 
    return m_uiMsgAttr;
}

inline void CwxMsgSendCtrl::setMsgAttr(CWX_UINT32 uiAttr) 
{ 
    m_uiMsgAttr = uiAttr;
}

inline void* CwxMsgSendCtrl::getUserData() 
{ 
    return m_userData;
}

inline void CwxMsgSendCtrl::setUserData(void* userData)
{ 
    m_userData = userData;
}

inline CWX_UINT32 CwxMsgSendCtrl::getConnResumeState() const 
{ 
    return m_uiConnResumeState;
}

inline bool CwxMsgSendCtrl::isResumeConn() const 
{ 
    return RESUME_CONN == m_uiConnResumeState;
}

inline bool CwxMsgSendCtrl::isSuspendConn() const 
{ 
    return SUSPEND_CONN == m_uiConnResumeState;
}

inline bool CwxMsgSendCtrl::isUndoConn() const 
{ 
    return !isResumeConn() && !isSuspendConn();
}

inline void CwxMsgSendCtrl::setConnResumeState(CWX_UINT32 uiState) 
{  
    m_uiConnResumeState = uiState;
}
inline void CwxMsgSendCtrl::setReconnDelay(CWX_UINT32 uiMiliSecond)
{
    m_uiReconnDelay = uiMiliSecond;
}

inline CWX_UINT32 CwxMsgSendCtrl::getReconnDelay() const
{
    return m_uiReconnDelay;
}

inline void CwxMsgSendCtrl::reset()
{
    m_userData = NULL;
    m_uiSvrId = 0;
    m_uiHostId = 0;
    m_uiConnId = 0;
    m_uiDefHostId = 0;
    m_uiMsgAttr = 0;
    m_uiConnResumeState = UNDO_CONN;
    m_uiReconnDelay = 0;
}

inline CwxMsgSendCtrl::CwxMsgSendCtrl()
{
    reset();
}

inline CwxMsgSendCtrl::~CwxMsgSendCtrl()
{
}

inline CWX_UINT16  CwxEventInfo::getEvent() const 
{ 
    return m_unEvent;
}

inline void CwxEventInfo::setEvent(CWX_UINT16 unEvent) 
{ 
    m_unEvent = unEvent;
}

inline CWX_UINT32 CwxEventInfo::getSvrId() const 
{ 
    return m_uiSvrId;
}

inline void CwxEventInfo::setSvrId(CWX_UINT32 uiSvrId) 
{ 
    m_uiSvrId = uiSvrId;
}

inline CWX_UINT32 CwxEventInfo::getHostId() const 
{ 
    return m_uiHostId;
}

inline void CwxEventInfo::setHostId(CWX_UINT32 uiHostId) 
{ 
    m_uiHostId = uiHostId;
}

inline CWX_UINT32 CwxEventInfo::getConnId() const 
{ 
    return m_uiConnId;
}

inline void CwxEventInfo::setConnId(CWX_UINT32 uiConnId) 
{ 
    m_uiConnId = uiConnId;
}

inline void* CwxEventInfo::getConnUserData() 
{ 
    return m_connUserData;
}

inline void CwxEventInfo::setConnUserData(void* userData) 
{ 
    m_connUserData = userData;
}


inline CWX_UINT32 CwxEventInfo::getTaskId() const 
{ 
    return m_uiTaskId;
}

inline void CwxEventInfo::setTaskId(CWX_UINT32 uiTaskId) 
{ 
    m_uiTaskId = uiTaskId;
}

inline CwxMsgHead& CwxEventInfo::getMsgHeader() 
{ 
    return m_msgHeader;
}

inline void CwxEventInfo::setMsgHeader(CwxMsgHead const& header) 
{ 
    m_msgHeader = header;
}

inline CWX_HANDLE CwxEventInfo::getIoHandle() 
{ 
    return m_ioHandle;
}

inline void CwxEventInfo::setIoHandle(CWX_HANDLE handle) 
{ 
    m_ioHandle = handle;
}

inline CWX_UINT16 CwxEventInfo::getRegEventMask() const 
{ 
    return m_unRegEventMask;
}

inline void CwxEventInfo::setRegEventMask(CWX_UINT16 unMask) 
{ 
    m_unRegEventMask = unMask;
}

inline CWX_UINT16 CwxEventInfo::getReadyEvent() const 
{ 
    return m_unReadyHandleEvent;
}

inline void CwxEventInfo::setReadyEvent(CWX_UINT16 unEvent) 
{ 
    m_unReadyHandleEvent = unEvent;
}

inline CWX_UINT64 CwxEventInfo::getTimestamp() const 
{ 
    return m_ullTimeStamp;
}

inline void CwxEventInfo::setTimestamp(CWX_UINT64 ullTimestamp)
{ 
    m_ullTimeStamp = ullTimestamp;
}

inline void CwxEventInfo::reset(){
    m_unEvent = DUMMY;
    m_uiSvrId = 0;
    m_uiHostId = 0;
    m_uiConnId = 0;
    m_connUserData = NULL;
    m_uiTaskId = 0;
    m_msgHeader.reset();
    m_ioHandle = CWX_INVALID_HANDLE;
    m_unRegEventMask = 0;
    m_unReadyHandleEvent = 0;
    m_ullTimeStamp = 0;
    m_uiArg = 0;
    m_ullArg = 0;
    m_pArg = NULL;
}

inline CwxEventInfo::CwxEventInfo()
{ 
    reset();
}

inline CwxEventInfo::~CwxEventInfo()
{

}

inline CwxEventInfo& CwxEventInfo::operator=(CwxEventInfo const& )
{ 
    return *this;
}


inline char *CwxMsgBlock::rd_ptr (void) const
{ 
    return m_buf + m_rdPos;
}

inline void CwxMsgBlock::rd_ptr (size_t n)
{
    m_rdPos += n;
}

inline char *CwxMsgBlock::wr_ptr (void) const 
{ 
    return m_buf + m_wrPos;
}

inline void CwxMsgBlock::wr_ptr(size_t n) 
{ 
    m_wrPos += n;
}

inline size_t CwxMsgBlock::length(void) const 
{ 
    return m_wrPos > m_rdPos?m_wrPos - m_rdPos:0;
}

inline size_t CwxMsgBlock::capacity(void) const
{ 
    return m_len;
}
inline void CwxMsgBlock::reset()
{
    m_rdPos = 0;
    m_wrPos = 0;
    m_event.reset();
    m_sendCtrl.reset();
}

inline CwxEventInfo& CwxMsgBlock::event() 
{ 
    return m_event;
}

inline CwxMsgSendCtrl& CwxMsgBlock::send_ctrl()
{ 
    return m_sendCtrl;
}

inline CwxMsgBlock::~CwxMsgBlock()
{
    if (m_buf) free(m_buf);
    m_buf = NULL;
}

inline CwxMsgBlock::CwxMsgBlock(size_t size)
{
    m_buf = (char*)malloc(size);
    if (m_buf)
        m_len = size;
    else
        m_len = 0;
    m_wrPos = 0;
    m_rdPos = 0;
    m_bFree = true;
    m_next = NULL;
}

inline CwxMsgBlock::CwxMsgBlock()
{
    m_buf = NULL;
    m_len = 0;
    m_wrPos = 0;
    m_rdPos = 0;
    m_bFree = true;
    m_next = NULL;
}

///创建signleton CwxEscapes 对象
inline CwxMsgBlockAlloc* CwxMsgBlockAlloc::instance()
{
    if (!m_pInstance)
    {
        CwxMutexGuard<CwxMutexLock> lock(&m_lock);
        if (!m_pInstance)
        {
            m_pInstance = new CwxMsgBlockAlloc();
        }
    }
    return m_pInstance;
}


CWINUX_END_NAMESPACE
