#ifndef __CWX_TAIL_MACRO_H__
#define __CWX_TAIL_MACRO_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxTailMacro.h
@brief 定义链表操作的宏定义
@author cwinux@gmail.com
@version 0.1
@date 2009-10-10
@warning
@bug
*/
#include "CwxPre.h"

///定义双向链表的入口
#define CWINUX_DTAIL_ENTRY(type)  \
type     *m_next;	/*<next item*/\
type     *m_prev	/*< previous next element */

///双向链表变量清空操作
#define CWINUX_DTAIL_RESET()\
    m_next = NULL;\
    m_prev = NULL

///定义前向链表的入口
#define CWINUX_STAIL_ENTRY(type)  \
type     *m_next	/*<next item*/

///前向链表变量的清空操作
#define CWINUX_STAIL_RESET()  \
    m_next = NULL	/*<next item*/


#include "CwxPost.h"

#endif