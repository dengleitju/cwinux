#ifndef __CWX_SOCK_BASE_H__
#define __CWX_SOCK_BASE_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxSockBase.h
@brief CwxSockBase类的定义。
@author cwinux@gmail.com
@version 0.1
@date 2009-10-10
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxType.h"
#include "CwxIpcSap.h"
#include "CwxSocket.h"
#include "CwxAddr.h"
#include <sys/socket.h>

CWINUX_BEGIN_NAMESPACE

/**
* @class CwxSockBase
*
* @brief 封装socket handle相关方法的类，此类不能创建实例。
* 
*/
class CWX_API CwxSockBase : public CwxIpcSap
{
public:

    /**
    * Close the socket.
    * This method also sets the object's handle value to CWX_INVALID_HANDLE.
    *
    * @return The result of closing the socket; 0 if the handle value
    *         was already CWX_INVALID_HANDLE.
    */
    int close (void);

    /// Return the local endpoint address in the referenced CwxAddr.
    /// Returns 0 if successful, else -1.
    int getLocalAddr (CwxAddr &) const;

    /**
    * Return the address of the remotely connected peer (if there is
    * one), in the referenced CwxAddr. Returns 0 if successful, else
    * -1.
    */
    int getRemoteAddr (CwxAddr &) const;

    /// Wrapper around the BSD-style @c socket system call (no QoS).
    int open (int domain,
        int type,
        int protocol,
        bool reuse_addr);

protected:

    /// Constructor with arguments to call the BSD-style @c socket system
    /// call (no QoS).
    CwxSockBase (int domain,
        int type,
        int protocol = 0,
        bool reuse_addr = false);
    /// Default constructor is protected to prevent instances of this class
    /// from being defined.
    CwxSockBase (void);

    /// Protected destructor.
    /**
    * Not a virtual destructor.  Protected destructor to prevent
    * operator delete() from being called through a base class CwxSockBase
    * pointer/reference.
    */
    ~CwxSockBase (void);

};


CWINUX_END_NAMESPACE

#include "CwxSockBase.inl"
#include "CwxPost.h"

#endif
