#ifndef __CWX_CHARSET_H__
#define __CWX_CHARSET_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxCharset.h
@brief 对与中文相关的UTF8、GBK（包含GB3212）、UTF16三种字符集，定义一致的操作接口
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
@class CwxMultiString
@brief 字符串的结构，存在字符串的内容及字符串的长度，用于字符替换及字符过滤相关的类
*/
struct CwxMultiString
{
    CWX_UINT32    m_uiLength;///<m_szStr的字符串的长度
    char      m_szStr[0];///<字符串
};

/**
@class CwxCharsetGbk
@brief GBK字符集的接口定义，全部为静态接口，禁止实例化
*/
class CWX_API CwxCharsetGbk
{
public:
    /**
    @brief 获取对象所定义的字符集的名字。
    @return 返回“gbk”的字符串
    */
    inline static char const* charset()
    {
        return "gbk";
    }
    /**
    @brief 判断一个字符集的特征是否符合CwxCharsetGbk类的接口描述。
    @param [in] szCharset 被检查的字符集的名字
    @return 对于"gbk"与"gb2312"，返回true；否则返回false
    */
    inline static bool charset(char const* szCharset) 
    {
        return ((strcasecmp("gbk", szCharset)==0) || (strcasecmp("gb2312", szCharset)==0))?true:false;
    }
    /**
    @brief 判断szGbkBuf中的第一个字符的字节数，0表示无效的字符或结束。
    @param [in] szGbkBuf 要检查的字符的buffer
    @param [in] uiBufLen szGbkBuf的字节长度
    @return 0表示无效的字符或结束；大于0表示szGbkBuf中第一个字符的字节长度
    */
    inline static CWX_UINT32 nextChrLen(char const* szGbkBuf, CWX_UINT32 uiBufLen)
    {
        if (uiBufLen)
        {
            if (szGbkBuf[0]<0)
            {
                return isCharset(szGbkBuf, uiBufLen)?2:0;
            }
            return szGbkBuf[0]?1:0;
        }
        return 0;
    }
    /**
    @brief 将szGbkBuf中的第一个字符通过szUpper的外部BUF返回，若是小写则变为大写。
    @param [in] szGbkBuf 字符串的buffer
    @param [in] uiBufLen szGbkBuf的字节长度
    @param [out] szUpper szGbkBuf中的第一个字符，若是小写，则变换为大写返回
    @return 0表示无效的字符或结束；大于0表示szUpper中返回的第一个字符的长度
    */
    inline static  CWX_UINT32 upperChr(char const* szGbkBuf, CWX_UINT32 uiBufLen, char* szUpper)
    {
        CWX_UINT32 uiNextLen = nextChrLen(szGbkBuf, uiBufLen);
        if (1 == uiNextLen)
        {
            if ((szGbkBuf[0]>='a') && (szGbkBuf[0]<='z'))
            {
                szUpper[0] = szGbkBuf[0] - ('a' - 'A');
            }
            else
            {
                szUpper[0] = szGbkBuf[0];
            }
        }
        else if (uiNextLen)
        {
           memcpy(szUpper, szGbkBuf, uiNextLen);
        }
        return uiNextLen;
    }

    /**
    @brief 将szGbkBuf中的第一个字符通过szLower的外部BUF返回，若是大写则变为小写。
    @param [in] szGbkBuf 字符串的buffer
    @param [in] uiBufLen szGbkBuf的字节长度
    @param [out] szLower szGbkBuf中的第一个字符，若是大写，则变换为小写返回
    @return 0表示无效的字符或结束；大于0表示szLower中返回的第一个字符的长度
    */
    inline static  CWX_UINT32 lowerChr(char const* szGbkBuf, CWX_UINT32 uiBufLen, char* szLower)
    {
        CWX_UINT32 uiNextLen = nextChrLen(szGbkBuf, uiBufLen);
        if (1 == uiNextLen)
        {
            if ((szGbkBuf[0]>='A') && (szGbkBuf[0]<='Z'))
            {
                szLower[0] = szGbkBuf[0] + ('a' - 'A');
            }
            else
            {
                szLower[0] = szGbkBuf[0];
            }
        }
        else if (uiNextLen)
        {
            memcpy(szLower, szGbkBuf, uiNextLen);
        }
        return uiNextLen;
    }

    /**
    @brief 检查szGbkBuf中的第一个字符，是否一个真正的双字节GBK字符。
    @param [in] szGbkBuf 字符串的buffer
    @param [in] uiBufLen szGbkBuf的字节长度
    @return true：是；false：不是
    */
    inline static  bool isCharset(char const* szGbkBuf, CWX_UINT32 uiBufLen)
    {
        if ( ((unsigned char)szGbkBuf[0] >=0x81) &&
            ((unsigned char)szGbkBuf[0] <=0xFE) &&
            ((unsigned char)szGbkBuf[1] >=0x40) &&
            ((unsigned char)szGbkBuf[1] <=0xFE) && uiBufLen>1)
        {
            return true;
        }
        return false;
    }

    /**
    @brief 检查szGbkBuf中字符，是否为GBK字符集支持的字符。
    @param [in] szGbkBuf 字符串的buffer
    @param [in] uiBufLen szGbkBuf的字节长度
    @return true：是GBK字符串；false：不是GBK字符串
    */
    inline static  bool isValid(char const * szGbkBuf, CWX_UINT32 uiBufLen)
    {
        if(uiBufLen)
        {
            CWX_UINT32 uiPos = 0;
            CWX_UINT32 uiLen = 0;
            while(uiPos < uiBufLen)
            {
                uiLen = nextChrLen(szGbkBuf+uiPos, uiBufLen - uiPos);
                if(0 == uiLen) return false;
                uiPos += uiLen;
            }
            return (uiPos == uiBufLen)?true:false;
        }
        return true;
    }

    /**
    @brief 获取szGbkBuf中的字符个数
    @param [in] szGbkBuf 字符串的buffer
    @param [in] uiBufLen szGbkBuf的字节长度
    @return szGbkBuf中字符的个数
    */
    inline static  CWX_UINT32 chrlen(char const* szGbkBuf, CWX_UINT32 uiBufLen)
    {
        CWX_UINT32 src_pos =0;
        CWX_UINT32 count =0;
        CWX_UINT32 len;
        while (src_pos < uiBufLen)
        {
            len =nextChrLen(szGbkBuf + src_pos, uiBufLen-src_pos);
            if (0 == len) return count;
            count ++;
            src_pos +=len;
        }
        return count;
    }

    /**
    @brief 在不对字符截断的情况下，计算从szGbkBuf中截取uiFetchLen个字节的字节结束位置。
    @param [in] szGbkBuf 字符串的buffer
    @param [in] uiBufLen szGbkBuf的字节长度
    @param [in] uiFetchLen 要获取的最大字节数量
    @return 返回截取的字节位置
    */
    inline static  CWX_UINT32 getPosByByteLen(char const* szGbkBuf, CWX_UINT32 uiBufLen, CWX_UINT32 uiFetchLen)
    {
        CWX_UINT32 pos =0;
        CWX_UINT32 len =0;
        while(1)
        {
            len =nextChrLen(szGbkBuf + pos, uiBufLen - pos);
            if (!len || (pos + len > uiBufLen) || (pos + len > uiFetchLen)) break;
            pos +=len;
        }
        return pos;
    }
    /**
    @brief 在不对字符截断的情况下，计算从szGbkBuf中截取uiFetchNum个字符的字节结束位置。
    @param [in] szGbkBuf 字符串的buffer
    @param [in] uiBufLen szGbkBuf的字节长度
    @param [in] uiFetchNum 要获取的最大字符数量
    @return 返回截取的字节位置
    */
    inline static  CWX_UINT32 getPosByChrLen(char const* szGbkBuf, CWX_UINT32 uiBufLen, CWX_UINT32 uiFetchNum)
    {
        CWX_UINT32 pos =0;
        CWX_UINT32 len =0;
        CWX_UINT32 num =0;
        while(1)
        {
            len =nextChrLen(szGbkBuf + pos, uiBufLen - pos);
            if (!len || (pos + len > uiBufLen) || (num + 1 > uiFetchNum)) break;
            num ++;
            pos +=len;
        }
        return pos;
    }
private:
    ///私有化构造函数，禁止实例化
    CwxCharsetGbk() {};
    ///私有化析构函数，禁止实例化
    ~CwxCharsetGbk() {};
};


/**
@class CwxCharsetUtf8
@brief UTF8字符集的接口定义，全部为静态接口，禁止实例化
*/
class CWX_API CwxCharsetUtf8
{
public:
    /**
    @brief 获取对象所定义的字符集的名字。
    @return 返回“utf-8”的字符串
    */
    inline static char const* charset()
    {
        return "utf-8";
    }
    /**
    @brief 判断一个字符集的特征是否符合CwxCharsetUtf8类的接口描述。
    @param [in] szCharset 被检查的字符集的名字
    @return 对于"utf-8"，返回true；否则返回false
    */
    inline static bool charset(char const* szCharset)
    {
        return strcasecmp("utf-8", szCharset)==0?true:false;
    }
    /**
    @brief 判断szUtf8Buf中的第一个字符的字节数，0表示无效的字符或结束。
    @param [in] szUtf8Buf 要检查的字符的buffer
    @param [in] uiBufLen szUtf8Buf的字节长度
    @return 0表示无效的字符或结束；大于0表示szUtf8Buf中第一个字符的字节长度
    */
    inline static  CWX_UINT32 nextChrLen(char const* szUtf8Buf, CWX_UINT32 uiBufLen)
    {
        CWX_UINT32 i;
        unsigned char c = szUtf8Buf[0];
        if (uiBufLen)
        {
            if ( (c & 0x80) == 0) return 1;
            for (i=1; i<8; i++)
            {
                c <<= 1;
                if ( (c & 0x80) == 0) break;
                if ((szUtf8Buf[i] &0xC0) != 0x80) return 0; //error, skip it.
            }
            return i<=uiBufLen?i:0;
        }
        return 0;
    }
    /**
    @brief 将szUtf8Buf中的第一个字符通过szUpper的外部BUF返回，若是小写则变为大写。
    @param [in] szUtf8Buf 字符串的buffer
    @param [in] uiBufLen szUtf8Buf的字节长度
    @param [out] szUpper szUtf8Buf中的第一个字符，若是小写，则变换为大写返回
    @return 0表示无效的字符或结束；大于0表示szUpper中返回的第一个字符的长度
    */
    inline static  CWX_UINT32 upperChr(char const* szUtf8Buf, CWX_UINT32 uiBufLen, char* szUpper)
    {
        CWX_UINT32 uiNextLen = nextChrLen(szUtf8Buf, uiBufLen);
        if (1 == uiNextLen)
        {
            if ((szUtf8Buf[0]>='a') && (szUtf8Buf[0]<='z'))
            {
                szUpper[0] = szUtf8Buf[0] - ('a' - 'A');
            }
            else
            {
                szUpper[0] = szUtf8Buf[0];
            }
        }
        else if (uiNextLen)
        {
            memcpy(szUpper, szUtf8Buf, uiNextLen);
        }
        return uiNextLen;
    }

    /**
    @brief 将szUtf8Buf中的第一个字符通过szLower的外部BUF返回，若是大写则变为小写。
    @param [in] szUtf8Buf 字符串的buffer
    @param [in] uiBufLen szUtf8Buf的字节长度
    @param [out] szLower szUtf8Buf中的第一个字符，若是大写，则变换为小写返回
    @return 0表示无效的字符或结束；大于0表示szLower中返回的第一个字符的长度
    */
    inline static  CWX_UINT32 lowerChr(char const* szUtf8Buf, CWX_UINT32 uiBufLen, char* szLower)
    {
        CWX_UINT32 uiNextLen = nextChrLen(szUtf8Buf, uiBufLen);
        if (1 == uiNextLen)
        {
            if ((szUtf8Buf[0]>='A') && (szUtf8Buf[0]<='Z'))
            {
                szLower[0] = szUtf8Buf[0] + ('a' - 'A');
            }
            else
            {
                szLower[0] = szUtf8Buf[0];
            }
        }
        else if (uiNextLen)
        {
            memcpy(szLower, szUtf8Buf, uiNextLen);
        }
        return uiNextLen;
    }

    /**
    @brief 检查szUtf8Buf中的第一个字符，是否一个UTF8字符。
    @param [in] szUtf8Buf 字符串的buffer
    @param [in] uiBufLen szUtf8Buf的字节长度
    @return true：是；false：不是
    */
    inline static  bool isCharset(char const* szUtf8Buf, CWX_UINT32 uiBufLen)
    {
        int ret = nextChrLen(szUtf8Buf, uiBufLen);
        return ret==0?false:true;
    }
    /**
    @brief 检查szUtf8Buf中字符，是否为UTF8字符集支持的字符。
    @param [in] szUtf8Buf 字符串的buffer
    @param [in] uiBufLen szUtf8Buf的字节长度
    @return true：是UTF8字符串；false：不是UTF8字符串
    */
    inline static bool isValid(char const * szUtf8Buf, CWX_UINT32 uiBufLen)
    {
        if(uiBufLen)
        {
            CWX_UINT32 uiPos = 0;
            CWX_UINT32 uiLen = 0;
            while(uiPos < uiBufLen)
            {
                uiLen = nextChrLen(szUtf8Buf+uiPos, uiBufLen - uiPos);
                if(0 == uiLen) return false;
                uiPos += uiLen;
            }
            return uiPos == uiBufLen?true:false;
        }
        return true;
    }
    /**
    @brief 获取szUtf8Buf中的字符个数
    @param [in] szUtf8Buf 字符串的buffer
    @param [in] uiBufLen szUtf8Buf的字节长度
    @return szUtf8Buf中字符的个数
    */
    inline static CWX_UINT32 chrlen(char const* szUtf8Buf, CWX_UINT32 uiBufLen)
    {
        CWX_UINT32 pos =0;
        CWX_UINT32 count =0;
        CWX_UINT32 len =0;
        while (pos < uiBufLen)
        {
            len =nextChrLen(szUtf8Buf + pos, uiBufLen - pos);
            if (0 == len) return count;
            count ++;
            pos +=len;
        }
        return count;
    }

    /**
    @brief 在不对字符截断的情况下，计算从szUtf8Buf中截取uiFetchLen个字节的字节结束位置。
    @param [in] szUtf8Buf 字符串的buffer
    @param [in] uiBufLen szUtf8Buf的字节长度
    @param [in] uiFetchLen 要获取的最大字节数量
    @return 返回截取的字节位置
    */
    inline static  CWX_UINT32 getPosByByteLen(char const* szUtf8Buf, CWX_UINT32 uiBufLen, CWX_UINT32 uiFetchLen)
    {
        CWX_UINT32 pos =0;
        CWX_UINT32 len =0;
        while(1)
        {
            len =nextChrLen(szUtf8Buf + pos, uiBufLen - pos);
            if (!len || (pos + len > uiBufLen) || (pos + len > uiFetchLen)) break;
            pos +=len;
        }
        return pos;
    }

    /**
    @brief 在不对字符截断的情况下，计算从szUtf8Buf中截取uiFetchNum个字符的字节结束位置。
    @param [in] szUtf8Buf 字符串的buffer
    @param [in] uiBufLen szUtf8Buf的字节长度
    @param [in] uiFetchNum 要获取的最大字符数量
    @return 返回截取的字节位置
    */
    inline static  CWX_UINT32 getPosByChrLen(char const* szUtf8Buf, CWX_UINT32 uiBufLen, CWX_UINT32 uiFetchNum)
    {
        CWX_UINT32 pos =0;
        CWX_UINT32 len =0;
        CWX_UINT32 num =0;
        while(1)
        {
            len =nextChrLen(szUtf8Buf + pos, uiBufLen - pos);
            if (!len || (pos + len > uiBufLen) || (num + 1 > uiFetchNum)) break;
            num ++;
            pos +=len;
        }
        return pos;
    }
private:
    ///私有化构造函数，禁止实例化
    CwxCharsetUtf8(){}
    ///私有化析构函数，禁止实例化
    ~CwxCharsetUtf8(){}

};

/**
@class CwxCharsetUtf16
@brief UTF16字符集的接口定义，全部为静态接口，禁止实例化
*/
class CWX_API CwxCharsetUtf16
{
public:
    /**
    @brief 获取对象所定义的字符集的名字。
    @return 返回“utf-16”的字符串
    */
    inline static char const* charset()
    { 
        return "utf-16";
    }
    /**
    @brief 判断一个字符集的特征是否符合CwxCharsetUtf8类的接口描述。
    @param [in] szCharset 被检查的字符集的名字
    @return 对于"utf-16"，返回true；否则返回false
    */
    inline static bool charset(char const* szCharset)
    {
        return strcasecmp("utf-16", szCharset)==0?true:false;
    }
    /**
    @brief 判断szUtf16Buf中的第一个字符的字节数，0表示无效的字符或结束。
    @param [in] szUtf16Buf 要检查的字符的buffer
    @param [in] uiBufLen szUtf16Buf的字节长度
    @return 0表示无效的字符或结束；大于0表示szUtf16Buf中第一个字符的字节长度
    */
    inline static  CWX_UINT32 nextChrLen(char const* szUtf16Buf, CWX_UINT32 uiBufLen)
    {
        return szUtf16Buf?(uiBufLen>=2?2:0):0;
    }
    /**
    @brief 将szUtf16Buf中的第一个字符通过szUpper的外部BUF返回，若是小写则变为大写。
    @param [in] szUtf16Buf 字符串的buffer
    @param [in] uiBufLen szUtf16Buf的字节长度
    @param [out] szUpper szUtf16Buf中的第一个字符，若是小写，则变换为大写返回
    @return 0表示无效的字符或结束；大于0表示szUpper中返回的第一个字符的长度
    */
    inline static  CWX_UINT32 upperChr(char const* szUtf16Buf, CWX_UINT32 uiBufLen, char* szUpper)
    {
        CWX_UINT32 uiNextLen = nextChrLen(szUtf16Buf, uiBufLen);
        if (uiNextLen)
        {
            if (szUtf16Buf[0])
            {
                szUpper[0] = 0;
                if ((szUtf16Buf[1]>='a') && (szUtf16Buf[1]<='z'))
                {
                    szUpper[1] = szUtf16Buf[1] - ('a' - 'A');
                }
                else
                {
                    szUpper[1] = szUtf16Buf[1];
                }
            }
            else
            {
                szUpper[0] = szUtf16Buf[0];
                szUpper[1] = szUtf16Buf[1];
            }
        }
        return uiNextLen;
    }

    /**
    @brief 将szUtf16Buf中的第一个字符通过szLower的外部BUF返回，若是大写则变为小写。
    @param [in] szUtf16Buf 字符串的buffer
    @param [in] uiBufLen szUtf16Buf的字节长度
    @param [out] szLower szUtf16Buf中的第一个字符，若是大写，则变换为小写返回
    @return 0表示无效的字符或结束；大于0表示szLower中返回的第一个字符的长度
    */
    inline static  CWX_UINT32 lowerChr(char const* szUtf16Buf, CWX_UINT32 uiBufLen, char* szLower)
    {
        CWX_UINT32 uiNextLen = nextChrLen(szUtf16Buf, uiBufLen);
        if (uiNextLen)
        {
            if (szUtf16Buf[0])
            {
                szLower[0] = 0;
                if ((szUtf16Buf[1]>='A') && (szUtf16Buf[1]<='Z'))
                {
                    szLower[1] = szUtf16Buf[1] + ('a' - 'A');
                }
                else
                {
                    szLower[1] = szUtf16Buf[1];
                }
            }
            else
            {
                szLower[0] = szUtf16Buf[0];
                szLower[1] = szUtf16Buf[1];
            }
        }
        return uiNextLen;
    }
    /**
    @brief 检查szUtf16Buf中的第一个字符，是否一个szUtf16Buf字符。
    @param [in] szUtf16Buf 字符串的buffer
    @param [in] uiBufLen szUtf16Buf的字节长度
    @return true：是；false：不是
    */
    inline static  bool isCharset(char const* szUtf16Buf, CWX_UINT32 uiBufLen)
    {
        return szUtf16Buf?(uiBufLen>=2?true:false):false;
    }
    /**
    @brief 检查szUtf16Buf中字符，是否为UTF16字符集支持的字符。
    @param [in] szUtf16Buf 字符串的buffer
    @param [in] uiBufLen szUtf16Buf的字节长度
    @return true：是UTF16字符串；false：不是UTF16字符串
    */
    inline static  bool isValid(char const * szUtf16Buf, CWX_UINT32 uiBufLen)
    {
        return szUtf16Buf?(uiBufLen%2?false:true):true;
    }
    /**
    @brief 获取szUtf16Buf中的字符个数
    @param [in] szUtf16Buf 字符串的buffer
    @param [in] uiBufLen szUtf16Buf的字节长度
    @return szUtf16Buf中字符的个数
    */
    inline static CWX_UINT32 chrlen(char const* szUtf16Buf, CWX_UINT32 uiBufLen)
    {
        if (szUtf16Buf) return uiBufLen/2;
        return 0;
    }
    /**
    @brief 在不对字符截断的情况下，计算从szUtf16Buf中截取uiFetchLen个字节的字节结束位置。
    @param [in] szUtf16Buf 字符串的buffer
    @param [in] uiBufLen szUtf16Buf的字节长度
    @param [in] uiFetchLen 要获取的最大字节数量
    @return 返回截取的字节位置
    */
    inline static  CWX_UINT32 getPosByByteLen(char const* szUtf16Buf, CWX_UINT32 uiBufLen, CWX_UINT32 uiFetchLen)
    {
        if (szUtf16Buf) return uiBufLen>uiFetchLen?uiFetchLen:uiBufLen;
        return 0;
    }

    /**
    @brief 在不对字符截断的情况下，计算从szUtf16Buf中截取uiFetchNum个字符的字节结束位置。
    @param [in] szUtf16Buf 字符串的buffer
    @param [in] uiBufLen szUtf16Buf的字节长度
    @param [in] uiFetchNum 要获取的最大字符数量
    @return 返回截取的字节位置
    */
    inline static  CWX_UINT32 getPosByChrLen(char const* szUtf16Buf, CWX_UINT32 uiBufLen, CWX_UINT32 uiFetchNum)
    {
        if (szUtf16Buf) return uiBufLen>uiFetchNum*2?uiFetchNum*2:uiBufLen;
        return 0;
    }
private:
    ///私有化构造函数，禁止实例化
    CwxCharsetUtf16() {}
    ///私有化析构函数，禁止实例化
    ~CwxCharsetUtf16() {}
};

CWINUX_END_NAMESPACE

#include "CwxPost.h"

#endif

