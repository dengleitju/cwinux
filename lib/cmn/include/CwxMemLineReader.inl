CWINUX_BEGIN_NAMESPACE

inline void CwxMemLineReader::setMem(char const* szData, CWX_UINT32 uiDataLen, bool bDataOwn)
{
    clear();
    m_szBuf = szData;
    m_uiMaxLen = uiDataLen;
    m_bDataOwn = bDataOwn;
}

inline char const* CwxMemLineReader::nextLine()
{
    if (m_uiCurLinePos >= m_uiMaxLen) return NULL;
    if (m_first)
    {
        m_uiCurLinePos = 0;
        m_uiNextLinePos = 0;
        m_uiCurLineNo = 0;
        m_first = false;
    }
    else
    {
        m_uiCurLinePos = m_uiNextLinePos;
    }
    while(m_uiNextLinePos < m_uiMaxLen)
    {
        if ('\n' == m_szBuf[m_uiNextLinePos++])
        {
            m_uiCurLineNo++;
            break;
        }
    }
    return &m_szBuf[m_uiCurLinePos];
}

inline CWX_UINT32 CwxMemLineReader::getCurLineNo() const
{
    return m_uiCurLineNo;
}

inline void CwxMemLineReader::seekHead()
{
    m_uiCurLineNo = 0;
    m_uiCurLinePos = 0;
    m_uiNextLinePos = 0;
    m_first = true;
}

inline char const* CwxMemLineReader::getCurLineBegin() const
{
    return &m_szBuf[m_uiCurLinePos];
}

inline char const* CwxMemLineReader::getCurLineEnd() const
{
    return &m_szBuf[m_uiNextLinePos];
}

inline void CwxMemLineReader::clear()
{
    m_uiCurLineNo = 0;
    m_uiCurLinePos = 0;
    m_uiNextLinePos = 0;
    if (m_bDataOwn && m_szBuf) delete [] m_szBuf;
    m_szBuf = NULL;
    m_uiMaxLen = 0;
    m_bDataOwn = false;
    m_first = true;
}

CWINUX_END_NAMESPACE
