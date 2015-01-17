CWINUX_BEGIN_NAMESPACE

inline int CwxINetAddr::set (const CwxINetAddr& sa) {
  if (sa.getType () == AF_ANY) {
    // Ugh, this is really a base class, so don't copy it.
    memset (&this->inet_addr_, 0, sizeof (this->inet_addr_));
  } else{
    // It's ok to make the copy.
    memcpy (&this->inet_addr_, &sa.inet_addr_, sa.getSize ());
    this->baseSet(sa.getType(), sa.getSize());
  }
  return 0;
}

inline int CwxINetAddr::set(const sockaddr_in *addr,  int len)
{
  if (addr->sin_family == AF_INET){
    int maxlen = static_cast<int> (sizeof (this->inet_addr_.in4_));
    if (len > maxlen) len = maxlen;
    memcpy (&this->inet_addr_.in4_, addr, len);
    this->baseSet(AF_INET, len);
    return 0;
  } else if (addr->sin_family == AF_INET6){
    int maxlen = static_cast<int> (sizeof (this->inet_addr_.in6_));
    if (len > maxlen) len = maxlen;
    memcpy (&this->inet_addr_.in6_, addr, len);
    this->baseSet(AF_INET6, len);
    return 0;
  }
  errno = EAFNOSUPPORT;
  return -1;
}

inline void CwxINetAddr::setPort (CWX_UINT16 unPort) {
  unPort = CWX_HTONS (unPort);
  if (this->getType () == AF_INET6) {
    this->inet_addr_.in6_.sin6_port = unPort;
  } else {
    this->inet_addr_.in4_.sin_port = unPort;
  }
}
/// Return the port number, converting it into host byte-order.
inline CWX_UINT16 CwxINetAddr::getPort(void) const
{
  if (this->getType () == AF_INET)
    return CWX_NTOHS (this->inet_addr_.in4_.sin_port);
  return CWX_NTOHS (this->inet_addr_.in6_.sin6_port);
}

inline char const* CwxINetAddr::getHostIp (char* szAddr, int len) const {
  if (this->getType () == AF_INET6) {
    const char *ch = inet_ntop (AF_INET6,
      &this->inet_addr_.in6_.sin6_addr,
      szAddr,
      len);
    if ((IN6_IS_ADDR_LINKLOCAL (&this->inet_addr_.in6_.sin6_addr) ||
      IN6_IS_ADDR_MC_LINKLOCAL (&this->inet_addr_.in6_.sin6_addr)) &&
      this->inet_addr_.in6_.sin6_scope_id != 0)
    {
      char scope_buf[32];
      sprintf (scope_buf, "%%%u", this->inet_addr_.in6_.sin6_scope_id);
      if ((strlen (ch) + strlen (scope_buf)) < (size_t)len)
      {
        strcat (szAddr, scope_buf);
      }
    }
    return ch;
  }
  return inet_ntop (AF_INET,
    &this->inet_addr_.in4_.sin_addr,
    szAddr,
    len);
}


inline CWX_UINT32 CwxINetAddr::getHostIp (void) const {
  if (this->getType () == AF_INET6)
  {
    if (IN6_IS_ADDR_V4MAPPED (&this->inet_addr_.in6_.sin6_addr) ||
      IN6_IS_ADDR_V4COMPAT (&this->inet_addr_.in6_.sin6_addr)    )
    {
      CWX_UINT32 addr;
      // Return the last 32 bits of the address
      char *thisaddrptr = (char*)this->ip_addr_pointer ();
      thisaddrptr += 128/8 - 32/8;
      memcpy (&addr, thisaddrptr, sizeof (addr));
      return CWX_NTOHL (addr);
    }
    errno = EAFNOSUPPORT;
    return 0;
  }
  return CWX_NTOHL (CWX_UINT32 (this->inet_addr_.in4_.sin_addr.s_addr));
}


/// Return @c true if the IP address is INADDR_ANY or IN6ADDR_ANY.
inline bool CwxINetAddr::isAny (void) const {
  if (this->getType () == AF_INET6)
    return IN6_IS_ADDR_UNSPECIFIED (&this->inet_addr_.in6_.sin6_addr);
  return (this->inet_addr_.in4_.sin_addr.s_addr == INADDR_ANY);
}

/// Return @c true if the IP address is IPv4/IPv6 loopback address.
inline bool CwxINetAddr::isLoopback (void) const {
  if (this->getType () == AF_INET6)
    return IN6_IS_ADDR_LOOPBACK (&this->inet_addr_.in6_.sin6_addr);
  // RFC 3330 defines loopback as any address with 127.x.x.x
  return ((this->getHostIp () & 0XFF000000) == (INADDR_LOOPBACK & 0XFF000000));
}

/// Return @c true if the IP address is IPv4/IPv6 multicast address.
inline bool CwxINetAddr::isMulticast (void) const {
  if (this->getType() == AF_INET6)
    return this->inet_addr_.in6_.sin6_addr.s6_addr[0] == 0xFF;
  return this->inet_addr_.in4_.sin_addr.s_addr >= 0xE0000000 &&  // 224.0.0.0
    this->inet_addr_.in4_.sin_addr.s_addr <= 0xEFFFFFFF; // 239.255.255.255
}


inline bool CwxINetAddr::operator == (const CwxINetAddr &item) const
{
  if (this->getType () != item.getType () || this->getSize () != item.getSize ())
    return false;
  return (memcmp (&this->inet_addr_,
    &item.inet_addr_,
    this->getSize ()) == 0);
}


/// Compare two addresses for inequality.
inline bool CwxINetAddr::operator != (const CwxINetAddr &item) const
{
  return !((*this) == item);
}


inline void * CwxINetAddr::ip_addr_pointer (void) const
{
  if (this->getType () == AF_INET)
    return (void*)&this->inet_addr_.in4_.sin_addr;
  else
    return (void*)&this->inet_addr_.in6_.sin6_addr;
}


inline void CwxINetAddr::reset (void) {
  memset (&this->inet_addr_, 0, sizeof (this->inet_addr_));
  if (this->getType() == AF_INET) {
    this->inet_addr_.in4_.sin_family = AF_INET;
  } else if (this->getType() == AF_INET6){
    this->inet_addr_.in6_.sin6_family = AF_INET6;
  }
}


CWINUX_END_NAMESPACE
