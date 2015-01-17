#ifndef __CWX_SINGLETON_H__
#define __CWX_SINGLETON_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxSingleton.h
@brief 单实例对象接口的定义。
@author cwinux@gmail.com
@version 0.1
@date 2009-10-02
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxType.h"
#include "CwxStl.h"

CWINUX_BEGIN_NAMESPACE
class CwxSingletonMgr;
/**
@class CwxSingleton
@brief 单实例对象接口。
*/
class CWX_API CwxSingleton
{
public:
    inline string const& getName() const
    {
        return m_strName;
    }
protected:
    ///构造函数，将往CwxSingletonMgr进行注册, strName为对象的名字，以便调试
    CwxSingleton(string const& strName);
    ///析构函数，对象的实例将有CwxSingletonMgr释放
    virtual ~CwxSingleton();
    friend class CwxSingletonMgr;
private:
    CwxSingleton*      m_next;
    string             m_strName;

};

CWINUX_END_NAMESPACE


#include "CwxPost.h"

#endif
