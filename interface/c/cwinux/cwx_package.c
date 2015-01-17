#include "cwx_package.h"

#ifdef __cplusplus
extern "C" {
#endif

int cwx_pg_get_next_key(char const* szMsg,
                             CWX_UINT32 uiMsgLen,
                             CWX_KEY_VALUE_ITEM_S* item)
{
    CWX_UINT32 byte4 = 0;
    CWX_UINT16 byte2 = 0;
    int    bKeyValue = 0;
    if (0 == uiMsgLen) return 0;
    memcpy(&byte4, szMsg, 4); //fetch key/value head
    byte4 = ntohl(byte4);
    bKeyValue = ((byte4 & (1<<CWX_PACKAGE_KV_BIT))>0)?1:0;
    byte4 = byte4 & CWX_PACKAGE_MAX_KV_LEN;
    if (byte4 > uiMsgLen) return -1;//bad msg data package.
    //get key len
    memcpy(&byte2, szMsg + 4, 2);
    byte2 = ntohs(byte2);
    item->m_unKeyLen = byte2;
    item->m_szKey = (char*)(szMsg + 6);
    if ((byte2>=byte4) || (0 != szMsg[6+byte2])) return -1;//bad msg data package.
    //get value len
    item->m_uiDataLen = cwx_pg_get_value_len(byte4, item->m_unKeyLen);
    item->m_szData = item->m_szKey + item->m_unKeyLen + 1;
    item->m_bKeyValue = bKeyValue;
    return (int)byte4;
}

int cwx_pg_get_key_by_index(char const *szMsg,
                                 CWX_UINT32 uiMsgLen,
                                 CWX_UINT32 uiIndex,
                                 CWX_KEY_VALUE_ITEM_S* item)
{
    CWX_UINT32 uiPos = 0;
    CWX_UINT32 i;
    int len=0;
    if (!uiMsgLen) return 0;
    for (i=0; i<=uiIndex; i++)
    {
        len = cwx_pg_get_next_key(szMsg + uiPos, uiMsgLen - uiPos, item);
        if (-1 == len) return -1;
        if (0 == len) return 0;
        uiPos += len;
    }
    return len;
}

int cwx_pg_get_key_by_name(char const *szMsg,
                                CWX_UINT32 uiMsgLen,
                                char const* szKey,
                                CWX_KEY_VALUE_ITEM_S* item,
                                int bCaseSensive)
{
    CWX_UINT32 uiPos = 0;
    int len=0;
    for ( ; uiPos<uiMsgLen; )
    {
        len = cwx_pg_get_next_key(szMsg + uiPos, uiMsgLen - uiPos, item);
        if (-1 == len) return -1;
        if (0 == len) return 0;
        if (bCaseSensive)
        {
            if (strcmp(item->m_szKey, szKey)==0) return len;
        }
        else
        {
            if (strcasecmp(item->m_szKey, szKey)==0) return len;
        }
        uiPos += len;
    }
    return 0;
}

int cwx_pg_append_key(char *szMsg,
                           CWX_UINT32 uiMsgLen,
                           char const* szKey,
                           char const* szValue,
                           CWX_UINT32 uiDatalen, 
                           int bKeyValue){
    CWX_UINT16 key_len = strlen(szKey);
    CWX_UINT32 byte4;
    CWX_UINT16 byte2;
    CWX_UINT32 pos=0;
    byte4 = cwx_pg_get_kv_len(key_len , uiDatalen);
    if (byte4 > uiMsgLen) return -1;
    if (byte4 > CWX_PACKAGE_MAX_KV_LEN) return -1;
    //append pair len
    if (bKeyValue) byte4 |= (1<<CWX_PACKAGE_KV_BIT);
    byte4 = htonl(byte4);
    memcpy(szMsg, &byte4, 4); pos +=4;
    //append key len
    byte2 = htons(key_len);
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

int cwx_pg_remove_key(char *szMsg,
                            CWX_UINT32* uiMsgLen,
                            char const* szKey,
                            int bAll,
                            int bCaseSensive)
{
    CWX_UINT32 uiPos = 0;
    int remove_key = 0;
    int len=0;
    CWX_KEY_VALUE_ITEM_S item;
    int bFind = 0;
    for ( ;uiPos<*uiMsgLen ; )
    {
        len = cwx_pg_get_next_key(szMsg + uiPos, *uiMsgLen - uiPos, &item);
        if (-1 == len) return -1;
        if (0 == len) break;
        if (bCaseSensive)
        {
            bFind = strcmp(item.m_szKey, szKey)==0?1:0;
        }
        else
        {
            bFind =strcasecmp(item.m_szKey, szKey)==0?1:0;
        }
        if (bFind)
        {
            memmove(szMsg + uiPos, szMsg+uiPos+len, *uiMsgLen-uiPos-len);
            *uiMsgLen -= len;
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

int  cwx_pg_remove_key_by_index(char *szMsg,
                                     CWX_UINT32* uiMsgLen,
                                     CWX_UINT16 unIndex)
{
    CWX_UINT32 i;
    CWX_UINT32 uiPos = 0;
    int len=0;
    CWX_KEY_VALUE_ITEM_S item;
    for ( i=0; i<=unIndex; i++)
    {
        len = cwx_pg_get_next_key(szMsg + uiPos, *uiMsgLen - uiPos, &item);
        if (-1 == len) return -1;
        if (0 == len) return 0;
        uiPos += len;
    }
    memmove(szMsg + uiPos - len, szMsg+uiPos, *uiMsgLen-uiPos);
    *uiMsgLen -= len;
    return 1;
}

int  cwx_pg_modify_key(char *szMsg,
                            CWX_UINT32* uiMsgLen,
                            CWX_UINT32 uiMaxMsgLen,
                            char const* szKey,
                            char const* szData,
                            CWX_UINT32 uiDataLen,
                            int bKeyValue,
                            int bCaseSensive)
{
    CWX_UINT32 uiNewKeyLen = cwx_pg_get_kv_len(strlen(szKey), uiDataLen);
    CWX_UINT32 uiOldKeyLen = 0;
    CWX_UINT32 uiPos = 0;
    int len=0;
    CWX_KEY_VALUE_ITEM_S item;
    int bFind = 0;
    for ( ; uiPos<*uiMsgLen ; )
    {
        len = cwx_pg_get_next_key(szMsg + uiPos, *uiMsgLen - uiPos, &item);
        if (-1 == len) return -1;
        if (0 == len) return 0;
        if (bCaseSensive)
        {
            bFind = strcmp(item.m_szKey, szKey)==0?1:0;
        }
        else
        {
            bFind = strcasecmp(item.m_szKey, szKey)==0?1:0;
        }
        if (bFind)
        {
            uiOldKeyLen = (CWX_UINT32)len;
            if (*uiMsgLen - uiOldKeyLen + uiNewKeyLen > uiMaxMsgLen) return -2;
            memmove(szMsg + uiPos + uiNewKeyLen, szMsg+uiPos+len, *uiMsgLen-uiPos-len);
            cwx_pg_append_key(szMsg + uiPos, uiNewKeyLen, szKey, szData, uiDataLen, bKeyValue);
            *uiMsgLen = *uiMsgLen - uiOldKeyLen + uiNewKeyLen;
            return 1;
        }
        uiPos += len;
    }
    return 0;
}

int  cwx_pg_modify_key_by_index(char *szMsg,
                                     CWX_UINT32* uiMsgLen,
                                     CWX_UINT32 uiMaxMsgLen,
                                     CWX_UINT16 unIndex,
                                     char const* szData,
                                     CWX_UINT32 uiDataLen,
                                     int bKeyValue)
{
    CWX_UINT32 uiNewKeyLen = 0;
    CWX_UINT32 uiOldKeyLen = 0;
    CWX_UINT32 uiPos = 0;
    int len=0;
    CWX_UINT32 i = 0;
    CWX_KEY_VALUE_ITEM_S item;
    for (i=0; i<unIndex; i++)
    {
        len = cwx_pg_get_next_key(szMsg + uiPos, *uiMsgLen - uiPos, &item);
        if (-1 == len) return -1;
        if (0 == len) return 0;
        uiPos += len;
    }
    uiNewKeyLen = cwx_pg_get_kv_len(item.m_unKeyLen, uiDataLen);
    //find 
    uiOldKeyLen = (CWX_UINT32)len;
    if (*uiMsgLen - uiOldKeyLen + uiNewKeyLen > uiMaxMsgLen) return -2;
    memmove(szMsg + uiPos + uiNewKeyLen, szMsg+uiPos+len, *uiMsgLen-uiPos-len);
    cwx_pg_append_key(szMsg + uiPos, uiNewKeyLen, item.m_szKey, szData, uiDataLen, bKeyValue);
    *uiMsgLen = *uiMsgLen - uiOldKeyLen + uiNewKeyLen;
    return 1;

}

int cwx_pg_is_valid(char const *szMsg, CWX_UINT32 uiMsgLen)
{
    CWX_UINT32 uiPos = 0;
    int len=0;
    CWX_KEY_VALUE_ITEM_S item;
    for ( ; uiPos<uiMsgLen ; )
    {
        len = cwx_pg_get_next_key(szMsg + uiPos, uiMsgLen - uiPos, &item);
        if (-1 == len) return 0;
        if (0 == len) return 1;
        uiPos += len;
    }
    return 1;
}


int cwx_pg_get_key_num(char const* szMsg, CWX_UINT32 uiMsgLen)
{
    int iKeyNum = 0;
    CWX_UINT32 uiPos = 0;
    int len=0;
    CWX_KEY_VALUE_ITEM_S item;
    for ( ; uiPos<uiMsgLen ; )
    {
        len = cwx_pg_get_next_key(szMsg + uiPos, uiMsgLen - uiPos, &item);
        if (-1 == len) return -1;
        if (0 == len) return iKeyNum;
        uiPos += len;
        iKeyNum++;
    }
    return iKeyNum;
}



CWX_UINT32 cwx_pg_get_kv_len(CWX_UINT16 unKeyLen, CWX_UINT32 uiDataLen)
{
    return 8 + unKeyLen + uiDataLen ;
}

CWX_UINT32 cwx_pg_get_value_len(CWX_UINT32 uiKeyValueLen, CWX_UINT16 unKeyLen)
{
    return uiKeyValueLen - unKeyLen - 8;
}

CWX_UINT16 cwx_pg_get_key_offset()
{
    return 6;
}

#ifdef __cplusplus
}
#endif
