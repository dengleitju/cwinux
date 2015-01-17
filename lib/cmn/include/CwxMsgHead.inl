
CWINUX_BEGIN_NAMESPACE
inline char const* CwxMsgHead::toNet()
{
    CWX_UINT32 byte4;
    CWX_UINT16 byte2;
    CWX_UINT8  pos = 0;
    CWX_UINT16 unCheckSum=0;

    unCheckSum += (CWX_UINT16)(m_ucVersion + m_ucAttr + m_unMsgType + m_uiTaskId + m_uiDataLen);
    //version
    m_szHead[pos] = m_ucVersion; pos++;
    //attr
    m_szHead[pos] = m_ucAttr; pos++;
    //Msg Type
    byte2 = CWX_HTONS(m_unMsgType); memcpy(m_szHead+pos, &byte2, 2); pos+=2;
    //task ID
    byte4 = CWX_HTONL(m_uiTaskId); memcpy(m_szHead+pos, &byte4, 4); pos+=4;
    //data length
    byte4 = CWX_HTONL(m_uiDataLen); memcpy(m_szHead+pos, &byte4, 4); pos+=4;
    //checksum
    byte2 = CWX_HTONS(unCheckSum); memcpy(m_szHead+pos, &byte2, 2); pos+=2;
    return m_szHead;
}
inline bool CwxMsgHead::fromNet(char const* szHead)
{
    CWX_UINT32 byte4;
    CWX_UINT16 byte2;
    CWX_UINT8  pos = 0;
    CWX_UINT16  unCheckSum=0;
    //version
    m_ucVersion = szHead[pos];pos++;
    //attr
    m_ucAttr = szHead[pos]; pos++;
    //Msg Type
    memcpy(&byte2, szHead+pos, 2); m_unMsgType = CWX_NTOHS(byte2); pos+=2;
    //task ID
    memcpy(&byte4, szHead+pos, 4); m_uiTaskId = CWX_NTOHL(byte4); pos+=4;
    //data length
    memcpy(&byte4, szHead+pos, 4); m_uiDataLen = CWX_NTOHL(byte4); pos+=4;
    //checksum
    memcpy(&byte2, szHead+pos, 2); unCheckSum = CWX_NTOHS(byte2); pos+=2;
    if (m_uiMaxMsgSize < m_uiDataLen) return false;
    return (unCheckSum == (CWX_UINT16)(m_ucVersion + m_ucAttr + m_unMsgType + m_uiTaskId + m_uiDataLen))?true:false;
}

CWINUX_END_NAMESPACE

