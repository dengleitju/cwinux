#include "CwxPackageEx.h"

CWINUX_BEGIN_NAMESPACE

//-1：包的格式非法；0:不存在；>0：Key/Value的长度。
int CwxPackageEx::getNextKey(char const* szMsg,
                             CWX_UINT32 uiMsgLen,
                             CwxKeyValueItemEx& item)
{
    CWX_UINT32 byte4 = 0;
    CWX_UINT16 byte2 = 0;
    CWX_UINT32 uiLeftLen = uiMsgLen;
    bool    bKeyValue = false;
    unsigned char const* pos = (unsigned char*)szMsg;
    if (0 == uiLeftLen) return 0;
    ///data_len|key_len|key_value|key|\0|value\0
    ///get data len
    pos = decodeUint32(pos, uiLeftLen, byte4);
    if (!pos) return -1;
    if (byte4 > uiMsgLen) return -1;//bad msg data package.
    //get key len
    pos = decodeUint16(pos, uiLeftLen, byte2);
    if (!pos) return -1;
    bKeyValue = pos[0];
    pos++;
    item.m_unKeyLen = byte2;
    item.m_szKey = (char*)pos;
    if (item.m_szKey[byte2]) return -1;//bad msg data package.
    //get value len
    item.m_uiDataLen = byte4;
    item.m_szData = item.m_szKey +  item.m_unKeyLen + 1;
    item.m_bKeyValue = bKeyValue;
    if (item.m_szData[byte4]) return -1;
    return getKvLen(byte2, byte4);
}

//-1：包的空间太小；>0 打入的包的长度。
int CwxPackageEx::appendKey(char *szMsg, CWX_UINT32 uiMsgLen, char const* szKey, CWX_UINT16 unKeyLen, char const* szValue, CWX_UINT32 uiDatalen, bool bKeyValue){
    CWX_UINT32 byte4;
    CWX_UINT32 pos=0;
    CWX_UINT32 len=0;
    byte4 = getKvLen(unKeyLen, uiDatalen);
    if (byte4 > uiMsgLen) return -1;
    if (byte4 > CwxKeyValueItemEx::MAX_KV_LEN) return -1;
    ///data_len|key_len|key_value|key|\0|value\0
    //append pair len
    encodeUint32(uiDatalen, (unsigned char*)szMsg, len); pos +=len;
    //append key len
    encodeUint16(unKeyLen, (unsigned char*)(szMsg + pos),len); pos +=len;
    //add key value sign
    szMsg[pos++]=bKeyValue?1:0;
    //append key
    memcpy(szMsg + pos, szKey, unKeyLen);pos += unKeyLen;
    //append \0
    szMsg[pos] = 0x00; pos += 1;
    //append data
    if (uiDatalen){
        memcpy(szMsg + pos, szValue, uiDatalen);pos += uiDatalen;
    }
    szMsg[pos] = 0x00;
    pos += 1;
    return (int)pos;
}

//-1：无效的package，0：没有发现，>0：删除的数量。
int CwxPackageEx::removeKey(char *szMsg, CWX_UINT32& uiMsgLen, char const* szKey, bool bAll, bool bCaseSensive)
{
    CWX_UINT32 uiPos = 0;
    int remove_key = 0;
    int len=0;
    CwxKeyValueItemEx item;
    bool bFind = false;
    for ( ;uiPos<uiMsgLen ; ){
        len = getNextKey(szMsg + uiPos, uiMsgLen - uiPos, item);
        if (-1 == len) return -1;
        if (0 == len) break;
        if (bCaseSensive){
            bFind = strcmp(item.m_szKey, szKey)==0?true:false;
        }else{
            bFind =strcasecmp(item.m_szKey, szKey)==0?true:false;
        }
        if (bFind){
            CwxCommon::memMove(szMsg + uiPos, szMsg+uiPos+len, uiMsgLen-uiPos-len);
            uiMsgLen -= len;
            remove_key++;
            if (!bAll) break;
        }else{
            uiPos += len;
        }
    }
    return remove_key;
}


//-2空间不够，-1：无效的package，0：没有发现，1：修改了一个KEY。
int CwxPackageEx::modifyKey(char *szMsg, CWX_UINT32& uiMsgLen, CWX_UINT32 uiMaxMsgLen, char const* szKey, CWX_UINT16 unKeyLen, char const* szData, CWX_UINT32 uiDataLen, bool bKeyValue, bool bCaseSensive)
{
    CWX_UINT32 uiNewKeyLen = getKvLen(strlen(szKey), uiDataLen);
    CWX_UINT32 uiOldKeyLen = 0;
    CWX_UINT32 uiPos = 0;
    int len=0;
    CwxKeyValueItemEx item;
    bool bFind = false;
    for ( ; uiPos<uiMsgLen ; )
    {
        len = getNextKey(szMsg + uiPos, uiMsgLen - uiPos, item);
        if (-1 == len) return -1;
        if (0 == len) return 0;
        if (bCaseSensive){
            bFind = strcmp(item.m_szKey, szKey)==0?true:false;
        }else{
            bFind = strcasecmp(item.m_szKey, szKey)==0?true:false;
        }
        if (bFind){
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
int CwxPackageEx::modifyKey(char *szMsg, CWX_UINT32& uiMsgLen, CWX_UINT32 uiMaxMsgLen, CWX_UINT16 unIndex,char const* szData, CWX_UINT32 uiDataLen, bool bKeyValue)
{
    CWX_UINT32 uiNewKeyLen = 0;
    CWX_UINT32 uiOldKeyLen = 0;
    CWX_UINT32 uiPos = 0;
    int len=0;
    CWX_UINT32 i = 0;
    CwxKeyValueItemEx item;
    for (i=0; i<unIndex; i++){
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

int CwxPackageEx::dumpEx(char const* szMsg, CWX_UINT32 uiMsgLen, char* szOutBuf, CWX_UINT32& uiOutBufLen, char const* szTab, CWX_UINT32 uiTabNum, char const* szKeyBegin, char const* szKeyEnd, CwxEscape const* pEscape)
{
    CWX_UINT32 i = 0;
    CWX_UINT32 uiMsgPos = 0;
    CWX_UINT32 uiOutPos = 0;
    CWX_UINT32 uiTmp = 0;
    CWX_UINT32 uiTabLen=szTab?strlen(szTab):0;
    CWX_UINT32 uiBeginLen = szKeyBegin?strlen(szKeyBegin):0;
    CWX_UINT32 uiEndLen = szKeyEnd?strlen(szKeyEnd):0;
    int len=0;
    CwxKeyValueItemEx item;
    for ( ; uiMsgPos<uiMsgLen ; )
    {
        len = getNextKey(szMsg + uiMsgPos, uiMsgLen - uiMsgPos, item);
        if (-1 == len) return -1;
        if (0 == len) break;
        uiMsgPos += len;
        //add tab
        if (uiTabLen){
            if (uiTabLen * uiTabNum + uiOutPos >= uiOutBufLen) return -2;
            for (i=0; i<uiTabNum; i++){
                memcpy(szOutBuf + uiOutPos, szTab, uiTabLen);
                uiOutPos+=uiTabLen;
            }
        }
        //add key begin
        if (uiBeginLen){
            if (uiOutPos + uiBeginLen >= uiOutBufLen) return -2;
            memcpy(szOutBuf + uiOutPos, szKeyBegin, uiBeginLen);
            uiOutPos += uiBeginLen;
        }
        //add key
        if (pEscape){
            uiTmp = uiOutBufLen - uiOutPos - 1;
            if (!pEscape->encode(item.m_szKey, item.m_unKeyLen, szOutBuf + uiOutPos, uiTmp)) return -2;
            uiOutPos += uiTmp;
        }else{
            if (uiOutPos + item.m_unKeyLen >= uiOutBufLen) return -2;
            memcpy(szOutBuf + uiOutPos, item.m_szKey, item.m_unKeyLen);
            uiOutPos += item.m_unKeyLen;
        }
        szOutBuf[uiOutPos++] = ':';
        //add data
        if (item.m_bKeyValue){
            if (uiOutPos + uiEndLen >= uiOutBufLen) return -2;
            memcpy(szOutBuf + uiOutPos, szKeyEnd, uiEndLen);
            uiOutPos += uiEndLen;
            uiTmp = uiOutBufLen - uiOutPos;
            len = dumpEx(item.m_szData, item.m_uiDataLen, szOutBuf + uiOutPos, uiTmp, szTab, uiTabNum+1, szKeyBegin, szKeyEnd, pEscape);
            if (0>len) return len;
            uiOutPos += len;
        }else{
            if (uiOutPos + item.m_uiDataLen >= uiOutBufLen) return -2;
            memcpy(szOutBuf + uiOutPos, item.m_szData, item.m_uiDataLen);
            uiOutPos += item.m_uiDataLen;
        }        
        //add key end
        if (uiEndLen){
            if (uiOutPos + uiEndLen >= uiOutBufLen) return -2;
            memcpy(szOutBuf + uiOutPos, szKeyEnd, uiEndLen);
            uiOutPos += uiEndLen;
        }
    }
    uiOutBufLen = uiOutPos;
    return uiOutPos;
}

CWINUX_END_NAMESPACE
