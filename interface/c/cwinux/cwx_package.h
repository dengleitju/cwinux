#ifndef __CWX_PACKAGE_H__
#define __CWX_PACKAGE_H__
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
#include "cwx_config.h"

#ifdef __cplusplus
extern "C" {
#endif

///Key/value标志位
#define   CWX_PACKAGE_KV_BIT  31
///最大的kv长度
#define   CWX_PACKAGE_MAX_KV_LEN  0X7FFFFFFF

///定义key/value数据对
typedef struct CWX_KEY_VALUE_ITEM_S
{
    char*           m_szKey; ///<key的名字
    char*           m_szData; ///<key的数据
    CWX_UINT32      m_uiDataLen; ///<数据的长度
    CWX_UINT16      m_unKeyLen; ///<key的长度
    CWX_INT16       m_bKeyValue; ///<1：value的本身也是key/value格式;0：value不是key/value格式
}CWX_KEY_VALUE_ITEM_S;

/**
*@brief 获取package中的下一个Key。
*@param [in] szMsg 需要解包的package。
*@param [in] uiMsgLen package的长度。
*@param [out] item 若key/value存在，则通过item返回Key/value的信息。
*@return -1：包的格式非法；0:不存在；>0：Key/Value的长度。
*/
int cwx_pg_get_next_key(char const* szMsg,
                             CWX_UINT32 uiMsgLen,
                             CWX_KEY_VALUE_ITEM_S* item);

/**
*@brief 获取package中的第uiIndex Key/Value,如果unIndex为0，则相当于GetNextKey()。
*@param [in] szMsg 需要解包的package。
*@param [in] uiMsgLen package的长度。
*@param [in] unIndex 要获取的key的索引。
*@param [out] item 若key/value存在，则通过item返回Key/value的信息。
*@return -1：包的格式非法；0:不存在；>0：Key/Value的长度。
*/
int cwx_pg_get_key_by_index(char const *szMsg,
                                 CWX_UINT32 uiMsgLen,
                                 CWX_UINT32 uiIndex,
                                 CWX_KEY_VALUE_ITEM_S* item);

/**
*@brief 获取package中的第一个key的名字为szKey的Key/Value。
*@param [in] szMsg 需要解包的package。
*@param [in] uiMsgLen package的长度。
*@param [in] szKey 要获取的key的名字，若key的名字重复，则只获取第一个。
*@param [out] item 若key/value存在，则通过item返回Key/value的信息。
*@param [in] bCaseSensive key的名字是否大小写敏感。1：大小写敏感；0为不敏感。
*@return -1：包的格式非法；0:不存在；>0：Key/Value的长度。
*/
int cwx_pg_get_key_by_name(char const *szMsg,
                                CWX_UINT32 uiMsgLen,
                                char const* szKey,
                                CWX_KEY_VALUE_ITEM_S* item,
                                int bCaseSensive
                                );

/**
*@brief 往package中添加一个新key/value。
*@param [in,out] szMsg 需要解包的package。
*@param [in] uiMsgLen package的长度。
*@param [in] szKey 要添加的key的名字。
*@param [in] szValue key的data。
*@param [in] uiDatalen data的长度
*@param [in] bKeyValue data是否为key/value,1:是;0:不是
*@return -1：包的空间太小；>0 打入的包的长度。
*/
int cwx_pg_append_key(char *szMsg,
                           CWX_UINT32 uiMsgLen,
                           char const* szKey,
                           char const* szValue,
                           CWX_UINT32 uiDatalen,
                           int bKeyValue);
/**
*@brief 从package中删除key名字为szKey的Key/value。
*@param [in, out] szMsg package。
*@param [in, out] uiMsgLen 传入package的长度，返回新的长度。
*@param [in] szKey 要删除的key名字。
*@param [in] bAll 是否要删除所有key的名字为szKey的key/value。1：是；0：不是
*@param [in] bCaseSensive key的名字是否大小写敏感。1：大小写敏感；0为不敏感。
*@return -1：无效的package，0：没有发现，>0：删除的数量。
*/
int  cwx_pg_remove_key(char *szMsg,
                            CWX_UINT32* uiMsgLen,
                            char const* szKey,
                            int bAll,
                            int bCaseSensive);
/**
*@brief 从package中删除第unIndex的Key。
*@param [in,out] szMsg package。
*@param [in,out] uiMsgLen 传入package的长度，返回新的长度。
*@param [in] unIndex 要删除key的Index。
*@return -1：无效的package，0：没有发现，1：删除了一个KEY。
*/
int  cwx_pg_remove_key_by_index(char *szMsg,
                                     CWX_UINT32* uiMsgLen,
                                     CWX_UINT16 unIndex);
/**
*@brief 将package中第一个Key的名字为szKey的内容，修改为szData指定的内容。
*@param [in,out] szMsg package。
*@param [in,out] uiMsgLen 传入package的长度，返回新的长度。
*@param [in] uiMaxMsgLen Package的最大容量。
*@param [in] szKey 要修改的key。
*@param [in] szData 要改变成的新data。
*@param [in] uiDataLen 要改变成的新data的长度。
*@param [in] bKeyValue 新数据是否为Key/value格式。1：是；0：不是
*@param [in] bCaseSensive key的名字是否大小写敏感。1：大小写敏感，0：不敏感。
*@return -2空间不够，-1：无效的package，0：没有发现，1：修改了一个KEY。
*/
int  cwx_pg_modify_key(char *szMsg,
                      CWX_UINT32* uiMsgLen,
                      CWX_UINT32 uiMaxMsgLen,
                      char const* szKey,
                      char const* szData,
                      CWX_UINT32 uiDataLen,
                      int bKeyValue,
                      int bCaseSensive);
/**
*@brief 将package中第unIndex的Key的内容，修改为szData指定的内容。
*@param [in,out] szMsg package。
*@param [in,out] uiMsgLen 传入package的长度，返回新的长度。
*@param [in] uiMaxMsgLen Package的最大容量。
*@param [in] unIndex 要修改的key的索引。
*@param [in] szData 要改变成的新data。
*@param [in] uiDataLen 要改变成的新data的长度。
*@param [in] bKeyValue 新数据是否为Key/value格式。1：是；0：不是
*@return -2空间不够，-1：无效的package，0：没有发现，1：修改了一个KEY。
*/
int  cwx_pg_modify_key_by_index(char *szMsg,
                                     CWX_UINT32* uiMsgLen,
                                     CWX_UINT32 uiMaxMsgLen,
                                     CWX_UINT16 unIndex,
                                     char const* szData,
                                     CWX_UINT32 uiDataLen,
                                     int bKeyValue);

/**
*@brief 检查szMsg是否是一个有效的Package。uiMsgLen为0的时候，表示为空包。空包是一个有效的包。
*@param [in] szMsg 要检查的包
*@param [in] uiMsgLen 包的长度
*@return 1:有效的包；0：无效的包.
*/
int cwx_pg_is_valid(char const *szMsg, CWX_UINT32 uiMsgLen);
/**
*@brief 获取package的key的数量。
*@param [in] szMsg package的buf
*@param [in] uiMsgLen 包的长度
*@return -1：无效的包，否则为key的数量。
*/
int cwx_pg_get_key_num(char const* szMsg, CWX_UINT32 uiMsgLen);
/**
*@brief 通过Key的长度及data的长度，获取打包后的Key/value长度。
*@param [in] unKeyLen key的长度
*@param [in] uiDataLen valude的长度
*@return 形成的key-value长度。
*/
CWX_UINT32 cwx_pg_get_kv_len(CWX_UINT16 unKeyLen, CWX_UINT32 uiDataLen);
/**
*@brief 根据key-value长度与key的长度，计算出value部分的长度。
*@param [in] uiKeyValueLen key-value的长度
*@param [in] unKeyLen key的长度
*@return 返回value的长度。
*/
CWX_UINT32 cwx_pg_get_value_len(CWX_UINT32 uiKeyValueLen, CWX_UINT16 unKeyLen);

/**
*@brief 获取key在key/value中的内存数据偏移值。
*@return 返回key在key/value中的偏移。
*/
CWX_UINT16 cwx_pg_get_key_offset();
#ifdef __cplusplus
}
#endif

#endif

