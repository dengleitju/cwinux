#ifndef  __CWX_STL_FUNC_H__
#define  __CWX_STL_FUNC_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
*@file  CwxStlFunc.h
*@brief STL contain's method
*@author cwinux@gmail.com
*@version 0.1
*@date  2009-06-30
*@warning  none.
*/
#include "CwxPre.h"
#include "CwxType.h"
#include "CwxGlobalMacro.h"
#include "CwxStl.h"

CWINUX_BEGIN_NAMESPACE

/**
*@brief  忽略大小写的字符串的hash-code的形成函数.
*@param [in] __s 字符串.
*@return 字符串的hash-code
*/
inline size_t __stl_hash_case_string(const char* __s)
{
    unsigned long __h = 0;
    for ( ; *__s; ++__s)
        __h = 5*__h + ((*__s>='a')||(*__s<='z'))?'A'+(*__s-'a'):*__s;
    return size_t(__h);
}

/**
*@class  CwxHash
*@brief  此时一个函数对象类，根据一个对象的API，hash()，计算其hash-code，<br>
*        此对象必须提供hash()的api.
*/
template<typename T> class CwxHash
{
public:
    /**
    *@brief  根据T的指针，获取T的hash-code。<br>
    *        T 必须提供 [size_t hash() const]的方法。<br>
    *@return T的指针对象的hash-code
    */
    inline size_t operator()(T const &key)const
    {
        return key.hash();
    }
};

/**
*@class  CwxPointHash
*@brief  此时一个函数对象类，根据一个对象的指针，计算其hash-code，<br>
*        此对象必须提供hash()的api.
*/
template<typename T> class CwxPointHash
{
public:
    /**
    *@brief  根据T的指针，获取T的hash-code。<br>
    *        T 必须提供 [size_t hash() const]的方法。<br>
    *@return T的指针对象的hash-code
    */
    inline size_t operator()(T const *key)const 
    {
        return key->hash();
    }
};

/**
*@class  CwxPointEqual
*@brief  比较类型T的指针对象的内容是否相等，此是一个函数对象类。
*/
template<typename T> class CwxPointEqual
{
public:
    /**
    *@brief  比较T的指针key1与key2的内容而不是指针本身是否相等。<br>
    *        此是对对象的operator()进行了重载。<br>
    *        类型T必须支持【==】操作来比较其实例是否相等。
    *@return true: 相等; false: 不相等
    */
    inline bool operator()(T const *key1, T const *key2) const 
    {
        return *key1 == *key2;
    }
};

/**
*@class  CwxPointLess
*@brief  进行对象指针内容而不是地址本身的less比较。此时一个函数对象类
*/
template<typename T> class CwxPointLess
{
public:
    /**
    *@brief  比较*key1<*key2是否成立
    *        重载了CwxPointLess的operator()(T const *key1, T const *key2).
    *@return true:*key1<*key2；false：*key1不小于*key2
    */
    inline bool operator()(T const *key1, T const *key2) const 
    {
        return *key1 < *key2;
    }
};


/**
/@class  CwxCharHash
*@brief  获取char*的hash-code，同CwxPointHash.
**/
class CwxCharHash{
public:
    /**
    *@brief  获取字符串str的hash-code,
    *@return str的hash-key
    */
    inline size_t operator()(const char* str) const 
    {
        return __stl_hash_string(str);
    }
};

/**
/@class  CwxCharEqual
*@brief  比较两个字符串是否相等，同CwxPointLess.
**/
class CwxCharEqual
{
public:
    /**
    *@brief  比较str1的内容是否等于str2的内容。
    *@return true: 等于; false: 不等于.
    */
    inline bool operator()(const char* str1, const char* str2) const
    {
        return strcmp(str1,str2)==0;
    }
};

/**
*@class  CwxCharLess
*@brief  字符串less比较，同CwxPointLess  
*/
class CwxCharLess
{
public:
    /**
    *@brief  比较str1的字符串是否小于str2的字符串.
    *@return true:小于；false：不小于。
    */
    inline bool operator()(const char* str1, const char* str2) const
    {
        return strcmp(str1,str2)<0;
    }
};


/**
/@class  CwxCharCaseHash
*@brief  获取char*的大小写不敏感hash-code，同CwxCharHash.
**/
class CwxCharCaseHash
{
public:
    /**
    *@brief  获取字符串str的大小写不敏感hash-code
    *@return str的hash-code
    */
    inline size_t operator()(const char* str) const 
    {
        return __stl_hash_case_string(str);
    }
};

/**
*@class  CwxCharCaseEqual
*@brief  比较两个大小写不敏感的字符串是否相等，同CwxCharEqual.
*/
class CwxCharCaseEqual
{
public:
    /**
    *@brief  比较两个大小写不敏感的字符串是否相等.
    *@return true: 相等; false: 不相等.
    */
    inline bool operator()(const char* str1, const char* str2) const 
    {
        return strcasecmp(str1,str2)==0;
    }
};

/**
*@class  CwxCharCaseLess
*@brief  比较大小写不敏感的字符串str1是否小于str2.同CwxCharLess.
*/
class CwxCharCaseLess
{
public:
    /**
    *@brief  比较大小写不敏感的字符串str1是否小于str2。
    *@return true：小于；false：不小于
    */
    inline bool operator()(const char* str1, const char* str2) const 
    {
        return strcasecmp(str1,str2)<0;
    }
};



/**
/@class  CwxCharCaseHash2
*@brief  通过参数控制获取获取大小写敏感还是不敏感的hash-code，同CwxCharHash.
**/
class CwxCharCaseHash2
{
public:
    ///缺省构造函数，默认大小写敏感
    CwxCharCaseHash2():m_bCaseSensive(true)
    {

    }
    ///构造函数，设置是否大小写敏感。bCaseSensive：true，大小写敏感；false，大小写不敏感
    CwxCharCaseHash2(bool bCaseSensive):m_bCaseSensive(bCaseSensive)
    {
    }
    ///拷贝构造
    CwxCharCaseHash2(CwxCharCaseHash2 const& obj)
    {
        m_bCaseSensive = obj.m_bCaseSensive;
    }
public:
    /**
    *@brief  获取字符串str的大小写不敏感或不敏感的hash-code
    *@return str的hash-code
    */
    inline size_t operator()(const char* str) const 
    {
        return m_bCaseSensive?__stl_hash_string(str):__stl_hash_case_string(str);
    }
private:
    bool    m_bCaseSensive;
};

/**
*@class  CwxCharCaseEqual2
*@brief  通过参数，控制两个字符串的是大小写敏感的相等还是不敏感的相等比较.
*/
class CwxCharCaseEqual2
{
public:
    ///缺省构造函数,缺省大小写敏感
    CwxCharCaseEqual2():m_bCaseSensive(true)
    {

    }
    ///明确指定大小写敏感参数的构造函数.bCaseSensive：true，大小写敏感；false：大小写不敏感
    CwxCharCaseEqual2(bool bCaseSensive):m_bCaseSensive(bCaseSensive)
    {
    }
    ///拷贝构造
    CwxCharCaseEqual2(CwxCharCaseEqual2 const& obj)
    {
        m_bCaseSensive = obj.m_bCaseSensive;
    }
public:
    /**
    *@brief  比较两个大小写敏感或不敏感的字符串是否相等.
    *@return true: 相等; false: 不相等.
    */
    inline bool operator()(const char* str1, const char* str2) const
    {
        return m_bCaseSensive?strcmp(str1, str2)==0:strcasecmp(str1,str2)==0;
    }
private:
    bool    m_bCaseSensive;
};

/**
*@class  CwxCharCaseLess2
*@brief  比较大小写敏感或不敏感的字符串str1是否小于str2.同CwxCharLess.
*/
class CwxCharCaseLess2
{
public:
    ///缺省构造函数，此时的字符大小写敏感
    CwxCharCaseLess2():m_bCaseSensive(true)
    {
    }
    ///指定大小写敏感或不敏感参数的构造函数
    CwxCharCaseLess2(bool bCaseSensive):m_bCaseSensive(bCaseSensive)
    {
    }
    ///拷贝构造
    CwxCharCaseLess2(CwxCharCaseLess2 const& obj)
    {
        m_bCaseSensive = obj.m_bCaseSensive;
    }
public:
    /**
    *@brief  比较大小写敏感或不敏感的字符串str1是否小于str2。
    *@return true：小于；false：不小于
    */
    inline bool operator()(const char* str1, const char* str2) const
    {
        return m_bCaseSensive?strcmp(str1, str2)<0:strcasecmp(str1,str2)<0;
    }
private:
    bool       m_bCaseSensive;
};


/**
*@class  CwxStringHash
*@brief  获取string类型字符串的hash-code，同CwxCharHash
*/
class CwxStringHash
{
public:
    /**
    *@brief  获取string类型字符串的hash-code.
    *@return str的 hash-key
    */
    inline size_t operator()(string const& str) const
    {
        return __stl_hash_string(str.c_str()); 
    }
};

/**
*@class  CwxCaseStringHash
*@brief  获取string类型字符串的大小写不敏感的hash-code.
*/
class CwxCaseStringHash
{
public:
    /**
    *@brief  获取string类型字符串的大小写不敏感的hash-code.
    *@return str的 hash-key
    */
    inline size_t operator()(string const & str) const 
    {
        return __stl_hash_case_string(str.c_str()); 
    }
};

/**
*@class  CwxCaseStringEqual
*@brief  比较大小写不敏感的两个string类型字符串是否相等。
*/
class CwxCaseStringEqual
{
public:
    /**
    *@brief  比较大小写不敏感的两个string类型字符串是否相等。
    *@return true：相等；false：不相等
    */
    inline bool operator()(string const & str1, string const& str2) const 
    {
        return strcasecmp(str1.c_str(), str2.c_str())==0; 
    }
};
/**
*@class  CwxCaseStringLess
*@brief  比较大小写不敏感的string类型的str1是否小于str2。
*/
class CwxCaseStringLess
{
public:
    /**
    *@brief  比较大小写不敏感的string类型的str1是否小于str2。
    *@return true：小于；false：不小于
    */
    inline bool operator()(string const & str1, string const& str2) const
    {
        return strcasecmp(str1.c_str(), str2.c_str())<0; 
    }
};

/**
*@class  CwxCaseStringHash2
*@brief  获取string类型字符串的大小写敏感或不敏感的hash-code.
*/
class CwxCaseStringHash2
{
public:
    ///默认构造函数，为大小写敏感的hash-code
    CwxCaseStringHash2():m_bCaseSensive(true)
    {
    }
    ///可指定大小写敏感开关的构造函数。bCaseSensive：true，大小写敏感；false，大小写不敏感
    CwxCaseStringHash2(bool bCaseSensive)
        :m_bCaseSensive(bCaseSensive)
    {

    }
    ///拷贝构造
    CwxCaseStringHash2(CwxCaseStringHash2 const& obj)
    {
        m_bCaseSensive = obj.m_bCaseSensive;
    }
public:
    /**
    *@brief  获取string类型字符串的大小写敏感或不敏感的hash-code.
    *@return str的 hash-key
    */
    inline size_t operator()(string const & str) const 
    {
        return m_bCaseSensive? __stl_hash_string(str.c_str()):__stl_hash_case_string(str.c_str());
    }
private:
    bool     m_bCaseSensive;
};

/**
*@class  CwxCaseStringEqual2
*@brief  比较大小写敏感或不敏感的两个string类型字符串是否相等。
*/
class CwxCaseStringEqual2
{
public:
    ///默认构造函数，缺省为大小写敏感
    CwxCaseStringEqual2():m_bCaseSensive(true)
    {

    }
    ///可以指定大小写敏感或不明感开关的构造函数
    CwxCaseStringEqual2(bool bCaseSensive)
        :m_bCaseSensive(bCaseSensive)
    {

    }
    ///拷贝构造
    CwxCaseStringEqual2(CwxCaseStringEqual2 const& obj)
    {
        m_bCaseSensive=obj.m_bCaseSensive;
    }
public:
    /**
    *@brief  比较大小写敏感或不敏感的两个string类型字符串是否相等。
    *@return true：相等；false：不相等
    */
    inline bool operator()(string const & str1, string const& str2) const
    {
        return m_bCaseSensive?strcmp(str1.c_str(), str2.c_str())==0:strcasecmp(str1.c_str(), str2.c_str())==0;
    }
private:
    bool       m_bCaseSensive;
};

/**
*@class  CwxCaseStringLess2
*@brief  比较大小写敏感或不敏感的string类型的str1是否小于str2。
*/
class CwxCaseStringLess2
{
public:
    ///默认构造函数
    CwxCaseStringLess2():m_bCaseSensive(true)
    {

    }
    ///可以指定敏感或不敏感开关的构造函数
    CwxCaseStringLess2(bool bCaseSensive)
        :m_bCaseSensive(bCaseSensive)
    {

    }
    ///拷贝构造
    CwxCaseStringLess2(CwxCaseStringLess2 const& obj)
    {
        m_bCaseSensive=obj.m_bCaseSensive;
    }
public:
    /**
    *@brief  比较大小写敏感或不敏感的string类型的str1是否小于str2。
    *@return true：小于；false：不小于
    */
    inline bool operator()(string const & str1, string const& str2) const
    {
        return m_bCaseSensive?strcmp(str1.c_str(), str2.c_str())<0:strcasecmp(str1.c_str(), str2.c_str())<0;
    }
private:
    bool       m_bCaseSensive;
};



/**
*@class  CwxNumHash
*@brief  解决因类型typedef定义或hash定义不存在的数字类型的hash.
*/
template <typename T>
class CwxNumHash
{
public:
    /**
    *@brief  获得范数字类型的hash-code
    *@return hash-code
    */
    inline size_t operator()(T const key)const
    { 
        return (size_t)key;
    }
};

/**
*@class  CwxSize
*@brief  计算一个对象的sizeof的大小模板函数对象.
*/
template<typename T>
class CwxSize
{
public:
    /**
    *@brief  返回对象T的sizeof()
    *@return 对象的sizeof
    */
    inline size_t operator()(T const key) const
    {
        return sizeof(key);
    }
};



CWINUX_END_NAMESPACE

#include "CwxPost.h"

#endif

