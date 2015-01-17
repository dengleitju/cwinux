#ifndef __CWX_TRIE_TREE_H__
#define __CWX_TRIE_TREE_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxTrieTree.h
@brief 支持多字符集的模板trie树的定义。
@author cwinux@gmail.com
@version 0.1
@date 2009-10-10
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxType.h"
#include "CwxStl.h"
#include "CwxTypePoolEx.h"

CWINUX_BEGIN_NAMESPACE
template<typename CHARSET, typename DATA>
class CwxTrieTree;
/**
@class CwxTrieNode
@brief Trie 树节点的模板类。末节点上的设置的数据类型通过模板参数DATA定义。
*/
template<typename DATA>
class CwxTrieNode
{
public:
    enum{
        SIGN_WORD_END_BIT = 0,///<节点是一个字符的末节点
        SIGN_KEY_END_BIT = 1,///<节点是trie 树的字符串的末节点
    };
public:
    ///构造函数
    CwxTrieNode()
    {
        memset(this, 0x00, sizeof(CwxTrieNode));
    }
public:
    /**
    @brief 以本节点为父节点，增加szWord代表的路径上的节点。
    @param [in] szWord 增加的节点对应的字符串
    @param [in] uiWordLen 字符串的长度
    @param [in] pool 若需要增加新节点，则新节点有pool进行分配
    @return NULL：添加失败；否则，为szWord字符串的末节点。
    */
    inline CwxTrieNode<DATA>* add(char const* szWord, CWX_UINT32 uiWordLen, CwxTypePoolEx<CwxTrieNode<DATA> >& pool);
    ///设置本节点为一个字符串的末节点
    inline CwxTrieNode<DATA>* endKey();
    /**
    @brief 返回szWord字符对应的孩子结点。
    @param [in] szWord 孩子结点对应的word
    @param [in] uiWordLen word的长度
    @return NULL：节点不存在或不是word节点；否则，为szWord对应的节点。
    */
    inline CwxTrieNode<DATA> const* word(char const* szWord, CWX_UINT32 uiWordLen) const;
    /**
    @brief 返回以szKey为路径的第一个字符串节点，为最小匹配。
    @param [in] szKey 节点的路径
    @param [in] uiKeyLen key的长度
    @return NULL：节点不存在；否则，为以szKey为路径的第一个字符串节点。
    */
    inline CwxTrieNode<DATA> const* firstKey(char const* szKey, CWX_UINT32& uiKeyLen) const;
    /**
    @brief 返回以szKey为路径的最后一个字符串节点，为最大匹配
    @param [in] szKey 节点的路径
    @param [in] uiKeyLen key的长度
    @return NULL：节点不存在；否则，为以szKey为路径的最后一个字符串节点。
    */
    inline CwxTrieNode<DATA> const* lastKey(char const* szKey, CWX_UINT32& uiKeyLen) const;
public:
    ///获取节点的父节点
    inline CwxTrieNode<DATA> const* getParent() const{ return m_parent;}
    ///设置节点的父节点
    inline void setParent(CwxTrieNode<DATA>* parent) { m_parent = parent;}
    ///获取节点的DATA类型的数据地址
    DATA* getData() const { return m_data;}
    ///设置节点的DATA类型的数据地址
    void setData(DATA* data) { m_data = data;}
    ///获取节点的用户属性。
    CWX_UINT8 getAttr() const { return m_ucAttr;}
    ///设置节点的用户属性
    void setAttr(CWX_UINT8 ucAttr) { m_ucAttr = ucAttr;}
    ///判断节点是否为一个word节点
    bool isWordEnd() const { return CWX_CHECK_BIT(m_ucSign, SIGN_WORD_END_BIT);}
    ///判断节点是否为一个字符串节点
    bool isKeyEnd() const { return CWX_CHECK_BIT(m_ucSign, SIGN_KEY_END_BIT);}
    ///清除KEY标记SIGN_KEY_END_BIT
    void clrKeyEnd() { CWX_CLR_BIT(m_ucSign, SIGN_KEY_END_BIT);}
private:
    template <class A1,class A2>  friend class CwxTrieTree;
private:
    CwxTrieNode<DATA>*      m_child[16];///<节点的孩子结点
    CwxTrieNode<DATA>*      m_parent;///<节点的父节点
    DATA*                  m_data;///<节点上的数据
    CWX_UINT8              m_ucAttr;///<节点的用户属性
    CWX_UINT8              m_ucSign;///<节点的类型
};


/**
@class CwxTrieTree
@brief Trie 树的模板类。字符集由CHARSET定义，末节点上的设置的数据类型通过模板参数DATA定义。<br>
       Trie 树支持字符串是否大小写敏感，一个对象实例要么大小写敏感，要么大小写不敏感。
*/
template<typename CHARSET, typename DATA>
class CwxTrieTree
{
public:
    /**
    @brief 构造函数。
    @param [in] uiNodePerPool 树结点的内存池的每个trunk的节点树
    */
    CwxTrieTree(CWX_UINT32 uiNodePerPool):m_pool(uiNodePerPool), m_uiDataNum(0){}
public:
    /**
    @brief 往Trie Tree添加szKeyword，此为大小写敏感。
    @param [in] szKey 添加的字符串
    @param [in] uiKeyLen 字符串的长度
    @return NULL：添加失败；否则，为szKey字符串的末节点。
    */
    inline CwxTrieNode<DATA>* addKey(char const* szKey, CWX_UINT32 uiKeyLen);
    /**
    @brief 往Trie Tree添加szKeyword，此为大小写不敏感。
    @param [in] szKey 添加的字符串
    @param [in] uiKeyLen 字符串的长度
    @return NULL：添加失败；否则，为szKey字符串的末节点。
    */
    inline CwxTrieNode<DATA>* addCaseKey(char const* szKey, CWX_UINT32 uiKeyLen);
    /**
    @brief 删除指定的Key节点，此节点必须为一个SIGN_KEY_END_BIT节点，若还有孩子，则只删除SIGN_KEY_END_BIT标记。
           若孩子为空，则从此节点一直往parent遍历，删除所有的父节点直到发现了一下的节点：
           1、SIGN_KEY_END_BIT节点，此意味着另外的一个keyword，或
           2、一个节点有孩子。
    @param [in] pNode 要删除的node
    @return true：成功删除；false：无效的节点，此时，节点不是SIGN_KEY_END_BIT节点。
    */
    inline bool eraseNode(CwxTrieNode<DATA>* pNode);
    /**
    @brief 返回以szKey为路径的第一个字符串节点，为最小匹配。此为大小写敏感。
    @param [in] szKey 节点的路径
    @param [in,out] uiKeyLen 传入key的长度，返回在那个位置发现了第一个字符串节点
    @return NULL：节点不存在；否则，为以szKey为路径的第一个字符串节点。
    */
    inline CwxTrieNode<DATA> const* firstKey(char const* szKey, CWX_UINT32& uiKeyLen) const;
    /**
    @brief 返回以szKey为路径的第一个字符串节点，为最小匹配。此为大小写不敏感。
    @param [in] szKey 节点的路径
    @param [in,out] uiKeyLen 传入key的长度，返回在那个位置发现了第一个字符串节点
    @return NULL：节点不存在；否则，为以szKey为路径的第一个字符串节点。
    */
    inline CwxTrieNode<DATA> const* firstCaseKey(char const* szKey, CWX_UINT32& uiKeyLen) const;
    /**
    @brief 返回以szKey为路径的最后一个字符串节点，为最大匹配。此为大小写敏感。
    @param [in] szKey 节点的路径
    @param [in,out] uiKeyLen 传入key的长度，返回在那个位置发现了最后一个字符串节点
    @return NULL：节点不存在；否则，为以szKey为路径的最后一个字符串节点。
    */
    inline CwxTrieNode<DATA> const* lastKey(char const* szKey, CWX_UINT32& uiKeyLen) const;
    /**
    @brief 返回以szKey为路径的最后一个字符串节点，为最大匹配。此为大小写不敏感。
    @param [in] szKey 节点的路径
    @param [in,out] uiKeyLen 传入key的长度，返回在那个位置发现了最后一个字符串节点
    @return NULL：节点不存在；否则，为以szKey为路径的最后一个字符串节点。
    */
    inline CwxTrieNode<DATA> const* lastCaseKey(char const* szKey, CWX_UINT32& uiKeyLen) const;
    ///获取Trie Tree的根节点
    inline CwxTrieNode<DATA> const* getRoot() const { return &m_root;}
    ///获取Trie Tree中字符串的数量
    inline CWX_UINT32 getDataNum() const { return m_uiDataNum;}
    ///清空Trie Tree
    inline void clear();
private:
    CwxTrieNode<DATA>  m_root; ///<Trie Tree的根节点
    CwxTypePoolEx<CwxTrieNode<DATA> > m_pool;///<Trie Tree节点分配的内存池
    CWX_UINT32        m_uiDataNum;///<Trie Tree中字符串的数量
};

CWINUX_END_NAMESPACE

#include "CwxTrieTree.inl"
#include "CwxPost.h"

#endif

