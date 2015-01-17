CWINUX_BEGIN_NAMESPACE

inline int CwxSockBase::close (void)
{
    int result = 0;
    if (this->getHandle () != CWX_INVALID_HANDLE)
    {
        result = ::close(this->getHandle ());
        this->setHandle (CWX_INVALID_HANDLE);
    }
    return result;
}


CWINUX_END_NAMESPACE
