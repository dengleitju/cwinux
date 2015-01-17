#ifndef __CWX_COMMANDER_H__
#define __CWX_COMMANDER_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxCommander.h
@brief 架构Command模式对象的定义，Command基于事件的Event-type，进行事件的分发
@author cwinux@gmail.com
@version 0.1
@date 2009-07-20
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxType.h"
#include "CwxStl.h"
#include "CwxStlFunc.h"
#include "CwxMsgBlock.h"
#include "CwxTss.h"

CWINUX_BEGIN_NAMESPACE

class CWX_API CwxCommander;
/**
@class CwxCmdOp
@brief 基于SVR-ID的事件的处理HANDLE的接口定义。
*/
class CWX_API CwxCmdOp
{
public:
    ///构造函数
    CwxCmdOp()
    {
    }
    ///析构函数
    virtual ~CwxCmdOp()
    {

    }
public:
    /**
    @brief 连接建立事件的处理函数。
    @param [in] msg 连接建立的事件对象。
    @param [in] pThrEnv 线程的TSS对象。
    @return -1：处理失败，0：不处理此事件，1：处理此事件。
    */
    virtual int onConnCreated(CwxMsgBlock*& msg, CwxTss* pThrEnv)
    {
        CWX_UNUSED_ARG(msg);
        CWX_UNUSED_ARG(pThrEnv);
        return 0;
    }
    /**
    @brief 连接关闭事件的的函数。
    @param [in] msg 连接关闭的事件对象。
    @param [in] pThrEnv 线程的TSS对象。
    @return -1：处理失败，0：不处理此事件，1：处理此事件。
    */
    virtual int onConnClosed(CwxMsgBlock*& msg, CwxTss* pThrEnv)
    {
        CWX_UNUSED_ARG(msg);
        CWX_UNUSED_ARG(pThrEnv);
        return 0;
    }
    /**
    @brief 收到通信数据包事件的处理函数。
    @param [in] msg 收到通信数据包的事件对象。
    @param [in] pThrEnv 线程的TSS对象。
    @return -1：处理失败，0：不处理此事件，1：处理此事件。
    */
    virtual int onRecvMsg(CwxMsgBlock*& msg, CwxTss* pThrEnv)
    {
        CWX_UNUSED_ARG(msg);
        CWX_UNUSED_ARG(pThrEnv);
        return 0;
    }
    /**
    @brief 消息发送完毕事件的处理函数。
    @param [in] msg 消息发送完毕的事件对象。
    @param [in] pThrEnv 线程的TSS对象。
    @return -1：处理失败，0：不处理此事件，1：处理此事件。
    */
    virtual int onEndSendMsg(CwxMsgBlock*& msg, CwxTss* pThrEnv)
    {
        CWX_UNUSED_ARG(msg);
        CWX_UNUSED_ARG(pThrEnv);
        return 0;
    }
    /**
    @brief 消息发送失败事件的处理函数。
    @param [in] msg 消息发送失败的事件对象。
    @param [in] pThrEnv 线程的TSS对象。
    @return -1：处理失败，0：不处理此事件，1：处理此事件。
    */
    virtual int onFailSendMsg(CwxMsgBlock*& msg, CwxTss* pThrEnv)
    {
        CWX_UNUSED_ARG(msg);
        CWX_UNUSED_ARG(pThrEnv);
        return 0;
    }
    /**
    @brief 超时检查事件的处理函数。
    @param [in] msg 超时检查的事件对象。
    @param [in] pThrEnv 线程的TSS对象。
    @return -1：处理失败，0：不处理此事件，1：处理此事件。
    */
    virtual int onTimeoutCheck(CwxMsgBlock*& msg, CwxTss* pThrEnv)
    {
        CWX_UNUSED_ARG(msg);
        CWX_UNUSED_ARG(pThrEnv);
        return 0;
    }
    /**
    @brief HANDLE的READY的事件的处理函数。
    @param [in] msg HANDLE的READY的事件的事件对象。
    @param [in] pThrEnv 线程的TSS对象。
    @return -1：处理失败，0：不处理此事件，1：处理此事件。
    */
    virtual int onEvent4Handle(CwxMsgBlock*& msg, CwxTss* pThrEnv)
    {
        CWX_UNUSED_ARG(msg);
        CWX_UNUSED_ARG(pThrEnv);
        return 0;
    }
    /**
    @brief 用户自定义事件的处理函数。
    @param [in] msg 用户自定义事件的事件对象。
    @param [in] pThrEnv 线程的TSS对象。
    @return -1：处理失败，0：不处理此事件，1：处理此事件。
    */
    virtual int onUserEvent(CwxMsgBlock*& msg, CwxTss* pThrEnv)
    {
        CWX_UNUSED_ARG(msg);
        CWX_UNUSED_ARG(pThrEnv);
        return 0;
    }
};

/**
@class CwxCommander
@brief Command类，基于事件的SVR-ID，实现事件与其处理Handle的映射。
*/
class  CWX_API CwxCommander
{
    ///消息映射函数类型定义
    typedef int (*fnEventApi)(CwxCmdOp* pEventOp, CwxMsgBlock*& msg, CwxTss* pThrEnv);
    ///SVR-ID与其处理Handle的映射Hash
    typedef hash_map<CWX_UINT32, CwxCmdOp*, CwxNumHash<CWX_UINT32> > CwxEventCommandHash;
public:
    ///构造函数
    CwxCommander()
        :m_msgHash(1024)
    {
        m_arrEvent[CwxEventInfo::DUMMY] = NULL;
        m_arrEvent[CwxEventInfo::CONN_CREATED] = &CwxCommander::onConnCreated;
        m_arrEvent[CwxEventInfo::CONN_CLOSED] = &CwxCommander::onConnClosed;
        m_arrEvent[CwxEventInfo::RECV_MSG] = &CwxCommander::onRecvMsg;
        m_arrEvent[CwxEventInfo::END_SEND_MSG] = &CwxCommander::onEndSendMsg;
        m_arrEvent[CwxEventInfo::FAIL_SEND_MSG] = &CwxCommander::onFailSendMsg;
        m_arrEvent[CwxEventInfo::TIMEOUT_CHECK] = &CwxCommander::onTimeoutCheck;
        m_arrEvent[CwxEventInfo::EVENT_4_HANDLE] = &CwxCommander::onEvent4Handle;
        m_arrEvent[CwxEventInfo::SYS_EVENT_NUM] = &CwxCommander::onUserEvent;
    }
    ///析构函数
    ~CwxCommander()
    {
        m_msgHash.clear();
    }
public:
    ///注册SVR-ID为uiSvrID的事件的处理函数。返回值，0:success, -1: 此SVR-ID已经存在
    int regHandle(CWX_UINT32 uiSvrID, CwxCmdOp* pHandle);
    /**
    @brief 将消息分发给其处理Handle
    @param [in] msg 要分发的事件
    @param [in] pThrEnv 线程的TSS对象。
    @param [in] iRet Handle对消息的处理结果，-1：处理失败，0：指定的Handle不处理此事件，1：处理成功。
    @return true：将消息分发给了指定的处理Handle；false：没有handle处理此消息
    */
    bool dispatch(CwxMsgBlock*& msg, CwxTss* pThrEnv, int& iRet);
    ///清空Command注册的SVR-ID
    void reset()
    {
        m_msgHash.clear(); 
    }
private:
    /**
    @brief 将连接建立事件分发给事件处理Handle。
    @param [in] pEventOp 事件的处理Handle。
    @param [in] msg 连接建立的事件对象。
    @param [in] pThrEnv 线程的TSS对象。
    @return -1：Handle处理失败，0：Handle不处理此事件，1：Handle成功处理此事件。
    */
    static int onConnCreated(CwxCmdOp* pEventOp, CwxMsgBlock*& msg, CwxTss* pThrEnv);
    /**
    @brief 将连接关闭事件分发给事件处理Handle。
    @param [in] pEventOp 事件的处理Handle。
    @param [in] msg 连接建立的事件对象。
    @param [in] pThrEnv 线程的TSS对象。
    @return -1：Handle处理失败，0：Handle不处理此事件，1：Handle成功处理此事件。
    */
    static int onConnClosed(CwxCmdOp* pEventOp, CwxMsgBlock*& msg, CwxTss* pThrEnv);
    /**
    @brief 将收到通信数据包事件分发给事件处理Handle。
    @param [in] pEventOp 事件的处理Handle。
    @param [in] msg 连接建立的事件对象。
    @param [in] pThrEnv 线程的TSS对象。
    @return -1：Handle处理失败，0：Handle不处理此事件，1：Handle成功处理此事件。
    */
    static int onRecvMsg(CwxCmdOp* pEventOp, CwxMsgBlock*& msg, CwxTss* pThrEnv);
    /**
    @brief 将通信数据包发送完毕事件分发给事件处理Handle。
    @param [in] pEventOp 事件的处理Handle。
    @param [in] msg 连接建立的事件对象。
    @param [in] pThrEnv 线程的TSS对象。
    @return -1：Handle处理失败，0：Handle不处理此事件，1：Handle成功处理此事件。
    */
    static int onEndSendMsg(CwxCmdOp* pEventOp, CwxMsgBlock*& msg, CwxTss* pThrEnv);
    /**
    @brief 将通信数据包发送失败事件分发给事件处理Handle。
    @param [in] pEventOp 事件的处理Handle。
    @param [in] msg 连接建立的事件对象。
    @param [in] pThrEnv 线程的TSS对象。
    @return -1：Handle处理失败，0：Handle不处理此事件，1：Handle成功处理此事件。
    */
    static int onFailSendMsg(CwxCmdOp* pEventOp, CwxMsgBlock*& msg, CwxTss* pThrEnv);
    /**
    @brief 将超时检查事件分发给事件处理Handle。
    @param [in] pEventOp 事件的处理Handle。
    @param [in] msg 连接建立的事件对象。
    @param [in] pThrEnv 线程的TSS对象。
    @return -1：Handle处理失败，0：Handle不处理此事件，1：Handle成功处理此事件。
    */
    static int onTimeoutCheck(CwxCmdOp* pEventOp, CwxMsgBlock*& msg, CwxTss* pThrEnv);
    /**
    @brief 将IO Handle Ready事件分发给事件处理Handle。
    @param [in] pEventOp 事件的处理Handle。
    @param [in] msg 连接建立的事件对象。
    @param [in] pThrEnv 线程的TSS对象。
    @return -1：Handle处理失败，0：Handle不处理此事件，1：Handle成功处理此事件。
    */
    static int onEvent4Handle(CwxCmdOp* pEventOp, CwxMsgBlock*& msg, CwxTss* pThrEnv);
    /**
    @brief 将用户事件分发给事件处理Handle。
    @param [in] pEventOp 事件的处理Handle。
    @param [in] msg 连接建立的事件对象。
    @param [in] pThrEnv 线程的TSS对象。
    @return -1：Handle处理失败，0：Handle不处理此事件，1：Handle成功处理此事件。
    */
    static int onUserEvent(CwxCmdOp* pEventOp, CwxMsgBlock*& msg, CwxTss* pThrEnv);
private:
    ///获取SVR-ID的处理Handle
    CwxCmdOp* getEventOp(CWX_UINT32 uiSvrID);
private:
    fnEventApi          m_arrEvent[CwxEventInfo::SYS_EVENT_NUM + 1];///事件类型与处理API的映射
    CwxEventCommandHash   m_msgHash;///<事件SVR-ID与事件处理Hanlde的映射
};

CWINUX_END_NAMESPACE

#include "CwxCommander.inl"
#include "CwxPost.h"
#endif

