#ifndef __CWX_GBK_UNICODE_MAP_H__
#define __CWX_GBK_UNICODE_MAP_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxGbkUnicodeMap.h
@brief 定义了GBK与UTF-8、UTF-16转换静态类CwxGbkUnicodeMap。
@author cwinux@gmail.com
@version 0.1
@date 2009-10-10
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxType.h"
#include "CwxCharset.h"

CWINUX_BEGIN_NAMESPACE
/**
@class CwxGbkUnicodeMap
@brief 提供公共静态API，实现GBK、UTF-8、UTF-16间的相互转换
*/
class CWX_API CwxGbkUnicodeMap
{
public:
    /**
    @brief 将一个UTF16的字符串转化为UTF8的字符
    @param [in] szUtf16 UTF16的字符串
    @param [in,out] uiUtf16Len 传入UTF16的字符串的长度，传出转化的UTF16的字节数量
    @param [out] szUtf8 输出的UTF8的字符串，形成的UTF8字符串以\\0结束。
    @param [in,out] uiUtf8Len 传入szUtf8的buffer大小，传出szUtf8的长度
    @return void
    */
    static void utf16ToUtf8(char const* szUtf16, size_t& uiUtf16Len, char* szUtf8, size_t& uiUtf8Len);
    /**
    @brief 将一个UTF16的字符串转化为GBK的字符
    @param [in] szUtf16 UTF16的字符串
    @param [in,out] uiUtf16Len 传入UTF16的字符串的长度，传出转化的UTF16的字节数量
    @param [out] szGbk 输出的GBK的字符串，形成的GBK字符串以\\0结束。
    @param [in,out] uiGbkLen 传入szGbk的buffer大小，传出szGbk的长度
    @return void
    */
    static void utf16ToGbk(char const* szUtf16, CWX_UINT32& uiUtf16Len, char* szGbk, CWX_UINT32& uiGbkLen);
    /**
    @brief 将一个UTF8的字符串转化为UTF16的字符
    @param [in] szUtf8 UTF8的字符串
    @param [in,out] uiUtf8Len 传入UTF8的字符串的长度，传出转化的UTF8的字节数量
    @param [out] szUtf16 输出的UTF16的字符串，形成的UTF16字符串以\\0结束。
    @param [in,out] uiUtf16Len 传入szUtf16的buffer大小，传出szUtf16的长度
    @return void
    */
    static void utf8ToUtf16(char const* szUtf8, size_t& uiUtf8Len, char* szUtf16, size_t& uiUtf16Len);
    /**
    @brief 将一个UTF8的字符串转化为GBK的字符
    @param [in] szUtf8 UTF8的字符串
    @param [in,out] uiUtf8Len 传入UTF8的字符串的长度，传出转化的UTF8的字节数量
    @param [out] szGbk 输出的GBK的字符串，形成的GBK字符串以\\0结束。
    @param [in,out] uiGbkLen 传入szGbk的buffer大小，传出szGbk的长度
    @return void
    */
    static void utf8ToGbk(char const* szUtf8, CWX_UINT32& uiUtf8Len, char* szGbk, CWX_UINT32& uiGbkLen);
    /**
    @brief 将一个GBK的字符串转化为UTF8的字符
    @param [in] szGbk GBK的字符串
    @param [in,out] uiGbkLen 传入GBK的字符串的长度，传出转化的GBK的字节数量
    @param [out] szUtf8 输出的UTF8的字符串，形成的utf8字符串以\\0结束。
    @param [in,out] uiUtf8Len 传入szUtf8的buffer大小，传出szUtf8的长度
    @return void
    */
    static void gbkToUtf8(char const* szGbk, CWX_UINT32& uiGbkLen, char* szUtf8, CWX_UINT32& uiUtf8Len);
    /**
    @brief 将一个GBK的字符串转化为UTF16的字符
    @param [in] szGbk GBK的字符串
    @param [in,out] uiGbkLen 传入GBK的字符串的长度，传出转化的GBK的字节数量
    @param [out] szUtf16 输出的UTF16的字符串，形成的utf16字符串以\\0结束。
    @param [in,out] uiUtf16Len 传入szUtf16的buffer大小，传出szUtf16的长度
    @return void
    */
    static void gbkToUtf16(char const* szGbk, CWX_UINT32& uiGbkLen, char* szUtf16, CWX_UINT32& uiUtf16Len);
    /**
    @brief 将一个utf16的字符编码变为一个UTF8的字符
    @param [in] unUtf16 utf16字符编码
    @param [out] szUtf8 输出的UTF8的字符
    @param [in,out] unUtf8Len 传入UTF8的buffer大小，传出unUtf16对应的UTF8字符的长度
    @return void
    */
    static void utf16ChrToUtf8(CWX_UINT16 unUtf16, char* szUtf8, CWX_UINT8& unUtf8Len);
    /**
    @brief 将一个utf8的字符变为一个UTF16的字符
    @param [in] szUtf8 一个utf8字符
    @param [in] unUtf8Len 一个UTF8的字符的长度
    @param [out] unUtf16 UTF8字符对应的UTF16编码
    @return void
    */
    static void utf8ChrToUtf16(char const* szUtf8, CWX_UINT16 unUtf8Len, CWX_UINT16& unUtf16);
    ///根据GBK的编码，获取对应的UTF16编码，若为0表示不存在，此仅限于双字节的GBK编码
    static CWX_UINT16 gbkToUtf16 (CWX_UINT16 unGbk);
    ///根据UTF16的编码，获取对应的GBK编码，若为0表示不存在，此仅限于双字节的GBK编码
    static CWX_UINT16 utf16ToGbk(CWX_UINT16 unUtf16);
private:
    static unsigned short  m_gbkUtf16Map[65536];
    static unsigned short  m_utf16GbkMap[65536];
private:
    ///构造函数，禁止实例化
    CwxGbkUnicodeMap(){}
    ///析构函数，禁止删除
    ~CwxGbkUnicodeMap(){}
};

CWINUX_END_NAMESPACE

#include "CwxGbkUnicodeMap.inl"
#include "CwxPost.h"

#endif
