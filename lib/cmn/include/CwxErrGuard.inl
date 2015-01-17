CWINUX_BEGIN_NAMESPACE

inline CwxErrGuard::CwxErrGuard()
{
    m_errno = errno;
}

inline CwxErrGuard::~CwxErrGuard()
{
    errno = m_errno;
}


CWINUX_END_NAMESPACE
