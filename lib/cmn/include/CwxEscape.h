#ifndef __CWX_ESCAPE_H__
#define __CWX_ESCAPE_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxEscape.h
@brief escape接口的定义，实现各种字符变换。
@author cwinux@gmail.com
@version 0.1
@date 2009-11-10
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxType.h"
#include "CwxChrType.h"
#include "CwxEscapeUtil.h"
#include "CwxCommon.h"
CWINUX_BEGIN_NAMESPACE
/**
@class CwxEscape
@brief 定义字符的编码、解码操作，默认什么都不做
*/
class CWX_API CwxEscape
{
public:
    ///构造函数
    CwxEscape(){}
    ///析构函数
    virtual ~CwxEscape(){}
public:
    /**
    @brief 对szSrc的字符串进行编码，编码后的字符存放在szOut中。
    @param [in] szSrc 需要进行编码的字符串
    @param [in] uiSrcLen 源字符串的长度
    @param [out] szOut 编码后的字符串，形成字符串以\\0结束。
    @param [in,out] uiOutLen 输入szOut的空间大小，输出编码后的长度
    @return true：成功；false:szOut的空间不足。
    */
    virtual bool encode(char const* szSrc, CWX_UINT32 uiSrcLen, char* szOut, CWX_UINT32& uiOutLen) const
    {
        if (uiSrcLen >= uiOutLen) return false;
        memcpy(szOut, szSrc, uiSrcLen);
        szOut[uiSrcLen] = 0x00;
        uiOutLen = uiSrcLen;
        return true;
    }
    /**
    @brief 对szSrc的字符串进行解码，解码后的字符存放在szOut中。
    @param [in] szSrc 需要进行解码的字符串
    @param [in] uiSrcLen 源字符串的长度
    @param [out] szOut 解码后的字符串，形成字符串以\\0结束。
    @param [in,out] uiOutLen 输入szOut的空间大小，输出编码后的长度
    @return true：成功；false:szOut的空间不足。
    */
    virtual bool decode(char const* szSrc, CWX_UINT32 uiSrcLen, char* szOut, CWX_UINT32& uiOutLen) const
    {
        if (uiSrcLen >= uiOutLen) return false;
        memcpy(szOut, szSrc, uiSrcLen);
        szOut[uiSrcLen] = 0x00;
        uiOutLen = uiSrcLen;
        return true;
    }
};

CWINUX_END_NAMESPACE

#include "CwxPost.h"

#endif
