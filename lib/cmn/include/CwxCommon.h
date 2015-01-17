#ifndef __CWX_COMMON_H__
#define __CWX_COMMON_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxCommon.h
@brief 定义了CwxCommon类，此类定义了一个常用的公共静态API。
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
#include "CwxStlFunc.h"


CWINUX_BEGIN_NAMESPACE

/**
@class CwxCommon
@brief 实现了一些公共的、不可归类的常用API。
*/
class CWX_API CwxCommon
{
public:
    /**
    @brief 将src的内容拷贝到dest中，拷贝的长度不能超过len,dest的字符串以\\0结束。
    @param [in,out] dest 拷贝的目标buf
    @param [in] src 源字符串
    @param [in] len 拷贝的最大长度
    @return 返回dest中字符串的长度
    */
    static size_t copyStr(char* dest, char const* src, size_t len);
    /**
    @brief 将src的前src_len个字符，拷贝到dest中，但，不能超过dest_len的长度，dest的字符串以\\0结束。
    @param [in,out] dest 拷贝的目标buf
    @param [in] dest_len dest的buffer能接受的最多字符数。
    @param [in] src 源字符串
    @param [in] src_len 拷贝src的前src_len个字符
    @return 返回dest中字符串的长度
    */
    static size_t copyStr(char* dest, size_t dest_len, char const* src, size_t src_len);
    /**
    @brief 将【key=value】格式的src字符串，拆为key和value两部分，存在返回的kv的pair变量中。
    @param [in] src 【key=value】格式的字符串
    @param [out] kv 输出的key/value对
    @return false：src不是有效的key/value对；true：成功拆成key/value对
    */
    static bool keyValue(string const& src, pair<string, string>& kv);
    /**
    @brief 将以某个分隔符进行分割的字符串，按照分隔符拆成字段，默认分隔符为'|’。
    @param [in] src 需要分割的字符串
    @param [out] value 按照分隔符拆成的字段列表，若某个字段为空，表示两个分隔符相连或分隔符位于头部或尾部
    @param [in] ch 分隔符，默认为'|'
    @return 分割成的字段数量
    */
    static int split(string const& src, list<string>& value, char ch='|');
    /**
    @brief 将以某个分隔符进行分割的KEY/VALUE对字符串，按照分隔符拆成KEY/VALUE对的列表，默认分隔符为'|’。
    @param [in] src 需要分割的KEY/VALUE对字符串
    @param [out] value 按照分隔符拆成的KEY/VALUE对，若某个域不是KEY/VALUE对，则忽略
    @param [in] ch 分隔符，默认为'|'
    @return 分割成的KEY/VALUE对的数量
    */
    static int split(string const& src, list< pair<string, string> >& value, char ch='|');
    /**
    @brief 在一个KEY/VALUE对的LIST中，查找KEY为name的key/value对。此API的name为大小写敏感比较
    @param [in] values KEY/VALUE对列表
    @param [in] name 要查找的Key的名字
    @param [out] item 返回找到的第一个Key/value对。
    @return true：找到；false：没有找到
    */
    static bool findKey(list< pair<string, string> > const & values, string const& name, pair<string, string>& item);
    /**
    @brief 在一个KEY/VALUE对的LIST中，查找KEY为name的key/value对。此API的name为大小写不敏感比较
    @param [in] values KEY/VALUE对列表
    @param [in] name 要查找的Key的名字
    @param [out] item 返回找到的第一个Key/value对。
    @return true：找到；false：没有找到
    */
    static bool findCaseKey(list< pair<string, string> > const & values, string const& name, pair<string, string>& item);
    /**
    @brief 在一个KEY/VALUE对的LIST中，查找KEY为name的key/value对。此API的name为大小写敏感比较
    @param [in] values KEY/VALUE对列表
    @param [in] name 要查找的Key的名字
    @param [out] item 返回找到的第一个Key/value对。
    @return true：找到；false：没有找到
    */
    static bool findKey(list< pair<char*, char*> > const & values, char const* name, pair<char*, char*>& item);
    /**
    @brief 在一个KEY/VALUE对的LIST中，查找KEY为name的key/value对。此API的name为大小写敏感比较
    @param [in] values KEY/VALUE对列表
    @param [in] name 要查找的Key的名字
    @param [out] item 返回找到的第一个Key/value对。
    @return true：找到；false：没有找到
    */
    static bool findCaseKey(list< pair<char*, char*> > const & values, char const* name, pair<char*, char*>& item);
    /**
    @brief 在一块内存中将src开始的字符移动到dest的位置，移动的长度为n
    @param [in] dest 要移到的位置
    @param [in] src 要移动的字符的开始位置
    @param [in] n 移动的字符长度。
    @return 返回dest
    */
    static char* memMove(char *dest, char const *src, size_t n);
    /**
    @brief 去掉value的字符串开头及结尾的空格，若chrs不为空，则chrs中包含的字符也在移除范围内。
    @param [in] value 被TRIM的字符串
    @param [in] chrs 除空格外，chrs包含的字符也在移除的范围内。
    @return 返回value
    */
    static string& trim(string& value, char const* chrs=NULL);
    /**
    @brief 去掉value的字符串结尾的空格，若chrs不为空，则chrs中包含的字符也在移除范围内。
    @param [in] value 被rtrim的字符串
    @param [in] chrs 除空格外，chrs包含的字符也在移除的范围内。
    @return 返回value
    */
    static string& rtrim(string& value, char const* chrs=NULL);
    /**
    @brief 去掉value的字符串开头的空格，若chrs不为空，则chrs中包含的字符也在移除范围内。
    @param [in] value 被ltrim的字符串
    @param [in] chrs 除空格外，chrs包含的字符也在移除的范围内。
    @return 返回value
    */
    static string& ltrim(string& value, char const* chrs=NULL);
    /**
    @brief 去掉value的字符串开头及结尾的空格，若chrs不为空，则chrs中包含的字符也在移除范围内。
    @param [in] value 被TRIM的字符串
    @param [in] chrs 除空格外，chrs包含的字符也在移除的范围内。
    @return 返回value
    */
    static char* trim(char* value, char const* chrs=NULL);
    /**
    @brief 去掉value的字符串结尾的空格，若chrs不为空，则chrs中包含的字符也在移除范围内。
    @param [in] value 被rtrim的字符串
    @param [in] chrs 除空格外，chrs包含的字符也在移除的范围内。
    @return 返回value
    */
    static char* rtrim(char* value, char const* chrs=NULL);
    /**
    @brief 去掉value的字符串开头的空格，若chrs不为空，则chrs中包含的字符也在移除范围内。
    @param [in] value 被ltrim的字符串
    @param [in] chrs 除空格外，chrs包含的字符也在移除的范围内。
    @return 返回value
    */
    static char* ltrim(char* value, char const* chrs=NULL);
    /**
    @brief 将strSrc中出现的str1字符串替换为str2字符串。
    @param [in] strSrc 执行置换的字符串
    @param [in] str1 被替换的字符串。
    @param [in] str2 替换成的字符串。
    @return 返回strSrc
    */
    static string& replaceAll(string& strSrc, string const& str1, string const& str2);
    /**
    @brief 将src中出现的from字符串替换为to字符串，替换后的字符串放到dest的buf中，dest以\\0结束。
    @param [in] src 执行置换的字符串
    @param [in] dest 形成的替换后字符串
    @param [in,out] dest_len 传入dest的buffer大小，输出替换后的字符长度
    @param [in] from 被替换的字符串。
    @param [in] to 替换成的字符串。
    @return 返回dest
    */
    static char* replaceAll(char const* src, char* dest, size_t& dest_len, char const* from, char const* to);
    /**
    @brief 将AF12BC格式的16进制字符串，变为正常的字符串。dest以\\0结束。
    @param [in] src 16进制字符串
    @param [in] dest 形成的字符串
    @param [in,out] dest_len 传入dest的buffer大小，输出替换后的字符长度
    @return 返回dest
    */
    static char* hexToString(char const* src, char* dest, size_t& dest_len);
    /**
    @brief 将src中的大写字符变为小写字符。
    @param [in,out] src 变为小写的字符串
    @return 返回src
    */
    static char* lower(char* src);
    /**
    @brief 将src中的小写字符变为大写字符。
    @param [in,out] src 变为大写的字符串
    @return 返回src
    */
    static char* upper(char* src);
    ///将szIP的点格式的网络地址变为网络字节序的整数
    static CWX_UINT32 ipDot2Int(char const* szIp);
    ///将网络字节序的整数变为点格式的地址字符串
    static char* ipInt2Doc(CWX_UINT32 ip, char* szIp);
    ///实现snprintf,实现window与unix平台的统一
    static int snprintf(char *buf, size_t maxlen, const char *format, ...);
    ///输出long long unsigned数值的字符串, base为进制，支持16进制base=16；10进制base！=16
    static char const* toString(CWX_UINT64 ullNum, char* szBuf, int base=0);
    ///输出long long数值的字符串, base为进制，支持16进制base=16；10进制base！=16
    static char const* toString(CWX_INT64 llNum, char* szBuf, int base=0);
private:
    ///禁止实例化
    CwxCommon(){};
};

CWINUX_END_NAMESPACE

#include "CwxCommon.inl"
#include "CwxPost.h"

#endif

