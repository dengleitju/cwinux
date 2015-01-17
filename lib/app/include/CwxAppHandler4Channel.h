#ifndef __CWX_APP_HANDLER_4_CHANNEL_H__
#define __CWX_APP_HANDLER_4_CHANNEL_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxAppHandler4Channel.h
@brief Channel的io通信基类。
@author cwinux@gmail.com
@version 1.0
@date 2011-04-17
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
#include "CwxDTail.h"
#include "CwxMsgBlock.h"

CWINUX_BEGIN_NAMESPACE
/**
@class CwxAppHandler4Channel
@brief Channel的io通信基类
*/

class CwxAppChannel;

class CWX_API CwxAppHandler4Channel:public CwxAppHandler4Base
{
public:
    ///构造函数
    CwxAppHandler4Channel(CwxAppChannel *channel);
    ///析构函数
    virtual ~CwxAppHandler4Channel();
public:
    /**
    @brief 初始化建立的连接，并往Reactor注册连接
    @param [in] arg 建立连接的acceptor或为NULL
    @return -1：放弃建立的连接； 0：连接建立成功
    */
    virtual int open (void * arg= 0);
    /**
    @brief 连接可写事件，返回-1，close()会被调用
    @return -1：处理失败，会调用close()； 0：处理成功
    */
    virtual int onOutput();
    /**
    @brief 连接可读事件，返回-1，close()会被调用
    @return -1：处理失败，会调用close()； 0：处理成功
    */
    virtual int onInput();
    /**
    @brief 连接超时事件，返回-1，close()会被调用
    @return -1：处理失败，会调用close()； 0：处理成功
    */
    virtual int onTimeout(CwxTimeValue const& current);
    /**
    @brief Handler的redo事件，在每次dispatch时执行。
    @return -1：处理失败，会调用close()； 0：处理成功
    */
    virtual int onRedo();
    /**
    @brief 连接关闭
    @param [in] handle  连接handle。
    @return -1：处理失败； 0：处理成功
    */
    virtual int close(CWX_HANDLE handle=CWX_INVALID_HANDLE);
    /**
    @brief 通知连接开始发送一个消息。<br>
    只有在MSG指定BEGIN_NOTICE的时候才调用.
    @param [in] msg 要发送的消息。
    @return -1：取消消息的发送。 0：发送消息。
    */
    virtual int onStartSendMsg(CwxMsgBlock* msg);
    /**
    @brief 通知连接完成一个消息的发送。<br>
    只有在Msg指定FINISH_NOTICE的时候才调用.
    @param [in,out] msg 传入发送完毕的消息，若返回NULL，则msg有上层释放，否则底层释放。
    @return 
    CwxMsgSendCtrl::UNDO_CONN：不修改连接的接收状态
    CwxMsgSendCtrl::RESUME_CONN：让连接从suspend状态变为数据接收状态。
    CwxMsgSendCtrl::SUSPEND_CONN：让连接从数据接收状态变为suspend状态
    */
    virtual CWX_UINT32 onEndSendMsg(CwxMsgBlock*& msg);

    /**
    @brief 通知连接上，一个消息发送失败。<br>
    只有在Msg指定FAIL_NOTICE的时候才调用.
    @param [in,out] msg 发送失败的消息，若返回NULL，则msg有上层释放，否则底层释放。
    @return void。
    */
    virtual void onFailSendMsg(CwxMsgBlock*& msg);
    /**
    @brief 通知连接关闭。
    @return 1：不从engine中移除注册；0：从engine中移除注册但不删除handler；-1：从engine中将handle移除并删除。
    */
    virtual int onConnClosed();

public:
    ///清空对象
    void clear();
    ///获取下一个待发送的消息，返回值：0，没有待发送信息；1,获得了一个待发送消息
    inline int getNextMsg();
    ///对要发送的消息排队，返回值：true：成功；false：失败，失败时因为队列已满
    inline bool putMsg(CwxMsgBlock* msg);
    ///由于没有消息发送，使连接的发送监测休眠.返回值， -1: failure, 0: success
    int cancelWakeup();
    ///唤醒连接的可写监控，以发送未发送完毕的数据.返回值， -1:failure； 0:success。
    int wakeUp();
    ///是否有可发送的数据包
    bool isEmpty() const;
    ///获取channel
    CwxAppChannel* channel();
public:
    ///返回值：-1：读取失败；0：数据包没有读完；1：读取了一个数据包
    static int recvPackage(CWX_HANDLE handle, 
        CWX_UINT32& uiRecvHeadLen,
        CWX_UINT32& uiRecvDataLen,
        char*      szRecvHead,
        CwxMsgHead& header,
        CwxMsgBlock*& msg);
protected:
    /**
    @brief 接受连接上的事件
    @param [in] event 连接的handle上的事件
    @param [in] handle  发生的事件的handle。
    @return -1：处理失败； 0：处理成功
    */
    virtual int handle_event(int event, CWX_HANDLE handle=CWX_INVALID_HANDLE);
private:
    ///以非阻塞的方式，发送消息。返回值,-1: failure; 0: not send all;1:send a msg
    inline int nonBlockSend();
    friend class CwxAppChannel;
protected:
    CWX_UINT32             m_uiSendByte; ///the sent bytes number for current message.
    CwxMsgBlock*           m_curSndingMsg; ///<current sending msg;
    CwxMsgBlock*           m_waitSendMsgHead; ///<The header for wait to be sent msg.
    CwxMsgBlock*           m_waitSendMsgTail;   ///<The tail for wait to be sent msg.
    CwxAppChannel*         m_channel;
private:
    bool                   m_bRedo;
};
CWINUX_END_NAMESPACE

#include "CwxAppHandler4Channel.inl"

#include "CwxPost.h"

#endif
