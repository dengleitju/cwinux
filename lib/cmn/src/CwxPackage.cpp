#include "CwxPackage.h"

CWINUX_BEGIN_NAMESPACE

//-1：包的格式非法；0:不存在；>0：Key/Value的长度。
int CwxPackage::getNextKey(char const* szMsg, CWX_UINT32 uiMsgLen, CwxKeyValueItem& item)
{
    CWX_UINT32 byte4 = 0;
    CWX_UINT16 byte2 = 0;
    bool    bKeyValue = false;
    if (0 == uiMsgLen) return 0;
    memcpy(&byte4, szMsg, 4); //fetch key/value head
    byte4 = CWX_NTOHL(byte4);
    bKeyValue = ((byte4 & (1<<CwxKeyValueItem::BIT_PACKAGE))>0)?true:false;
    byte4 = byte4 & CwxKeyValueItem::MAX_KV_LEN;
    if (byte4 > uiMsgLen) return -1;//bad msg data package.
    //get key len
    memcpy(&byte2, szMsg + 4, 2);
    byte2 = CWX_NTOHS(byte2);
    item.m_unKeyLen = byte2;
    item.m_szKey = (char*)(szMsg + 6);
    if ((byte2>=byte4) || (0 != szMsg[6+byte2])) return -1;//bad msg data package.
    //get value len
    item.m_uiDataLen = getDataLen(byte4, item.m_unKeyLen);
    item.m_szData = item.m_szKey +  item.m_unKeyLen + 1;
    item.m_bKeyValue = bKeyValue;
    return (int)byte4;
}

//-1：包的空间太小；>0 打入的包的长度。
int CwxPackage::appendKey(char *szMsg, CWX_UINT32 uiMsgLen, char const* szKey, CWX_UINT16 unKeyLen, char const* szValue, CWX_UINT32 uiDatalen, bool bKeyValue){
    CWX_UINT16 key_len = unKeyLen;
    CWX_UINT32 byte4;
    CWX_UINT16 byte2;
    CWX_UINT32 pos=0;
    byte4 = getKvLen(key_len , uiDatalen);
    if (byte4 > uiMsgLen) return -1;
    if (byte4 > CwxKeyValueItem::MAX_KV_LEN) return -1;
    //append pair len
    if (bKeyValue) byte4 |= (1<<CwxKeyValueItem::BIT_PACKAGE);
    byte4 = CWX_HTONL(byte4);
    memcpy(szMsg, &byte4, 4); pos +=4;
    //append key len
    byte2 = CWX_HTONS(key_len);
    memcpy(szMsg + pos, &byte2, 2); pos+=2;
    //append key
    memcpy(szMsg + pos, szKey, key_len);pos += key_len;
    //append \0
    szMsg[pos] = 0x00; pos += 1;
    //append data
    if (uiDatalen)
    {
        memcpy(szMsg + pos, szValue, uiDatalen);pos += uiDatalen;
    }
    szMsg[pos] = 0x00;
    pos += 1;
    return (int)pos;
}
//-1：无效的package，0：没有发现，>0：删除的数量。
int CwxPackage::removeKey(char *szMsg, CWX_UINT32& uiMsgLen, char const* szKey, bool bAll, bool bCaseSensive)
{
    CWX_UINT32 uiPos = 0;
    int remove_key = 0;
    int len=0;
    CwxKeyValueItem item;
    bool bFind = false;
    for ( ;uiPos<uiMsgLen ; )
    {
        len = getNextKey(szMsg + uiPos, uiMsgLen - uiPos, item);
        if (-1 == len) return -1;
        if (0 == len) break;
        if (bCaseSensive)
        {
            bFind = strcmp(item.m_szKey, szKey)==0?true:false;
        }
        else
        {
            bFind =strcasecmp(item.m_szKey, szKey)==0?true:false;
        }
        if (bFind)
        {
            CwxCommon::memMove(szMsg + uiPos, szMsg+uiPos+len, uiMsgLen-uiPos-len);
            uiMsgLen -= len;
            remove_key++;
            if (!bAll) break;
        }
        else
        {
            uiPos += len;
        }
    }
    return remove_key;
}
//-2空间不够，-1：无效的package，0：没有发现，1：修改了一个KEY。
int CwxPackage::modifyKey(char *szMsg, CWX_UINT32& uiMsgLen, CWX_UINT32 uiMaxMsgLen, char const* szKey, CWX_UINT16 unKeyLen, char const* szData, CWX_UINT32 uiDataLen, bool bKeyValue, bool bCaseSensive)
{
    CWX_UINT32 uiNewKeyLen = getKvLen(strlen(szKey), uiDataLen);
    CWX_UINT32 uiOldKeyLen = 0;
    CWX_UINT32 uiPos = 0;
    int len=0;
    CwxKeyValueItem item;
    bool bFind = false;
    for ( ; uiPos<uiMsgLen ; )
    {
        len = getNextKey(szMsg + uiPos, uiMsgLen - uiPos, item);
        if (-1 == len) return -1;
        if (0 == len) return 0;
        if (bCaseSensive)
        {
            bFind = strcmp(item.m_szKey, szKey)==0?true:false;
        }
        else
        {
            bFind = strcasecmp(item.m_szKey, szKey)==0?true:false;
        }
        if (bFind)
        {
            uiOldKeyLen = (CWX_UINT32)len;
            if (uiMsgLen - uiOldKeyLen + uiNewKeyLen > uiMaxMsgLen) return -2;
            CwxCommon::memMove(szMsg + uiPos + uiNewKeyLen, szMsg+uiPos+len, uiMsgLen-uiPos-len);
            appendKey(szMsg + uiPos, uiNewKeyLen, szKey, unKeyLen, szData, uiDataLen, bKeyValue);
            uiMsgLen = uiMsgLen - uiOldKeyLen + uiNewKeyLen;
            return 1;
        }
        uiPos += len;
    }
    return 0;
}
//-2空间不够，-1：无效的package，0：没有发现，1：修改了一个KEY。
int CwxPackage::modifyKey(char *szMsg, CWX_UINT32& uiMsgLen, CWX_UINT32 uiMaxMsgLen, CWX_UINT16 unIndex,char const* szData, CWX_UINT32 uiDataLen, bool bKeyValue)
{
    CWX_UINT32 uiNewKeyLen = 0;
    CWX_UINT32 uiOldKeyLen = 0;
    CWX_UINT32 uiPos = 0;
    int len=0;
    CWX_UINT32 i = 0;
    CwxKeyValueItem item;
    for (i=0; i<unIndex; i++)
    {
        len = getNextKey(szMsg + uiPos, uiMsgLen - uiPos, item);
        if (-1 == len) return -1;
        if (0 == len) return 0;
        uiPos += len;
    }
    uiNewKeyLen = getKvLen(item.m_unKeyLen, uiDataLen);
    //find 
    uiOldKeyLen = (CWX_UINT32)len;
    if (uiMsgLen - uiOldKeyLen + uiNewKeyLen > uiMaxMsgLen) return -2;
    CwxCommon::memMove(szMsg + uiPos + uiNewKeyLen, szMsg+uiPos+len, uiMsgLen-uiPos-len);
    appendKey(szMsg + uiPos, uiNewKeyLen, item.m_szKey, item.m_unKeyLen, szData, uiDataLen, bKeyValue);
    uiMsgLen = uiMsgLen - uiOldKeyLen + uiNewKeyLen;
    return 1;
}

int CwxPackage::dumpEx(char const* szMsg, CWX_UINT32 uiMsgLen, char* szOutBuf, CWX_UINT32& uiOutBufLen, char const* szTab, CWX_UINT32 uiTabNum, char const* szKeyBegin, char const* szKeyEnd, CwxEscape const* pEscape)
{
    CWX_UINT32 i = 0;
    CWX_UINT32 uiMsgPos = 0;
    CWX_UINT32 uiOutPos = 0;
    CWX_UINT32 uiTmp = 0;
    CWX_UINT32 uiTabLen=szTab?strlen(szTab):0;
    CWX_UINT32 uiBeginLen = szKeyBegin?strlen(szKeyBegin):0;
    CWX_UINT32 uiEndLen = szKeyEnd?strlen(szKeyEnd):0;
    int len=0;
    CwxKeyValueItem item;
    for ( ; uiMsgPos<uiMsgLen ; )
    {
        len = getNextKey(szMsg + uiMsgPos, uiMsgLen - uiMsgPos, item);
        if (-1 == len) return -1;
        if (0 == len) break;
        uiMsgPos += len;
        //add tab
        if (uiTabLen)
        {
            if (uiTabLen * uiTabNum + uiOutPos >= uiOutBufLen) return -2;
            for (i=0; i<uiTabNum; i++)
            {
                memcpy(szOutBuf + uiOutPos, szTab, uiTabLen);
                uiOutPos+=uiTabLen;
            }
        }
        //add key begin
        if (uiBeginLen)
        {
            if (uiOutPos + uiBeginLen >= uiOutBufLen) return -2;
            memcpy(szOutBuf + uiOutPos, szKeyBegin, uiBeginLen);
            uiOutPos += uiBeginLen;
        }
        //add key
        if (pEscape)
        {
            uiTmp = uiOutBufLen - uiOutPos - 1;
            if (!pEscape->encode(item.m_szKey, item.m_unKeyLen, szOutBuf + uiOutPos, uiTmp)) return -2;
            uiOutPos += uiTmp;
        }
        else
        {
            if (uiOutPos + item.m_unKeyLen >= uiOutBufLen) return -2;
            memcpy(szOutBuf + uiOutPos, item.m_szKey, item.m_unKeyLen);
            uiOutPos += item.m_unKeyLen;
        }
        szOutBuf[uiOutPos++] = ':';
        //add data
        if (item.m_bKeyValue)
        {
            if (uiOutPos + uiEndLen >= uiOutBufLen) return -2;
            memcpy(szOutBuf + uiOutPos, szKeyEnd, uiEndLen);
            uiOutPos += uiEndLen;
            uiTmp = uiOutBufLen - uiOutPos;
            len = dumpEx(item.m_szData, item.m_uiDataLen, szOutBuf + uiOutPos, uiTmp, szTab, uiTabNum+1, szKeyBegin, szKeyEnd, pEscape);
            if (0>len) return len;
            uiOutPos += len;
        }
        else
        {
            if (uiOutPos + item.m_uiDataLen >= uiOutBufLen) return -2;
            memcpy(szOutBuf + uiOutPos, item.m_szData, item.m_uiDataLen);
            uiOutPos += item.m_uiDataLen;
        }        
        //add key end
        if (uiEndLen)
        {
            if (uiOutPos + uiEndLen >= uiOutBufLen) return -2;
            memcpy(szOutBuf + uiOutPos, szKeyEnd, uiEndLen);
            uiOutPos += uiEndLen;
        }
    }
    uiOutBufLen = uiOutPos;
    return uiOutPos;
}

CWINUX_END_NAMESPACE
