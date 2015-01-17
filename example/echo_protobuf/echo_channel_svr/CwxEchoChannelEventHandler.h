#ifndef __CWX_ECHO_CHANNEL_EVENT_HANDLER_H__
#define __CWX_ECHO_CHANNEL_EVENT_HANDLER_H__
/*
版权声明：
    本软件遵循GNU GPL V3（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/

#include "CwxAppHandler4Channel.h"
#include "CwxAppChannel.h"

CWINUX_USING_NAMESPACE

///echo请求的处理handle，为command的handle
class CwxEchoChannelEventHandler : public  CwxAppHandler4Channel
{
public:
    ///构造函数
    CwxEchoChannelEventHandler(CwxAppChannel *channel):CwxAppHandler4Channel(channel)
    {
        m_ullMsgNum = 0;
        m_uiRecvHeadLen = 0; ///<recieved msg header's byte number.
        m_uiRecvDataLen = 0; ///<recieved data's byte number.
        m_recvMsgData = NULL; ///<the recieved msg data
    }
    ///析构函数
    virtual ~CwxEchoChannelEventHandler()
    {
        if (m_recvMsgData) CwxMsgBlockAlloc::free(m_recvMsgData);
    }
public:
    /**
    @brief 连接可读事件，返回-1，close()会被调用
    @return -1：处理失败，会调用close()； 0：处理成功
    */
    virtual int onInput();
    /**
    @brief 通知连接关闭。
    @return 对于主动连接，1：不从engine中移除注册；0：不从engine中移除注册但不删除handler；-1：从engine中将handle移除并删除。
    */
    virtual int onConnClosed();
private:
    void   replyMessage();

private:
    CWX_UINT64              m_ullMsgNum;
    CwxMsgHead             m_header;
    char                   m_szHeadBuf[CwxMsgHead::MSG_HEAD_LEN];
    CWX_UINT32             m_uiRecvHeadLen; ///<recieved msg header's byte number.
    CWX_UINT32             m_uiRecvDataLen; ///<recieved data's byte number.
    CwxMsgBlock*           m_recvMsgData; ///<the recieved msg data
};

#endif 
