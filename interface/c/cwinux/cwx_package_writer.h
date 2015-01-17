#ifndef __CWX_PACKAGE_WRITER_H__
#define __CWX_PACKAGE_WRITER_H__
/*
版权声明：
    本软件遵循GNU GPL V3（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/

/**
@file cwx_package.h
@brief 定义key/value数据包的操作
@author cwinux@gmail.com
@version 1.0
@date 2010-10-04
@warning
@bug
*/
#include "cwx_package.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CWX_PG_WRITER;

/**
*@brief 创建package writer的对象。
*@param [in] uiBufLen 初始package的buf空间。
*@return 0：创建失败；否则：创建的writer 对象。
*/
struct CWX_PG_WRITER* cwx_pg_writer_create(CWX_UINT32 uiBufLen);

/**
*@brief 释放package writer的对象。
*@return void。
*/
void cwx_pg_writer_destory(struct CWX_PG_WRITER* writer);

/**
*@brief 开始pack一个package。
*@return void。
*/
void cwx_pg_writer_begin_pack(struct CWX_PG_WRITER* writer);
/**
*@brief 往package中添加一个值为buf的key。
*@param [in] writer package的writer。
*@param [in] szKey key的名字。
*@param [in] szData key的value。
*@param [in] uiDataLen data的长度，可以为0。
*@param [in] bKeyValue data是否为key/value。1：是；0：不是。
*@return -1:错误；0:正确。通过cwx_pg_writer_error()获取失败的原因。
*/
int cwx_pg_writer_add_key(struct CWX_PG_WRITER* writer,
                           char const* szKey,
                           char const* szData,
                           CWX_UINT32 uiDataLen,
                           int bKeyValue);
/**
*@brief 往package中添加一个值为string的key。
*@param [in] writer package的writer。
*@param [in] szKey key的名字。
*@param [in] szData key的value。
*@return -1:错误；0:正确。通过cwx_pg_writer_error()获取失败的原因。
*/
int cwx_pg_writer_add_key_str(struct CWX_PG_WRITER* writer,
                              char const* szKey,
                              char const* szData);
/**
*@brief 往package中添加一个值为UINT8的key。
*@param [in] writer package的writer。
*@param [in] szKey key的名字。
*@param [in] ucData key的value。
*@return -1:错误；0:正确。通过cwx_pg_writer_error()获取失败的原因。
*/
int cwx_pg_writer_add_key_uint8(struct CWX_PG_WRITER* writer,
                                char const* szKey,
                                CWX_UINT8 ucData);
/**
*@brief 往package中添加一个值为INT8的key。
*@param [in] writer package的writer。
*@param [in] szKey key的名字。
*@param [in] cData key的value。
*@return -1:错误；0:正确。通过cwx_pg_writer_error()获取失败的原因。
*/
int cwx_pg_writer_add_key_int8(struct CWX_PG_WRITER* writer,
                             char const* szKey,
                             CWX_INT8 cData);
/**
*@brief 往package中添加一个值为UINT16的key。
*@param [in] writer package的writer。
*@param [in] szKey key的名字。
*@param [in] unData key的value。
*@return -1:错误；0:正确。通过cwx_pg_writer_error()获取失败的原因。
*/
int cwx_pg_writer_add_key_uint16(struct CWX_PG_WRITER* writer,
                                char const* szKey,
                                CWX_UINT16 unData);
/**
*@brief 往package中添加一个值为INT16的key。
*@param [in] writer package的writer。
*@param [in] szKey key的名字。
*@param [in] nData key的value。
*@return -1:错误；0:正确。通过cwx_pg_writer_error()获取失败的原因。
*/
int cwx_pg_writer_add_key_int16(struct CWX_PG_WRITER* writer,
                               char const* szKey,
                               CWX_INT16 nData);

/**
*@brief 往package中添加一个值为UINT32的key。
*@param [in] writer package的writer。
*@param [in] szKey key的名字。
*@param [in] uiData key的value。
*@return -1:错误；0:正确。通过cwx_pg_writer_error()获取失败的原因。
*/
int cwx_pg_writer_add_key_uint32(struct CWX_PG_WRITER* writer,
                                char const* szKey,
                                CWX_UINT32 uiData);
/**
*@brief 往package中添加一个值为INT32的key。
*@param [in] writer package的writer。
*@param [in] szKey key的名字。
*@param [in] iData key的value。
*@return -1:错误；0:正确。通过cwx_pg_writer_error()获取失败的原因。
*/
int cwx_pg_writer_add_key_int32(struct CWX_PG_WRITER* writer,
                               char const* szKey,
                               CWX_INT32 iData);

/**
*@brief 往package中添加一个值为UINT64的key。
*@param [in] writer package的writer。
*@param [in] szKey key的名字。
*@param [in] ullData key的value。
*@return -1:错误；0:正确。通过cwx_pg_writer_error()获取失败的原因。
*/
int cwx_pg_writer_add_key_uint64(struct CWX_PG_WRITER* writer,
                                char const* szKey,
                                CWX_UINT64 ullData);
/**
*@brief 往package中添加一个值为INT64的key。
*@param [in] writer package的writer。
*@param [in] szKey key的名字。
*@param [in] llData key的value。
*@return -1:错误；0:正确。通过cwx_pg_writer_error()获取失败的原因。
*/
int cwx_pg_writer_add_key_int64(struct CWX_PG_WRITER* writer,
                               char const* szKey,
                               CWX_INT64 llData);

/**
*@brief 形成k/v的package。
*@return -1:错误；0:正确。通过cwx_pg_writer_error()获取失败的原因。
*/
int cwx_pg_writer_pack(struct CWX_PG_WRITER* writer);
///获取当前package的Key的数量。
CWX_UINT32 cwx_pg_writer_get_key_num(struct CWX_PG_WRITER* writer);
///获取形成的package的size
CWX_UINT32 cwx_pg_writer_get_msg_size(struct CWX_PG_WRITER* writer);
///获取形成的package
char const* cwx_pg_writer_get_msg(struct CWX_PG_WRITER* writer);
///获取错误消息
char const* cwx_pg_writer_get_error(struct CWX_PG_WRITER* writer);

#ifdef __cplusplus
}
#endif

#endif

