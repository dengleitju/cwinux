#ifndef __CWX_REPL_MACHINE_H__
#define __CWX_REPL_MACHINE_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxReplMachine.h
@brief 通过Trie树实现的字符替换类CwxReplBase
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
#include "CwxCommon.h"
#include "CwxCharset.h"
#include "CwxCharPool.h"

CWINUX_BEGIN_NAMESPACE
/**
@class CwxReplBase
@brief 字符替换类的基类，定义了类的API。若替换是大小写敏感的，则使用不带‘Case’的API，<br>
       否则，使用带Case的API。
*/
class CWX_API CwxReplBase
{
public:
    ///构造函数
    CwxReplBase():m_uiReplNum(0)
    {

    }
    ///析构函数
    virtual ~CwxReplBase()
    {

    }
public:
    /**
    @brief 往替换器中添加大小写敏感的替换字符对，只有大小写敏感的替换才使用此接口
    @param [in] szSrc 源字符串
    @param [in] uiSrcLen 源字符串的长度
    @param [in] szRepl 替换字符串
    @param [in] uiReplLen 替换字符串的长度
    @return true：添加成功；false：添加失败
    */
    virtual bool addStr(char const* szSrc, CWX_UINT32 uiSrcLen, char const* szRepl, CWX_UINT32 uiReplLen)=0;
    /**
    @brief 往替换器中添加大小写不敏感的替换字符对，只有大小写不敏感的替换才使用此接口
    @param [in] szSrc 源字符串
    @param [in] uiSrcLen 源字符串的长度
    @param [in] szRepl 替换字符串
    @param [in] uiReplLen 替换字符串的长度
    @return true：添加成功；false：添加失败
    */
    virtual bool addCaseStr(char const* szSrc, CWX_UINT32 uiSrcLen, char const* szRepl, CWX_UINT32 uiReplLen)=0;
    /**
    @brief 执行大小写敏感的字符替换
    @param [in] szSrc 被替换的源字符串
    @param [in,out] uiSrcLen 传入被替换的源字符串的长度，执行替换的长度
    @param [in] szOut 替换后的字符串的buffer，以\\0结束
    @param [in,out] uiOutLen 传入szOut的buffer大小，输出替换后的字符串的长度，
    @return true：替换成功；false：替换失败
    */
    virtual bool replace(char const* szSrc, CWX_UINT32& uiSrcLen, char* szOut, unsigned int &uiOutLen) const=0;
    /**
    @brief 执行大小写不敏感的字符替换
    @param [in] szSrc 被替换的源字符串
    @param [in,out] uiSrcLen 传入被替换的源字符串的长度，执行替换的长度
    @param [in] szOut 替换后的字符串的buffer，以\\0结束
    @param [in,out] uiOutLen 传入szOut的buffer大小，输出替换后的字符串的长度，
    @return true：替换成功；false：替换失败
    */
    virtual bool caseReplace(char const* szSrc, CWX_UINT32& uiSrcLen, char* szOut, unsigned int &uiOutLen) const=0;
    ///获取替换器的字符集
    virtual char const* charset() const = 0;
    ///清空替换器中添加的替换字符对
    virtual void clear() = 0;
public:
    ///获取设置的替换字符对的数量
    inline CWX_UINT32 getReplNum() const 
    {
        return m_uiReplNum;
    }
    ///设置替换字符对的数量
    inline void setReplNum(CWX_UINT32 uiReplNum)
    {
        m_uiReplNum = uiReplNum;
    }
    ///将替换字符对的数量加一
    inline void incReplNum() 
    {
        m_uiReplNum++;
    }
private:
    CWX_UINT32         m_uiReplNum;
};

/**
@class CwxReplMachine
@brief 支持多字符集的字符替换类模板类
*/
template<typename CHARSET>
class CwxReplMachine:public CwxReplBase
{
public:
    /**
    @brief 构造函数
    @param [in] uiMaxKeyNum 最大的替换Key的数量
    */
    CwxReplMachine(CWX_UINT32 uiMaxKeyNum)
        :m_charPool(1024, 2048), m_trieTree(uiMaxKeyNum/4)
    {

    }
public:
    /**
    @brief 往替换器中添加大小写敏感的替换字符对，只有大小写敏感的替换才使用此接口
    @param [in] szSrc 源字符串
    @param [in] uiSrcLen 源字符串的长度
    @param [in] szRepl 替换字符串
    @param [in] uiReplLen 替换字符串的长度
    @return true：添加成功；false：添加失败
    */
    virtual bool addStr(char const* szSrc, CWX_UINT32 uiSrcLen, char const* szRepl, CWX_UINT32 uiReplLen);
    /**
    @brief 往替换器中添加大小写不敏感的替换字符对，只有大小写不敏感的替换才使用此接口
    @param [in] szSrc 源字符串
    @param [in] uiSrcLen 源字符串的长度
    @param [in] szRepl 替换字符串
    @param [in] uiReplLen 替换字符串的长度
    @return true：添加成功；false：添加失败
    */
    virtual bool addCaseStr(char const* szSrc, CWX_UINT32 uiSrcLen, char const* szRepl, CWX_UINT32 uiReplLen);
    /**
    @brief 执行大小写敏感的字符替换
    @param [in] szSrc 被替换的源字符串
    @param [in] uiSrcLen 被替换的源字符串的长度
    @param [in] szOut 替换后的字符串的buffer，以\\0结束
    @param [in,out] uiOutLen 传入szOut的buffer大小，输出替换后的字符串的长度，
    @return true：替换成功；false：替换失败
    */
    virtual bool replace(char const* szSrc, CWX_UINT32& uiSrcLen, char* szOut, unsigned int &uiOutLen) const;
    /**
    @brief 执行大小写不敏感的字符替换
    @param [in] szSrc 被替换的源字符串
    @param [in] uiSrcLen 被替换的源字符串的长度
    @param [in] szOut 替换后的字符串的buffer，以\\0结束
    @param [in,out] uiOutLen 传入szOut的buffer大小，输出替换后的字符串的长度，
    @return true：替换成功；false：替换失败
    */
    virtual bool caseReplace(char const* szSrc, CWX_UINT32& uiSrcLen, char* szOut, unsigned int &uiOutLen) const;
    ///获取CHARSET的字符集的名称
    virtual char const* charset() const;
    ///清空设置的字符替换对
    virtual void clear();
private:
    CwxCharPool        m_charPool;///<字符MEM POOL
    CwxTrieTree<CHARSET, CwxMultiString> m_trieTree;///<字符替换的trie tree
};

CWINUX_END_NAMESPACE

#include "CwxReplMachine.inl"
#include "CwxPost.h"

#endif

