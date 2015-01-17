#ifndef __CWX_ERR_GUARD_H__
#define __CWX_ERR_GUARD_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxErrGuard.h
@brief 定义了CwxErrGuard类，确保errno不被覆盖。
@author cwinux@gmail.com
@version 0.1
@date 2010-07-10
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxType.h"

CWINUX_BEGIN_NAMESPACE

/**
@class CwxErrGuard
@brief 确保errno不被覆盖。
*/
class CWX_API CwxErrGuard
{
public:
    CwxErrGuard();
    ~CwxErrGuard();
private:
    int    m_errno;
};

CWINUX_END_NAMESPACE

#include "CwxErrGuard.inl"
#include "CwxPost.h"

#endif

