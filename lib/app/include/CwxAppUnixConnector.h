#ifndef __CWX_APP_UNIX_CONNECTOR_H__
#define __CWX_APP_UNIX_CONNECTOR_H__

/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/


/**
@file CwxAppUnixConnector.h
@brief Unix-domain的连接管理对象，负责主动连接的建立与管理
@author cwinux@gmail.com
@version 0.1
@date 2009-07-20
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxAppConfig.h"
#include "CwxAppMacro.h"
#include "CwxUnixConnector.h"
#include "CwxAppHandler4UnixConn.h"
#include "CwxAppReactor.h"

CWINUX_BEGIN_NAMESPACE
class CwxAppFramework;
/**
@class CwxAppUnixConnector
@brief unix-domain的连接管理对象，负责主动连接的建立与管理
*/
class CWX_API CwxAppUnixConnector
{
public:
    ///构造函数
    CwxAppUnixConnector (CwxAppFramework* pApp, CwxAppReactor *reactor);
    ///析构函数
    ~CwxAppUnixConnector ();
public:
    //建立连接， -1：失败；0：等待连接；1：建立连接
    int connect (CwxAppHandler4UnixConn* pHandler,
        char const* szPathFile, ///<connect addr
        CWX_NET_SOCKET_ATTR_FUNC fn=NULL, ///<socket设置的function
        void* fnArg = NULL
        );
private:
    CwxAppFramework* m_pApp;
    CwxAppReactor * m_reactor;
};

CWINUX_END_NAMESPACE

#include "CwxPost.h"


#endif
