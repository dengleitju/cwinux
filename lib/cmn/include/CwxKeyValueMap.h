#ifndef __CWX_KEY_VALUE_MAP_H__
#define __CWX_KEY_VALUE_MAP_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxKeyValueMap.h
@brief 封装STL的multimap实现Key与Value都是string类型的map。而且Key可选择是否大小写敏感
@author cwinux@gmail.com
@version 0.1
@date 2009-10-17
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxType.h"
#include "CwxStl.h"
#include "CwxStlFunc.h"

CWINUX_BEGIN_NAMESPACE
/**
@class CwxKeyValueMap
@brief 封装STL的MAP实现Key与Value都是string类型的map。而且Key可选择是否大小写敏感
*/
class CWX_API CwxKeyValueMap
{
public:
    typedef multimap<std::string, std::string, CwxCaseStringLess2> MAP;
    typedef MAP::iterator iterator;
    typedef MAP::const_iterator const_iterator;
    /**
    @brief 构造函数
    @param [in] bCaseSensive key是否大小写敏感。true：是；否则不是。
    */
    CwxKeyValueMap(bool bCaseSensive)
        :m_bCaseSensive(bCaseSensive)
    {
        m_index = new MAP(CwxCaseStringLess2(bCaseSensive));
    }
    ///析构函数
    ~CwxKeyValueMap()
    {
        if (m_index) delete m_index;
    }
    ///拷贝构造
    CwxKeyValueMap(CwxKeyValueMap const& obj);
    ///赋值操作
    CwxKeyValueMap& operator=(CwxKeyValueMap const& obj);
public:
    ///对两个map的key/value进行交换
    void swap(CwxKeyValueMap& obj);
    MAP& map();
    bool isCaseSensive() const;
private:
    bool           m_bCaseSensive;
    MAP*           m_index;
};

CWINUX_END_NAMESPACE

#include "CwxKeyValueMap.inl"
#include "CwxPost.h"


#endif
