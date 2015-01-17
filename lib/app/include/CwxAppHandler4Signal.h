#ifndef __CWX_APP_HANDLER_4_SIGNAL_H__
#define __CWX_APP_HANDLER_4_SIGNAL_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxAppHandler4Signal.h
@brief signal相应Handle
@author cwinux@gmail.com
@version 0.1
@date 2009-07-20
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxType.h"
#include "CwxAppMacro.h"
#include "CwxAppConfig.h"
#include "CwxAppHandler4Base.h"
#include "CwxAppReactor.h"

CWINUX_BEGIN_NAMESPACE

class CwxAppFramework;
/**
@class CwxAppHandler4Signal
@brief signal Handle
*/
class CWX_API CwxAppHandler4Signal:public CwxAppHandler4Base
{
public:
    ///构造函数
    CwxAppHandler4Signal(CwxAppFramework* pApp, CwxAppReactor* reactor, int sig);
    ///析构函数
    ~CwxAppHandler4Signal();
public:
    /**
    @brief handler open。往reactor的注册。
    @param [in] arg  无效参数
    @return -1：失败； 0：成功
    */
    virtual int open (void * arg= 0);
    /**
    @brief 接受signal的事件
    @param [in] event signal 事件
    @param [in] handle  发生的事件的handle。
    @return -1：处理失败； 0：处理成功
    */
    virtual int handle_event(int event, CWX_HANDLE handle=CWX_INVALID_HANDLE);
    ///handle close
    virtual int close(CWX_HANDLE handle=CWX_INVALID_HANDLE);
public:
    ///获取app
    CwxAppFramework* getApp()
    {
        return m_pApp;
    }
private:
    CwxAppFramework* m_pApp;

};

CWINUX_END_NAMESPACE
#include "CwxPost.h"

#endif
