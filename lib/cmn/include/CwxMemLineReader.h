#ifndef __CWX_MEM_LINE_READER_H__
#define __CWX_MEM_LINE_READER_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxMemLineReader.h
@brief 基于内存的行读取器类的定义
@author cwinux@gmail.com
@version 0.1
@date 2009-10-10
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxType.h"

CWINUX_BEGIN_NAMESPACE
/**
@class CwxMemLineReader
@brief 内存的行读取封装对象。行的分割时\\n
*/
class CWX_API CwxMemLineReader
{
public:
    ///构造函数
    CwxMemLineReader()
    {
        m_uiCurLineNo = 0;
        m_uiCurLinePos = 0;
        m_uiNextLinePos = 0;
        m_szBuf = NULL;
        m_uiMaxLen = 0;
        m_bDataOwn = false;
        m_first = true;
    }
    ///析构函数
    ~CwxMemLineReader()
    {
        clear();
    }
public:
    /**
    @brief 设置要行读取的内存
    @param [in] szData 行格式的数据BUFFER
    @param [in] uiDataLen 数据的长度
    @param [in] bDataOwn 内存是否由此对象实例进行释放
    @return void
    */
    void setMem(char const* szData, CWX_UINT32 uiDataLen, bool bDataOwn=false);
    /**
    @brief 将内存指针移到下一行的开始位置，并返回此位置,若为NULL，表示读完所有内存<br>
           对于第一次调用，则指向内存的开始位置，<br>
           若内存总共有N行，则前N次的调用，返回值不为NULL；第N+1次返回NULL。
    @return NULL：内存已经读完；否则，为下一行的开始位置。
    */
    char const* nextLine();
    ///获取当前的行号，从1开始编号
    CWX_UINT32 getCurLineNo() const;
    ///将读的指针，移到内存的头，与setMem()后的状态一致。
    void seekHead();
    ///获取当前行的头
    char const* getCurLineBegin() const;
    ///获取当前行的尾，但不包含当前的字符，此为下一行的头。
    char const* getCurLineEnd() const;
    ///清空对象
    void clear();
private:
    CWX_UINT32  m_uiCurLineNo;///<当前行号
    CWX_UINT32  m_uiCurLinePos;///<当前行在BUF中的位置
    CWX_UINT32  m_uiNextLinePos;///<下一行的开始在buf中的位置
    char const* m_szBuf;///<内存BUF
    CWX_UINT32  m_uiMaxLen;///<内存BUF的长度
    bool        m_bDataOwn;///<是否有对象负责BUF的空间释放
    bool        m_first;///<是否第一次调用nextLine() API
};

CWINUX_END_NAMESPACE

#include "CwxMemLineReader.inl"
#include "CwxPost.h"

#endif
