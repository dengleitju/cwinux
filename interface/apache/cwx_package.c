#include "cwx_package.h"

int cwx_get_next_key(char const* szMsg, CWX_UINT32 uiMsgLen, CWX_KEY_VALUE_ITEM_S* item){
    CWX_UINT32 byte4 = 0;
    CWX_UINT16 byte2 = 0;
    bool    bKeyValue = false;
    if (0 == uiMsgLen) return 0;
    memcpy(&byte4, szMsg, 4); //fetch key/value head
    byte4 = ntohl(byte4);
    bKeyValue = ((byte4 & (1<<CWX_PACKAGE_KV_BIT))>0)?true:false;
    byte4 = byte4 & CWX_MAX_KV_LEN;
    if (byte4 > uiMsgLen) return -1;//bad msg data package.
    //get key len
    memcpy(&byte2, szMsg + 4, 2);
    byte2 = ntohs(byte2);
    item->m_unKeyLen = byte2;
    item->m_szKey = (char*)(szMsg + 6);
    if ((byte2>=byte4) || (0 != szMsg[6+byte2])) return -1;//bad msg data package.
    //get value len
    item->m_uiDataLen = cwx_get_data_len(byte4, item->m_unKeyLen);
    item->m_szData = item->m_szKey + item->m_unKeyLen + 1;
    item->m_bKeyValue = bKeyValue;
    return (int)byte4;
}

int cwx_append_key(char *szMsg, CWX_UINT32 uiMsgLen, char const* szKey, char const* szValue, CWX_UINT32 uiDatalen, bool bKeyValue){
    CWX_UINT16 key_len = strlen(szKey);
    CWX_UINT32 byte4;
    CWX_UINT16 byte2;
    CWX_UINT32 pos=0;
    byte4 = cwx_get_kv_len(key_len , uiDatalen);
    if (byte4 > uiMsgLen) return -1;
    if (byte4 > CWX_MAX_KV_LEN) return -1;
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
    if (uiDatalen){
        memcpy(szMsg + pos, szValue, uiDatalen);pos += uiDatalen;
    }
    szMsg[pos] = 0x00;
    pos += 1;
    return (int)pos;
}
int cwx_get_key_by_index(char const *szMsg, CWX_UINT32 uiMsgLen, CWX_UINT32 uiIndex, CWX_KEY_VALUE_ITEM_S* item){
    CWX_UINT32 uiPos = 0;
    CWX_UINT32 i;
    int len=0;
    if (!uiMsgLen) return 0;
    for (i=0; i<=uiIndex; i++){
        len = cwx_get_next_key(szMsg + uiPos, uiMsgLen - uiPos, item);
        if (-1 == len) return -1;
        if (0 == len) return 0;
        uiPos += len;
    }
    return len;
}

int cwx_get_key_by_name(char const *szMsg, CWX_UINT32 uiMsgLen, char const* szKey, CWX_KEY_VALUE_ITEM_S* item){
    CWX_UINT32 uiPos = 0;
    int len=0;
    for ( ; uiPos<uiMsgLen; ){
        len = cwx_get_next_key(szMsg + uiPos, uiMsgLen - uiPos, item);
        if (-1 == len) return -1;
        if (0 == len) return 0;
        if (strcasecmp(item->m_szKey, szKey)==0) return len;
        uiPos += len;
    }
    return 0;
}


int cwx_get_key_value_num(char const* szMsg, CWX_UINT32 uiMsgLen){
    int iKeyNum = 0;
    CWX_UINT32 uiPos = 0;
    int len=0;
    CWX_KEY_VALUE_ITEM_S item;
    for ( ; uiPos<uiMsgLen ; ){
        len = cwx_get_next_key(szMsg + uiPos, uiMsgLen - uiPos, &item);
        if (-1 == len) return -1;
        if (0 == len) return iKeyNum;
        uiPos += len;
        iKeyNum++;
    }
    return iKeyNum;
}

bool cwx_is_valid_package(char const *szMsg, CWX_UINT32 uiMsgLen){
    CWX_UINT32 uiPos = 0;
    int len=0;
    CWX_KEY_VALUE_ITEM_S item;
    for ( ; uiPos<uiMsgLen ; ){
        len = cwx_get_next_key(szMsg + uiPos, uiMsgLen - uiPos, &item);
        if (-1 == len) return false;
        if (0 == len) return true;
        uiPos += len;
    }
    return true;
}


CWX_UINT32 cwx_get_kv_len(CWX_UINT16 unKeyLen, CWX_UINT32 uiDataLen){
    return 8 + unKeyLen + uiDataLen ;
}

CWX_UINT32 cwx_get_data_len(CWX_UINT32 uiKeyValueLen, CWX_UINT16 unKeyLen){
    return uiKeyValueLen - unKeyLen - 8;
}
