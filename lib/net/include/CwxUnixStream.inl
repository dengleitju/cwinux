CWINUX_BEGIN_NAMESPACE
inline int CwxUnixStream::getRemoteAddr (CwxAddr & addr) const
{
    return getLocalAddr(addr);
}

CWINUX_END_NAMESPACE
