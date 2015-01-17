#ifndef  __CWX_PACKAGE_READER_EX_H__
#define  __CWX_PACKAGE_READER_EX_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
*@file CwxPackageReaderEx.h
*@brief CwxPackageReaderEx定义
*@author cwinux@gmail.com
*@version 0.1
*@date  2012-05-29
*@warning  nothing
*@bug    
*/

#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxCommon.h"
#include "CwxPackageEx.h"

CWINUX_BEGIN_NAMESPACE
/**
@class CwxPackageReaderEx
@brief Key/Value格式的Package的读取类
*/

class CWX_API CwxPackageReaderEx
{
    typedef map<char const*, CwxKeyValueItemEx*, CwxCharCaseLess2 > CWX_PACKAGE_KEY_MAP;
    typedef map<char const*, CwxKeyValueItemEx*, CwxCharCaseLess2 >::const_iterator CWX_PACKAGE_KEY_MAP_ITER;
public:
    ///enum for CwxPackageReaderEx
    enum{
        KEY_VALUE_ALIGN = 16,
        ERR_MSG_LEN=511 ///< ERR bug size
    };
    ///构造函数，bCaseSensive表示key是否为大小写敏感。缺省为敏感
    CwxPackageReaderEx(bool bCaseSensive=true);
    ///析构
    ~CwxPackageReaderEx();
public:
    ///对长度为uiMsgLen的package解包。
    /**
    * 其将会检查szMsg是否是一个有效的package。若bIndex=true，其会对package的Key建立索引。<br>
    * szMsg可以为一个空包，空包是一个有效的package。
    *@param [in] szMsg 需要解包的package。
    *@param [in] uiMsgLen package的长度。
    *@param [in] bIndex 是否对package的Key建立索引。
    *@param [in] bCaseSensive key的名字是否大小写敏感。true：是；false：不是。
    *@return false:错误；true:解包正确。通过GetError()获取失败的原因。
    */
    bool  unpack(char const* szMsg, CWX_UINT32 uiMsgLen, bool bIndex = true, bool bCaseSensive=true);
    ///Get key for name [szKey]. null if not exists.
    CwxKeyValueItemEx const*  getKey(char const* szKey, bool bSubKey=false) const;
    ///NULL: not exist, otherwise, return the key/value, index from 0.
    CwxKeyValueItemEx const* getKey(CWX_UINT32 index) const;
    ///Get data by key for string. false:not exist, true:get one. 
    bool getKey(char const* szKey, string& value, bool bSubKey=false) const ;
    ///Get data by key for UINT32. false:not exist, true:get one. 
    bool getKey(char const* szKey, CWX_UINT32& value, bool bSubKey=false) const ;
    ///Get data by key for INT32. false:not exist, true:get one. 
    bool getKey(char const* szKey, CWX_INT32& value, bool bSubKey=false) const;
    ///Get data by key for UINT16. false:not exist, true:get one. 
    bool getKey(char const* szKey, CWX_UINT16& value, bool bSubKey=false) const;
    ///Get data by key for INT16. false:not exist, true:get one. 
    bool getKey(char const* szKey, CWX_INT16& value, bool bSubKey=false) const;
    ///Get data by key for UINT8. false:not exist, true:get one. 
    bool getKey(char const* szKey, CWX_UINT8& value, bool bSubKey=false) const;
    ///Get data by key for char. false:not exist, true:get one. 
    bool getKey(char const* szKey, char& value, bool bSubKey=false) const;
    ///Get data by key for INT64. false:not exist, true:get one. 
    bool getKey(char const* szKey, CWX_INT64& value, bool bSubKey=false) const ;
    ///Get data by key for UINT64. false:not exist, true:get one. 
    bool getKey(char const* szKey, CWX_UINT64& value, bool bSubKey=false) const ;
    ///获取当前package的Key的数量。
    CWX_UINT32 getKeyNum() const ;
    ///return msg's size
    CWX_UINT32 getMsgSize() const ;
    ///return msg
    char const* getMsg() const ;
    ///return the errmsg
    char const* getErrMsg() const;
    ///return case-sensive sign
    bool isCaseSensive() const;
private:
    ///清空pack/unpack的痕迹
    void reset();
private:
    CwxKeyValueItemEx*        m_pKeyValue; ///<key/value's vector
    CWX_PACKAGE_KEY_MAP*    m_pKeyIndex; ///<key's index
    CWX_UINT32             m_uiTotalKeyNum;///m_pKeyValue's array size
    CWX_UINT32             m_uiKeyNum; ///<package's key number
    char const*            m_szPackMsg; ///<package's dat
    CWX_UINT32             m_uiPackBufLen;///<package's buf length
    bool                  m_bIndex;///<是否建立package的索引
    bool                  m_bCaseSensive;
    mutable CwxKeyValueItemEx       m_tmpKey; ///<临时key;
    char                  m_szErr[ERR_MSG_LEN + 1];///<错误内容
};


CWINUX_END_NAMESPACE

#include "CwxPackageReaderEx.inl"

#include "CwxPost.h"

#endif
