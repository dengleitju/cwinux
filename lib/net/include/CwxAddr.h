#ifndef __CWX_ADDR_H__
#define __CWX_ADDR_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxAddr.h
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
#include <net/if.h>

CWINUX_BEGIN_NAMESPACE

/**
* @class CwxAddr
*
* @brief Defines the base class for the "address family independent"
* address format.
*/
class CWX_API CwxAddr
{
public:
    ///构造函数
    CwxAddr (CWX_INT32 iType = -1, CWX_INT32 iSize = -1);
    /// 析构
    virtual ~CwxAddr (void);
    ///获取地址的size
    CWX_INT32 getSize(void) const;
    ///设置地址的size.
    void setSize (CWX_INT32 iSize);
    ///获取地址类型
    CWX_INT32 getType (void) const;
    /// 设置地址类型
    void setType (CWX_INT32 iType);
    ///返回地址对象.
    virtual void * getAddr (void) const;
    ///设置地址对象.
    virtual int setAddr (void *, CWX_INT32 iLen);
    ///检查地址是否相等.
    bool operator == (const CwxAddr &sap) const;
    ///检查地址是否不相等.
    bool operator != (const CwxAddr &sap) const;
    ///Initializes instance variables.
    void baseSet(CWX_INT32 iType, CWX_INT32 iSize);
    /// Wild-card address.
    static const CwxAddr sap_any;
private:
    /// e.g., AF_UNIX, AF_INET, AF_SPIPE, etc.
    CWX_INT32  m_iAddrType;
    ///Number of bytes in the address.
    CWX_INT32  m_iAddrSize;
};


CWINUX_END_NAMESPACE

#include "CwxAddr.inl"
#include "CwxPost.h"

#endif
