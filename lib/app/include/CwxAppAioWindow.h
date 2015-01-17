#ifndef __CWX_APP_AIO_WINDOW_H__
#define __CWX_APP_AIO_WINDOW_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxAppAioWindow.h
@brief 异步通信的消息发送、接收控制类的定义。类似TCP的滑窗
@author cwinux@gmail.com
@version 0.1
@date 2009-10-12
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxType.h"
#include "CwxStl.h"
#include "CwxStlFunc.h"

CWINUX_BEGIN_NAMESPACE
/**
@class CwxAppAioWindow
@brief 异步通信的消息发送、接收控制类，类似TCP的滑窗。
*/
class CWX_API CwxAppAioWindow
{
public:
    ///连接状态定义
    enum{
        STATE_CLOSED = 0,///<关闭状态
        STATE_CONNECTED = 1,///<连接建立状态
        STATE_SYNCING = 2///<数据发送状态
    };
    ///通信窗口控制参数
    enum{
        DEF_WINDOW_SIZE = 512,///<缺省的通信窗口的大小
        MAX_WINDOW_SIZE = 8192///<最大的通信窗口大小
    };
    /**
    @brief 构造函数
    @param [in] uiSvrId 连接的SVR-ID
    @param [in] uiHostId 连接的HOST-ID
    @param [in] uiConnId 连接的连接ID
    @param [in] uiWindowSize 连接的异步通信滑窗大小
    */
    CwxAppAioWindow(CWX_UINT32 uiSvrId,
        CWX_UINT32 uiHostId,
        CWX_UINT32 uiConnId,
        CWX_UINT32 uiWindowSize=DEF_WINDOW_SIZE)
    {
        m_pHandle = NULL;
        m_uiWindowSize = uiWindowSize;
        if (!m_uiWindowSize) m_uiWindowSize = 1;
        if (m_uiWindowSize > MAX_WINDOW_SIZE) m_uiWindowSize = MAX_WINDOW_SIZE;
        m_uiSvrId = uiSvrId;
        m_uiHostId = uiHostId;
        m_uiConnId = uiConnId;
        m_uiGroupId = 0;
        m_uiGroupNum = 0;
        m_uiState = STATE_CLOSED;
        m_ullStartSid = 0;
    };
    ///对象的拷贝构造
    CwxAppAioWindow& operator=(CwxAppAioWindow const& item)
    {
        if (&item != this)
        {
            m_pHandle = item.m_pHandle;
            m_uiSvrId = item.m_uiSvrId;
            m_uiHostId = item.m_uiHostId;
            m_uiConnId = item.m_uiConnId;
            m_uiGroupId = item.m_uiGroupId;
            m_uiGroupNum = item.m_uiGroupNum;
            m_uiState = item.m_uiState;
            m_ullStartSid = item.m_ullStartSid;
            m_strHostName = item.m_strHostName;
        }
        return *this;
    }
public:
    ///获取窗口的数据同步的句柄
    void*& getHandle()
    {
        return m_pHandle; 
    }
    ///设置窗口的数据同步的句柄
    void setHandle(void* pHandle)
    {
        m_pHandle = pHandle;
    }
    ///获取窗口连接的SVR-ID
    CWX_UINT32 getSvrId() const
    {
        return m_uiSvrId;
    }
    ///获取窗口连接的HOST-ID
    CWX_UINT32 getHostId() const 
    {
        return m_uiHostId;
    }
    ///获取窗口连接的CONN-ID
    CWX_UINT32 getConnId() const 
    {
        return m_uiConnId;
    }
    ///获取窗口连接的分组ID
    CWX_UINT32 getGroupId() const 
    {
        return m_uiGroupId;
    }
    ///设置窗口连接的分组ID
    void setGroupId(CWX_UINT32 uiGroup) 
    {
        m_uiGroupId = uiGroup;
    }
    ///获取窗口连接的分组数量
    CWX_UINT32 getGroupNum() const 
    {
        return m_uiGroupNum;
    }
    ///设置窗口连接的分组数量
    void setGroupNum(CWX_UINT32 uiNum) 
    {
        m_uiGroupNum = uiNum;
    }
    ///获取窗口连接的连接的状态
    CWX_UINT32 getState() const 
    {
        return m_uiState;
    }
    ///设置窗口连接的连接的状态
    void setState(CWX_UINT32 uiState) 
    {
        m_uiState = uiState;
    }
    ///设置窗口的初始同步的SID
    void setStartSid(CWX_UINT64 const& sid)
    {
        setState(STATE_SYNCING);
        m_ullStartSid = sid;
        m_sendSidSet.clear();
    }
    ///获取窗口的初始同步的SID
    CWX_UINT64 const& getStartSid() const
    {
        return m_ullStartSid;
    }
    ///设置窗口发送的SID。true：正确；false：消息已经发送
    bool sendOneMsg(CWX_UINT64 ullSid)
    {
        return m_sendSidSet.insert(ullSid).second?true:false;
    }
    ///设置窗口收到的SID。true：正确；false：此消息没有发送或次序错误
    bool recvOneMsg(CWX_UINT64 ullSid, bool bOrder=true/*是否应该有序返回*/) 
    {
        if (bOrder)
        {
            set<CWX_UINT64>::iterator iter = m_sendSidSet.begin();
            if ((iter != m_sendSidSet.end()) && (*iter == ullSid))
            {
                m_sendSidSet.erase(iter);
                return true;
            }
            return false;
        }
        return m_sendSidSet.erase(ullSid)?true:false;
    }
    ///获取下一个需要返回的消息。true：存在；false：不存在
    bool getNextRecvMsg(CWX_UINT64& ullSid)
    {
        set<CWX_UINT64>::iterator iter = m_sendSidSet.begin();
        if (iter != m_sendSidSet.end())
        {
            ullSid = *iter;
            return true;
        }
        return false;
    }
    ///判断窗口连接的连接是否关闭
    bool isClosed() const 
    {
        return STATE_CLOSED == m_uiState;
    }
    ///判断窗口连接的连接是否建立
    bool isConnected() const 
    {
        return STATE_CONNECTED == m_uiState;
    }
    ///判断窗口连接是否在数据发送状态
    bool isSyncing() const
    {
        return STATE_SYNCING == m_uiState;
    }
    ///判断是否有空闲的异步消息发送窗口
    bool isEnableSend() const 
    {
        return m_sendSidSet.size() < m_uiWindowSize;
    }
    ///获取窗口的大小
    CWX_UINT32 getWindowSize() const 
    {
        return m_uiWindowSize;
    }
    ///获取窗口被使用的大小
    CWX_UINT32 getUsedSize() const 
    {
        return m_sendSidSet.size();
    }
    ///获取所有未回复的消息的SID
    set<CWX_UINT64> const& getWaitingSid() const
    {
        return m_sendSidSet;
    }
    ///获取消息接收端的机器名字
    string const& getHostName() const
    {
        return m_strHostName;
    }
    ///设置消息接收端的机器名字
    void setHostName(string const& name)
    {
        m_strHostName = name;
    }
private:
    CWX_UINT32     m_uiWindowSize;///<窗口大小
    void*           m_pHandle;///<窗口数据异步发送的句柄
    CWX_UINT32      m_uiSvrId;///<窗口连接的SVR-ID
    CWX_UINT32      m_uiHostId;///<窗口连接的HOST ID
    CWX_UINT32      m_uiConnId;///<窗口连接的CONN ID
    CWX_UINT32      m_uiGroupId;///<窗口连接的分组ID
    CWX_UINT32      m_uiGroupNum;///<窗口连接的分组数量
    CWX_UINT32      m_uiState;///<窗口连接的状态
    CWX_UINT64      m_ullStartSid;///<窗口连接的起始同步SID
    set<CWX_UINT64> m_sendSidSet;///<待回复的数据同步SID
    string          m_strHostName;///<对端的机器名
};

///窗口连接的hash定义
typedef hash_map<CWX_UINT32/*conn_id*/, CwxAppAioWindow*, CwxNumHash<CWX_UINT32>, CwxNumHash<CWX_UINT32> > CwxAppAioWindowHash;

CWINUX_END_NAMESPACE

#endif
