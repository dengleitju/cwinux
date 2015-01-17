#ifndef __CWX_APP_CONN_INFO_H__
#define __CWX_APP_CONN_INFO_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
*@file  CwxAppConnInfo.h
*@brief 定义了CwxAppConnInfo
*@author cwinux@gmail.com
*@version 0.1
*@date  2010-07-30
*@warning  无.
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxType.h"
#include "CwxMsgHead.h"
#include "CwxAppMacro.h"
#include "CwxAppConfig.h"
#include "CwxStl.h"
#include "CwxStlFunc.h"
#include "CwxLogger.h"
#include "CwxNetMacro.h"

CWINUX_BEGIN_NAMESPACE
class CwxAppHandler4Msg;
/**
@class CwxAppConnInfo
@brief 连接对象，保存连接的状态、类型及数据收发等信息。
*/
class CWX_API CwxAppConnInfo{
public:
    ///连接状态定义
    enum{
        IDLE = 0,      ///<初始化的状态
        CONNECTING,    ///<连接中的状态，此有connector设置，可转换为ESTABLISHING、FAILED
        TIMEOUT,       ///<等待超时状态，也就是等待下一次重新连接。可转换为ESTABLISHING、FAILED
        ESTABLISHING,  ///<等待建立，可转换为ESTABLISHED、FAILED。
        ESTABLISHED,   ///<连接已经建立的状态，可转换为FAILED
        FAILED         ///<连接失败的状态，可转换为CONNECTING、TIMEOUT
    };
public:
    /**
    @brief 构造函数
    */
    CwxAppConnInfo();
    ///析构函数
    ~CwxAppConnInfo();
public:
    ///获取连接的SVR-ID
    CWX_UINT32 getSvrId() const; 
    ///设置连接的SVR-ID
    void setSvrId(CWX_UINT32 uiSvrId);

    ///获取连接的HOST-ID
    CWX_UINT32 getHostId() const;
    ///设置连接的HOST-ID
    void setHostId(CWX_UINT32 uiHostId);

    ///获取连接的连接ID
    CWX_UINT32 getConnId() const; 
    ///设置连接的连接ID
    void setConnId(CWX_UINT32 uiConnId);

    ///获取被动连接的Listen ID
    CWX_UINT32 getListenId() const;
    ///设置被动连接的Listen ID
    void setListenId(CWX_UINT32 uiListenId);
    ///获取连接超时时间
    CWX_UINT32 getConnectTimeout() const;
    ///设置连接超时时间
    void setConnectTimeout(CWX_UINT32 uiTimeout);
    ///获取连接的状态
    CWX_UINT16 getState() const;
    ///设置连接的状态
    void setState(CWX_UINT16 unState);
    ///获取连接的创建时间
    time_t getCreateTime() const;
    ///设置连接的创建时间
    void setCreateTime(time_t ttTime);
    ///获取连续失败连接的次数
    CWX_UINT32 getFailConnNum() const;
    ///设置连接失败连接次数
    void setFailConnNum(CWX_UINT32 uiNum);
    ///增加连接失败连接次数
    CWX_UINT32 incFailConnNum();
    ///获取失效主动连接最小重连接时间间隔
    CWX_UINT16 getMinRetryInternal() const;
    ///设置失效主动连接最小重连接时间间隔
    void setMinRetryInternal(CWX_UINT16 unInternal);
    ///获取失效主动连接最大重连接时间间隔
    CWX_UINT16 getMaxRetryInternal() const;
    ///设置失效主动连接最大重连接时间间隔
    void setMaxRetryInternal(CWX_UINT16 unInternal);
    ///获取连接是否为主动连接
    bool isActiveConn() const;
    ///设置为主动连接
    void setActiveConn(bool bActive);
    ///获取连接是否主动关闭
    bool isActiveClose() const;
    ///设置连接时主动关闭
    void setActiveClose(bool bActive);
    ///获取连接的数据包是否有包头
    bool isRawData() const ;
    ///设置连接的数据包是raw格式
    void setRawData(bool bRaw);
    ///获取连接最新收到消息的时间
    time_t  getLastRecvMsgTime() const;
    ///设置连接最新收到消息的时间
    void setLastRecvMsgTime(time_t ttTime);
    ///获取连接最新发送消息的时间
    time_t  getLastSendMsgTime() const;
    ///设置连接最新发送消息的时间
    void setLastSendMsgTime(time_t ttTime);
    ///获取连接的用户数据
    void*  getUserData() const;
    ///设置连接的用户数据
    void setUserData(void* userData);
    ///获取连接等待发送的最大消息的数量，0表示没有限制
    CWX_UINT32 getMaxWaitingMsgNum() const;
    ///设置连接最大的等待发送的消息数量，默认0表示没有限制
    void setMaxWaitingMsgNum(CWX_UINT32 uiNum=0);
    ///判断是否连接待发送队列已满
    bool isWaitingMsgQueueFull() const;
    ///获取连接等待发送的消息的数量
    CWX_UINT32 getWaitingMsgNum() const;
    ///设置连接等待发送的消息的数量
    void setWaitingMsgNum(CWX_UINT32 uiNum);
    ///增加连接等待发送的消息的数量
    CWX_UINT32 incWaitingMsgNum();
    ///减少连接等待发送的消息的数量
    CWX_UINT32 decWaitingMsgNum();
    ///获取连接已经连续接收到的消息包的数量
    CWX_UINT32 getContinueRecvNum() const;
    ///设置连接已经连续收到的消息包的数量
    void setContinueRecvNum(CWX_UINT32 uiNum);
    ///获取连续发送的消息数量
    CWX_UINT32 getContinueSendNum() const;
    ///设置连续发送的消息数量
    void setContinueSendNum(CWX_UINT32 uiNum);
    ///判断断开的连接是否需要重连
    bool isNeedReconnect() const;
    ///是否调用CwxAppFramework::onCreate
    bool isInvokeCreate() const;
    ///设置是否调用CwxAppFramework::onCreate
    void setInvokeCreate(bool bInvoke);
    ///是否重新连接
    bool isReconn() const;
    ///设置是否重连
    void setReconn(bool bReconnect);
    ///获取重新连接延时的毫秒数
    CWX_UINT32 getReconnDelay() const;
    ///设置重新连接延时的毫秒数
    void setReconnDelay(CWX_UINT32 uiDelay);
    ///获取socket设置的function
    CWX_NET_SOCKET_ATTR_FUNC getSockFunc() const;
    ///设置socket设置的function
    void setSockFunc(CWX_NET_SOCKET_ATTR_FUNC fn);
    ///获取socket设置function的arg
    void* getSockFuncArg() const;
    ///设置socket设置function的arg
    void setSockFuncArg(void* arg);
    ///获取连接的family
    CWX_INT32 getFamily() const;
    ///设置连接的family
    void setFamily(CWX_INT32 family);
    ///获取连接对应的handler
    CwxAppHandler4Msg* getHandler();
    ///设置连接对应的handler
    void setHandler(CwxAppHandler4Msg*  pHandler);
public:
    ///将连接对象的状态信息复位
    void reset();
private:
    CWX_UINT32         m_uiSvrId;  ///<svr id
    CWX_UINT32         m_uiHostId; ///<host id
    CWX_UINT32         m_uiConnId;  ///<connection id
    CWX_UINT32         m_uiConnectTimeout; ///<连接超时时间单位为ms
    CWX_UINT32         m_uiListenId; ///<accept connection's  acceptor ID. for passive conn.
    CWX_UINT16         m_unState; ///<connection state.
    time_t             m_ttCreateTime;///<connection's create time
    CWX_UINT16          m_unMinRetryInternal; ///<min re-connect internal
    CWX_UINT16         m_unMaxRetryInternal; ///<max re-connect internal
    CWX_UINT32          m_uiFailConnNum; ///<count for failure connect
    bool               m_bActiveConn; ///< sign for active connection.
    bool               m_bActiveClose; ///< sign for close connection by user.
    bool               m_bRawData; ///< sign for raw data connection
    time_t             m_ttLastRecvMsgTime;///<last recv msg time
    time_t             m_ttLastSendMsgTime;///<last send msg time
    void*              m_pUserData; ///<user dat for connection
    CWX_UINT32         m_uiContinueRecvNum; ///<conintue recv msg num
    CWX_UINT32         m_uiContinueSendNum; ///<连续发送的最大数量
    CWX_UINT32         m_uiMaxWaitingMsgNum; ///<最大等待消息的数量
    CWX_UINT32         m_uiWaitingMsgNum;///<waiting msg num
    bool               m_bInvokeCreate; ///<是否在open的时候，调用CwxAppFramework::onCreate，默认调用
    bool               m_bReconn; ///<是否是重连
    CWX_UINT32         m_uiReconnDelay; ///<重连延时的毫秒数
    CWX_INT32          m_iFamily;  ///<AF_INET或AF_INET6
    CWX_NET_SOCKET_ATTR_FUNC m_fn; ///<socket 设置的function
    void*              m_fnArg; ////<socket 设置的function的arg 参数
    CwxAppHandler4Msg*  m_pHandler; ///<连接对应的Handler
};


CWINUX_END_NAMESPACE
#include "CwxAppConnInfo.inl"
#include "CwxPost.h"
#endif

