#ifndef  __CWX_PACKAGE_WRITER_EX_H__
#define  __CWX_PACKAGE_WRITER_EX_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
*@file CwxPackageWriterEx.h
*@brief CwxPackageWriteEx定义
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
@class CwxPackageWriterEx
@brief 形成KEY/VALUE的PACKAGE的writer类。
*/

class  CWX_API CwxPackageWriterEx
{
public:
    ///enum for CwxPackageWriterEx
    enum{
        ERR_MSG_LEN=511, ///< ERR bug size
        DEF_BUF_LEN=32*1024 ///<def buf len
    };
    ///构造函数，
    CwxPackageWriterEx(CWX_UINT32 uiBufLen=DEF_BUF_LEN);
    ///析构
    ~CwxPackageWriterEx();
public:
    ///建立一个新package
    void beginPack();
    ///Add a new key.true: success; false:failure, get err-msg by GetError().
    bool addKeyValue(char const* szKey, char const* szData, CWX_UINT32 uiDataLen, bool bKeyValue = false);
    ///Add a new key.true: success; false:failure, get err-msg by GetError().
    bool addKeyValue(char const* szKey, string const& strData);
    ///Add a new key.true: success; false:failure, get err-msg by GetError().
    bool addKeyValue(char const* szKey, CWX_UINT8 ucData);
    ///Add a new key.true: success; false:failure, get err-msg by GetError().
    bool addKeyValue(char const* szKey, CWX_INT8 cData);
    ///Add a new key.true: success; false:failure, get err-msg by GetError().
    bool addKeyValue(char const* szKey, CWX_UINT16 unData);
    ///Add a new key.true: success; false:failure, get err-msg by GetError().
    bool addKeyValue(char const* szKey, CWX_INT16 nData);
    ///Add a new key.true: success; false:failure, get err-msg by GetError().
    bool addKeyValue(char const* szKey, CWX_UINT32 uiData);
    ///Add a new key.true: success; false:failure, get err-msg by GetError().
    bool addKeyValue(char const* szKey, CWX_INT32 iData);
    ///Add a new key.true: success; false:failure, get err-msg by GetError().
    bool addKeyValue(char const* szKey, CWX_UINT64 ullData);
    ///Add a new key.true: success; false:failure, get err-msg by GetError().
    bool addKeyValue(char const* szKey, CWX_INT64 llData);


    ///Add a new key.true: success; false:failure, get err-msg by GetError().
    bool addKeyValue(char const* szKey, CWX_UINT16 unKeyLen, char const* szData, CWX_UINT32 uiDataLen, bool bKeyValue = false);
    ///Add a new key.true: success; false:failure, get err-msg by GetError().
    bool addKeyValue(char const* szKey, CWX_UINT16 unKeyLen, string const& strData);
    ///Add a new key.true: success; false:failure, get err-msg by GetError().
    bool addKeyValue(char const* szKey, CWX_UINT16 unKeyLen, CWX_UINT8 ucData);
    ///Add a new key.true: success; false:failure, get err-msg by GetError().
    bool addKeyValue(char const* szKey, CWX_UINT16 unKeyLen, CWX_INT8 cData);
    ///Add a new key.true: success; false:failure, get err-msg by GetError().
    bool addKeyValue(char const* szKey, CWX_UINT16 unKeyLen, CWX_UINT16 unData);
    ///Add a new key.true: success; false:failure, get err-msg by GetError().
    bool addKeyValue(char const* szKey, CWX_UINT16 unKeyLen, CWX_INT16 nData);
    ///Add a new key.true: success; false:failure, get err-msg by GetError().
    bool addKeyValue(char const* szKey, CWX_UINT16 unKeyLen, CWX_UINT32 uiData);
    ///Add a new key.true: success; false:failure, get err-msg by GetError().
    bool addKeyValue(char const* szKey, CWX_UINT16 unKeyLen, CWX_INT32 iData);
    ///Add a new key.true: success; false:failure, get err-msg by GetError().
    bool addKeyValue(char const* szKey, CWX_UINT16 unKeyLen, CWX_UINT64 ullData);
    ///Add a new key.true: success; false:failure, get err-msg by GetError().
    bool addKeyValue(char const* szKey, CWX_UINT16 unKeyLen, CWX_INT64 llData);

    ///Package msg . true: success; false:failure, get err-msg by GetError().
    bool pack();
    ///获取当前package的Key的数量。
    CWX_UINT32 getKeyNum() const ;
    ///return package's buf size
    CWX_UINT32 getBufSize() const;
    ///return msg's size
    CWX_UINT32 getMsgSize() const ;
    ///return msg
    char const* getMsg() const ;
    ///return the errmsg
    char const* getErrMsg() const;
private:
    ///清空pack/unpack的痕迹
    void reset();
private:
    CWX_UINT32            m_uiKeyNum; ///<package's key number
    char*                 m_szPackMsg; ///<package's dat
    CWX_UINT32            m_uiPackBufLen;///<package's buf length
    CWX_UINT32            m_uiCurPackPos;///<package's size
    char                  m_szErr[ERR_MSG_LEN + 1];///<错误内容
};


CWINUX_END_NAMESPACE

#include "CwxPackageWriterEx.inl"

#include "CwxPost.h"

#endif
