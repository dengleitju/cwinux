#ifndef __CWX_INET_ADDR_H__
#define __CWX_INET_ADDR_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxINetAddr.h
@brief 网络的地址对象的定义文件。
@author cwinux@gmail.com
@version 0.1
@date 2009-10-10
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxType.h"
#include "CwxNetMacro.h"
#include "CwxAddr.h"
#include "CwxSocket.h"
#include <netdb.h>

CWINUX_BEGIN_NAMESPACE
/**
@class CwxINetAddr
@brief 网络地址对象。
*/
class CWX_API CwxINetAddr:public CwxAddr
{
public:
    /// Default constructor.
    CwxINetAddr ();
    /// Copy constructor.
    CwxINetAddr (const CwxINetAddr &);
    /// Creates an CwxINetAddr from a sockaddr_in structure.
    CwxINetAddr (const sockaddr_in *addr, int len, CWX_INT32 iFamily = AF_UNSPEC);
    /// Creates an CwxINetAddr from a @a unPort and the remote
    /// @a szHost. The port number is assumed to be in host byte order.
    /// To set a port already in network byte order, please @see set().
    /// Use iFamily to select IPv6 (AF_INET6) vs. IPv4 (AF_INET).
    CwxINetAddr (CWX_UINT16 unPort, char const* szHost, CWX_INT32 iFamily = AF_UNSPEC);
    /**
    * Creates an CwxINetAddr from a @a unPort and an Internet
    * @a uiIpAddr.  This method assumes that @a unPort and @a uiIpAddr
    * are in host byte order. If you have addressing information in
    * network byte order, @see set().
    */
    CwxINetAddr (CWX_UINT16 unPort, CWX_INT32 iFamily = AF_INET);

    /// Uses getservbyname() to create an CwxINetAddr from a
    /// szPortName, the remote @a szHostName, and the @a protocol.
    CwxINetAddr (char const* szPortName, char const* szHostName, char const* protocol = "tcp");
    /// Default dtor.
    ~CwxINetAddr (void);
    /// Initializes from another CwxINetAddr.
    int set (const CwxINetAddr&);
    /// Creates an CwxINetAddr from a sockaddr_in structure.
    int set (const sockaddr_in *,  int len);
    /**
    * Initializes an CwxINetAddr from a @a unPort and the
    * remote @a szHost.
    * address_family can be used to select IPv4/IPv6 if the OS has
    * IPv6 capability . To specify IPv6, use
    * the value AF_INET6. To specify IPv4, use AF_INET.
    */
    int set (CWX_UINT16 unPort, char const* szHost, CWX_INT32 iFamily = AF_UNSPEC);
    /**
    * Initializes an CwxINetAddr from a @a unPort and an Internet
    * @a uiIpAddr.  If @a bEncode is true then the port number and IP address
    * are converted into network byte order, otherwise they are assumed to be
    * in network byte order already and are passed straight through.
    *
    * If bMap is true and IPv6 support has been compiled in,
    * then this address will be set to the IPv4-mapped IPv6 address of it.
    */
    int set (CWX_UINT16 unPort, CWX_INT32 iFamily = AF_INET);

    /// Uses getservbyname() to initialize an CwxINetAddr from a
    /// szPortName, the remote @a szHostName, and the @a protocol.
    int set (char const* szPortName, char const* szHostName,  char const* protocol = "tcp");

    /**
    * Uses getservbyname() to initialize an CwxINetAddr from a
    * szPortName, an @a uiIpAddr, and the @a protocol.  This assumes that
    * @a uiIpAddr is already in network byte order.
    */
    int set (char const* szPortName, char const* protocol = "tcp");
    /// Return a pointer to the underlying network address.
    virtual void *getAddr (void) const;
    /// Set a pointer to the address.
    virtual int setAddr (void *, CWX_INT32 iLen);
    /// set port
    void setPort (CWX_UINT16 unPort);
    /// Return the port number, converting it into host byte-order.
    CWX_UINT16 getPort(void) const;
    /**
    * For ipv4, return the "dotted decimal" Internet address representation of
    * the hostname; For ipv6, it's xx:xx:xx::. host is storied in the @a szAddr (which is assumed to be
    * @a len bytes long).  
    */
    char const* getHostIp (char* szAddr, int len) const;
    /// Return the 4-byte IP address, converting it into host byte
    /// order.
    CWX_UINT32 getHostIp (void) const;
    /// Return @c true if the IP address is INADDR_ANY or IN6ADDR_ANY.
    bool isAny (void) const;
    /// Return @c true if the IP address is IPv4/IPv6 loopback address.
    bool isLoopback (void) const;
    /// Return @c true if the IP address is IPv4/IPv6 multicast address.
    bool isMulticast (void) const;
    /// Compare two addresses for equality.  The addresses are considered
    /// equal if they contain the same IP address and port number.
    bool operator == (const CwxINetAddr &item) const;
    /// Compare two addresses for inequality.
    bool operator != (const CwxINetAddr &item) const;
    // the underlying internet address structure.
    void * ip_addr_pointer (void) const;
private:
    // Methods to gain access to the actual address of
    /// Initialize underlying inet_addr_ to default values
    void reset (void);
    /// Underlying representation.
    /// This union uses the knowledge that the two structures share the
    /// first member, sa_family (as all sockaddr structures do).
    union
    {
        sockaddr_in  in4_;
        sockaddr_in6 in6_;
    } inet_addr_;
};

CWINUX_END_NAMESPACE

#include "CwxINetAddr.inl"
#include "CwxPost.h"

#endif
