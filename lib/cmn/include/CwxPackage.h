#ifndef  __CWX_PACKAGE_H__
#define  __CWX_PACKAGE_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
*@file CwxPackage.h
*@brief CwxPackage定义
*@author cwinux@gmail.com
*@version 1.0
*@date  2009-06-05
*@warning  nothing
*@bug    
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxCommon.h"
#include "CwxEscape.h"


CWINUX_BEGIN_NAMESPACE
///Key/value的Item类
class CWX_API CwxKeyValueItem
{
public:
    enum{
        BIT_PACKAGE = 31, ///<package's sign
        MAX_KV_LEN=0X7FFFFFFF///<MAX dat length
    };
public:
    ///构造函数
    CwxKeyValueItem()
    {
        memset(this, 0x00, sizeof(CwxKeyValueItem));
    }
    ///析构函数
    ~CwxKeyValueItem()
    {

    }
    ///拷贝构造
    CwxKeyValueItem(CwxKeyValueItem const& obj)
    {
        memcpy(this, &obj, sizeof(CwxKeyValueItem));
    }
    ///赋值操作
    CwxKeyValueItem& operator=(CwxKeyValueItem const& obj)
    {
        memcpy(this, &obj, sizeof(CwxKeyValueItem));
        return *this;
    }
public:
    ///对象清空操作
    void reset()
    {
        memset(this, 0x00, sizeof(CwxKeyValueItem));
    }
public:
    char const*          m_szKey; ///<key的名字
    char const*          m_szData; ///<key的数据
    CWX_UINT32      m_uiDataLen; ///<数据的长度
    CWX_UINT16      m_unKeyLen; ///<key的长度
    bool            m_bKeyValue; ///<true：value的本身也是key/value格式;false：value不是key/value格式
};

/**
*@class CwxPackage
*@brief pack/unpack一个数据包,其内部的package buffer是外部的buf，内部并不分配。
*
*数据包是有一个多个key/value对组成，每个key/value有四部分组成，格式如下：<br>
*【网络字节序的KEY/VALUE标记与长度】【key长度】【以\\0结束的key】【以\\0结束的value】，每部分的含义如下：<br>
*【网络字节序的KEY/VALUE标记与长度】：此为一个网络字节序的32位数字，变为机器字节序后，BIT31为标记位，<br>
*                        低31位为包括【网络字节序的KEY/VALUE标记与长度】在内的KEY/VALUE的长度。<br>
*                        对于标记位：1表示key的value是一个key/value结构，否则不是。<br>
*【key长度】：为网络字节序的16数字，变为机器字节序后表示key的长度，此为了加快解包速度,key的名字大小写敏感<br>
*【key】：key的名字，在package中以\\0结束。key的名字可以重复，在重复的情况下，只能通过index获取<br>
*【value】：key的数值，在package中以\\0结束。<br>
* m_bIndex的数值，在unpack()及beginPack()的时候可以指定，在makeIndex()的时候也会设定为TRUE。
*/
class CWX_API CwxPackage
{
public:
    /**
    *@brief 获取package中的下一个Key。
    *@param [in] szMsg 需要解包的package。
    *@param [in] uiMsgLen package的长度。
    *@param [out] item 若key/value存在，则通过item返回Key/value的信息。
    *@return -1：包的格式非法；0:不存在；>0：Key/Value的长度。
    */
    static int  getNextKey(char const* szMsg, CWX_UINT32 uiMsgLen, CwxKeyValueItem& item);
    /**
    *@brief 获取package中的第uiIndex Key/Value。如果unIndex为0，则相当于GetNextKey()。
    *@param [in] szMsg 需要解包的package。
    *@param [in] uiMsgLen package的长度。
    *@param [in] uiIndex 要获取的key的索引。
    *@param [out] item 若key/value存在，则通过item返回Key/value的信息。
    *@return -1：包的格式非法；0:不存在；>0：Key/Value的长度。
    */
    static int  getKeyByIndex(char const *szMsg, CWX_UINT32 uiMsgLen, CWX_UINT32 uiIndex, CwxKeyValueItem& item);
    /**
    *@brief 获取package中的第一个key的名字为szKey的Key/Value。
    *@param [in] szMsg 需要解包的package。
    *@param [in] uiMsgLen package的长度。
    *@param [in] szKey 要获取的key的名字，若key的名字重复，则只获取第一个。
    *@param [out] item 若key/value存在，则通过item返回Key/value的信息。
    *@param [in] bCaseSensive key的名字是否大小写敏感。true：大小写敏感，否则为不敏感。缺省敏感。
    *@return -1：包的格式非法；0:不存在；>0：Key/Value的长度。
    */
    static int  getKeyByName(char const *szMsg, CWX_UINT32 uiMsgLen, char const* szKey, CwxKeyValueItem& item, bool bCaseSensive=true);
    /**
    *@brief 往package中添加一个新key/value。
    *@param [in,out] szMsg 需要解包的package。
    *@param [in] uiMsgLen package的长度。
    *@param [in] szKey 要添加的key的名字。
    *@param [in] szValue key的data。
    *@param [in] uiDatalen data的长度
    *@param [in] bKeyValue data是否为key/value
    *@return -1：包的空间太小；>=0 打入的包的长度。
    */
    static int  appendKey(char *szMsg, CWX_UINT32 uiMsgLen, char const* szKey, CWX_UINT16 unKeyLen, char const* szValue, CWX_UINT32 uiDatalen, bool bKeyValue = false);
    /**
    *@brief 从package中删除key名字为szKey的Key/value。
    *@param [in, out] szMsg package。
    *@param [in, out] uiMsgLen package的长度。
    *@param [in] szKey 要删除的key名字。
    *@param [in] bAll 是否要删除所有key的名字为szKey的key/value
    *@param [in] bCaseSensive key的名字是否大小写敏感。true：大小写敏感，否则为不敏感。缺省敏感。
    *@return -1：无效的package，0：没有发现，>0：删除的数量。
    */
    static int  removeKey(char *szMsg, CWX_UINT32& uiMsgLen, char const* szKey, bool bAll=false, bool bCaseSensive=true);
    /**
    *@brief 从package中删除第unIndex的Key。
    *@param [in,out] szMsg package。
    *@param [in,out] uiMsgLen package的长度。
    *@param [in] unIndex 要删除key的Index。
    *@return -1：无效的package，0：没有发现，1：删除了一个KEY。
    */
    static int  removeKey(char *szMsg, CWX_UINT32& uiMsgLen, CWX_UINT16 unIndex);
    /**
    *@brief 将package中第一个Key的名字为szKey的内容，修改为szData指定的内容。
    *@param [in,out] szMsg package。
    *@param [in,out] uiMsgLen package的长度。
    *@param [in] uiMaxMsgLen Package的最大容量。
    *@param [in] szKey 要修改的key。
    *@param [in] szData 要改变成的新data。
    *@param [in] uiDataLen 要改变成的新data的长度。
    *@param [in] bKeyValue 新数据是否为Key/value格式。
    *@param [in] bCaseSensive key的名字是否大小写敏感。true：大小写敏感，否则为不敏感。缺省敏感。
    *@return -2空间不够，-1：无效的package，0：没有发现，1：修改了一个KEY。
    */
    static int  modifyKey(char *szMsg, CWX_UINT32& uiMsgLen, CWX_UINT32 uiMaxMsgLen, char const* szKey, CWX_UINT16 unKeyLen, char const* szData, CWX_UINT32 uiDataLen, bool bKeyValue=false, bool bCaseSensive=true);
    /**
    *@brief 将package中第unIndex的Key的内容，修改为szData指定的内容。
    *@param [in,out] szMsg package。
    *@param [in,out] uiMsgLen package的长度。
    *@param [in] uiMaxMsgLen Package的最大容量。
    *@param [in] unIndex 要修改的key的索引。
    *@param [in] szData 要改变成的新data。
    *@param [in] uiDataLen 要改变成的新data的长度。
    *@param [in] bKeyValue 新数据是否为Key/value格式。
    *@return -2空间不够，-1：无效的package，0：没有发现，1：修改了一个KEY。
    */
    static int  modifyKey(char *szMsg, CWX_UINT32& uiMsgLen, CWX_UINT32 uiMaxMsgLen, CWX_UINT16 unIndex,char const* szData, CWX_UINT32 uiDataLen, bool bKeyValue=false);
    /**
    *@brief 将package的内容，输出成文本，对于嵌套的key，会嵌套输出。
    *@param [in] szMsg package。
    *@param [in] uiMsgLen package的长度。
    *@param [out] szOutBuf dump出的key/value的内容
    *@param [in,out] uiOutBufLen 传入szOutBuf的长度，传出形成的数据的长度
    *@param [in] szTab 定义每层次的嵌套key，相对于上一级key的缩进字符创，NULL表示不缩进，默认为\\t。
    *@param [in] szKeyBegin 一个key/value的开始字符，默认为NULL。
    *@param [in] szKeyEnd 一个key/value的结束字符，默认为"\\n"。
    *@param [in] pEscape 对key名字与data的escape对象，NULL表示不进行字符编码，使用escape的encode方法。
    *@return -2空间不够，-1：无效的package，否则返回dump出的字符串的长度。
    */
    static int  dump(char const* szMsg, CWX_UINT32 uiMsgLen, char* szOutBuf, CWX_UINT32& uiOutBufLen, char const* szTab="\t", char const* szKeyBegin=NULL, char const* szKeyEnd="\n", CwxEscape const* pEscape=NULL);
    /**
    *@brief 检查szMsg是否是一个有效的Package.uiMsgLen为0的时候，表示为空包。空包是一个有效的包。
    *@param [in] szMsg 要检查的包
    *@param [in] uiMsgLen 包的长度
    *@return true:有效的包；false：无效的包.
    */
    static bool isValidPackage(char const *szMsg, CWX_UINT32 uiMsgLen);
    ///获取package的key的数量, -1: invalid package
    static int getKeyValueNum(char const* szMsg, CWX_UINT32 uiMsgLen);
    ///通过Key的长度及data的长度，获取打包后的Key/value长度。
    static CWX_UINT32 getKvLen(CWX_UINT16 unKeyLen, CWX_UINT32 uiDataLen);
    ///通过key/value的长度及key的长度，获取data的长度
    static CWX_UINT32 getDataLen(CWX_UINT32 uiKeyValueLen, CWX_UINT16 unKeyLen);
    ///返回key在key/value中的偏移
    static CWX_UINT16 getKeyOffset();
private:
    /**
    *@brief 将package的内容，输出成文本，对于嵌套的key，会嵌套输出。
    *@param [in] szMsg package。
    *@param [in] uiMsgLen package的长度。
    *@param [out] szOutBuf dump出的key/value的内容
    *@param [in,out] uiOutBufLen 传入szOutBuf的长度，传出形成的数据的长度
    *@param [in] szTab 定义每层次的嵌套key，相对于上一级key的缩进字符创，NULL表示不缩进，默认为\\t。
    *@param [in] uiTabNum 当前key的嵌套层次。
    *@param [in] szKeyBegin 一个key/value的开始字符，默认为NULL。
    *@param [in] szKeyEnd 一个key/value的结束字符，默认为"\\n"。
    *@param [in] pEscape 对key名字与data的escape对象，NULL表示不进行字符编码，使用escape的encode方法。
    *@return -2空间不够，-1：无效的package，否则返回dump出的字符串的长度。
    */
    static int dumpEx(char const* szMsg, CWX_UINT32 uiMsgLen, char* szOutBuf, CWX_UINT32& uiOutBufLen, char const* szTab="\t", CWX_UINT32 uiTabNum = 1, char const* szKeyBegin=NULL, char const* szKeyEnd="\n", CwxEscape const* pEscape=NULL);

private:
    ///清空pack/unpack的痕迹
    void reset();
    ///构造函数，
    CwxPackage(){}
    ///析构
    ~CwxPackage(){}
};


CWINUX_END_NAMESPACE

#include "CwxPackage.inl"

#include "CwxPost.h"

#endif
