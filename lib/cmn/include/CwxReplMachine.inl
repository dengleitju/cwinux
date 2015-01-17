CWINUX_BEGIN_NAMESPACE

template<typename CHARSET>
bool CwxReplMachine<CHARSET>::addStr(char const* szSrc, CWX_UINT32 uiSrcLen, char const* szRepl, CWX_UINT32 uiReplLen)
{
    if (uiSrcLen)
    {
        CwxMultiString* szReplStr = NULL;
        CwxTrieNode<CwxMultiString>* pNode = m_trieTree.addKey(szSrc, uiSrcLen);
        if (!pNode) return false;
        if (szRepl)
        {
            szReplStr = (CwxMultiString*)m_charPool.malloc(sizeof(CwxMultiString) + uiReplLen + 1);
            szReplStr->m_uiLength = uiReplLen;
            memcpy(szReplStr->m_szStr, szRepl, uiReplLen);
            szReplStr->m_szStr[uiReplLen] = 0x00;
        }
        else
        {
            szReplStr = (CwxMultiString*)m_charPool.malloc(sizeof(CwxMultiString) + 1);
            szReplStr->m_uiLength = 0;
            szReplStr->m_szStr[0] = 0x00;
        }
        pNode->setData(szReplStr);
        incReplNum();
    }
    return true;
}

template<typename CHARSET>
bool CwxReplMachine<CHARSET>::addCaseStr(char const* szSrc, CWX_UINT32 uiSrcLen, char const* szRepl, CWX_UINT32 uiReplLen)
{
    if (uiSrcLen)
    {
        CwxMultiString* szReplStr = NULL;
        CwxTrieNode<CwxMultiString>* pNode = m_trieTree.addCaseKey(szSrc, uiSrcLen);
        if (!pNode) return false;
        if (szRepl)
        {
            szReplStr = (CwxMultiString*)m_charPool.malloc(sizeof(CwxMultiString) + uiReplLen + 1);
            szReplStr->m_uiLength = uiReplLen;
            memcpy(szReplStr->m_szStr, szRepl, uiReplLen);
            szReplStr->m_szStr[uiReplLen] = 0x00;
        }
        else
        {
            szReplStr = (CwxMultiString*)m_charPool.malloc(sizeof(CwxMultiString) + 1);
            szReplStr->m_uiLength = 0;
            szReplStr->m_szStr[0] = 0x00;
        }
        pNode->setData(szReplStr);
        incReplNum();
    }
    return true;
}

template<typename CHARSET>
bool CwxReplMachine<CHARSET>::replace(char const* szSrc, CWX_UINT32& uiSrcLen, char* szOut, unsigned int &uiOutLen) const
{
    if (!uiSrcLen)
    {
        szOut[0] = 0x00;
        uiOutLen = 0;
        uiSrcLen = 0;
        return true;
    }
    CWX_UINT32 uiSrcPos = 0;
    CWX_UINT32 uiOutPos = 0;
    CWX_UINT32 uiCheckLen = 0;
    CWX_UINT32 uiReplLen = 0;
    char const*   szReplStr = NULL;
    CwxTrieNode<CwxMultiString> const* pNode = NULL;

    if (!getReplNum())
    {
        uiOutLen = CwxCommon::copyStr(szOut, uiOutLen,  szSrc, uiSrcLen);
        return true;
    }


    while(uiSrcPos < uiSrcLen)
    {
        uiCheckLen = uiSrcLen - uiSrcPos;
        pNode = m_trieTree.lastKey(szSrc + uiSrcPos, uiCheckLen);
        if (pNode)
        {
            uiReplLen = pNode->getData()->m_uiLength;
            szReplStr = pNode->getData()->m_szStr;
        }
        else
        {
            uiCheckLen = uiSrcLen - uiSrcPos;
            uiCheckLen = CHARSET::nextChrLen(szSrc + uiSrcPos, uiCheckLen);
            if (!uiCheckLen)
            {
                szOut[uiOutPos] = 0x00;
                uiOutLen = uiOutPos;
                uiSrcLen = uiSrcPos;
                return false;
            }
            szReplStr = szSrc + uiSrcPos;
            uiReplLen = uiCheckLen;
        }
        if (uiReplLen + uiOutPos >= uiOutLen)
        {
            uiOutLen = uiOutPos;
            szOut[uiOutLen] = 0x00;
            uiSrcLen = uiSrcPos;
            return true;
        }
        memcpy(szOut + uiOutPos, szReplStr, uiReplLen);
        uiOutPos += uiReplLen;
        uiSrcPos += uiCheckLen;
    }
    uiSrcLen = uiSrcPos;
    uiOutLen = uiOutPos;
    szOut[uiOutLen] = 0x00;
    return true;
}

template<typename CHARSET>
bool CwxReplMachine<CHARSET>::caseReplace(char const* szSrc, CWX_UINT32& uiSrcLen, char* szOut, unsigned int &uiOutLen) const
{
    if (!uiSrcLen)
    {
        szOut[0] = 0x00;
        uiOutLen = 0;
        uiSrcLen = 0;
        return true;
    }
    CWX_UINT32 uiSrcPos = 0;
    CWX_UINT32 uiOutPos = 0;
    CWX_UINT32 uiCheckLen = 0;
    CWX_UINT32 uiReplLen = 0;
    char const*  szReplStr = NULL;
    CwxTrieNode<CwxMultiString> const* pNode = NULL;

    if (!getReplNum())
    {
        uiOutLen = CwxCommon::copyStr(szOut, uiOutLen,  szSrc, uiSrcLen);
        return true;
    }

    while(uiSrcPos < uiSrcLen)
    {
        uiCheckLen = uiSrcLen - uiSrcPos;
        pNode = m_trieTree.lastCaseKey(szSrc + uiSrcPos, uiCheckLen);
        if (pNode)
        {
            uiReplLen = pNode->getData()->m_uiLength;
            szReplStr = pNode->getData()->m_szStr;
        }
        else
        {
            uiCheckLen = uiSrcLen - uiSrcPos;
            uiCheckLen = CHARSET::nextChrLen(szSrc + uiSrcPos, uiCheckLen);
            if (!uiCheckLen)
            {
                uiSrcLen = uiSrcPos;
                szOut[uiOutPos] = 0x00;
                uiOutLen = uiOutPos;
                return false;
            }
            szReplStr = szSrc + uiSrcPos;
            uiReplLen = uiCheckLen;
        }
        if (uiReplLen + uiOutPos >= uiOutLen)
        {
            uiSrcLen = uiSrcPos;
            uiOutLen = uiOutPos;
            szOut[uiOutLen] = 0x00;
            return true;
        }
        memcpy(szOut + uiOutPos, szReplStr, uiReplLen);
        uiOutPos += uiReplLen;
        uiSrcPos += uiCheckLen;
    }
    uiSrcLen = uiSrcPos;
    uiOutLen = uiOutPos;
    szOut[uiOutLen] = 0x00;
    return true;
}

template<typename CHARSET>
char const* CwxReplMachine<CHARSET>::charset() const
{
    return CHARSET::charset();
}

template<typename CHARSET>
void CwxReplMachine<CHARSET>::clear()
{
    m_charPool.free();
    m_trieTree.clear();
    setReplNum(0);
}


CWINUX_END_NAMESPACE
