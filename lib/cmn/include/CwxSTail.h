#ifndef __CWX_STAIL_H__
#define __CWX_STAIL_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
*@file  CwxSTail.h
*@brief 单向链表的模板类
*@author cwinux@gmail.com
*@version 1.0
*@date  2010-09-19
*@warning  无.
*/

#include "CwxPre.h"
#include "CwxGlobalMacro.h"

CWINUX_BEGIN_NAMESPACE

/**
* @class CwxSTail
*
* @brief 单向链表的模板类，对象必须有m_next的对象指针，指向下一个链表元素。
*/
template <typename CELL>
class CwxSTail
{
public:
    ///构造函数，若lock不为空，则加锁
    CwxSTail():m_count(0),m_head(NULL),m_tail(NULL)
    {
    }
    ///析构函数，若锁对象不为空，则解锁
    ~CwxSTail()
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
        cell->m_next = m_head;
        m_head = cell;
        if (!m_tail) m_tail = m_head;
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
            if (!m_head)
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
private:
    CWX_UINT32   m_count; ///<记录的数量
    CELL*       m_head; ///<链表的头
    CELL*       m_tail; ///<链表的尾
};

CWINUX_END_NAMESPACE

#include "CwxPost.h"
#endif

