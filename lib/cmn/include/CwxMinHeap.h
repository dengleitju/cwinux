#ifndef __CWX_MIN_HEAP_H__
#define __CWX_MIN_HEAP_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html）
*/
/************************************************************************
* 本代码Copy自Libevent，min_heap.h，我只是将其翻译成了模板类，一下是其license声明。
* Copyright (c) 2006 Maxim Yegorushkin <maxim.yegorushkin@gmail.com>
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
* 3. The name of the author may not be used to endorse or promote products
*    derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
* THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

************************************************************************/
/**
@file CwxMinHeap.h
@brief Min heap的实现，可以动态的增删元素。
@author cwinux@gmail.com
@version 1.0
@date 2011-03-02
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxType.h"

CWINUX_BEGIN_NAMESPACE
/**
@class CwxMinHeap
@brief 可以增加元素的min heap；
       TYPE对象必须有index(),index(CWX_INT32 index)的方法，以设置在heap array中的index。
       CMP为TYPE对象的比较函数，缺省为stl::less，
       对于CMP(T1,T2)的比较返回值来说，定义如下：
       T1<T2：返回true
       T1>=T2：返回false
*/
template <class TYPE, class CMP=greater<TYPE> >
class CWX_API CwxMinHeap
{
public:
    /**
    @brief 构造函数
    @param [in] num heap中最多的元素数量
    @param [in] cmp 比较函数
    */
    CwxMinHeap(CWX_UINT32 num, CMP const& cmp=greater<TYPE>()):m_cmp(cmp)
    {
        m_uiAll = num;
        m_uiCount = 0;
        m_pElement = NULL;
    }
    ///析构函数
    ~CwxMinHeap()
    {
        if (m_pElement) free(m_pElement);
    }
public:
    /**
    @brief 初始化HEAP
    @return -1：失败；0：成功
    */
    int init()
    {
        if (m_pElement) free(m_pElement);
        m_uiCount = 0;
        m_pElement = (TYPE**)malloc(m_uiAll * sizeof(TYPE*));
        return 0;
    }
    /**
    @brief 往HEAP中添加一个元素
    @param [in] e 添加的元素
    @return true：成功；false：内存不够而失败
    */
    bool push(TYPE* const& e)
    {
        if (-1 != e->index()) return false;
        if(!resize(m_uiCount+1))
            return false;
        shift_up(m_uiCount++, e);
        return true;
    }
    /**
    @brief 主动的从HEAP中POP一个最小的元素
    @return NULL：heap空；否则pop出最小的元素
    */
    TYPE* pop()
    {
        if(m_uiCount)
        {
            TYPE* e = m_pElement[0];
            shift_down(0u, m_pElement[--m_uiCount]);
            e->index(-1);
            return e;
        }
        return NULL;
    }
    /**
    @brief 从heap中删除一个元素
    @param [in] e 删除的元素
    @return true：成功；false：失败
    */
    bool erase(TYPE* e)
    {
        if(e->index()>=0)
        {
            TYPE *last = m_pElement[--m_uiCount];
            unsigned parent = (e->index() - 1) / 2;
            /* we replace e with the last element in the heap.  We might need to
            shift it upward if it is less than its parent, or downward if it is
            greater than one or both its children. Since the children are known
            to be less than the parent, it can't need to shift both up and
            down. */
            if (e->index() > 0 && m_cmp(*m_pElement[parent], *last))
                shift_up(e->index(), last);
            else
                shift_down(e->index(), last);
            e->index(-1);
            return 0;
        }
        return -1;
    }
    ///返回top的元素
    inline TYPE const* top() const
    {
        return m_uiCount?m_pElement[0]:NULL;
    }

    ///获取HEAP中元素的数量
    inline CWX_UINT32 count() const
    { 
        return m_uiCount;
    }
    ///true：heap为空；false：heap不为空。
    inline bool isEmpty() const 
    { 
        return 0==m_uiCount;
    }
    inline TYPE const* at(CWX_UINT32 index)
    {
        if (index >= m_uiCount) return NULL;
        return m_pElement[index];
    }
private:
    bool resize(CWX_UINT32 size)
    {
        if(m_uiAll < size)
        {
            TYPE ** p;
            CWX_UINT32 a = m_uiAll ? m_uiAll * 2 : 8;
            if(m_uiAll < size)
                a = size;
            if(!(p = (TYPE**)realloc(m_pElement, a * sizeof *p)))
                return false;
            m_pElement = p;
            m_uiAll = a;
        }
        return true;
    }

    void shift_up(CWX_UINT32 index, TYPE* e)
    {
        CWX_UINT32 parent = (index - 1) / 2;
        while(index && m_cmp(*m_pElement[parent], *e))
        {
            (m_pElement[index] = m_pElement[parent])->index(index);
            index = parent;
            parent = (index - 1) / 2;
        }
        (m_pElement[index] = e)->index(index);
    }

    void shift_down(CWX_UINT32 index, TYPE* e)
    {
        CWX_UINT32 min_child = 2 * (index + 1);
        while(min_child <= m_uiCount)
        {
            min_child -= min_child == m_uiCount || m_cmp(*m_pElement[min_child], *m_pElement[min_child - 1]);
            if(!(m_cmp(*e, *m_pElement[min_child])))
                break;
            (m_pElement[index] = m_pElement[min_child])->index(index);
            index = min_child;
            min_child = 2 * (index + 1);
        }
        shift_up(index,  e);
    }
private:
    CWX_UINT32        m_uiAll;///<heap中元素空间的总量
    CWX_UINT32        m_uiCount;///<heap中元素的数量
    TYPE**            m_pElement;///<heap的数组，大小为m_uiAll个
    CMP               m_cmp;///<比较函数对象
};

CWINUX_END_NAMESPACE

#include "CwxPost.h"


#endif
