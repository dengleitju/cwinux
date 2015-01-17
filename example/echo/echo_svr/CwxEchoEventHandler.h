#ifndef __CWX_ECHO_EVENT_HANDLER_H__
#define __CWX_ECHO_EVENT_HANDLER_H__
/*
版权声明：
    本软件遵循GNU GPL V3（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/
#include "CwxCommander.h"

CWINUX_USING_NAMESPACE

class CwxEchoApp;
///echo请求的处理handle，为command的handle
class CwxEchoEventHandler : public CwxCmdOp 
{
public:
    ///构造函数
    CwxEchoEventHandler(CwxEchoApp* pApp):m_pApp(pApp)
    {
        m_ullMsgNum = 0;
    }
    ///析构函数
    virtual ~CwxEchoEventHandler()
    {

    }
public:
    ///收到echo请求的处理函数
    virtual int onRecvMsg(CwxMsgBlock*& msg,///<echo数据包及相关的请求连接信息
                            CwxTss* pThrEnv///<处理线程的thread-specific-store
                            );
private:
    CwxEchoApp*     m_pApp;  ///<app对象
    CWX_UINT64      m_ullMsgNum;
};

#endif 
