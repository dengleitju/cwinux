#include "cwx_package_reader_ex.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CWX_PG_READER_ERR_MSG_LEN 511
struct CWX_PG_READER_EX
{
    CWX_KEY_VALUE_ITEM_S*        m_pKeyValue; ///<key/value's vector
    CWX_UINT32                   m_uiTotalKeyNum;///m_pKeyValue's array size
    CWX_UINT32                   m_uiKeyNum; ///<package's key number
    char const*                  m_szPackMsg; ///<package's dat
    CWX_UINT32                   m_uiPackBufLen;///<package's buf length
    CWX_INT8                     m_bIndex;///<是否建立package的索引
    CWX_INT8                     m_bCaseSensive;
    CWX_KEY_VALUE_ITEM_S         m_tmpKey; ///<临时key
    char                         m_szErr[CWX_PG_READER_ERR_MSG_LEN + 1];///<错误内容
};

static void reset(struct CWX_PG_READER_EX* reader)
{
    if (reader)
    {
        if (reader->m_pKeyValue)
        {
            memset(reader->m_pKeyValue, 0x00, sizeof(CWX_KEY_VALUE_ITEM_S) * reader->m_uiTotalKeyNum);
        }
        reader->m_uiKeyNum = 0;
        reader->m_szPackMsg = 0;
        reader->m_uiPackBufLen = 0;
        reader->m_bIndex = 0;
        reader->m_bCaseSensive = 0;
        memset(reader->m_szErr, 0x00, CWX_PG_READER_ERR_MSG_LEN+1);
    }
}

struct CWX_PG_READER_EX* cwx_pg_reader_create_ex()
{
    struct CWX_PG_READER_EX* reader = (struct CWX_PG_READER_EX*)malloc(sizeof(struct CWX_PG_READER_EX));
    if (!reader) return 0;
    reader->m_pKeyValue = 0;
    reader->m_uiTotalKeyNum = 0;
    reader->m_uiKeyNum = 0;
    reader->m_szPackMsg = 0;
    reader->m_uiPackBufLen = 0;
    reader->m_bIndex = 0;
    reader->m_bCaseSensive = 0;
    memset(reader->m_szErr, 0x00, CWX_PG_READER_ERR_MSG_LEN+1);
    return reader;
}

void cwx_pg_reader_destory_ex(struct CWX_PG_READER_EX* reader)
{
    if(reader)
    {
        if (reader->m_pKeyValue) free(reader->m_pKeyValue);
        free(reader);
    }
}

int cwx_pg_reader_unpack_ex(struct CWX_PG_READER_EX* reader,
                         char const* szMsg,
                         CWX_UINT32 uiMsgLen,
                         int bindex,
                         int bCaseSensive)
{
    int  iRet = 0;
    reset(reader);
    reader->m_uiPackBufLen = uiMsgLen;
    reader->m_szPackMsg = szMsg;
    reader->m_bIndex = bindex;
    reader->m_bCaseSensive = bCaseSensive;
    if (0 == uiMsgLen) return 0;
    iRet = cwx_pg_get_key_num_ex(szMsg, uiMsgLen);
    if (0 > iRet)
    {
        strcpy(reader->m_szErr, "It's not a valid package");
        return -1;
    }
    reader->m_uiKeyNum = iRet;
    if (reader->m_uiTotalKeyNum < reader->m_uiKeyNum)
    {
        if (reader->m_pKeyValue) free(reader->m_pKeyValue);
        reader->m_uiTotalKeyNum = (reader->m_uiKeyNum + 16 -1)/16;
        reader->m_uiTotalKeyNum *= 16;
        reader->m_pKeyValue = (CWX_KEY_VALUE_ITEM_S*)malloc(sizeof(CWX_KEY_VALUE_ITEM_S) * reader->m_uiTotalKeyNum);
        memset(reader->m_pKeyValue, 0x00, sizeof(CWX_KEY_VALUE_ITEM_S) * reader->m_uiTotalKeyNum);
    }
    CWX_UINT32 uiPos = 0;
    CWX_UINT32 uiIndex = 0;
    while(uiPos < reader->m_uiPackBufLen)
    {
        if (uiIndex == reader->m_uiKeyNum)
        {
            snprintf(reader->m_szErr, CWX_PG_READER_ERR_MSG_LEN, "The package should be kv[%u], but exceed.", reader->m_uiKeyNum);
            return -1;
        }
        if (-1 == (iRet = cwx_pg_get_next_key_ex(reader->m_szPackMsg + uiPos,
            reader->m_uiPackBufLen - uiPos,
            &reader->m_pKeyValue[uiIndex])))
        {
            snprintf(reader->m_szErr, CWX_PG_READER_ERR_MSG_LEN, "Can't get the %dth key/value", uiIndex);
            return -1;
        }
        if (0 == iRet) break; //finish
        uiIndex ++;
        uiPos += iRet;
        if (uiPos == uiMsgLen) return 0; //finish.
    }
    return 0;
}

CWX_KEY_VALUE_ITEM_S const*  cwx_pg_reader_get_n_key_ex(struct CWX_PG_READER_EX const* reader,
                                                   char const* szKey,
                                                   CWX_UINT32  uiKeyLen,
                                                   int bSubKey)
{
    char const* pNextSub = 0;
    CWX_UINT32 i = 0;
    struct CWX_PG_READER_EX* local_reader = 0;
    CWX_KEY_VALUE_ITEM_S const* pItem = 0;
    if (bSubKey)
    {
        pNextSub = strchr(szKey, '.');
    }
    if (!bSubKey || !pNextSub)
    {
        for (i=0; i< reader->m_uiKeyNum; i++)
        {
            if (reader->m_bCaseSensive)
            {
                if (strncmp(szKey, reader->m_pKeyValue[i].m_szKey, uiKeyLen) == 0)
                {
                    return &reader->m_pKeyValue[i];
                }
            }
            else
            {
                if (strncasecmp(szKey, reader->m_pKeyValue[i].m_szKey, uiKeyLen) == 0)
                {
                    return &reader->m_pKeyValue[i];
                }
            }
        }
    }
    else
    {
        pNextSub++;
        pItem = cwx_pg_reader_get_n_key_ex(reader, szKey, szKey - pNextSub, 0);
        if (!pItem) return 0;
        if (!pItem->m_bKeyValue) return 0;
        local_reader = cwx_pg_reader_create_ex();
        if (0 != cwx_pg_reader_unpack_ex(local_reader, pItem->m_szData, pItem->m_uiDataLen, reader->m_bIndex, reader->m_bCaseSensive))
        {
            cwx_pg_reader_destory_ex(local_reader);
            return 0;
        }
        pItem = cwx_pg_reader_get_n_key_ex(local_reader, pNextSub, strlen(pNextSub), 1);
        if (pItem)
        {
            memcpy((void*)(&(reader->m_tmpKey)), pItem, sizeof(CWX_KEY_VALUE_ITEM_S));
            cwx_pg_reader_destory_ex(local_reader);
            return &reader->m_tmpKey;
        }
        cwx_pg_reader_destory_ex(local_reader);
        return 0;
    }
    return 0;
}


CWX_KEY_VALUE_ITEM_S const*  cwx_pg_reader_get_key_ex(struct CWX_PG_READER_EX const* reader,
                                                          char const* szKey,
                                                          int bSubKey)
{
    return cwx_pg_reader_get_n_key_ex(reader, szKey, strlen(szKey), bSubKey);
}

CWX_KEY_VALUE_ITEM_S const* cwx_pg_reader_get_key_by_index_ex(struct CWX_PG_READER_EX const* reader,
                                                                  CWX_UINT32 index)
{
    if (index >= reader->m_uiKeyNum) return NULL;
    return &reader->m_pKeyValue[index];
}

int cwx_pg_reader_get_uint64_ex(struct CWX_PG_READER_EX const* reader,
                                    char const* szKey,
                                    CWX_UINT64* value,
                                    int bSubKey)
{
    CWX_KEY_VALUE_ITEM_S const* item = cwx_pg_reader_get_key_ex(reader, szKey, bSubKey);
    if (item)
    {
        *value = strtoull(item->m_szData, NULL, 0);
        return 1;
    }
    return 0;
}

int cwx_pg_reader_get_int64_ex(struct CWX_PG_READER_EX const* reader,
                                   char const* szKey,
                                   CWX_INT64* value,
                                   int bSubKey)
{
    CWX_KEY_VALUE_ITEM_S const* item = cwx_pg_reader_get_key_ex(reader, szKey, bSubKey);
    if (item)
    {
        *value = strtoll(item->m_szData, NULL, 0);
        return 1;
    }
    return 0;
}

int cwx_pg_reader_get_uint32_ex(struct CWX_PG_READER_EX const* reader,
                                    char const* szKey,
                                    CWX_UINT32* value,
                                    int bSubKey)
{
    CWX_KEY_VALUE_ITEM_S const* item = cwx_pg_reader_get_key_ex(reader, szKey, bSubKey);
    if (item)
    {
        *value = strtoul(item->m_szData, NULL, 0);
        return 1;
    }
    return 0;
}

int cwx_pg_reader_get_int32_ex(struct CWX_PG_READER_EX const* reader,
                                   char const* szKey,
                                   CWX_INT32* value,
                                   int bSubKey)
{
    CWX_KEY_VALUE_ITEM_S const* item = cwx_pg_reader_get_key_ex(reader, szKey, bSubKey);
    if (item)
    {
        *value = strtol(item->m_szData, NULL, 0);
        return 1;
    }
    return 0;
}

int cwx_pg_reader_get_uint16_ex(struct CWX_PG_READER_EX const* reader,
                                    char const* szKey,
                                    CWX_UINT16* value,
                                    int bSubKey)
{
    CWX_KEY_VALUE_ITEM_S const* item = cwx_pg_reader_get_key_ex(reader, szKey, bSubKey);
    if (item)
    {
        *value = strtoul(item->m_szData, NULL, 0);
        return 1;
    }
    return 0;
}

int cwx_pg_reader_get_int16_ex(struct CWX_PG_READER_EX const* reader,
                                   char const* szKey,
                                   CWX_INT16* value,
                                   int bSubKey)
{
    CWX_KEY_VALUE_ITEM_S const* item = cwx_pg_reader_get_key_ex(reader, szKey, bSubKey);
    if (item)
    {
        *value = strtol(item->m_szData, NULL, 0);
        return 1;
    }
    return 0;
}

int cwx_pg_reader_get_uint8_ex(struct CWX_PG_READER_EX const* reader,
                                   char const* szKey,
                                   CWX_UINT8* value,
                                   int bSubKey)
{
    CWX_KEY_VALUE_ITEM_S const* item = cwx_pg_reader_get_key_ex(reader, szKey, bSubKey);
    if (item)
    {
        *value = strtoul(item->m_szData, NULL, 0);
        return 1;
    }
    return 0;
}

int cwx_pg_reader_get_int8_ex(struct CWX_PG_READER_EX const* reader,
                                  char const* szKey,
                                  CWX_INT8* value,
                                  int bSubKey)
{
    CWX_KEY_VALUE_ITEM_S const* item = cwx_pg_reader_get_key_ex(reader, szKey, bSubKey);
    if (item)
    {
        *value = strtol(item->m_szData, NULL, 0);
        return 1;
    }
    return 0;
}

CWX_UINT32 cwx_pg_reader_get_key_num_ex(struct CWX_PG_READER_EX const* reader)
{
    return reader->m_uiKeyNum;
}

///获取全部的key
CWX_KEY_VALUE_ITEM_S const* cwx_pg_reader_get_keys_ex(struct CWX_PG_READER_EX const* reader)
{
    return reader->m_pKeyValue;
}


CWX_UINT32 cwx_pg_reader_get_msg_size_ex(struct CWX_PG_READER_EX const* reader)
{
    return reader->m_uiPackBufLen;
}

char const* cwx_pg_reader_get_msg_ex(struct CWX_PG_READER_EX const* reader)
{
    return reader->m_szPackMsg;
}

char const* cwx_pg_reader_get_error_ex(struct CWX_PG_READER_EX const* reader)
{
    return reader->m_szErr;
}

int cwx_pg_reader_case_sensive_ex(struct CWX_PG_READER_EX const* reader)
{
    return reader->m_bCaseSensive;
}

int cwx_pg_reader_is_index_ex(struct CWX_PG_READER_EX const* reader)
{
    return reader->m_bIndex;
}



#ifdef __cplusplus
}
#endif
