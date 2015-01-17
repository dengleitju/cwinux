#ifndef __CWX_CHAR_POOL_H__
#define __CWX_CHAR_POOL_H__

/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxCharPool.h
@brief 只分配，不释放的char pool，其目的是防止内存的碎片。
@author cwinux@gmail.com
@version 0.1
@date 2009-10-10
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxStl.h"
#include "CwxCommon.h"
#include "CwxGlobalMacro.h"

CWINUX_BEGIN_NAMESPACE
/**
@class CwxCharPool
@brief 只分配，不释放的char pool，其目的是防止内存的碎片。
*/
class CWX_API CwxCharPool
{
public:
    enum{
        DEF_MEM_TRUNK_SIZE = 4096,///<缺省内存块的大小
        DEF_KEEP_MEM_SIZE = 16 * DEF_MEM_TRUNK_SIZE,///<当reset的时候，持有的内存大小
        MEM_ALIGN_SIZE = 1024,///<内存块的对齐大小
        POINTER_SIZE = sizeof(void*),///<指针的字节数
        TRUNK_HEAD_SIZE = POINTER_SIZE + sizeof(CWX_UINT32)///<内存块头的大小
    };
public:
    /**
    @brief 构造函数。
    @param [in] uiMemTrunkPoolSize 内存块的大小
    @param [in] uiKeepMemSize 当reset的时候，持有的内存大小
    */
    CwxCharPool(CWX_UINT32 uiMemTrunkPoolSize=DEF_MEM_TRUNK_SIZE, CWX_UINT32 uiKeepMemSize = DEF_KEEP_MEM_SIZE)
    {
        m_uiMemTrunkSize = ((uiMemTrunkPoolSize+MEM_ALIGN_SIZE-1)/MEM_ALIGN_SIZE)*MEM_ALIGN_SIZE;
        m_uiMemTrunkDataSize = m_uiMemTrunkSize - TRUNK_HEAD_SIZE;
        m_uiKeepMemSize = uiKeepMemSize;
        if (m_uiKeepMemSize < m_uiMemTrunkSize) m_uiKeepMemSize = m_uiMemTrunkSize;
        m_pUsedHead = NULL;
        m_pUsedTail = NULL;
        m_pFreeHead = NULL;
        m_uiTailLeft = 0;
        m_uiTotalSize = 0;
        m_uiFreeSize = 0;
    }
    ///析构函数
    ~CwxCharPool()
    {
        free();
    }
    ///分配uiNum大小的内存
    char* malloc(CWX_UINT32 uiNum);
    ///是否内存池中多余的内存块
    void reset();
    ///释放所有的内存块
    void free();
private:
    ///释放内存块链表
    inline void free(char* pChain) const
    {
        char* pNext = pChain;
        while(pNext)
        {
            pChain = getNext(pNext);
            delete [] pNext;
            pNext = pChain;
        }
    }
    ///获取内存块链表的头内存块
    inline char* getNext(char const* pTrunk) const
    {
        char* pNext = NULL;
        memcpy(&pNext, pTrunk, POINTER_SIZE);
        return pNext;
    }
    ///将一个内存块放到内存块链表的头部
    inline void setNext(char* pTrunk, char const* pNext) const
    {
        memcpy(pTrunk, &pNext, POINTER_SIZE);
    }
    ///获取记录在内存块的头的内存大小信息
    inline CWX_UINT32 getSize(char const* pTrunk) const
    {
        CWX_UINT32 uiSize = 0;
        memcpy(&uiSize, pTrunk + POINTER_SIZE, sizeof(uiSize));
        return uiSize;
    }
    ///设置记录在内存块头的内存大小信息
    inline void setSize(char* pTrunk, CWX_UINT32 uiSize) const
    {
        memcpy(pTrunk + POINTER_SIZE, &uiSize, sizeof(uiSize));
    }

private:
    CWX_UINT32       m_uiMemTrunkSize;///<最小的内存块大小
    CWX_UINT32       m_uiKeepMemSize;///<当reset的时候持有的内存大小
    CWX_UINT32       m_uiMemTrunkDataSize;///<内存块中数据部分的大小
    char*           m_pUsedHead;///<使用的内存块的链表头
    char*           m_pUsedTail;///<使用的内存块的链表尾
    char*           m_pFreeHead;///<空闲的内存块链表的头
    CWX_UINT32      m_uiTailLeft;///<当前内存块的空闲内存大小
    CWX_UINT32      m_uiTotalSize;///<分配的内存块的内存总量
    CWX_UINT32      m_uiFreeSize;///<空闲的内存块的内存大小
};

CWINUX_END_NAMESPACE

#endif
