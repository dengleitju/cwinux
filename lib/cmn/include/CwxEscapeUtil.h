#ifndef __CWX_ESCAPE_UTIL_H__
#define __CWX_ESCAPE_UTIL_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxEscapeUtil.h
@brief 集合了各种场合的escape方法的定义，只针对符合UTF8的编码字符。
@author cwinux@gmail.com
@version 0.1
@date 2009-10-02
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxType.h"
#include "CwxChrType.h"
#include "CwxStl.h"
#include "CwxMutexLock.h"
#include "CwxLockGuard.h"
#include "CwxSingleton.h"
CWINUX_BEGIN_NAMESPACE
/**
@class CwxEscapeUtil
@brief 管理一个字符是否需要进行encode，及进行编码处理，只针对符合UTF8的编码字符。
*/
class CWX_API CwxEscapeUtil:public CwxSingleton
{
public:
    enum{
        URL_ESCAPE = 0x01///<url的escape类型
    };
public:
    ///创建signleton CwxEscapes 对象
    static CwxEscapeUtil* instance();
public:
    /**
    @brief 判断字符chr在unEscapeType类型的escape中，是否需要做escape。
    @param [in] chr 检查的字符
    @param [in] unEscapeType escape的类型，escape的类型为enum的值
    @return true：需要做escape变换；false：不需要做escape变换
    */
    bool isEncode(CWX_UINT8 chr, CWX_UINT16 unEscapeType) const;
    /**
    @brief 将一个字符what用2个16进制的字符表示。
    @param [in] what 需要变为16进制表示的字符
    @param [in] prefix 16进制的前缀字符，如%，0表示没有前缀
    @param [in] out 形成的带前缀的16进制字符串
    @return out字符串的下一个写入位置。
    */
    char* c2x(CWX_UINT8 what, CWX_UINT8 prefix, char* out) const;
    /**
    @brief 将两个16进制的字符串，转变为一个字符。
    @param [in] szStr 需要变为16进制表示的字符
    @return 转化后的字符。
    */
    CWX_UINT8 x2c(char const* szStr) const;
    /**
    @brief 按照URL的编码规则，对url的字符串进行编码处理。
    @param [in] szUrl 需要进行url编码的字符串
    @param [out] szOut 进行url编码后的字符串，形成字符串以\\0结束。
    @param [in,out] uiOutLen 输入szOut的空间大小，输出编码后的长度
    @return true：成功；false:szOut的空间不足。
    */
    bool urlEncode(char const* szUrl, char* szOut, CWX_UINT32& uiOutLen) const;
    /**
    @brief 按照URL的编码规则，对url的编码的字符串进行解码处理。
    @param [in] szUrl 需要进行url解码的字符串
    @param [out] szOut 进行url解码后的字符串，形成字符串以\\0结束。
    @param [in,out] uiOutLen 输入szOut的空间大小，输出解码后的长度
    @return true：成功；false:szOut的空间不足。
    */
    bool urlDecode(char const* szUrl, char* szOut, CWX_UINT32& uiOutLen) const;
    /**
    @brief 按照URL的编码规则，对url的字符串进行编码处理。
    @param [in] szUrl 需要进行url编码的字符串
    @param [out] strOut 进行url编码后的字符串。
    @return void。
    */
    void urlEncode(char const* szUrl, string& strOut) const;
    /**
    @brief 按照URL的编码规则，对url的编码的字符串进行解码处理。
    @param [in] szUrl 需要进行url解码的字符串
    @param [out] strOut 进行url解码后的字符串。
    @return void。
    */
    void urlDecode(char const* szUrl, string & strOut) const;

private:
    ///初始化需要做字符变换的map。
    void init();
    ///禁止创建对象，保证singleton
    CwxEscapeUtil();
    ~CwxEscapeUtil();
private:
    CWX_UINT16    m_chrMap[256];
    static CwxMutexLock  m_lock;
    static CwxEscapeUtil*   m_pInstance;
};

CWINUX_END_NAMESPACE

#include "CwxEscapeUtil.inl"
#include "CwxPost.h"

#endif
