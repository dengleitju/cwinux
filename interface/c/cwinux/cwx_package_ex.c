#include "cwx_package_ex.h"

#ifdef __cplusplus
extern "C" {
#endif

int cwx_pg_get_next_key_ex(char const* szMsg,
                             CWX_UINT32 uiMsgLen,
                             CWX_KEY_VALUE_ITEM_S* item)
{
    CWX_UINT32 uiLeftLen = uiMsgLen;
    CWX_UINT32 byte4 = 0;
    CWX_UINT16 byte2 = 0;
    int    bKeyValue = 0;
    unsigned char const* pos = (unsigned char*)szMsg;
    if (0 == uiLeftLen) return 0;
    ///data_len|key_len|key_value|key|\0|value\0
    ///get data len
    pos = cwx_pg_decode_uint32(pos, &uiLeftLen, &byte4);
    if (!pos) return -1;
    if (byte4 > uiMsgLen) return -1;//bad msg data package.
    //get key len
    pos = cwx_pg_decode_uint16(pos, &uiLeftLen, &byte2);
    if (!pos) return -1;
    bKeyValue = pos[0];
    pos++;
    item->m_unKeyLen = byte2;
    item->m_szKey = (char*)pos;
    if (item->m_szKey[byte2]) return -1;//bad msg data package.
    //get value len
    item->m_uiDataLen = byte4;
    item->m_szData = item->m_szKey +  item->m_unKeyLen + 1;
    item->m_bKeyValue = bKeyValue;
    if (item->m_szData[byte4]) return -1;
    return cwx_pg_get_kv_len_ex(byte2, byte4);
}

int cwx_pg_get_key_by_index_ex(char const *szMsg,
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
        len = cwx_pg_get_next_key_ex(szMsg + uiPos, uiMsgLen - uiPos, item);
        if (-1 == len) return -1;
        if (0 == len) return 0;
        uiPos += len;
    }
    return len;
}

int cwx_pg_get_key_by_name_ex(char const *szMsg,
                                CWX_UINT32 uiMsgLen,
                                char const* szKey,
                                CWX_KEY_VALUE_ITEM_S* item,
                                int bCaseSensive)
{
    CWX_UINT32 uiPos = 0;
    int len=0;
    for ( ; uiPos<uiMsgLen; )
    {
        len = cwx_pg_get_next_key_ex(szMsg + uiPos, uiMsgLen - uiPos, item);
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

int cwx_pg_append_key_ex(char *szMsg,
                           CWX_UINT32 uiMsgLen,
                           char const* szKey,
                           char const* szValue,
                           CWX_UINT32 uiDatalen, 
                           int bKeyValue)
{
    ///data_len|key_len|key_value|key|\0|value\0
    CWX_UINT16 key_len = strlen(szKey);
    CWX_UINT32 byte4;
    CWX_UINT32 pos=0;
    CWX_UINT32 len=0;
    byte4 = cwx_pg_get_kv_len_ex(key_len, uiDatalen);
    if (byte4 > uiMsgLen) return -1;
    if (byte4 > CWX_PACKAGE_MAX_KV_LEN) return -1;
    cwx_pg_encode_uint32(uiDatalen, (unsigned char*)szMsg,&len); pos +=len;
    //append key len
    cwx_pg_encode_uint16(key_len, (unsigned char*)(szMsg + pos),&len); pos +=len;
    //add key value sign
    szMsg[pos++]=bKeyValue?1:0;
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

int cwx_pg_remove_key_ex(char *szMsg,
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
        len = cwx_pg_get_next_key_ex(szMsg + uiPos, *uiMsgLen - uiPos, &item);
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

int  cwx_pg_remove_key_by_index_ex(char *szMsg,
                                     CWX_UINT32* uiMsgLen,
                                     CWX_UINT16 unIndex)
{
    CWX_UINT32 i;
    CWX_UINT32 uiPos = 0;
    int len=0;
    CWX_KEY_VALUE_ITEM_S item;
    for ( i=0; i<=unIndex; i++)
    {
        len = cwx_pg_get_next_key_ex(szMsg + uiPos, *uiMsgLen - uiPos, &item);
        if (-1 == len) return -1;
        if (0 == len) return 0;
        uiPos += len;
    }
    memmove(szMsg + uiPos - len, szMsg+uiPos, *uiMsgLen-uiPos);
    *uiMsgLen -= len;
    return 1;
}

int  cwx_pg_modify_key_ex(char *szMsg,
                            CWX_UINT32* uiMsgLen,
                            CWX_UINT32 uiMaxMsgLen,
                            char const* szKey,
                            char const* szData,
                            CWX_UINT32 uiDataLen,
                            int bKeyValue,
                            int bCaseSensive)
{
    CWX_UINT32 uiNewKeyLen = cwx_pg_get_kv_len_ex(strlen(szKey), uiDataLen);
    CWX_UINT32 uiOldKeyLen = 0;
    CWX_UINT32 uiPos = 0;
    int len=0;
    CWX_KEY_VALUE_ITEM_S item;
    int bFind = 0;
    for ( ; uiPos<*uiMsgLen ; )
    {
        len = cwx_pg_get_next_key_ex(szMsg + uiPos, *uiMsgLen - uiPos, &item);
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
            cwx_pg_append_key_ex(szMsg + uiPos, uiNewKeyLen, szKey, szData, uiDataLen, bKeyValue);
            *uiMsgLen = *uiMsgLen - uiOldKeyLen + uiNewKeyLen;
            return 1;
        }
        uiPos += len;
    }
    return 0;
}

int  cwx_pg_modify_key_by_index_ex(char *szMsg,
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
        len = cwx_pg_get_next_key_ex(szMsg + uiPos, *uiMsgLen - uiPos, &item);
        if (-1 == len) return -1;
        if (0 == len) return 0;
        uiPos += len;
    }
    uiNewKeyLen = cwx_pg_get_kv_len_ex(item.m_unKeyLen, uiDataLen);
    //find 
    uiOldKeyLen = (CWX_UINT32)len;
    if (*uiMsgLen - uiOldKeyLen + uiNewKeyLen > uiMaxMsgLen) return -2;
    memmove(szMsg + uiPos + uiNewKeyLen, szMsg+uiPos+len, *uiMsgLen-uiPos-len);
    cwx_pg_append_key_ex(szMsg + uiPos, uiNewKeyLen, item.m_szKey, szData, uiDataLen, bKeyValue);
    *uiMsgLen = *uiMsgLen - uiOldKeyLen + uiNewKeyLen;
    return 1;

}

int cwx_pg_is_valid_ex(char const *szMsg, CWX_UINT32 uiMsgLen)
{
    CWX_UINT32 uiPos = 0;
    int len=0;
    CWX_KEY_VALUE_ITEM_S item;
    for ( ; uiPos<uiMsgLen ; )
    {
        len = cwx_pg_get_next_key_ex(szMsg + uiPos, uiMsgLen - uiPos, &item);
        if (-1 == len) return 0;
        if (0 == len) return 1;
        uiPos += len;
    }
    return 1;
}


int cwx_pg_get_key_num_ex(char const* szMsg, CWX_UINT32 uiMsgLen)
{
    int iKeyNum = 0;
    CWX_UINT32 uiPos = 0;
    int len=0;
    CWX_KEY_VALUE_ITEM_S item;
    for ( ; uiPos<uiMsgLen ; )
    {
        len = cwx_pg_get_next_key_ex(szMsg + uiPos, uiMsgLen - uiPos, &item);
        if (-1 == len) return -1;
        if (0 == len) return iKeyNum;
        uiPos += len;
        iKeyNum++;
    }
    return iKeyNum;
}



CWX_UINT32 cwx_pg_get_kv_len_ex(CWX_UINT16 unKeyLen, CWX_UINT32 uiDataLen)
{
    ///data_len|key_len|key_value|key|\0|value\0
    return cwx_pg_encode_uint32_size(uiDataLen) + cwx_pg_encode_uint16_size(unKeyLen) + 1 + unKeyLen + 1 + uiDataLen + 1 ;
}

CWX_UINT16 cwx_pg_get_key_offset_ex(CWX_UINT16 unKeyLen, CWX_UINT32 uiDataLen){
    return cwx_pg_encode_uint16_size(unKeyLen) + cwx_pg_encode_uint32_size(uiDataLen) + 1;
}

///获得对UINT16的value进行encode后的长度
CWX_UINT8 cwx_pg_encode_uint16_size(CWX_UINT16 value){
    if ((value>>14)&0x03) return 3;
    if ((value>>7)&0x7F) return 2;
    return 1;
}

///对UINT16整数进行解码，返回解码结束的位置，若为NULL表示解码失败
unsigned char const* cwx_pg_decode_uint16(unsigned char const* szBuf, ///<
                                          CWX_UINT32* uiBufLen, ///<传入buf的空间大小，返回剩余的大小
                                          CWX_UINT16* value ///<解码出的数值
                                          )
{
    CWX_UINT8 i=0, b=0;
    CWX_UINT16 d=0;
    *value = 0;
    while(i<3){
        b = *(szBuf++);
        d = b&0x7F;
        *value |= (d <<=(7*i++));
        if (!(b & 0x80)) break;
    }
    if ((3==i) && (b & 0x80)) return NULL;
    if (*uiBufLen < i) return NULL;
    *uiBufLen -= i;
    return szBuf;
}

///对整数进行编码，返回编码的值及编码后的长度
unsigned char* cwx_pg_encode_uint16(CWX_UINT16 value, ///<要encode的值
                                    unsigned char* szBuf, ///<空间
                                    CWX_UINT32* uiLen ///<encode后的长度
                                    )
{
    szBuf[0] = (value|0x80);
    if (value >= (1 << 7)) {
        szBuf[1] = ((value >>  7) | 0x80);
        if (value >= (1 << 14)) {
            szBuf[2] = (value >> 14);
            szBuf[2] &= 0x7F; *uiLen=3;
            return szBuf;
        }
        szBuf[1] &= 0x7F; *uiLen=2;
        return szBuf;
    }
    szBuf[0] &= 0x7F; *uiLen=1;
    return szBuf;
}


///获得对UINT32的value进行encode后的长度
CWX_UINT8 cwx_pg_encode_uint32_size(CWX_UINT32 value){
    if ((value>>28)&0x0F) return 5;
    if ((value>>21)&0x7F) return 4;
    if ((value>>14)&0x7F) return 3;
    if ((value>>7)&0x7F) return 2;
    return 1;
}

///对UINT32整数进行解码，返回解码结束的位置，若为NULL表示解码失败
unsigned char const* cwx_pg_decode_uint32(unsigned char const* szBuf, ///<
                                          CWX_UINT32* uiBufLen, ///<传入buf的空间大小，返回剩余的大小
                                          CWX_UINT32* value ///<解码出的数值
                                          )
{
    CWX_UINT8 i=0, b=0;
    CWX_UINT32 d=0;
    *value = 0;
    while(i<5){
        b = *(szBuf++);
        d = b&0x7F;
        *value |= (d <<=(7*i++));
        if (!(b & 0x80)) break;
    }
    if ((5==i) && (b & 0x80)) return NULL;
    if (*uiBufLen < i) return NULL;
    *uiBufLen -= i;
    return szBuf;
}

///对整数进行编码，返回编码的值及编码后的长度
unsigned char* cwx_pg_encode_uint32(CWX_UINT32 value, ///<要encode的值
                                    unsigned char* szBuf, ///<空间
                                    CWX_UINT32* uiLen ///<encode后的长度
                                    )
{
    szBuf[0] = (value|0x80);
    if (value >= (1 << 7)) {
        szBuf[1] = ((value >>  7) | 0x80);
        if (value >= (1 << 14)) {
            szBuf[2] = ((value >> 14) | 0x80);
            if (value >= (1 << 21)) {
                szBuf[3] = ((value >> 21) | 0x80);
                if (value >= (1 << 28)) {
                    szBuf[4] = (value >> 28);
                    *uiLen=5;
                    return szBuf; 
                }
                szBuf[3] &= 0x7F; *uiLen=4;
                return szBuf;
            }
            szBuf[2] &= 0x7F; *uiLen=3;
            return szBuf;
        }
        szBuf[1] &= 0x7F; *uiLen=2;
        return szBuf;
    }
    szBuf[0] &= 0x7F; *uiLen=1;
    return szBuf;
}

///获得对UINT64的value进行encode后的长度
CWX_UINT8 cwx_pg_encode_uint64_size(CWX_UINT64 value){
    if ((value>>63)&0x01) return 10;
    if ((value>>56)&0x7F) return 9;
    if ((value>>49)&0x7F) return 8;
    if ((value>>42)&0x7F) return 7;
    if ((value>>35)&0x7F) return 6;
    if ((value>>28)&0x7F) return 5;
    if ((value>>21)&0x7F) return 4;
    if ((value>>14)&0x7F) return 3;
    if ((value>>7)&0x7F) return 2;
    return 1;

}

///对UINT64整数进行解码，返回解码结束的位置，若为NULL表示解码失败
unsigned char const* cwx_pg_decode_uint64(unsigned char const* szBuf, ///<
                                          CWX_UINT32* uiBufLen, ///<传入buf的空间大小，返回剩余的大小
                                          CWX_UINT64* value ///<解码出的数值
                                          )
{
    CWX_UINT8 i=0, b=0;
    CWX_UINT64 d=0;
    *value = 0;
    while(i<10){
        b = *(szBuf++);
        d = b&0x7F;
        *value |= (d <<=(7*i++));
        if (!(b & 0x80)) break;
    }
    if ((10==i) && (b & 0x80)) return NULL;
    if (*uiBufLen < i) return NULL;
    *uiBufLen -= i;
    return szBuf;
}

///对UINT64整数进行编码，返回编码的值及编码后的长度
unsigned char* cwx_pg_encode_uint64(CWX_UINT64 value, ///<要encode的值
                                    unsigned char* szBuf, ///<空间
                                    CWX_UINT32* uiLen ///<encode后的长度
                                    )
{
    CWX_UINT32 part0 = (value);
    CWX_UINT32 part1 = (value >> 28);
    CWX_UINT32 part2 = (value >> 56);
    if (part2 == 0) {
        if (part1 == 0) {
            if (part0 < (1 << 14)) {
                if (part0 < (1 << 7)) {
                    *uiLen = 1; goto size1;
                } else {
                    *uiLen = 2; goto size2;
                }
            } else {
                if (part0 < (1 << 21)) {
                    *uiLen = 3; goto size3;
                } else {
                    *uiLen = 4; goto size4;
                }
            }
        } else {
            if (part1 < (1 << 14)) {
                if (part1 < (1 << 7)) {
                    *uiLen = 5; goto size5;
                } else {
                    *uiLen = 6; goto size6;
                }
            } else {
                if (part1 < (1 << 21)) {
                    *uiLen = 7; goto size7;
                } else {
                    *uiLen = 8; goto size8;
                }
            }
        }
    } else {
        if (part2 < (1 << 7)) {
            *uiLen = 9; goto size9;
        } else {
            *uiLen = 10; goto size10;
        }
    }
size10: szBuf[9] = ((part2 >>  7) | 0x80);
size9 : szBuf[8] = ((part2      ) | 0x80);
size8 : szBuf[7] = ((part1 >> 21) | 0x80);
size7 : szBuf[6] = ((part1 >> 14) | 0x80);
size6 : szBuf[5] = ((part1 >>  7) | 0x80);
size5 : szBuf[4] = ((part1      ) | 0x80);
size4 : szBuf[3] = ((part0 >> 21) | 0x80);
size3 : szBuf[2] = ((part0 >> 14) | 0x80);
size2 : szBuf[1] = ((part0 >>  7) | 0x80);
size1 : szBuf[0] = ((part0      ) | 0x80); 
    return szBuf;
}
#ifdef __cplusplus
}
#endif
