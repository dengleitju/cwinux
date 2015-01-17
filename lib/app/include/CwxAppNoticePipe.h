#ifndef __CWX_APP_NOTICE_PIPE_H__
#define __CWX_APP_NOTICE_PIPE_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
*@file  CwxAppNoticePipe.h
*@brief Framework的Notice消息的管道
*@author cwinux@gmail.com
*@version 0.1
*@date  2009-05-30
*@warning  无.
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxAppMacro.h"
#include "CwxAppConfig.h"
#include "CwxLockGuard.h"
#include "CwxMutexLock.h"
#include "CwxLogger.h"
#include <unistd.h>
#include <sys/socket.h>
CWINUX_BEGIN_NAMESPACE

/**
@class CwxAppNotice
@brief Framework的Notice对象，用于别的线程与通信线程的通信
*/
class CWX_API CwxAppNotice
{
public:
    ///notice类型定义
    enum{
        DUMMY = 0,///<dummy类型
        SEND_MSG_BY_CONN = 1,///<往连接发送消息
        SEND_MSG_BY_SVR = 2, ///<往指定的SVR分组发送消息
        TCP_CONNECT = 3,///<建立TCP连接
        UNIX_CONNECT = 4,///<建立UNIX DOMAIN连接
        ADD_IO_HANDLE = 5,///<IO HANDLE数据接收
        TCP_LISTEN = 6,///<TCP listen
        UNIX_LISTEN = 7,///<unix-domain listen
        CLOSE_LISTEN = 8,///<关闭监听
        CLOSE_CONNECT = 9,///<关闭连接
        RECONNECTION = 10,///<重新连接
        ALL_NOTICE_NUM = 11///<Notice类型的数量
    };
public:
    ///构造函数
    CwxAppNotice();
    ///清空对象
    void reset();
public:
    CWX_UINT16              m_unNoticeType;///<notice msg type
    void*                  m_noticeArg; ///<notice's arguement
    void*                  m_noticeArg1;///<notice's arguement2
    CwxAppNotice*           m_next;///<建立notice链表的指针
};


/**
*@class  CwxAppNoticePipe
*@brief  Framework的Notice消息的管道，建立通信线程与其他线程间的通信。
*/
class CWX_API CwxAppNoticePipe
{
public:
    ///构造函数
    CwxAppNoticePipe();
    ///析构函数
    ~CwxAppNoticePipe();
public:
    ///管道初始化。返回值：-1：失败；0：成功。
    int init();
    /**
    @brief 往管道对象push一个notice消息
    @param [in] pItem 通知的消息
    @return -1：写管道失败； 0：成功
    */
    int notice(CwxAppNotice* pItem);
    /**
    @brief 往管道对象push一个dummy notice消息，其目的是：若通信线程阻塞，则保证唤醒通信线程。
    @return -1：写管道失败； 0：成功
    */
    int noticeDummy();
    /**
    @brief 通信线程获取积压的Notice消息。
    @return void
    */
    void noticed(CwxAppNotice*& head);
    ///获取管道的读句柄
    CWX_HANDLE getPipeReader();
    ///获取管道的写句柄
    CWX_HANDLE getPipeWriter();
private:
    ///释放对象的资源
    void clear();
private:
    CwxMutexLock   m_lock;///<thread lock for sync.
    bool           m_bPipeEmpty; ///<notice pipe empty sign.
    CwxAppNotice*  m_noticeHead; ///<notice's head.
    CwxAppNotice*  m_noticeTail; ///<notice's tail
    CWX_HANDLE      m_pipeReader; ///<pipe handle for read
    CWX_HANDLE      m_pipeWriter; ///<pipe handle for write
};

CWINUX_END_NAMESPACE

#include "CwxAppNoticePipe.inl"
#include "CwxPost.h"

#endif
