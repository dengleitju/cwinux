#ifndef __CWX_HEAP_H__
#define __CWX_HEAP_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxHeap.h
@brief 通过STL的HEAP 接口，实现最小heap。
@author cwinux@gmail.com
@version 0.1
@date 2009-10-10
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
@class CwxHeap
@brief 通过STL的HEAP 接口，实现最小heap模板类；<br>
TYPE为排序的对象类型，CMP为TYPE的比较函数对象。<br>
对于CMP(T1,T2)的比较返回值来说，定义如下,缺省为stl::less
T1<T2：返回true
T1>=T2：返回false
*/
template <class TYPE, class CMP=less<TYPE> >
class CWX_API CwxHeap
{
public:
    /**
    @brief 构造函数
    @param [in] num heap中最多的元素数量
    @param [in] cmp 比较函数
    */
    CwxHeap(CWX_UINT32 num, CMP const& cmp=less<TYPE>()):m_cmp(cmp)
    {
        m_uiNum = num;
        m_uiCurNum = 0;
        m_pElement = NULL;
        m_bHeap = true;
    }
    ///析构函数
    ~CwxHeap()
    {
        if (m_pElement) delete [] m_pElement;
    }
public:
    /**
    @brief 初始化HEAP
    @return -1：失败；0：成功
    */
    int init()
    {
        if (!m_uiNum) return -1;
        if (NULL == m_pElement) m_pElement = new TYPE[m_uiNum];
        m_uiCurNum = 0;
        m_bHeap = true;
        return 0;
    }
    /**
    @brief 往HEAP中添加一个元素，若HEAP以满，则溢出最小的一个元素
    @param [in] in 添加的元素
    @param [out] out 若HEAP已满，则溢出最小的元素
    @return true：溢出了最小的元素；false：没有元素从HEAP中溢出
    */
    bool push(TYPE const& in, TYPE& out)
    {
        CWX_ASSERT(m_bHeap);
        if (m_uiCurNum == m_uiNum)
        {
            if (!m_cmp(in, m_pElement[0]))
            {
                out = in;
                return true;
            }
            pop_heap(m_pElement, m_pElement + m_uiNum, m_cmp);
            out = m_pElement[m_uiNum - 1];
            m_pElement[m_uiNum - 1] = in;
            push_heap(m_pElement, m_pElement + m_uiNum, m_cmp);
            return true;
        }
        m_pElement[m_uiCurNum] = in;
        m_uiCurNum++;
        push_heap(m_pElement, m_pElement + m_uiCurNum, m_cmp);
        return false;
    }
    /**
    @brief 主动的从HEAP中POP一个最小的元素
    @param [out] out pop出的最小的元素
    @return true：pop出了一个最小的元素；false：heap已空
    */
    bool pop(TYPE& out)
    {
        CWX_ASSERT(m_bHeap);
        if (0 == m_uiCurNum) return false;
        pop_heap(m_pElement, m_pElement + m_uiCurNum, m_cmp);
        m_uiCurNum --;
        out = m_pElement[m_uiCurNum];
        return true;
    }
    ///对HEAP按照CMP对象的排序规则，按升序进行排序
    void sort()
    {
        sort_heap(m_pElement, m_pElement+m_uiCurNum, m_cmp);
        m_bHeap = false;
    }
    ///获取HEAP中的第index个元素，若返回NULL，表示元素不存在
    TYPE const* operator[](CWX_UINT32 index) const
    {
        if (index<m_uiCurNum) return m_pElement + index;
        return NULL;
    }
    ///获取HEAP中元素的数量
    CWX_UINT32 count() const
    { 
        return m_uiCurNum;
    }
    ///true：HEAP中的元素是HEAP的次序；false：HEAP中的元素不是HEAP的次序。
    bool isHeap() const 
    { 
        return m_bHeap;
    }
private:
    CWX_UINT32  m_uiNum;///<heap的容量
    CWX_UINT32  m_uiCurNum;///<heap中元素的数量
    TYPE*   m_pElement;///<heap的数组，大小为m_uiNum个
    bool    m_bHeap;///<m_pElement中的数据，是否是HEAP的次序
    CMP     m_cmp;///<比较函数对象
};

CWINUX_END_NAMESPACE

#include "CwxPost.h"


#endif
