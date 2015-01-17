#include "CwxCharPool.h"

CWINUX_BEGIN_NAMESPACE

char* CwxCharPool::malloc(CWX_UINT32 uiNum)
{
    char* pBuf = NULL;
    if (m_uiTailLeft < uiNum) 
    {
        if ((uiNum <= m_uiMemTrunkDataSize)&&m_pFreeHead)
        {//use from free list
            pBuf = m_pFreeHead;
            m_pFreeHead = getNext(m_pFreeHead);
            m_uiFreeSize -= m_uiMemTrunkSize;
            m_uiTailLeft = m_uiMemTrunkSize - TRUNK_HEAD_SIZE;
        }
        else
        {
            CWX_UINT32 uiTrunkSize = m_uiMemTrunkSize;
            if (uiNum > m_uiMemTrunkDataSize)
            {
                uiTrunkSize = ((uiNum + TRUNK_HEAD_SIZE + MEM_ALIGN_SIZE -1)/MEM_ALIGN_SIZE)*MEM_ALIGN_SIZE;
            }
            pBuf = new char[uiTrunkSize];
            setSize(pBuf, uiTrunkSize);
            m_uiTailLeft = uiTrunkSize - TRUNK_HEAD_SIZE;
        }
        setNext(pBuf, NULL);
        if (!m_pUsedHead)
        {
            m_pUsedHead = m_pUsedTail = pBuf;
        }
        else
        {
            setNext(m_pUsedTail, pBuf);
            m_pUsedTail = pBuf;
        }
    }
    CWX_ASSERT(m_uiTailLeft >= uiNum);
    pBuf = m_pUsedTail +  getSize(m_pUsedTail) - m_uiTailLeft;
    m_uiTailLeft -= uiNum;
    return pBuf;
}
void CwxCharPool::reset()
{
    char* pNext = NULL;
    while (m_pUsedHead)
    {
        pNext = getNext(m_pUsedHead);
        if ((m_uiFreeSize < m_uiKeepMemSize) && (getSize(m_pUsedHead) == m_uiMemTrunkSize))
        {
            setNext(m_pUsedHead, m_pFreeHead);
            m_pFreeHead = m_pUsedHead;
            m_uiFreeSize += m_uiMemTrunkSize;
        }
        else
        {
            m_uiTotalSize -= getSize(m_pUsedHead);
            delete [] m_pUsedHead;
        }
        m_pUsedHead = pNext;
    }
    m_pUsedHead = m_pUsedTail = NULL;
    m_uiTailLeft = 0;
}
void CwxCharPool::free()
{
    free(m_pUsedHead);
    m_pUsedHead = NULL;
    m_pUsedTail = NULL;
    free(m_pFreeHead);
    m_pFreeHead = NULL;
    m_uiTailLeft = 0;
    m_uiTotalSize = 0;
    m_uiFreeSize = 0;
}


CWINUX_END_NAMESPACE

