#ifndef __CWX_THREAD_POOL_EX_H__
#define __CWX_THREAD_POOL_EX_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxThreadPoolEx.h
@brief 可以控制单个线程的线程池对象实现
@author cwinux@gmail.com
@version 0.1
@date 2009-07-20
@warning
@bug
*/

#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxType.h"
#include "CwxMsgBlock.h"
#include "CwxTpi.h"
#include "CwxThread.h"
#include "CwxTss.h"


CWINUX_BEGIN_NAMESPACE

/**
@class CwxThreadPoolEx
@brief 可以控制线程池每个线程的线程池。
       此线程池本身就是一个生产、消费者队列。
*/
class CWX_API CwxThreadPoolEx:public CwxTpi
{
public :
    ///构造函数
    CwxThreadPoolEx(CWX_UINT16 unThreadNum,///<线程池中线程的数量
        CwxCommander* commander,///<队列消息消费的缺省commander，若指定func可以不指定
        CWX_TSS_THR_FUNC func=NULL, ///<用户的线程main函数
        void*            arg=NULL
        );
    ///析构函数
    ~CwxThreadPoolEx();
public:
    /**
    @brief 启动线程池
    @param [in] pThrEnv 线程池的线程Tss的数组，若不指定，可通过线程的onThreadCreated创建。
    @param [in] stack_size 线程堆栈的大小，若为0，则采用系统默认大小。
    @return -1：失败； 0：成功
    */
    virtual int start(CwxTss** pThrEnv=NULL, size_t stack_size = 0);
    ///停止线程池
    virtual void stop();
    ///check thread 是否停止。若需要改变检查的规则，则重载此API
    virtual bool isStop();
    ///获取线程的TSS，及Thread env
    virtual CwxTss* getTss(CWX_UINT16 unThreadIndex);
    ///锁住整个线程池。返回值0：成功；-1：失败
    virtual int lock();
    ///解锁这个线程池。返回值0：成功；-1：失败
    virtual int unlock();
public:
    ///获取线程的消息队列排队消息数
    inline size_t getQueuedMsgNum();
    ///获取线程的消息队列排队消息数
    inline size_t getQueuedMsgNum(CWX_UINT16 unThreadIndex);
    /**
    @brief 往线程的消息队列添加一个新消息。
    @param [in] pMsg append的消息
    @param [in] uiThread 消息的线程队列，内部会根据Thread的数量，计算出所属的线程。
    @return -1：失败；>=0为对应线程的队列中排队的消息数量
    */
    inline int append(CwxMsgBlock* pMsg, CWX_UINT32 uiThread);
    /**
    @brief 往线程的消息队列头添加一个新消息，以便及时消费。
    @param [in] pMsg append的消息
    @return -1：失败；>=0队列中排队的消息数量
    */
    int  appendHead(CwxMsgBlock* pMsg, CWX_UINT32 uiThread);

protected:
    CwxCommander*          m_commander; ///<commander
    CWX_TSS_THR_FUNC       m_func; ///<用户指定的thread main function
    void*                  m_arg; ///<线程的参数
    CwxThread**            m_threadArr;  ///<thead的数组
};


CWINUX_END_NAMESPACE

#include "CwxThreadPoolEx.inl"
#include "CwxPost.h"

#endif

