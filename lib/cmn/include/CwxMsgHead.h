#ifndef __CWX_MSG_HEAD_H__
#define __CWX_MSG_HEAD_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
*@file  CwxMsgHead.h
*@brief CWINUX通信结构的协议头定义文件
*@author cwinux@gmail.com
*@version 0.1
*@date  2009-05-30
*@warning  无.
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxType.h"

CWINUX_BEGIN_NAMESPACE

class CwxMsgBlock;
/**
*@class  CwxMsgHead
*@brief  消息头打包、解包类
*/
class CWX_API CwxMsgHead
{
public:
    enum{
        ATTR_SYS_MSG = (1<<0), ///<系统消息, bit0
        ATTR_COMPRESS = (1<<1) ///<数据是压缩数据
    };
    enum{
        MSG_TYPE_KEEPALIVE = 1, ///<keep alive  msg 
        MSG_TYPE_KEEPALIVE_REPLY = 2 ///<keep alive reply msg 
    };
public:
    enum{
        MSG_HEAD_LEN = 14 ///<消息头的长度
    };    
    ///构造函数
    CwxMsgHead()
    {
        memset(this, 0x00, sizeof(CwxMsgHead));
    }
    CwxMsgHead(CWX_UINT8 ucVersion,
        CWX_UINT8 ucAttr,
        CWX_UINT16 unMsgType,
        CWX_UINT32 uiTaskId,
        CWX_UINT32 uiDateLen)
    {
        m_ucVersion = ucVersion;
        m_ucAttr = ucAttr;
        m_unMsgType = unMsgType;
        m_uiTaskId = uiTaskId;
        m_uiDataLen = uiDateLen;
    }

    ///拷贝构造
    CwxMsgHead(CwxMsgHead const& item)
    {
        memcpy(this, &item, sizeof(item));
    }
    ///赋值操作
    CwxMsgHead& operator=(CwxMsgHead const& item)
    {
        memcpy(this, &item, sizeof(item));
        return *this;
    }
    ///将消息头打包成网络字节序的数据包返回, 
    inline char const* toNet();
    ///将收到的消息头，解成消息头对象。
    /**
    *@param [in] szHead 消息包头
    *@return false:校验码错误；true:解包正确
    */
    inline bool fromNet(char const* szHead);
    ///是否keepalive消息
    inline bool isKeepAlive(bool bReply) const
    {
        if (bReply) return isAttr(ATTR_SYS_MSG) && (MSG_TYPE_KEEPALIVE_REPLY==m_unMsgType);
        return isAttr(ATTR_SYS_MSG) && (MSG_TYPE_KEEPALIVE==m_unMsgType);
    }
    ///是否是系统消息
    inline bool isSysMsg() const
    {
        return isAttr(ATTR_SYS_MSG);
    }
    ///形成KeepAlive消息返回
    /**
    *@param [in] bReply false：keepalive查询包；true：keepalive回复包。
    *@return keepalive的消息包。
    */
    CwxMsgBlock* packKeepalive(bool bReply);
    ///形成一个系统消息的返回消息，若为空，则不回复
    /**
    *@param [in] header 收到的系统消息的消息头
    *@param [in] msg 收到的系统消息的数据。
    *@return keepalive的消息包。
    */
    CwxMsgBlock* packSysMsgReply(CwxMsgHead const& header, CwxMsgBlock* msg);
    ///清空
    inline void reset()
    {
        memset(this, 0x00, sizeof(CwxMsgHead));
    }
public:
    ///检查是否设置了指定的属性
    inline bool isAttr(CWX_UINT8 ucAttr) const
    {
        return 0 != (m_ucAttr&ucAttr);
    }
    ///设置指定的属性
    inline void addAttr(CWX_UINT8 ucAttr)
    {
        m_ucAttr |= ucAttr;
    }
    ///清空指定的属性
    inline void clrAttr(CWX_UINT8 ucAttr)
    {
        m_ucAttr &= ~ucAttr;
    }
    ///set attr
    inline void setAttr(CWX_UINT8 ucAttr)
    {
        m_ucAttr = ucAttr;
    }
    ///get attr
    inline CWX_UINT8 getAttr() const
    {
        return m_ucAttr;
    }
    ///set verion
    inline void setVersion(CWX_UINT8 ucVersion)
    {
        m_ucVersion = ucVersion;
    }
    ///get version
    inline CWX_UINT8 getVersion() const
    {
        return m_ucVersion;
    }
    ///set msg type
    inline void setMsgType(CWX_UINT16 unMsgType)
    {
        m_unMsgType = unMsgType;
    }
    ///get msg type
    inline CWX_UINT16 getMsgType() const
    {
        return m_unMsgType;
    }
    ///set task id
    inline void setTaskId(CWX_UINT32 uiTaskId)
    {
        m_uiTaskId = uiTaskId;
    }
    ///get task id
    inline CWX_UINT32 getTaskId() const
    {
        return m_uiTaskId;
    }
    ///set data len，不包括包头
    inline void setDataLen(CWX_UINT32 uiDataLen)
    {
        m_uiDataLen = uiDataLen;
    }
    ///get data len，不包括包头
    inline CWX_UINT32 getDataLen() const
    {
        return m_uiDataLen;
    }
    ///设置package的最大大小
    static void setMaxMsgSize(CWX_UINT32 uiSize);
    ///获取package的最大大小
    static CWX_UINT32 getMaxMsgSize();
private:
    CWX_UINT8      m_ucVersion;  ///<消息版本号
    CWX_UINT8      m_ucAttr;     ///<消息属性
    CWX_UINT16     m_unMsgType;  ///<消息类型
    CWX_UINT32     m_uiTaskId;   ///<任务号ID
    CWX_UINT32     m_uiDataLen;  ///<发送的数据长度，不包括包头
    char           m_szHead[MSG_HEAD_LEN]; ///< msg's data
    static CWX_UINT32  m_uiMaxMsgSize;
};

CWINUX_END_NAMESPACE
#include "CwxMsgHead.inl"
#include "CwxPost.h"
#endif

