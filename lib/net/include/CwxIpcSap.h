#ifndef __CWX_IPC_SAP_H__
#define __CWX_IPC_SAP_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxIpcSap.h
@brief 进程间通信handle对象的基类。
@author cwinux@gmail.com
@version 0.1
@date 2009-10-10
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxType.h"
#include <unistd.h>
#include <fcntl.h>
CWINUX_BEGIN_NAMESPACE

/**
* @class CwxIpcSap
*
* @brief Defines the member functions for the base class of the
* CwxIpcSap abstraction
*/
class CWX_API CwxIpcSap
{
public:
    ///enable或disable asynchronous。返回值，0成功；-1：失败。
    int setSigio(bool enable=true) const;
    ///enable或disable  non-blocking I/O。返回值，0成功；-1：失败。
    int setNonblock(bool enable=true) const;
    ///enable或disable   close-on-exec。返回值，0成功；-1：失败。
    int setCloexec (bool enable=true) const;
    ///enable或disable   sigurg。返回值，0成功；-1：失败。
    int setSigurg (bool enable=true) const;
    ///是否设置了sigio, -1：失败，0：没有，1：设置
    int  isSigio() const;
    ///是否设置了nonblock, -1：失败，0：没有，1：设置
    int  isNonBlock() const;
    ///是否设置了Cloexec, -1：失败，0：没有，1：设置
    int  isCloexec() const;
    ///是否设置了Sigurg, -1：失败，0：没有，1：设置
    int  isSigurg() const;
    /// Get the underlying handle.
    CWX_HANDLE getHandle (void) const;
    /// Set the underlying handle.
    void setHandle (CWX_HANDLE handle);
public:
    ///通过fcntl设置F_SETFL状态，返回值，0成功；-1：失败。
    static int setFlags (CWX_HANDLE handle, int flags);
    ///通过fcntl清空F_SETFL状态，返回值，0成功；-1：失败。
    static int clrFlags (CWX_HANDLE handle, int flags);
    ///判断是否设置了flag，-1：失败，0：没有；1：设置
    static int isFlags(CWX_HANDLE handle, int flags);
    ///enable或disable asynchronous。返回值，0成功；-1：失败。
    static int setSigio(CWX_HANDLE handle, bool enable=true);
    ///enable或disable  non-blocking I/O。返回值，0成功；-1：失败。
    static int setNonblock(CWX_HANDLE handle, bool enable=true);
    ///enable或disable   close-on-exec。返回值，0成功；-1：失败。
    static int setCloexec (CWX_HANDLE handle, bool enable=true);
    ///enable或disable   sigurg。返回值，0成功；-1：失败。
    static int setSigurg (CWX_HANDLE handle, bool enable=true);
    ///是否设置了sigio, -1：失败，0：没有，1：设置
    int  isSigio(CWX_HANDLE handle) const;
    ///是否设置了nonblock, -1：失败，0：没有，1：设置
    int  isNonBlock(CWX_HANDLE handle) const;
    ///是否设置了Cloexec, -1：失败，0：没有，1：设置
    int  isCloexec(CWX_HANDLE handle) const;
    ///是否设置了Sigurg, -1：失败，0：没有，1：设置
    int  isSigurg(CWX_HANDLE handle) const;
protected:
    // = Ensure that CwxIpcSap is an abstract base class.
    /// Default constructor.
    CwxIpcSap (void);

    /// Protected destructor.
    /**
    * Not a virtual destructor.  Protected destructor to prevent
    * operator delete() from being called through a base class
    * CwxIpcSap pointer/reference.
    */
    ~CwxIpcSap (void);

private:
    /// Underlying I/O handle.
    CWX_HANDLE handle_;
};


CWINUX_END_NAMESPACE

#include "CwxIpcSap.inl"
#include "CwxPost.h"

#endif
