#ifndef __CWX_UNIX_STREAM_H__
#define __CWX_UNIX_STREAM_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxUnixStream.h
@brief UNix domain流数据通信连接对象的定义。
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
#include "CwxSockStream.h"

CWINUX_BEGIN_NAMESPACE
/**
@class CwxUnixStream
@brief 面向流的Unix domain连接对象。
*/
class CWX_API CwxUnixStream:public CwxSockStream
{
public:
    ///默认构造函数
    CwxUnixStream();
    ///构造函数，创建对象并设定连接handle
    CwxUnixStream(CWX_HANDLE handle);
    ///析构函数.
    ~CwxUnixStream(void);
public:
    /**
    * Return the address of the remotely connected peer (if there is
    * one), in the referenced CwxAddr. Returns 0 if successful, else
    * -1.
    */
    int getRemoteAddr (CwxAddr &) const;
};

CWINUX_END_NAMESPACE

#include "CwxUnixStream.inl"
#include "CwxPost.h"

#endif
