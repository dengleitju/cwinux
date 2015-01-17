#ifndef __CWX_MSG_BLOCK_H__
#define __CWX_MSG_BLOCK_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxMsgBlock.h
@brief 通信数据收发的数据Block对象定义
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
#include "CwxLockGuard.h"
#include "CwxMutexLock.h"
#include "CwxMsgHead.h"
#include "CwxSingleton.h"
#include "CwxCondition.h"
CWINUX_BEGIN_NAMESPACE

class CwxMsgBlockAlloc;
class CwxMsgBlock;
class CwxMsgQueue;

/**
@class CwxMsgSendCtrl
@brief 通信数据发送的行为控制对象
*/
class CWX_API CwxMsgSendCtrl
{
public:
    enum{
        UNDO_CONN = 0,
        RESUME_CONN = 1,
        SUSPEND_CONN = 2
    };
    enum{
        FAIL_NOTICE = (1<<0),///<发送失败通知标志
        BEGIN_NOTICE = (1<<1),///<开始发送通知标记
        FINISH_NOTICE = (1<<2),///<发送结束通知标记
        CLOSE_NOTICE = (1<<3), ///<发送完毕后，关闭连接
        RECONN_NOTICE= (1<<4), ///<重新连接，对于被动连接则忽略
        FAIL_FINISH_NOTICE = FAIL_NOTICE|FINISH_NOTICE,///<消息发送完成与失败的时候通知
        ALL_NOTICE = FAIL_FINISH_NOTICE|BEGIN_NOTICE,///<设置全部标记，除了关闭、重连连接
        NONE = 0///<没有任何的标记
    };
public:
    ///检查是否消息发送前通知
    inline bool isBeginNotice() const;
    ///检查是否消息完毕通知
    inline bool isFinishNotice() const;
    ///检查是否消息失败通知
    inline bool isFailNotice() const;
public:
    ///设置基于连接的发送控制
    inline void setConnCtrl(CWX_UINT32 uiSvrId,
        CWX_UINT32 uiHostId,
        CWX_UINT32 uiConnId,
        CWX_UINT32 uiMsgAttr=FAIL_FINISH_NOTICE,
        void* userData=NULL,
        CWX_UINT32 uiConnState=UNDO_CONN);
    ///获取SVR ID
    inline CWX_UINT32 getSvrId() const;
    ///设置SVR ID
    inline void setSvrId(CWX_UINT32 uiSvrId);
    ///获取HOST ID
    inline CWX_UINT32 getHostId() const;
    ///设置HOST ID
    inline void setHostId(CWX_UINT32 uiHostId);
    ///获取CONN ID
    inline CWX_UINT32 getConnId() const ;
    ///设置CONN ID
    inline void setConnId(CWX_UINT32 uiConnId) ;
    ///获取SVR的缺省Host id
    inline CWX_UINT32 getDefHostId() const;
    ///设置发送SVR的缺省HOST ID
    inline void setDefHostId(CWX_UINT32 uiDefHostId);
    ///获取发送的属性
    inline CWX_UINT32 getMsgAttr() const ;
    ///设置发送的属性
    inline void setMsgAttr(CWX_UINT32 uiAttr) ;
    ///获取发送的user data
    inline void* getUserData() ;
    ///设置发送的user data
    inline void setUserData(void* userData) ;
    ///获取连接resume状态
    inline CWX_UINT32 getConnResumeState() const ;
    ///检查是否resume连接
    inline bool isResumeConn() const ;
    ///检查是否suspend连接
    inline bool isSuspendConn() const ;
    ///检查是否不执行undo连接
    inline bool isUndoConn() const ;
    ///设置连接resume状态
    inline void setConnResumeState(CWX_UINT32 uiState) ;
    ///设置重连的延时，单位为ms
    inline void setReconnDelay(CWX_UINT32 uiMiliSecond);
    ///获取重连的延时，单位为ms
    inline CWX_UINT32 getReconnDelay() const;
public:
    ///清空控制对象
    inline void reset();
private:
    ///禁止创建对象实例。只能有CwxMsgBlock对象创建
    CwxMsgSendCtrl();
    ~CwxMsgSendCtrl();
    friend class CwxMsgBlock;
private:
    void*                  m_userData; ///<userdata
    CWX_UINT32             m_uiSvrId; ///<service id
    CWX_UINT32             m_uiHostId; ///<host id
    CWX_UINT32             m_uiConnId;///<connection id
    CWX_UINT32             m_uiDefHostId; ///<缺省host id
    CWX_UINT32             m_uiMsgAttr; ///<msg's attribute for send
    CWX_UINT32             m_uiConnResumeState; ///<连接的suspend、resume状态
    CWX_UINT32             m_uiReconnDelay; ///<重新连接的延时，单位为毫秒
};


/**
@class CwxEventInfo
@brief 架构的事件类对象，定义系统的各种事件类型。此为commander消息分发的基础
*/
class CWX_API CwxEventInfo
{
public:
    ///事件类型定义
    enum {
        DUMMY = 0,///<dummy事件，此为无效事件类型
        CONN_CREATED = 1,///<连接建立事件
        CONN_CLOSED = 2,///<连接关闭事件
        RECV_MSG = 3,///<收到通信数据事件
        END_SEND_MSG = 4,///<通信数据包发送完毕事件
        FAIL_SEND_MSG = 5,///<通信数据包发送失败事件
        TIMEOUT_CHECK = 6,///<超时检查事件
        EVENT_4_HANDLE = 7,///<IO上注册的事件发生的事件
        SYS_EVENT_NUM = 8,///<系统事件的数量
    };
public:
    ///获取事件类型
    inline CWX_UINT16  getEvent() const ;
    ///设置事件类型
    inline void setEvent(CWX_UINT16 unEvent) ;
    ///获取事件对应的SVR-ID
    inline CWX_UINT32  getSvrId() const ;
    ///设置事件对应的SVR-ID
    inline void setSvrId(CWX_UINT32 uiSvrId) ;
    ///获取事件对应的HOST-ID
    inline CWX_UINT32 getHostId() const ;
    ///设置事件对应的HOST-ID
    inline void setHostId(CWX_UINT32 uiHostId) ;
    ///获取事件对应的连接ID
    inline CWX_UINT32 getConnId() const;
    ///设置事件对应的连接ID
    inline void setConnId(CWX_UINT32 uiConnId) ;
    ///获取事件对应的连接的用户数据
    inline void* getConnUserData() ;
    ///设置事件对应的连接的用户数据
    inline void setConnUserData(void* userData) ;
    ///获取事件对应的TASK ID
    inline CWX_UINT32 getTaskId() const ;
    ///设置事件对应的TASK ID
    inline void setTaskId(CWX_UINT32 uiTaskId) ;
    ///获取事件对应的通信数据包的包头
    inline CwxMsgHead& getMsgHeader();
    ///设置事件对应的通信数据包的包头
    inline void setMsgHeader(CwxMsgHead const& header) ;
    ///获取事件对应连接的连接handle
    inline CWX_HANDLE getIoHandle() ;
    ///设置事件对应连接的连接handle
    inline void setIoHandle(CWX_HANDLE handle) ;
    ///获取事件对应的IO事件监测的掩码
    inline CWX_UINT16 getRegEventMask() const ;
    ///设置事件对应的IO事件监测的掩码
    inline void setRegEventMask(CWX_UINT16 unMask) ;
    ///获取事件对应的IO事件监测的IO事件
    inline CWX_UINT16 getReadyEvent() const ;
    ///设置事件对应的IO事件监测的IO事件
    inline void setReadyEvent(CWX_UINT16 unEvent) ;
    ///获取事件的时间戳
    inline CWX_UINT64 getTimestamp() const ;
    ///设置事件的时间戳
    inline void setTimestamp(CWX_UINT64 ullTimestamp) ;
    ///清空事件对象
    inline void reset();
private:
    ///禁止创建对象，其为CwxMsgBlock对象的附属对象
    CwxEventInfo();
    ///析构函数
    ~CwxEventInfo();
    ///禁止做赋值操作
    CwxEventInfo& operator=(CwxEventInfo const& );
    friend class CwxMsgBlock;
public:
    CWX_UINT32     m_uiArg;///<用户的UINT32类型的参数
    CWX_UINT64     m_ullArg;///<用户的UINT64类型的参数
    void*          m_pArg;///<用户的指针类型的参数
private:
    CWX_UINT16     m_unEvent;///<事件类型
    CWX_UINT32     m_uiSvrId;///<事件对应的SVR ID
    CWX_UINT32     m_uiHostId;///<事件对应的HOST ID
    CWX_UINT32     m_uiConnId;///<事件对应的连接ID
    void*          m_connUserData;///<连接上的userData
    CWX_UINT32     m_uiTaskId;///<事件对应TASK的Task-ID
    CwxMsgHead   m_msgHeader;///<事件对应通信数据包的包头
    CWX_HANDLE     m_ioHandle;///<事件的IO HANDLE
    CWX_UINT16     m_unRegEventMask;///<事件的IO 事件的掩码
    CWX_UINT16     m_unReadyHandleEvent;///<事件的IO 事件的事件
    CWX_UINT64     m_ullTimeStamp;///<事件的时间戳
};

/**
@class CwxMsgBlock
@brief 通信数据收发的数据Block对象。对象有三部分组成：
第一部分：接收或发送的网络通信数据包，此为数据block。
第二部分：接收数据的Event对象，用来保存收到数据包的连接信息等信息，其信息用于Command对象Event事件分发。
第三部分：发送数据的MsgInfo对象，用来包含消息发送的svr-id、host-id、conn-id及其他发送行为的控制信息。
此外，为了防止内存碎片，CwxMsgBlock只能有CwxMsgBlockAlloc对象分配、释放。
*/
class CWX_API CwxMsgBlock
{
public:
    ///获取Block内存数据块的当前读位置
    inline char *rd_ptr (void) const;
    ///将Block内存数据块的读位置前移n个字节
    inline void rd_ptr (size_t n) ;
    ///获取Block内存数据库的当前写位置
    inline char *wr_ptr (void) const ;
    ///将Block内存数据块的写位置前移n个字节
    inline void wr_ptr(size_t n) ;
    ///获取Block数据块中数据的大小，为写指针与读指针的内存位置差。
    inline size_t length(void) const ;
    ///获取Block数据库的空间大小
    inline size_t capacity(void) const ;
    ///将Block的读写指针移到Block的开头位置
    inline void reset() ;
    ///获取与Block相关联的Event信息，用户可以设置其中的内容。在发送信息的时候，不会修改此内容。
    inline CwxEventInfo& event() ;
    ///获取与Block相关的发送Msginfo信息。其设置有架构负责，用户只能在消息发送失败或完成的时候，查看此信息
    inline CwxMsgSendCtrl& send_ctrl();
public:
    CwxMsgBlock*          m_next;///<下一个Block对象
private:
    friend class CwxMsgBlockAlloc;
    friend class CwxMsgQueue;
    ///析构函数，禁止delete
    ~CwxMsgBlock();
    ///指定数据库大小的构造函数，禁止new
    CwxMsgBlock(size_t size);
    ///不分配Block数据块的构造函数
    CwxMsgBlock();
private:
    char*                   m_buf; ///<内存的buf
    size_t                  m_rdPos; ///<内存的读位置
    size_t                  m_wrPos; ///<内存的写位置
    size_t                  m_len;   ///<内存的大小
    CwxEventInfo            m_event;///<与收到数据包相关的连接及环境信息对象
    CwxMsgSendCtrl          m_sendCtrl;///<数据包发送时的连接及发送控制对象
    bool                    m_bFree; ///<是否被CwxMsgBlockAlloc释放
};

/**
@class CwxMsgBlockAlloc
@brief CwxMsgBlock的分配、释放及空闲block的cache对象。
当分配数据块的时候，若block大小小于1M，则CwxMsgBlockAlloc按
256、512、....1M的离散大小进行分配；若大于1M，则分配的内存按照4K的边界对齐。
当释放数据块的时候，若Block大小小于1M而且Cache的block没有超过指定的上线，则按照256、512、....1M，
cache释放的对象，以供再次循环使用。若大于1M，则直接是否。
*/
class CWX_API CwxMsgBlockAlloc:public CwxSingleton
{
public:
    ///BLOCK 分配、cache控制参数定义
    enum{
        CACHE_MIN_BLOCK_BITS = 8,///<最小的block的大小，为2^CACHE_MIN_BLOCK_BITS 
        CACHE_MIN_BLOCK_SIZE = 1<<CACHE_MIN_BLOCK_BITS,///<最小的block的大小
        CACHE_MAX_BLOCK_BITS = 20,///<CACHE的最大的block的大小，为2^CACHE_MIN_BLOCK_BITS
        CACHE_MAX_BLOCK_SIZE = 1<<CACHE_MAX_BLOCK_BITS,///<CACHE的最大的block的大小
        CACHE_BLOCK_SIZE_SCALE = CACHE_MAX_BLOCK_BITS - CACHE_MIN_BLOCK_BITS + 1, ///<cache的block大小不同的数量
        MAX_SCALE_FREE_MEM_MSIZE = 4,///<每种大小类型的block，最多cache的总内存量
        MAX_SCALE_FREE_BLOCK_NUM = 1024, ///<每种大小类型的block，最多cache的个数
        BLOCK_ALIGN_BIT = 12 ///<大小超过1M的block，按4K进行边界对齐。
    };
public:
    ///创建signleton CwxEscapes 对象
    static CwxMsgBlockAlloc* instance();
    ///<分配大小为size的block
    static CwxMsgBlock* malloc(size_t size);
    ///<克隆msg被copy内容
    static CwxMsgBlock* clone(CwxMsgBlock* block);
    ///<释放block
    static void free(CwxMsgBlock* block);
    static CwxMsgBlock* pack(CwxMsgHead& header,
        char const* szData,
        CWX_UINT32 uiDateLen);
    ///释放空间
    static void destroy();
private:
    ///禁止创建实例
    CwxMsgBlockAlloc();
    ///禁止释放实例
    ~CwxMsgBlockAlloc();
private:
    static CwxMsgBlockAlloc*   m_pInstance;
    static CwxMutexLock    m_lock;///<thread lock for sync.
    CWX_UINT16       m_arrCacheNum[CACHE_BLOCK_SIZE_SCALE];///<cache的各种尺寸大小的block的数量
    CwxMsgBlock*   m_arrCacheBlock[CACHE_BLOCK_SIZE_SCALE];///<cache的各种尺寸大小的block的链表
};
CWINUX_END_NAMESPACE

#include "CwxMsgBlock.inl"
#include "CwxPost.h"
#endif
