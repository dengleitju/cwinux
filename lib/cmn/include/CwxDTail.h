#ifndef __CWX_DTAIL_H__
#define __CWX_DTAIL_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
*@file  CwxDTail.h
*@brief 双向链表的模板类
*@author cwinux@gmail.com
*@version 1.0
*@date  2010-09-19
*@warning  无.
*/

#include "CwxPre.h"
#include "CwxGlobalMacro.h"

CWINUX_BEGIN_NAMESPACE

/**
* @class CwxDTail
*
* @brief 双向链表的模板类，对象必须有m_next, m_prev的对象指针，分别指向下一个、前一个链表元素。
*/
template <typename CELL>
class CwxDTail
{
public:
    ///构造函数，若lock不为空，则加锁
    CwxDTail():m_count(0),m_head(NULL),m_tail(NULL)
    {
    }
    ///析构函数，若锁对象不为空，则解锁
    ~CwxDTail()
    {
    }
public:
    ///返回记录的数量
    inline CWX_UINT32 count() const
    {
        return m_count;
    }
    ///获取链表的头，并不从链表中删除。NULL表示没有
    inline CELL const* head() const
    {
        return m_head;
    }
    ///push一个元素到链表的头
    inline void push_head(CELL* cell)
    {
        m_count++;
        cell->m_prev = NULL;
        cell->m_next = m_head;
        if (m_head)
        {
            m_head->m_prev = cell;
        }
        else
        {
            m_tail = cell;            
        }
        m_head = cell;
    }
    ///从链表的头，pop一个元素。
    inline CELL* pop_head()
    {
        CELL* obj = NULL;
        if (m_head)
        {
            m_count--;
            obj = m_head;
            m_head = m_head->m_next;
            obj->m_next = NULL;
            if (m_head)
            {
                m_head->m_prev = NULL;
            }
            else
            {
                m_tail = NULL;
                m_count = 0;
            }
        }
        return obj;
    }
    ///获取链表的尾，并不从链表中删除。NULL表示没有
    CELL const* tail() const
    {
        return m_tail;
    }
    ///push一个元素到链表的尾
    void push_tail(CELL* cell)
    {
        count++;
        cell->m_next = NULL;
        cell->m_prev = m_tail;
        if (m_tail)
        {
            m_tail->m_next = cell;
        }
        else
        {
            m_head = cell;
        }
        m_tail = cell;
    }
    ///从链表的尾，pop一个元素
    CELL* pop_tail()
    {
        CELL* obj = NULL;
        if (m_tail)
        {
            m_count--;
            obj = m_tail;
            m_tail = m_tail->m_prev;
            obj->m_prev = NULL;
            if (m_tail)
            {
                m_tail->m_next =NULL;
            }
            else
            {
                m_head = NULL;
                m_count = 0;
            }
        }
        return obj;
    }
    ///删除一个元素
    inline void remove(CELL* cell)
    {
        if (cell == m_head)
        {
            pop_head();
        }
        else if (cell == m_tail)
        {
            pop_tail();
        }
        else
        {
            if (cell->m_prev && cell->m_next)
            {
                cell->m_prev->m_next = cell->m_next;
                cell->m_next->m_prev = cell->m_prev;
                cell->m_prev=cell->m_next=NULL;
                m_count--;
            }
        }
    }
    ///将一个元素移到头部
    inline void to_head(CELL* cell)
    {
        if (cell != m_head)
        {
            if (cell->m_next)
            {
                cell->m_prev->m_next = cell->m_next;
                cell->m_next->m_prev = cell->m_prev;
            }
            else
            {
                m_tail=m_tail->m_prev;
                m_tail->m_next = NULL;
            }
            cell->m_prev = NULL;
            cell->m_next = m_head;
            m_head = cell;
        }
    }
    ///将一个元素移到尾部
    inline void to_tail(CELL* cell)
    {
        if (cell != m_tail)
        {
            if (cell->m_prev)
            {
                cell->m_prev->m_next = cell->m_next;
                cell->m_next->m_prev = cell->m_prev;
            }
            else
            {
                m_head = m_head->m_next;
                m_head->m_prev = NULL;
            }
            cell->m_next = NULL;
            cell->m_prev = m_tail;
            m_tail = cell;
        }

    }
private:
    CWX_UINT32   m_count; ///<记录的数量
    CELL*       m_head; ///<链表的头
    CELL*       m_tail; ///<链表的尾
};

CWINUX_END_NAMESPACE

#include "CwxPost.h"
#endif

