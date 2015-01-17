#ifndef __CWX_TYPE_POOL_EX_H__
#define __CWX_TYPE_POOL_EX_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxTypePoolEx.h
@brief 定义可中途释放的内存池对象模板：CwxTypePoolEx。
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
#include <new>

CWINUX_BEGIN_NAMESPACE

/**
@class CwxTypePoolEx
@brief 可分配、可释放的内存池模板类。
*/
template <typename T > 
class CwxTypePoolEx
{
private:
    ///内置数据类型，为类型T的对象增加一个CwxTypePoolExItem*的指针m_next
    class CwxTypePoolExItem
    {
    private:
        ///永不创建的对象
        CwxTypePoolExItem()
            :m_next(NULL)
        {

        }
        ~CwxTypePoolExItem()
        {

        }
    public:
        T      m_data;
        CwxTypePoolExItem*     m_next;
    };
    enum{
        MIN_POOL_OBJECT_NUM = 64///<每个内存块中最小的对象的数量
    };
private:
    vector<char*> m_vector;///<内存块的vector
    CWX_UINT32    m_uiSize;///<分配的对象的数量
    CWX_UINT32    m_uiCapacity;///<分配的内存块的总对象数量
    CWX_UINT32     m_uiPoolSize;///<每个内存块的大小
    CwxTypePoolExItem*  m_freeList;///<空闲对象列表
public:
    ///构造函数并设置每个内存块的对象数量
    CwxTypePoolEx(CWX_UINT32 uiPoolSize=MIN_POOL_OBJECT_NUM)
    {
        m_uiSize = 0;
        m_uiCapacity = 0;
        m_uiPoolSize = uiPoolSize;
        if (m_uiPoolSize < MIN_POOL_OBJECT_NUM) m_uiPoolSize = MIN_POOL_OBJECT_NUM;
        m_freeList = NULL;
    }
    ///析构函数
    ~CwxTypePoolEx()
    {
        clear();
    }
public:
    ///从内存池中分配一个对象
    T* malloc()
    {
        CwxTypePoolExItem* obj = NULL;
        if (!m_freeList) createFreeObj();
        if (!m_freeList) return NULL;
        obj = m_freeList;
        m_freeList = m_freeList->m_next;
        obj->m_next = (CwxTypePoolExItem*)-1;///used sign
        m_uiSize++;
        return new(obj) T();
    }
    ///从内存池中分配一个对象，并设置其初始值。此时的对象必须定义拷贝构造
    T* malloc(T const& item)
    {
        CwxTypePoolExItem* obj = NULL;
        if (!m_freeList) createFreeObj();
        if (!m_freeList) return NULL;
        obj = m_freeList;
        m_freeList = m_freeList->m_next;
        obj->m_next = (CwxTypePoolExItem*)-1;///used sign
        m_uiSize++;
        return new(obj) T(item);
    }
    ///释放一个对象
    void free(T* obj)
    {
        obj->~T();
        CwxTypePoolExItem* pItem = (CwxTypePoolExItem*)obj;
        pItem->m_next = m_freeList;
        m_freeList = pItem;
        m_uiSize--;
    }
    ///获取分配的对象的数量
    CWX_UINT32 size() const
    {
        return m_uiSize;
    }
    ///获取空闲对象的数量
    unsigned int freeSize() const
    {
        return m_uiCapacity - m_uiSize;
    }
    ///获取内存池当前的对象容量
    unsigned int capacity() const
    {
        return m_uiCapacity;
    }
    ///释放所有内存池的内存
    void clear()
    {
        CwxTypePoolExItem* pObj = NULL;
        CWX_UINT32 uiPoolIndex = m_vector.size();
        for (CWX_UINT32 i=0; i<uiPoolIndex; i++)
        {
            pObj = (CwxTypePoolExItem*)m_vector[i];
            for (CWX_UINT32 j=0; j<m_uiPoolSize; j++)
            {
                if (pObj->m_next == (CwxTypePoolExItem*)-1) ((T*)pObj)->~T();
                pObj++;
            }
            ::free(m_vector[i]);
        }
        m_vector.clear();
        m_uiCapacity = 0;
        m_uiSize = 0;
        m_freeList = NULL;
    }
private:
    ///创建新的对象内存块
    void createFreeObj()
    {
        char* szTrunk = (char*)::malloc(sizeof(CwxTypePoolExItem) * m_uiPoolSize);
        if (szTrunk)
        {
           m_vector.push_back(szTrunk);
           CwxTypePoolExItem* pItem = (CwxTypePoolExItem*)szTrunk;
           CWX_UINT32 i=0;
           for (i=0; i<m_uiPoolSize-1; i++)
           {
               pItem[i].m_next = &pItem[i+1];
           }
           pItem[i].m_next = m_freeList;
           m_freeList = pItem;
           m_uiCapacity += m_uiPoolSize;
        }
    }
};

CWINUX_END_NAMESPACE

#endif
