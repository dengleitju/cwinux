CWINUX_BEGIN_NAMESPACE
///enable或disable asynchronous。返回值，0成功；-1：失败。
inline int CwxIpcSap::setSigio(bool enable) const
{
    return setSigio(handle_, enable);
}
///enable或disable  non-blocking I/O。返回值，0成功；-1：失败。
inline int CwxIpcSap::setNonblock(bool enable) const
{
    return setNonblock(handle_, enable);
}
///enable或disable   close-on-exec。返回值，0成功；-1：失败。
inline int CwxIpcSap::setCloexec (bool enable) const
{
    return setCloexec(handle_, enable);
}
///enable或disable   sigurg。返回值，0成功；-1：失败。
inline int CwxIpcSap::setSigurg (bool enable) const
{
    return setSigurg(handle_, enable);
}
///是否设置了sigio, -1：失败，0：没有，1：设置
inline int  CwxIpcSap::isSigio() const
{
    return isSigio(handle_);
}
///是否设置了nonblock, -1：失败，0：没有，1：设置
inline int CwxIpcSap::isNonBlock() const
{
    return isNonBlock(handle_);
}
///是否设置了Cloexec, -1：失败，0：没有，1：设置
inline int  CwxIpcSap::isCloexec() const
{
    return isCloexec(handle_);
}
///是否设置了Sigurg, -1：失败，0：没有，1：设置
inline int  CwxIpcSap::isSigurg() const
{
    return isSigurg(handle_);
}

/// Get the underlying handle.
inline CWX_HANDLE CwxIpcSap::getHandle (void) const
{
    return handle_;
}
/// Set the underlying handle.
inline void CwxIpcSap::setHandle (CWX_HANDLE handle)
{
    handle_ = handle;
}

///通过fcntl设置状态，返回值，0成功；-1：失败。
inline int CwxIpcSap::setFlags (CWX_HANDLE handle, int flags)
{
    int val = ::fcntl (handle, F_GETFL, 0);
    if (val == -1) return -1;
    // Turn on flags.
    CWX_SET_ATTR(val, flags);
    if (::fcntl (handle, F_SETFL, val) == -1)
        return -1;
    else
        return 0;
}

inline int CwxIpcSap::clrFlags (CWX_HANDLE handle, int flags)
{
    int val = ::fcntl (handle, F_GETFL, 0);

    if (val == -1)  return -1;

    // Turn flags off.
    CWX_CLR_ATTR (val, flags);

    if (::fcntl (handle, F_SETFL, val) == -1)
        return -1;
    else
        return 0;
}

inline int CwxIpcSap::isFlags(CWX_HANDLE handle, int flags)
{
    int val = ::fcntl (handle, F_GETFL, 0);
    if (val == -1)  return -1;
    return CWX_CHECK_ATTR(val, flags)?true:false;
}

///enable或disable asynchronous。返回值，0成功；-1：失败。
inline int CwxIpcSap::setSigio(CWX_HANDLE handle, bool enable)
{
    if (enable)
    {
        if ((-1==::fcntl (handle, F_SETOWN, getpid())) ||
            (-1 == setFlags(handle, O_ASYNC)))
            return -1;
    }
    else
    {
        if ((-1 == ::fcntl (handle, F_SETOWN, 0)) ||
            (-1 == clrFlags (handle,  O_ASYNC)))
            return -1;
    }
    return 0;
}

///enable或disable  non-blocking I/O。返回值，0成功；-1：失败。
inline int CwxIpcSap::setNonblock(CWX_HANDLE handle, bool enable)
{
    if (enable)
    {
        if (-1 == setFlags (handle, O_NONBLOCK)) return -1;
    }
    else
    {
        if (-1 == clrFlags (handle, O_NONBLOCK)) return -1;
    }
    return 0;
}
///enable或disable   close-on-exec。返回值，0成功；-1：失败。
inline int CwxIpcSap::setCloexec (CWX_HANDLE handle, bool enable)
{
    if (enable)
    {
        if (-1 == ::fcntl (handle, F_SETFD, 1)) return -1;
    }
    else
    {
        if (-1 == ::fcntl (handle, F_SETFD, 0)) return -1;
    }
    return 0;
}
///enable或disable   sigurg。返回值，0成功；-1：失败。
inline int CwxIpcSap::setSigurg (CWX_HANDLE handle, bool enable)
{
    if (enable)
    {
        if (-1 == ::fcntl (handle, F_SETOWN, getpid())) return -1;
    }
    else
    {
        if (-1 == ::fcntl (handle, F_SETOWN, 0)) return -1;
    }
    return 0;
}


///是否设置了sigio, -1：失败，0：没有，1：设置
inline int CwxIpcSap::isSigio(CWX_HANDLE handle) const
{
    int ret = ::fcntl (handle, F_GETOWN);
    if (-1 == ret) return -1;
    if (getpid()!=ret) return 0;
    return isFlags(handle, O_ASYNC);

}
///是否设置了nonblock, -1：失败，0：没有，1：设置
inline int CwxIpcSap::isNonBlock(CWX_HANDLE handle) const
{
    return isFlags(handle, O_NONBLOCK);

}
///是否设置了Cloexec, -1：失败，0：没有，1：设置
inline int CwxIpcSap::isCloexec(CWX_HANDLE handle) const
{
    int ret = ::fcntl(handle, F_GETFD);
    return 0==ret?0:((-1 == ret)?-1:1);
}
///是否设置了Sigurg, -1：失败，0：没有，1：设置
inline int CwxIpcSap::isSigurg(CWX_HANDLE handle) const
{
    int pid = ::fcntl (handle, F_GETOWN);
    if (-1 == pid) return -1;
    return getpid()==pid?1:0;
}


CWINUX_END_NAMESPACE
