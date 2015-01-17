#ifndef __CWX_APP_HANDLER_4_MSG_H__
#define __CWX_APP_HANDLER_4_MSG_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxAppHandler4Msg.h
@brief 定义基于msg收发的TCP，PIPE、及其他IO通信、管理的Handle对象CwxAppHandler4Msg。
@author cwinux@gmail.com
@version 0.1
@date 2009-07-20
@warning
@bug
*/

#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxMsgBlock.h"
#include "CwxMsgHead.h"
#include "CwxSocket.h"
#include "CwxAppConfig.h"
#include "CwxAppMacro.h"
#include "CwxLogger.h"
#include "CwxAppHandler4Base.h"
#include "CwxAppConnInfo.h"
#include "CwxAppReactor.h"

CWINUX_BEGIN_NAMESPACE
/**
@class CwxAppHandler4Msg
@brief TCP，PIPE、及其他IO通信、状态管理的Handle对象
*/
class CwxAppFramework;
class CwxAppReactor;

class CWX_API CwxAppHandler4Msg:public CwxAppHandler4Base
{
public:
    ///构造函数
    CwxAppHandler4Msg(CwxAppFramework* pApp,
        CwxAppReactor *reactor);
    ///析构函数
    ~CwxAppHandler4Msg();
public:
    /**
    @brief 初始化建立的连接，并往Reactor注册连接
    @param [in] arg 建立连接的acceptor或为NULL
    @return -1：放弃建立的连接； 0：连接建立成功
    */
    virtual int open (void * arg= 0);
    /**
    @brief 接受连接上的事件
    @param [in] event 连接的handle上的事件
    @param [in] handle  发生的事件的handle。
    @return -1：处理失败； 0：处理成功
    */
    virtual int handle_event(int event, CWX_HANDLE handle=CWX_INVALID_HANDLE);
    ///handle close
    virtual int close(CWX_HANDLE handle=CWX_INVALID_HANDLE);
    /**
    @brief 获取连接的对端地址
    @param [in,out] szBuf 返回地址的buf,获取成功后以\\0结束。
    @param [in] unSize szBuf的大小。
    @return 返回szBuf
    */
    virtual char* getRemoteAddr(char* szBuf, CWX_UINT16 unSize);
    /**
    @brief 获取连接的对端port
    @return 连接对端的port
    */
    virtual CWX_UINT16 getRemotePort();
    /**
    @brief 获取连接本端的地址
    @param [in,out] szBuf 返回地址的buf,获取成功后以\\0结束。
    @param [in] unSize szBuf的大小。
    @return 返回szBuf
    */
    virtual char* getLocalAddr(char* szBuf, CWX_UINT16 unSize);
    /**
    @brief 获取连接的本端port
    @return 连接对端的port
    */
    virtual CWX_UINT16 getLocalPort();
    ///发送消息
    virtual int handle_output();
    ///接收消息
    virtual int handle_input();
    ///超时
    virtual void handle_timeout();
public:
    ///获取连接的信息对象
    CwxAppConnInfo& getConnInfo();
    ///获取连接的信息对象
    CwxAppConnInfo const& getConnInfo() const;
    ///清空对象
    void clear();
    ///获取下一个待发送的消息，返回值：0，没有待发送信息；1,获得了一个待发送消息
    inline int getNextMsg();
    ///对要发送的消息排队，返回值：true：成功；false：失败，失败时因为队列已满
    inline bool putMsg(CwxMsgBlock* msg);
    ///由于没有消息发送，使连接的发送监测休眠.返回值， -1: failure, 0: success
    inline int cancelWakeup();
    ///唤醒连接的可写监控，以发送未发送完毕的数据.返回值， -1:failure； 0:success。
    inline int wakeUp();
    ///检查是否suspend连接的可读、可写监测
    bool isStopListen() const;
    ///设置stop listen
    void setStopListen(bool bStop);
    ///是否有可发送的数据包
    bool isEmpty() const;
    ///获取连接时的错误代码
    int getConnErrNo() const;
    ///获取app
    CwxAppFramework* getApp()
    {
        return m_pApp;
    }
private:
    ///以非阻塞的方式，发送消息。返回值,-1: failure; 0: not send all;1:send a msg
    inline int nonBlockSend();
protected:
    CwxAppFramework* m_pApp;
    CwxMsgHead          m_header;
    CwxAppConnInfo         m_conn;///<connection information
    CWX_UINT32             m_uiSendByte; ///the sent bytes number for current message.
    CwxMsgBlock*         m_curSndingMsg; ///<current sending msg;
    CwxMsgBlock*         m_waitSendMsgHead; ///<The header for wait to be sent msg.
    CwxMsgBlock*         m_waitSendMsgTail;   ///<The tail for wait to be sent msg.
    char                  m_szHeadBuf[CwxMsgHead::MSG_HEAD_LEN];///<the buf for header
    CWX_UINT32             m_uiRecvHeadLen; ///<recieved msg header's byte number.
    CWX_UINT32             m_uiRecvDataLen; ///<recieved data's byte number.
    CwxMsgBlock*        m_recvMsgData; ///<the recieved msg data
    bool                  m_bStopListen; ///<stop listen for passive connection
    int                   m_connErrNo;
};
CWINUX_END_NAMESPACE

#include "CwxAppHandler4Msg.inl"

#include "CwxPost.h"

#endif
