CWINUX_BEGIN_NAMESPACE

inline int CwxUnixAddr::set(CwxUnixAddr const& addr)
{
    if (addr.getType () == AF_ANY)
        memset ((void *) &this->unix_addr_, 0, sizeof this->unix_addr_);
    else
        strcpy (this->unix_addr_.sun_path, addr.unix_addr_.sun_path);
    this->unix_addr_.sun_family = AF_UNIX;
    this->baseSet (addr.getType (), addr.getSize ());
    return 0;
}

inline int CwxUnixAddr::set(char const* szPathFile)
{
    memset ((void *) &this->unix_addr_,  0,    sizeof this->unix_addr_);
    this->unix_addr_.sun_family = AF_UNIX;
    CwxCommon::copyStr(this->unix_addr_.sun_path,
         szPathFile,
         sizeof this->unix_addr_.sun_path);
    this->baseSet (AF_UNIX,
        sizeof this->unix_addr_ -
        sizeof (this->unix_addr_.sun_path) +
        strlen (this->unix_addr_.sun_path));
    return 0;
}

inline int CwxUnixAddr::set(sockaddr_un const* addr, int len)
{
    memset ((void *) &this->unix_addr_, 0, sizeof this->unix_addr_);
    this->unix_addr_.sun_family = AF_UNIX;
    strcpy (this->unix_addr_.sun_path, addr->sun_path);
    this->baseSet (AF_UNIX, len);
    return 0;
}

inline char const* CwxUnixAddr::getPathFile() const
{
  return this->unix_addr_.sun_path;
}


CWINUX_END_NAMESPACE
