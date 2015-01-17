#ifndef __CWX_FILTER_MACHINE_H__
#define __CWX_FILTER_MACHINE_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxFilterMachine.h
@brief 通过Trie树，实现了一个内容过滤类，支持stopword，支持多字符集(utf8, gbk)。
@author cwinux@gmail.com
@version 0.1
@date 2009-10-10
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxType.h"
#include "CwxTrieTree.h"
#include "CwxCharPool.h"

CWINUX_BEGIN_NAMESPACE
/**
@class CwxFilterBase
@brief 过滤类的基类，定义了过滤类的API。若过滤是大小写敏感的，则使用不带‘Case’的API，<br>
       否则，使用带Case的API。
*/
class CWX_API CwxFilterBase
{
public:
    enum{
        ATTR_FILTER_LEVEL1 = 0x01,
        ATTR_FILTER_LEVEL2 = 0x02,
        ATTR_FILTER_LEVEL3 = 0x04,
        ATTR_FILTER_LEVEL4 = 0x08,
        ATTR_FILTER_LEVEL5 = 0x10,
        ATTR_FILTER_LEVEL6 = 0x20,
        ATTR_FILTER_LEVEL7 = 0x40,
        ATTR_FILTER_LEVEL8 = 0x80
    };
    ///构造函数
    CwxFilterBase():m_uiFilterNum(0)
    {

    }
    ///析构函数
    virtual ~CwxFilterBase()
    {

    }
public:
    /**
    @brief 往过滤器添加大小写敏感的stopword，只有大小写敏感的过滤器才使用此接口
    @param [in] szStopWord stopword的字符串
    @param [in] uiStopWordLen stopword字符串的长度
    @return true：添加成功；false：添加失败
    */
    virtual bool addStopWord(char const* szStopWord, CWX_UINT32 uiStopWordLen)  =0;
    /**
    @brief 往过滤器添加大小写不敏感的stopword，只有大小写不敏感的过滤器才使用此接口
    @param [in] szStopWord stopword的字符串
    @param [in] uiStopWordLen stopword字符串的长度
    @return true：添加成功；false：添加失败
    */
    virtual bool addCaseStopWord(char const* szStopWord, CWX_UINT32 uiStopWordLen) = 0;
    /**
    @brief 往过滤器添加大小写敏感的过滤词，只有大小写敏感的过滤器才使用此接口
    @param [in] szFilterStr 过滤词字符串
    @param [in] uiFilterLen 过滤词字符串的长度
    @param [in] ucFilterLevel ：过滤的级别，一个词可以设置多个过滤级别，最多为8个。
    @return true：添加成功；false：添加失败
    */
    virtual bool addFilterStr(char const* szFilterStr, CWX_UINT32 uiFilterLen, CWX_UINT8 ucFilterLevel) = 0;
    /**
    @brief 往过滤器添加大小写不敏感的过滤词，只有大小写不敏感的过滤器才使用此接口
    @param [in] szFilterStr 过滤词字符串
    @param [in] uiFilterLen 过滤词字符串的长度
    @param [in] ucFilterLevel ：过滤的级别，一个词可以设置多个过滤级别，最多为8个。
    @return true：添加成功；false：添加失败
    */
    virtual bool addCaseFilterStr(char const* szFilterStr, CWX_UINT32 uiFilterLen, CWX_UINT8 ucFilterLevel) = 0;
    /**
    @brief 按照大小写敏感的方式，检查szContent的内容中是否存在设置的过滤词。只有大小写敏感的过滤器使用此API。
    @param [in] szContent 被检查的内容
    @param [in] uiContentLen 被检查的内容的长度
    @param [out] uiStart 若level大于0，则返回对应level的词的开始位置
    @param [out] uiLen 若level大于0，则返回对应level的词的字节长度
    @return 0：不存在过滤词；>0：最高的过滤LEVEL
    */
    virtual CWX_UINT8 filterStr(char const* szContent, CWX_UINT32 uiContentLen, CWX_UINT32& uiStart, CWX_UINT32& uiLen) const = 0;
    /**
    @brief 按照大小写不敏感的方式，检查szContent的内容中是否存在设置的过滤词。只有大小写不敏感的过滤器使用此API。
    @param [in] szContent 被检查的内容
    @param [in] uiContentLen 被检查的内容的长度
    @param [out] uiStart 若level大于0，则返回对应level的词的开始位置
    @param [out] uiLen 若level大于0，则返回对应level的词的字节长度
    @return 0：不存在过滤词；>0：最高的过滤LEVEL
    */
    virtual CWX_UINT8 filterCaseStr(char const* szContent, CWX_UINT32 uiContentLen, CWX_UINT32& uiStart, CWX_UINT32& uiLen) const = 0;
    ///返回过滤器的字符集
    virtual char const* charset() const = 0;
    ///复位过滤器，清空设置的过滤字符串与stopword
    virtual void clear() = 0;
public:
    ///获取添加的过滤字符串的数量
    CWX_UINT32 getFilterNum() const 
    {
        return m_uiFilterNum;
    }
    ///设置添加的过滤字符串的数量
    void setFilterNum(CWX_UINT32 uiNum)
    {
        m_uiFilterNum = uiNum;
    }
    ///将过滤字符串的数量加1
    void incFilterNum()
    {
        m_uiFilterNum ++;
    }
private:
    CWX_UINT32         m_uiFilterNum;///<过滤词的数量
};

/**
@class CwxFilterMachine
@brief 支持多字符集的过滤器模板类。CHARSET的为CwxCharset.h中定义的字符集对象
*/
template<typename CHARSET>
class CwxFilterMachine:public CwxFilterBase{
public:
    /**
    @brief 构造函数
    @param [in] uiMaxFilteNum 过滤词的最大条目
    @param [in] uiMaxStopword stopword的最大条目
    */
    CwxFilterMachine(CWX_UINT32 uiMaxFilteNum, CWX_UINT32 uiMaxStopword):
        m_charPool(1024, 2048), m_filterTree(uiMaxFilteNum/4),m_stopwordTree(uiMaxStopword/4){
            m_ucMaxFilterLevel = 0;
    }
    ///析构函数
    ~CwxFilterMachine(){}
public:
    /**
    @brief 往过滤器添加大小写敏感的stopword，只有大小写敏感的过滤器才使用此接口
    @param [in] szStopWord stopword的字符串
    @param [in] uiStopWordLen stopword字符串的长度
    @return true：添加成功；false：添加失败
    */
    virtual bool addStopWord(char const* szStopWord, CWX_UINT32 uiStopWordLen);
    /**
    @brief 往过滤器添加大小写不敏感的stopword，只有大小写不敏感的过滤器才使用此接口
    @param [in] szStopWord stopword的字符串
    @param [in] uiStopWordLen stopword字符串的长度
    @return true：添加成功；false：添加失败
    */
    virtual bool addCaseStopWord(char const* szStopWord, CWX_UINT32 uiStopWordLen);
    /**
    @brief 往过滤器添加大小写敏感的过滤词，只有大小写敏感的过滤器才使用此接口
    @param [in] szFilterStr 过滤词字符串
    @param [in] uiFilterLen 过滤词字符串的长度
    @param [in] ucFilterLevel ：过滤的级别，一个词可以设置多个过滤级别，最多为8个。
    @return true：添加成功；false：添加失败
    */
    virtual bool addFilterStr(char const* szFilterStr, CWX_UINT32 uiFilterLen, CWX_UINT8 ucFilterLevel);
    /**
    @brief 往过滤器添加大小写不敏感的过滤词，只有大小写不敏感的过滤器才使用此接口
    @param [in] szFilterStr 过滤词字符串
    @param [in] uiFilterLen 过滤词字符串的长度
    @param [in] ucFilterLevel ：过滤的级别，一个词可以设置多个过滤级别，最多为8个。
    @return true：添加成功；false：添加失败
    */
    virtual bool addCaseFilterStr(char const* szFilterStr, CWX_UINT32 uiFilterLen, CWX_UINT8 ucFilterLevel);
    /**
    @brief 按照大小写敏感的方式，检查szContent的内容中是否存在设置的过滤词。只有大小写敏感的过滤器使用此API。
    @param [in] szContent 被检查的内容
    @param [in] uiContentLen 被检查的内容的长度
    @param [out] uiStart 若level大于0，则返回对应level的词的开始位置
    @param [out] uiLen 若level大于0，则返回对应level的词的字节长度
    @return 0：不存在过滤词；>0：最高的过滤LEVEL
    */
    virtual CWX_UINT8 filterStr(char const* szContent, CWX_UINT32 uiContentLen, CWX_UINT32& uiStart, CWX_UINT32& uiLen) const;
    /**
    @brief 按照大小写不敏感的方式，检查szContent的内容中是否存在设置的过滤词。只有大小写不敏感的过滤器使用此API。
    @param [in] szContent 被检查的内容
    @param [in] uiContentLen 被检查的内容的长度
    @param [out] uiStart 若level大于0，则返回对应level的词的开始位置
    @param [out] uiLen 若level大于0，则返回对应level的词的字节长度
    @return 0：不存在过滤词；>0：最高的过滤LEVEL
    */
    virtual CWX_UINT8 filterCaseStr(char const* szContent, CWX_UINT32 uiContentLen, CWX_UINT32& uiStart, CWX_UINT32& uiLen) const;
    ///返回【CHARSET】对应的字符集的名称
    virtual char const* charset() const;
    ///复位过滤器，清空设置的过滤字符串与stopword
    virtual void clear();
private:
    ///检查szContent的开始字符串是否是stopword，并通过uiContentLen返回stopword的长度
    bool isStopWord(char const* szContent, CWX_UINT32& uiContentLen) const;
    ///检查szContent的开始字符串是否是stopword，并通过uiContentLen返回stopword的长度
    bool isCaseStopWord(char const* szContent, CWX_UINT32& uiContentLen) const;
    ///从ucLevel中计算出最高的level
    CWX_UINT8 maxFilterLevel(CWX_UINT8 ucLevel) const;
private:
    CwxCharPool        m_charPool;///<字符串内存池
    CwxTrieTree<CHARSET, char> m_filterTree;///<过滤词的trie树
    CwxTrieTree<CHARSET, char> m_stopwordTree;///<stopword的trie树
    CWX_UINT8          m_ucMaxFilterLevel;///<设置的过滤词中的最大过滤Level
};

CWINUX_END_NAMESPACE

#include "CwxFilterMachine.inl"
#include "CwxPost.h"

#endif
