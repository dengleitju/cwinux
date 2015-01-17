CWINUX_BEGIN_NAMESPACE

///获取app
///设置handle的reactor
inline void CwxAppHandler4Base::reactor (CwxAppReactor *reactor)
{
    m_reactor = reactor;
}
///获取handle的reactor.
inline CwxAppReactor *CwxAppHandler4Base::reactor(void)
{
    return m_reactor;
}

///获取Io handle
inline CWX_HANDLE CwxAppHandler4Base::getHandle(void) const
{
    return m_handler;
}
///设置IO handle
inline void CwxAppHandler4Base::setHandle (CWX_HANDLE handle)
{
    m_handler = handle;
}

///设置handle type
inline void CwxAppHandler4Base::setType(int type)
{
    m_type = type;
}
///获取handle type
inline int CwxAppHandler4Base::getType() const
{
    return m_type;
}

///获取注册类型
inline int CwxAppHandler4Base::getRegType() const
{
    return m_regType;
}
///设置注册类型
inline void CwxAppHandler4Base::setRegType(int type)
{
    m_regType = type;
}
///获取超时时间
inline CWX_UINT64 CwxAppHandler4Base::getTimeout() const
{
    return m_ullTimeout;
}
///设置超时时间
inline void CwxAppHandler4Base::setTimeout(CWX_UINT64 ullTimeout)
{
    m_ullTimeout = ullTimeout;
}
///获取heap中的index
inline int CwxAppHandler4Base::index() const
{
    return m_index;
}
///设置heap中的index
inline void CwxAppHandler4Base::index(int index)
{
    m_index = index;
}


///超时比较函数
inline bool CwxAppHandler4Base::operator>(CwxAppHandler4Base const& base) const
{
    return m_ullTimeout>base.m_ullTimeout;
}


CWINUX_END_NAMESPACE

