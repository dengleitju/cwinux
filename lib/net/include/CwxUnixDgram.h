#ifndef __CWX_UNIX_DGRAM_H__
#define __CWX_UNIX_DGRAM_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxUnixDgram.h
@brief unix domain 协议族的UDP包数据通信对象的定义。
@author cwinux@gmail.com
@version 0.1
@date 2009-10-12
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxType.h"
#include "CwxNetMacro.h"
#include "CwxUnixAddr.h"
#include "CwxSockDgram.h"

CWINUX_BEGIN_NAMESPACE
/**
@class CwxUnixDgram
@brief Unix domain协议族的面向包的UDP通信对象。
*/
class CWX_API CwxUnixDgram:public CwxSockDgram
{
public:
    ///默认构造函数
    CwxUnixDgram();
    ///构造函数，创建对象并设定连接handle
    CwxUnixDgram(CWX_HANDLE handle);
    ///析构函数.
    ~CwxUnixDgram(void);
public:
private:
    /**
    * Return the address of the remotely connected peer (if there is
    * one), in the referenced CwxAddr. Returns 0 if successful, else
    * -1.
    */
    int getRemoteAddr (CwxAddr &) const;

};

CWINUX_END_NAMESPACE

#include "CwxUnixDgram.inl"
#include "CwxPost.h"

#endif
