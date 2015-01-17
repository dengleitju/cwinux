inline char const* cwx_pack_head(CWX_MSG_HEADER_S const*  header, char* szHead){
    CWX_UINT32 byte4;
    CWX_UINT16 byte2;
    CWX_UINT8  pos = 0;
    CWX_UINT16 unCheckSum=0;
    unCheckSum += (CWX_UINT16)(header->m_ucVersion + header->m_ucAttr + header->m_unMsgType + header->m_uiTaskId + header->m_uiDataLen);
    //version
    szHead[pos] = header->m_ucVersion; pos++;
    //attr
    szHead[pos] = header->m_ucAttr; pos++;
    //Msg Type
    byte2 = htons(header->m_unMsgType); memcpy(szHead+pos, &byte2, 2); pos+=2;
    //task ID
    byte4 = htonl(header->m_uiTaskId); memcpy(szHead+pos, &byte4, 4); pos+=4;
    //data length
    byte4 = htonl(header->m_uiDataLen); memcpy(szHead+pos, &byte4, 4); pos+=4;
    //checksum
    byte2 = htons(unCheckSum); memcpy(szHead+pos, &byte2, 2); pos+=2;
    return szHead;
}

inline bool cwx_unpack_head(char const* szHead, CWX_MSG_HEADER_S *  header){
    CWX_UINT32 byte4;
    CWX_UINT16 byte2;
    CWX_UINT8  pos = 0;
    CWX_UINT16  unCheckSum=0;
    //version
    header->m_ucVersion = szHead[pos];pos++;
    //attr
    header->m_ucAttr = szHead[pos]; pos++;
    //Msg Type
    memcpy(&byte2, szHead+pos, 2); header->m_unMsgType = ntohs(byte2); pos+=2;
    //task ID
    memcpy(&byte4, szHead+pos, 4); header->m_uiTaskId = ntohl(byte4); pos+=4;
    //data length
    memcpy(&byte4, szHead+pos, 4); header->m_uiDataLen = ntohl(byte4); pos+=4;
    //checksum
    memcpy(&byte2, szHead+pos, 2); unCheckSum = ntohs(byte2); pos+=2;
    return (unCheckSum == (CWX_UINT16)(header->m_ucVersion + header->m_ucAttr + header->m_unMsgType + header->m_uiTaskId + header->m_uiDataLen))?true:false;
}
