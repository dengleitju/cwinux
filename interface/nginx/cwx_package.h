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
@version 0.1
@date 2009-10-12
@warning
@bug
*/
#include "cwx_config.h"

///Key/value标志位
#define   CWX_PACKAGE_KV_BIT  31
///最大的kv长度
#define   CWX_MAX_KV_LEN  0X7FFFFFFF

///定义key/value数据对
typedef struct CWX_KEY_VALUE_ITEM_S{
    char*          m_szKey; ///<key的名字
    char*          m_szData; ///<key的数据
    CWX_UINT32      m_uiDataLen; ///<数据的长度
    CWX_UINT16      m_unKeyLen; ///<key的长度
    bool           m_bKeyValue; ///<true：value的本身也是key/value格式;false：value不是key/value格式
}CWX_KEY_VALUE_ITEM_S;

/**
*@brief 获取package中的下一个Key。
*@param [in] szMsg 需要解包的package。
*@param [in] uiMsgLen package的长度。
*@param [out] item 若key/value存在，则通过item返回Key/value的信息。
*@return -1：包的格式非法；0:不存在；>0：Key/Value的长度。
*/
int cwx_get_next_key(char const* szMsg, CWX_UINT32 uiMsgLen, CWX_KEY_VALUE_ITEM_S* item);

/**
*@brief 获取package中的第uiIndex Key/Value,如果unIndex为0，则相当于GetNextKey()。
*@param [in] szMsg 需要解包的package。
*@param [in] uiMsgLen package的长度。
*@param [in] unIndex 要获取的key的索引。
*@param [out] item 若key/value存在，则通过item返回Key/value的信息。
*@return -1：包的格式非法；0:不存在；>0：Key/Value的长度。
*/
int cwx_get_key_by_index(char const *szMsg, CWX_UINT32 uiMsgLen, CWX_UINT32 uiIndex, CWX_KEY_VALUE_ITEM_S* item);

/**
*@brief 获取package中的第一个key的名字为szKey的Key/Value。
*@param [in] szMsg 需要解包的package。
*@param [in] uiMsgLen package的长度。
*@param [in] szKey 要获取的key的名字，若key的名字重复，则只获取第一个。
*@param [out] item 若key/value存在，则通过item返回Key/value的信息。
*@return -1：包的格式非法；0:不存在；>0：Key/Value的长度。
*/
int cwx_get_key_by_name(char const *szMsg, CWX_UINT32 uiMsgLen, char const* szKey, CWX_KEY_VALUE_ITEM_S* item);

/**
*@brief 往package中添加一个新key/value。
*@param [in,out] szMsg 需要解包的package。
*@param [in] uiMsgLen package的长度。
*@param [in] szKey 要添加的key的名字。
*@param [in] szValue key的data。
*@param [in] uiDatalen data的长度
*@param [in] bKeyValue data是否为key/value
*@return -1：包的空间太小；>0 打入的包的长度。
*/
int cwx_append_key(char *szMsg, CWX_UINT32 uiMsgLen, char const* szKey, char const* szValue, CWX_UINT32 uiDatalen, bool bKeyValue);

/**
*@brief 检查szMsg是否是一个有效的Package.uiMsgLen为0的时候，表示为空包。空包是一个有效的包。
*@param [in] szMsg 要检查的包
*@param [in] uiMsgLen 包的长度
*@return true:有效的包；false：无效的包.
*/
bool cwx_is_valid_package(char const *szMsg, CWX_UINT32 uiMsgLen);
///获取package的key的数量, -1: invalid package
int cwx_get_key_value_num(char const* szMsg, CWX_UINT32 uiMsgLen);
///通过Key的长度及data的长度，获取打包后的Key/value长度。
CWX_UINT32 cwx_get_kv_len(CWX_UINT16 unKeyLen, CWX_UINT32 uiDataLen);
///通过key/value的长度及key的长度，获取data的长度
CWX_UINT32 cwx_get_data_len(CWX_UINT32 uiKeyValueLen, CWX_UINT16 unKeyLen);

#endif

