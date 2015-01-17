#ifndef __CWX_TYPE_POOL_H__
#define __CWX_TYPE_POOL_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxTypePool.h
@brief 定义只分配，不释放的内存池模板：CwxTypePool。
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
@class CwxTypePool
@brief 只分配，最后统一释放的内存池模板类。
*/
template <typename T > 
class CwxTypePool
{
public:
    enum{
        MIN_POOL_OBJECT_NUM = 64///<每个内存块中最小的对象的数量
    };
private:
    vector<char*>  m_vector;///<内存块的vector
    CWX_UINT32     m_uiSize;///<分配的对象的数量
    CWX_UINT32     m_uiCapacity;///<分配的内存块的总对象数量
    CWX_UINT32     m_uiPoolSize;///<每个内存块的大小
    CWX_UINT32     m_uiLeft;///<当前内存块剩余的空闲对象数量
    char*          m_curPool;///<当前的内存块
public:
    ///构造函数并设置每个内存块的对象数量
    CwxTypePool(CWX_UINT32 uiPoolSize=MIN_POOL_OBJECT_NUM)
    {
        m_uiSize = 0;
        m_uiCapacity = 0;
        m_uiLeft = 0;
        m_curPool = NULL;
        m_uiPoolSize = uiPoolSize>(CWX_UINT32)MIN_POOL_OBJECT_NUM?uiPoolSize:(CWX_UINT32)MIN_POOL_OBJECT_NUM;
    }
    ///析构函数
    ~CwxTypePool()
    {
        clear();
    }
public:
    ///从内存池中分配一个对象
    T* malloc()
    {
        if (!m_uiLeft)
        {//new pool
            m_curPool = (char*)::malloc(sizeof(T) * m_uiPoolSize);
            if (NULL == m_curPool) return NULL;
            m_vector.push_back(m_curPool);
            m_uiCapacity += m_uiPoolSize;
            m_uiLeft = m_uiPoolSize;
		}
        T* obj = new(m_curPool) T();
        m_uiLeft --;
        m_curPool += sizeof(T);
        m_uiSize ++;
        return obj;
    }
    ///从内存池中分配一个对象，并设置其初始值。此时的对象必须定义拷贝构造
    T* malloc(T const& item)
    {
        if (!m_uiLeft)
        {//new pool
            m_curPool = (char*)::malloc(sizeof(T) * m_uiPoolSize);
            if (NULL == m_curPool) return NULL;
            m_vector.push_back(m_curPool);
            m_uiCapacity += m_uiPoolSize;
            m_uiLeft = m_uiPoolSize;
        }
        T* obj = new(m_curPool) T(item);
        m_uiLeft --;
        m_curPool += sizeof(T);
        m_uiSize ++;
        return obj;
    }
    ///获取当前分配的总的对象的数量
    CWX_UINT32 size() const
    {
        return m_uiSize;
    }
    ///获取当前分配的内存块可容纳的总的对象的数量
    CWX_UINT32 capacity() const
    {
        return m_uiCapacity;
    }
    ///获取第uiIndex对象，返回值：NULL：不存在；否则为第uiIndex个对象地址
    T* at(CWX_UINT32 uiIndex)
    {
        if (uiIndex >= m_uiSize) return NULL;
        CWX_UINT32 uiPoolIndex = uiIndex/m_uiPoolSize;
        CWX_UINT32 uiPos = uiIndex%m_uiPoolSize;
        return ((T*)m_vector[uiPoolIndex]) + uiPos;
    }
    ///释放内存池中的对象及分配的内存块
    void clear()
    {
        T* pObj = NULL;
        CWX_UINT32 uiPoolIndex = m_uiSize/m_uiPoolSize;
        CWX_UINT32 uiPos = m_uiSize%m_uiPoolSize;
        for (CWX_UINT32 i=0; i<uiPoolIndex; i++)
        {
            pObj = (T*)m_vector[i];
            for (CWX_UINT32 j=0; j<m_uiPoolSize; j++)
            {
                pObj->~T();
                pObj++;
            }
            free(m_vector[i]);
        }
        if (uiPos)
        {
            m_curPool = m_vector[uiPoolIndex];
        }
        else
        {
            m_curPool = NULL;
        }
        m_vector.clear();

        pObj = (T*)m_curPool;
        if (m_curPool)
        {
            for (CWX_UINT32 j=0; j<uiPos; j++)
            {
                pObj->~T();
                pObj++;
            }
            if (m_curPool) free(m_curPool);
            m_curPool = NULL;
        }
        m_uiCapacity = 0;
        m_uiLeft = 0;
        m_uiSize = 0;
    }
};


CWINUX_END_NAMESPACE

#endif
