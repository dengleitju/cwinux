#include "cwx_package_writer_ex.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CWX_PG_WRITER_ERR_MSG_LEN 511

struct CWX_PG_WRITER_EX
{
    CWX_UINT32            m_uiKeyNum; ///<package's key number
    char*                 m_szPackMsg; ///<package's dat
    CWX_UINT32            m_uiPackBufLen;///<package's buf length
    CWX_UINT32            m_uiCurPackPos;///<package's size
    char                  m_szErr[CWX_PG_WRITER_ERR_MSG_LEN + 1];///<错误内容
};

static void reset(struct CWX_PG_WRITER_EX* writer)
{
    writer->m_uiKeyNum = 0;
    writer->m_uiCurPackPos = 0;
    memset(writer->m_szErr, 0x00, CWX_PG_WRITER_ERR_MSG_LEN+1);

}

struct CWX_PG_WRITER_EX* cwx_pg_writer_create_ex(CWX_UINT32 uiBufLen)
{
    struct CWX_PG_WRITER_EX* writer = (struct CWX_PG_WRITER_EX*)malloc(sizeof(struct CWX_PG_WRITER_EX));
    if (writer)
    {
        writer->m_uiKeyNum = 0;
        if (uiBufLen < 128) uiBufLen = 128;
        writer->m_szPackMsg = (char*)malloc(uiBufLen);
        writer->m_uiPackBufLen = uiBufLen;
        writer->m_uiCurPackPos = 0;
        memset(writer->m_szErr, 0x00, CWX_PG_WRITER_ERR_MSG_LEN+1);
    }
    return writer;
}

void cwx_pg_writer_destory_ex(struct CWX_PG_WRITER_EX* writer)
{
    if (writer)
    {
        if (writer->m_szPackMsg)
        {
            free(writer->m_szPackMsg);
            writer->m_szPackMsg = 0;
            writer->m_uiPackBufLen = 0;
        }
        free(writer);
    }
}

void cwx_pg_writer_begin_pack_ex(struct CWX_PG_WRITER_EX* writer)
{
    reset(writer);
}

int cwx_pg_writer_add_key_ex(struct CWX_PG_WRITER_EX* writer,
                          char const* szKey,
                          char const* szData,
                          CWX_UINT32 uiDataLen,
                          int bKeyValue)
{
    if (writer->m_uiPackBufLen < writer->m_uiCurPackPos + cwx_pg_get_kv_len_ex(strlen(szKey), uiDataLen))
    {
        CWX_UINT32 uiNewSize = writer->m_uiPackBufLen * 2 + cwx_pg_get_kv_len_ex(strlen(szKey), uiDataLen);
        char* pNew = (char*)malloc(uiNewSize);
        if (!pNew)
        {
            snprintf(writer->m_szErr, CWX_PG_WRITER_ERR_MSG_LEN, "Failure to malloc buf, size=%u", uiNewSize);
            return -1;
        }
        if (writer->m_uiCurPackPos)
        {
            memcpy(pNew, writer->m_szPackMsg, writer->m_uiCurPackPos);
        }
        writer->m_uiPackBufLen = uiNewSize;
        free(writer->m_szPackMsg);
        writer->m_szPackMsg = pNew;
    }
    int iRet = 0;
    if (0 ==(iRet = cwx_pg_append_key_ex(writer->m_szPackMsg + writer->m_uiCurPackPos,
        writer->m_uiPackBufLen - writer->m_uiCurPackPos,
        szKey,
        szData,
        uiDataLen,
        bKeyValue)))
    {
        snprintf(writer->m_szErr, CWX_PG_WRITER_ERR_MSG_LEN, "No memory to append the key: %s", szKey);
        return -1;
    }
    writer->m_uiKeyNum ++;
    writer->m_uiCurPackPos += iRet;
    return 0;
}

int cwx_pg_writer_add_key_str_ex(struct CWX_PG_WRITER_EX* writer,
                              char const* szKey,
                              char const* szData)
{
    return cwx_pg_writer_add_key_ex(writer, szKey, szData, strlen(szData), 0);
}

int cwx_pg_writer_add_key_uint8_ex(struct CWX_PG_WRITER_EX* writer,
                                char const* szKey,
                                CWX_UINT8 ucData)
{
    char buf[32];
    sprintf(buf, "%u", ucData);
    return cwx_pg_writer_add_key_str_ex(writer, szKey, buf);
}

int cwx_pg_writer_add_key_int8_ex(struct CWX_PG_WRITER_EX* writer,
                               char const* szKey,
                               CWX_INT8 cData)
{
    char buf[32];
    sprintf(buf, "%c", cData);
    return cwx_pg_writer_add_key_str_ex(writer, szKey, buf);
}

int cwx_pg_writer_add_key_uint16_ex(struct CWX_PG_WRITER_EX* writer,
                                 char const* szKey,
                                 CWX_UINT16 unData)
{
    char buf[32];
    sprintf(buf, "%u", unData);
    return cwx_pg_writer_add_key_str_ex(writer, szKey, buf);
}


int cwx_pg_writer_add_key_int16_ex(struct CWX_PG_WRITER_EX* writer,
                                char const* szKey,
                                CWX_INT16 nData)
{
    char buf[32];
    sprintf(buf, "%d", nData);
    return cwx_pg_writer_add_key_str_ex(writer, szKey, buf);
}

int cwx_pg_writer_add_key_uint32_ex(struct CWX_PG_WRITER_EX* writer,
                                 char const* szKey,
                                 CWX_UINT32 uiData)
{
    char buf[32];
    sprintf(buf, "%u", uiData);
    return cwx_pg_writer_add_key_str_ex(writer, szKey, buf);
}

int cwx_pg_writer_add_key_int32_ex(struct CWX_PG_WRITER_EX* writer,
                                char const* szKey,
                                CWX_INT32 iData)
{
    char buf[32];
    sprintf(buf, "%d", iData);
    return cwx_pg_writer_add_key_str_ex(writer, szKey, buf);
}

int cwx_pg_writer_add_key_uint64_ex(struct CWX_PG_WRITER_EX* writer,
                                 char const* szKey,
                                 CWX_UINT64 ullData)
{
    char buf[64];
    sprintf(buf, "%"PRIu64, ullData);
    return cwx_pg_writer_add_key_str_ex(writer, szKey, buf);
}

int cwx_pg_writer_add_key_int64_ex(struct CWX_PG_WRITER_EX* writer,
                                char const* szKey,
                                CWX_INT64 llData)
{
    char buf[64];
    sprintf(buf, "%"PRId64, llData);
    return cwx_pg_writer_add_key_str_ex(writer, szKey, buf);
}

int cwx_pg_writer_pack_ex(struct CWX_PG_WRITER_EX* writer)
{
    writer=0;
    return 0;
}

///获取当前package的Key的数量。
CWX_UINT32 cwx_pg_writer_get_key_num_ex(struct CWX_PG_WRITER_EX* writer)
{
    return writer->m_uiKeyNum;
}
///获取形成的package的size
CWX_UINT32 cwx_pg_writer_get_msg_size_ex(struct CWX_PG_WRITER_EX* writer)
{
    return writer->m_uiCurPackPos;
}
///获取形成的package
char const* cwx_pg_writer_get_msg_ex(struct CWX_PG_WRITER_EX* writer)
{
    return writer->m_szPackMsg;
}
///获取错误消息
char const* cwx_pg_writer_get_error_ex(struct CWX_PG_WRITER_EX* writer)
{
    return writer->m_szErr;
}



#ifdef __cplusplus
}
#endif
