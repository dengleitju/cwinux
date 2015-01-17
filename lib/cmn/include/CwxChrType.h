#ifndef __CWX_CHR_TYPE_H__
#define __CWX_CHR_TYPE_H__

/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxChrType.h
@brief 字符分类识别对象的定义。
@author cwinux@gmail.com
@version 0.1
@date 2009-10-02
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxType.h"
#include <ctype.h>

CWINUX_BEGIN_NAMESPACE

/**
@class CwxChrType
@brief 实现了字符类型识别的静态方法。
*/
class CWX_API CwxChrType
{
public:
    ///检查字符c是否为字符或数字，等效于isAlpha()||isDigit()。
    inline static bool isAlnum(int c);
    ///检查字符c是否为字符，对于"C" locale，等效于isUpper()||isLower()
    inline static bool  isAlpha(int c);
    ///检查字符c是否【7-bit unsigned char】
    inline static bool  isAscii(int c);
    ///检查字符c是否是空格或tab
    inline static bool  isBlank(int c);
    ///检查zifuc是否为控制字符
    inline static bool  isCntrl(int c);
    ///检查字符c是否为0~9的数字
    inline static bool  isDigit(int c);
    ///检查字符c是否为可打印字符，但空格除外
    inline static bool  isGraph(int c);
    ///检查字符c是否为小写
    inline static bool  isLower(int c);
    ///检查字符c是否为可打印字符，包括空格
    inline static bool  isPrint(int c);
    ///检查字符c是否为除空格、数字、字符外的可打印字符
    inline static bool  isPunct(int c);
    ///检查字符c是否为空格，对于“C"与"POSIX”的locale，除了空格外，\\f \\n \\r \\t \\v都符合
    inline static bool  isSpace(int c);
    ///检查字符c是否为大写
    inline static bool  isUpper(int c);
    ///检查字符c是否16进制的字符
    inline static bool  isXdigit(int c);
private:
    ///禁止创建对象
    CwxChrType(){}
    ~CwxChrType(){}
};

CWINUX_END_NAMESPACE

#include "CwxChrType.inl"
#include "CwxPre.h"

#endif
