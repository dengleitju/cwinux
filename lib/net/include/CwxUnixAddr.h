#ifndef __CWX_UNIX_ADDR_H__
#define __CWX_UNIX_ADDR_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxUnixAddr.h
@brief UNIX domain的地址对象的定义文件。
@author cwinux@gmail.com
@version 0.1
@date 2009-10-10
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxType.h"
#include "CwxCommon.h"
#include "CwxNetMacro.h"
#include "CwxAddr.h"
#include "CwxSocket.h"
#include <sys/un.h>
CWINUX_BEGIN_NAMESPACE
/**
@class CwxUnixAddr
@brief Unix domain地址对象。
*/
class CWX_API CwxUnixAddr:public CwxAddr
{
public:
    ///默认构造函数
    CwxUnixAddr();
    /**
    @brief 建立szPathFile的Unix domain地址。
    @param [in] szPathFile Unix domain对应的文件。
    */
    CwxUnixAddr(char const* szPathFile);
    /**
    @brief 根据sockaddr_un的结构，创建Unix domain地址。
    @param [in] addr 地址结构。
    @param [in] len addr的长度。
    */
    CwxUnixAddr (sockaddr_un const* addr, int len);
    ///拷贝构造
    CwxUnixAddr(CwxUnixAddr const& addr);
    ///析构函数
    ~CwxUnixAddr();
public:
    /**
    @brief 对象赋值。
    @param [in] addr 赋值的对象。
    @return 0：成功；-1：失败。
    */
    int set (CwxUnixAddr const& addr);
    /**
    @brief 修改地址对应的PathFile。
    @param [in] szPathFile Unix domain 对应的path file。
    @return 0：成功；-1：失败。
    */
    int set(char const* szPathFile);
    /**
    @brief 根据sockaddr_un的结构数据，修改地址。
    @param [in] addr 地址结构。
    @param [in] len addr的长度。
    @return 0：成功；-1：失败。
    */
    int set(sockaddr_un const* addr, int len);
    /// Return a pointer to the underlying network address.
    virtual void *getAddr (void) const;
    /// Set a pointer to the underlying network address.
    virtual int setAddr (void *addr, int len);
public:
    /**
    @brief 获取Unix domain的path file。
    @return NULL：失败；NOT NULL：Unix domain的Path file。
    */
    char const* getPathFile() const;
    /// Compare two addresses for equality.
    bool operator == (const CwxUnixAddr & sap) const;

    /// Compare two addresses for inequality.
    bool operator != (const CwxUnixAddr & sap) const;

private:
    /// Underlying socket address.
    sockaddr_un unix_addr_;
};

CWINUX_END_NAMESPACE

#include "CwxUnixAddr.inl"
#include "CwxPost.h"

#endif
