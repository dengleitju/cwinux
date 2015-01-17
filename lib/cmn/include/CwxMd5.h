#ifndef __CWX_MD5_H__
#define __CWX_MD5_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/
/* MD5DEEP - md5.h
 *
 * By Jesse Kornblum
 *
 * This is a work of the US Government. In accordance with 17 USC 105,
 * copyright protection is not available for any work of the US Government.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

/**
*@file  CwxMd5.h
*@brief  本代码来源于MD5DEEP，由CWINUX从C翻译为了C++，并采用CWINUX的接口
         此文件定义了CwxMd5的类，实现MD5的签名
*@author cwinux@gmail.com
*@version 0.1
*@date    2009-11-28
*@warning 无
*@bug   
*/

#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxType.h"

CWINUX_BEGIN_NAMESPACE
/**
*@class  CwxMd5
*@brief  Md5的签名对象。此对象多线程不安全，在多线程环境下，
        每个线程需要自己的一个对象实例。
*/
class CwxMd5
{
public:
    ///构造函数
    CwxMd5();
    ///析构函数
    ~CwxMd5();
public:
    ///情况对象，可以再次签名
    void reset();
    /**
    *@brief  根据buf的内容，对MD5签名进行更新
    *@param [in] szBuf 签名的内容.
    *@param [in] uiLen 内容的长度.
    *@return void.
    */
    void update(unsigned char const *szBuf, CWX_UINT32 uiLen);
    /**
    *@brief  输出16字节的签名内容
    *@param [out] digest 输出内容的签名.
    *@return void.
    */
    void final(unsigned char digest[16]);
private:
    void md5_process(unsigned char data[64]);
private:
    CWX_UINT32 total[2];
    CWX_UINT32 state[4];
    unsigned char buffer[64];
};

CWINUX_END_NAMESPACE

#include "CwxPost.h"

#endif
