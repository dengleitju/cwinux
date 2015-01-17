#ifndef __CWX_PACKAGE_READER_H__
#define __CWX_PACKAGE_READER_H__
/*
版权声明：
    本软件遵循GNU GPL V3（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/


/**
@file cwx_package_reader.h
@brief 定义key/value 数据包的读对象
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



///声明key/value数据包读对象的数据结构
struct CWX_PG_READER;

/**
*@brief 创建package reader的对象。
*@return 0：创建失败；否则：创建的reader 对象。
*/
struct CWX_PG_READER* cwx_pg_reader_create();

/**
*@brief 释放package reader的对象。
*@return void。
*/
void cwx_pg_reader_destory(struct CWX_PG_READER* reader);

/**
*@brief unpack一个package，szMsg可以为一个空包，空包是一个有效的package。
*@param [in] reader package的reader。
*@param [in] szMsg 需要解包的package。
*@param [in] uiMsgLen package的长度。
*@param [in] bindex 是否创建key的索引。1：是；0：不是。
*@param [in] bCaseSensive key的名字是否大小写敏感。1：是；0：不是。
*@return -1:错误；0:解包正确。通过cwx_pg_reader_error()获取失败的原因。
*/
int cwx_pg_reader_unpack(struct CWX_PG_READER* reader,
           char const* szMsg,
           CWX_UINT32 uiMsgLen,
           int bindex,
           int bCaseSensive);
/**
*@brief 从package reader中，获取一个key，key可以是k1:k2:k3这样格式的子key。
*@param [in] reader package的reader。
*@param [in] szKey 要获取的key的名字。
*@param [in] uiKeyLen 要获取的key的长度。
*@param [in] bSubKey szKey是否为子key。1：是；0：不是。
*@return 0:不存在；否则返回指定的key的key/value。
*/
CWX_KEY_VALUE_ITEM_S const*  cwx_pg_reader_get_n_key(struct CWX_PG_READER const* reader,
                               char const* szKey,
                               CWX_UINT32  uiKeyLen,
                               int bSubKey);
/**
*@brief 从package reader中，获取一个key，key可以是k1:k2:k3这样格式的子key。
*@param [in] reader package的reader。
*@param [in] szKey 要获取的key的名字。
*@param [in] bSubKey szKey是否为子key。1：是；0：不是。
*@return 0:不存在；否则返回指定的key的key/value。
*/
CWX_KEY_VALUE_ITEM_S const*  cwx_pg_reader_get_key(struct CWX_PG_READER const* reader,
                                                     char const* szKey,
                                                     int bSubKey);
/**
*@brief 从package reader中，获取第index个key，index从0开始。
*@param [in] reader package的reader。
*@param [in] index 返回key的序号，从0开始。
*@return 0:不存在；否则返回指定的key的key/value。
*/
CWX_KEY_VALUE_ITEM_S const* cwx_pg_reader_get_key_by_index(struct CWX_PG_READER const* reader,
                              CWX_UINT32 index);
/**
*@brief 从package reader中，获取Key的uint64的value值。
*@param [in] reader package的reader。
*@param [in] szKey 要获取的key的名字。
*@param [in] value  value的UINT64。
*@param [in] bSubKey szKey是否为子key。1：是；0：不是。
*@return 0:不存在；1：存在。
*/
int cwx_pg_reader_get_uint64(struct CWX_PG_READER const* reader,
                            char const* szKey,
                            CWX_UINT64* value,
                            int bSubKey);
/**
*@brief 从package reader中，获取Key的int64的value值。
*@param [in] reader package的reader。
*@param [in] szKey 要获取的key的名字。
*@param [in] value  value的INT64。
*@param [in] bSubKey szKey是否为子key。1：是；0：不是。
*@return 0:不存在；1：存在。
*/
int cwx_pg_reader_get_int64(struct CWX_PG_READER const* reader,
                             char const* szKey,
                             CWX_INT64* value,
                             int bSubKey);
/**
*@brief 从package reader中，获取Key的uint32的value值。
*@param [in] reader package的reader。
*@param [in] szKey 要获取的key的名字。
*@param [in] value  value的UINT32。
*@param [in] bSubKey szKey是否为子key。1：是；0：不是。
*@return 0:不存在；1：存在。
*/
int cwx_pg_reader_get_uint32(struct CWX_PG_READER const* reader,
                            char const* szKey,
                            CWX_UINT32* value,
                            int bSubKey);
/**
*@brief 从package reader中，获取Key的int32的value值。
*@param [in] reader package的reader。
*@param [in] szKey 要获取的key的名字。
*@param [in] value  value的INT32。
*@param [in] bSubKey szKey是否为子key。1：是；0：不是。
*@return 0:不存在；1：存在。
*/
int cwx_pg_reader_get_int32(struct CWX_PG_READER const* reader,
                             char const* szKey,
                             CWX_INT32* value,
                             int bSubKey);
/**
*@brief 从package reader中，获取Key的uint16的value值。
*@param [in] reader package的reader。
*@param [in] szKey 要获取的key的名字。
*@param [in] value  value的UINT16。
*@param [in] bSubKey szKey是否为子key。1：是；0：不是。
*@return 0:不存在；1：存在。
*/
int cwx_pg_reader_get_uint16(struct CWX_PG_READER const* reader,
                            char const* szKey,
                            CWX_UINT16* value,
                            int bSubKey);
/**
*@brief 从package reader中，获取Key的int16的value值。
*@param [in] reader package的reader。
*@param [in] szKey 要获取的key的名字。
*@param [in] value  value的INT16。
*@param [in] bSubKey szKey是否为子key。1：是；0：不是。
*@return 0:不存在；1：存在。
*/
int cwx_pg_reader_get_int16(struct CWX_PG_READER const* reader,
                             char const* szKey,
                             CWX_INT16* value,
                             int bSubKey);
/**
*@brief 从package reader中，获取Key的uint8的value值。
*@param [in] reader package的reader。
*@param [in] szKey 要获取的key的名字。
*@param [in] value  value的UINT8。
*@param [in] bSubKey szKey是否为子key。1：是；0：不是。
*@return 0:不存在；1：存在。
*/
int cwx_pg_reader_get_uint8(struct CWX_PG_READER const* reader,
                            char const* szKey,
                            CWX_UINT8* value,
                            int bSubKey);
/**
*@brief 从package reader中，获取Key的int8的value值。
*@param [in] reader package的reader。
*@param [in] szKey 要获取的key的名字。
*@param [in] value  value的INT8。
*@param [in] bSubKey szKey是否为子key。1：是；0：不是。
*@return 0:不存在；1：存在。
*/
int cwx_pg_reader_get_int8(struct CWX_PG_READER const* reader,
                            char const* szKey,
                            CWX_INT8* value,
                            int bSubKey);

///获取当前package的Key的数量。
CWX_UINT32 cwx_pg_reader_get_key_num(struct CWX_PG_READER const* reader);
///获取全部的key
CWX_KEY_VALUE_ITEM_S const* cwx_pg_reader_get_keys(struct CWX_PG_READER const* reader);
///获取当前package的大小
CWX_UINT32 cwx_pg_reader_get_msg_size(struct CWX_PG_READER const* reader);
///获取当前package的buf
char const* cwx_pg_reader_get_msg(struct CWX_PG_READER const* reader);
///获取失败时的错误消息
char const* cwx_pg_reader_get_error(struct CWX_PG_READER const* reader);
///获取reader的key是否大小写敏感，1：是；0：不是
int cwx_pg_reader_case_sensive(struct CWX_PG_READER const* reader);
///获取reader是否对key进行索引，1：是；0：不是
int cwx_pg_reader_is_index(struct CWX_PG_READER const* reader);


#ifdef __cplusplus
}
#endif

#endif

