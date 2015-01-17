CWINUX_BEGIN_NAMESPACE

template<typename CHARSET>
inline bool CwxFilterMachine<CHARSET>::addStopWord(char const* szStopWord, CWX_UINT32 uiStopWordLen)
{
    if (uiStopWordLen)
    {
        CwxTrieNode<char>* pNode = m_stopwordTree.addKey(szStopWord, uiStopWordLen);
        if (!pNode) return false;
    }
    return true;
}

template<typename CHARSET>
inline bool CwxFilterMachine<CHARSET>::addCaseStopWord(char const* szStopWord, CWX_UINT32 uiStopWordLen)
{
    if (uiStopWordLen)
    {
        CwxTrieNode<char>* pNode = m_stopwordTree.addCaseKey(szStopWord, uiStopWordLen);
        if (!pNode) return false;
    }
    return true;
}

template<typename CHARSET>
inline bool CwxFilterMachine<CHARSET>::addFilterStr(char const* szFilterStr, CWX_UINT32 uiFilterLen, CWX_UINT8 ucFilterLevel)
{
    if (uiFilterLen)
    {
        CwxTrieNode<char>* pNode = m_filterTree.addKey(szFilterStr, uiFilterLen);
        if (!pNode) return false;
        pNode->setAttr(CWX_SET_ATTR(ucFilterLevel, pNode->getAttr()));
        if (m_ucMaxFilterLevel < maxFilterLevel(ucFilterLevel)) m_ucMaxFilterLevel = maxFilterLevel(ucFilterLevel);
        incFilterNum();
    }
    return true;
}

template<typename CHARSET>
inline bool CwxFilterMachine<CHARSET>::addCaseFilterStr(char const* szFilterStr, CWX_UINT32 uiFilterLen, CWX_UINT8 ucFilterLevel)
{
    if (uiFilterLen)
    {
        CwxTrieNode<char>* pNode = m_filterTree.addCaseKey(szFilterStr, uiFilterLen);
        if (!pNode) return false;
        pNode->setAttr(CWX_SET_ATTR(ucFilterLevel, pNode->getAttr()));
        if (m_ucMaxFilterLevel < maxFilterLevel(ucFilterLevel)) m_ucMaxFilterLevel = maxFilterLevel(ucFilterLevel);
        incFilterNum();
    }
    return true;
}

//0:no filter, otherwise, it's filter level
template<typename CHARSET>
inline CWX_UINT8 CwxFilterMachine<CHARSET>::filterStr(char const* szContent, CWX_UINT32 uiContentLen, CWX_UINT32& uiStart, CWX_UINT32& uiLen) const
{
    if (!getFilterNum()) return 0;
    uiStart = 0;
    uiLen = 0;
    CWX_UINT32 uiLocStart = 0;
    CWX_UINT8 ucFilterLevel = 0;
    CWX_UINT32 uiContentPos = 0;
    CWX_UINT32 uiCurContentPos = 0;
    CWX_UINT32 uiCheckLen = 0;
    CwxTrieNode<char> const* pNode = NULL;
    while(uiContentPos < uiContentLen)
    {
        pNode = m_filterTree.getRoot();
        uiCurContentPos = uiContentPos;
        uiLocStart = uiCurContentPos;
        while(uiCurContentPos < uiContentLen)
        {
            uiCheckLen = uiContentLen - uiCurContentPos;
            if (isStopWord(szContent + uiCurContentPos, uiCheckLen))
            {
                uiCurContentPos += uiCheckLen;
                if (m_filterTree.getRoot() == pNode) uiContentPos += uiCheckLen;
                continue;
            }
            uiCheckLen = CHARSET::nextChrLen(szContent + uiCurContentPos, uiContentLen - uiCurContentPos);
            if (0 == uiCheckLen) return ucFilterLevel;
            pNode = pNode->word(szContent + uiCurContentPos, uiCheckLen);
            if (pNode)
            {
                if (pNode->isKeyEnd())
                {
                    if (ucFilterLevel < maxFilterLevel(pNode->getAttr()))
                    {
                        ucFilterLevel = maxFilterLevel(pNode->getAttr());
                        uiStart = uiLocStart;
                        uiLen = uiCurContentPos + uiCheckLen - uiStart;
                        if (ucFilterLevel == m_ucMaxFilterLevel) return ucFilterLevel;
                    }
                }
                uiCurContentPos += uiCheckLen;
            }
            else
            {
                break;
            }
        }
        if (uiContentPos >= uiContentLen) break;
        uiCheckLen = CHARSET::nextChrLen(szContent + uiContentPos, uiContentLen - uiContentPos);
        if (0 == uiCheckLen) return ucFilterLevel;
        uiContentPos += uiCheckLen;
    }
    return ucFilterLevel;
}
//0:no filter, otherwise, it's filter level
template<typename CHARSET>
inline CWX_UINT8 CwxFilterMachine<CHARSET>::filterCaseStr(char const* szContent, CWX_UINT32 uiContentLen, CWX_UINT32& uiStart, CWX_UINT32& uiLen) const
{
    if (!getFilterNum()) return 0;
    uiStart = 0;
    uiLen = 0;
    CWX_UINT32 uiLocStart = 0;
    CWX_UINT8 ucFilterLevel = 0;
    CWX_UINT32 uiContentPos = 0;
    CWX_UINT32 uiCurContentPos = 0;
    CWX_UINT32 uiCheckLen = 0;
    char      szUpper[9];
    CwxTrieNode<char> const* pNode = NULL;
    while(uiContentPos < uiContentLen)
    {
        pNode = m_filterTree.getRoot();
        uiCurContentPos = uiContentPos;
        uiLocStart = uiContentPos;
        while(uiCurContentPos < uiContentLen)
        {
            uiCheckLen = uiContentLen - uiCurContentPos;
            if (isCaseStopWord(szContent + uiCurContentPos, uiCheckLen))
            {
                uiCurContentPos += uiCheckLen;
                if (m_filterTree.getRoot() == pNode) uiContentPos += uiCheckLen;
                continue;
            }
            uiCheckLen = CHARSET::upperChr(szContent + uiCurContentPos, uiContentLen - uiCurContentPos, szUpper);
            if (0 == uiCheckLen) return ucFilterLevel;
            pNode = pNode->word(szUpper, uiCheckLen);
            if (pNode)
            {
                if (pNode->isKeyEnd())
                {
                    if (ucFilterLevel < maxFilterLevel(pNode->getAttr()))
                    {
                        ucFilterLevel = maxFilterLevel(pNode->getAttr());
                        uiStart = uiLocStart;
                        uiLen = uiCurContentPos + uiCheckLen - uiStart;
                        if (ucFilterLevel == m_ucMaxFilterLevel) return ucFilterLevel;
                    }
                }
                uiCurContentPos += uiCheckLen;
            }
            else
            {
                break;
            }
        }
        if (uiContentPos >= uiContentLen) break;
        uiCheckLen = CHARSET::nextChrLen(szContent + uiContentPos, uiContentLen - uiContentPos);
        if (0 == uiCheckLen) return ucFilterLevel;
        uiContentPos += uiCheckLen;
    }
    return ucFilterLevel;
}

template<typename CHARSET>
inline bool CwxFilterMachine<CHARSET>::isStopWord(char const* szContent, CWX_UINT32& uiContentLen) const
{
    return m_stopwordTree.firstKey(szContent, uiContentLen) != NULL;
}

template<typename CHARSET>
inline bool CwxFilterMachine<CHARSET>::isCaseStopWord(char const* szContent, CWX_UINT32& uiContentLen) const
{
    return m_stopwordTree.firstCaseKey(szContent, uiContentLen) != NULL;
}

template<typename CHARSET>
inline CWX_UINT8 CwxFilterMachine<CHARSET>::maxFilterLevel(CWX_UINT8 ucLevel) const
{
    CWX_UINT8 ucMaxLevel = 0;
    for (CWX_UINT8 i=0; i<8; i++)
    {
        if (CWX_CHECK_ATTR(ucLevel, 1<<i))
        {
            ucMaxLevel = 1<<i;
        }
    }
    return ucMaxLevel;
}

template<typename CHARSET>
inline char const* CwxFilterMachine<CHARSET>::charset() const
{
    return CHARSET::charset();
}

template<typename CHARSET>
inline void CwxFilterMachine<CHARSET>::clear()
{
    m_charPool.reset();
    m_filterTree.clear();
    m_stopwordTree.clear();
    m_ucMaxFilterLevel = 0;
    setFilterNum(0);
}

CWINUX_END_NAMESPACE
