CWINUX_BEGIN_NAMESPACE

template<typename DATA>
inline CwxTrieNode<DATA>* CwxTrieNode<DATA>::add(char const* szWord, CWX_UINT32 uiWordLen, CwxTypePoolEx<CwxTrieNode<DATA> >& pool)
{
    CWX_INT8 ucIndex = 0;
    CwxTrieNode<DATA>* pNode = this;
    if (uiWordLen)
    {
        for (CWX_UINT32 i=0; i<uiWordLen; i++)
        {
            ucIndex = ((szWord[i]>>4)&0x0F);
            if (!pNode->m_child[ucIndex])
            {
                pNode->m_child[ucIndex] = pool.malloc();
                pNode->m_child[ucIndex]->m_parent = pNode;
            }
            pNode = pNode->m_child[ucIndex];
            ucIndex = szWord[i]&0x0F;
            if (!pNode->m_child[ucIndex])
            {
                pNode->m_child[ucIndex] = pool.malloc();
                pNode->m_child[ucIndex]->m_parent = pNode;
            }
            pNode = pNode->m_child[ucIndex];
        }
        pNode->m_ucSign |= (1<<SIGN_WORD_END_BIT);
    }
    return pNode;
}

template<typename DATA>
inline CwxTrieNode<DATA>* CwxTrieNode<DATA>::endKey()
{
    this->m_ucSign |= ( 1<< SIGN_KEY_END_BIT);
    return this;
}

template<typename DATA>
inline CwxTrieNode<DATA> const* CwxTrieNode<DATA>::word(char const* szWord, CWX_UINT32 uiWordLen) const
{
    CWX_INT8 ucIndex = 0;
    CwxTrieNode<DATA> const* pNode = this;
    if (uiWordLen)
    {
        for (CWX_UINT32 i=0; i<uiWordLen; i++)
        {
            ucIndex = ((szWord[i]>>4)&0x0F);
            if (!pNode->m_child[ucIndex]) return NULL;
            pNode = pNode->m_child[ucIndex];
            ucIndex = szWord[i]&0x0F;
            if (!pNode->m_child[ucIndex]) return NULL;
            pNode = pNode->m_child[ucIndex];
        }
        if (pNode->isWordEnd()) return pNode;
    }
    return NULL;
}



template<typename DATA>
inline CwxTrieNode<DATA> const* CwxTrieNode<DATA>::firstKey(char const* szKey, CWX_UINT32& uiKeyLen) const
{
    CWX_INT8 ucIndex = 0;
    CwxTrieNode<DATA> const* pNode = this;
    for (CWX_UINT32 i=0; i<uiKeyLen; i++)
    {
        ucIndex = ((szKey[i]>>4)&0x0F);
        if (!pNode->m_child[ucIndex]) return NULL;
        pNode = pNode->m_child[ucIndex];
        ucIndex = szKey[i]&0x0F;
        if (!pNode->m_child[ucIndex]) return NULL;
        pNode = pNode->m_child[ucIndex];
        if (pNode->isKeyEnd())
        {
            uiKeyLen = i + 1;
            return pNode;
        }
    }
    return NULL;
}

template<typename DATA>
inline CwxTrieNode<DATA> const* CwxTrieNode<DATA>::lastKey(char const* szKey, CWX_UINT32& uiKeyLen) const
{
    CWX_INT8 ucIndex = 0;
    CwxTrieNode<DATA> const* pFind = NULL;
    CWX_UINT32 uiFindLen = 0;
    CwxTrieNode<DATA> const* pNode = this;
    for (CWX_UINT32 i=0; i<uiKeyLen; i++)
    {
        ucIndex = ((szKey[i]>>4)&0x0F);
        if (!pNode->m_child[ucIndex]) return pFind;
        pNode = pNode->m_child[ucIndex];
        ucIndex = szKey[i]&0x0F;
        if (!pNode->m_child[ucIndex]) return pFind;
        pNode = pNode->m_child[ucIndex];
        if (pNode->isKeyEnd())
        {
            uiFindLen = i + 1;
            pFind = pNode;
        }
    }
    uiKeyLen = uiFindLen;
    return pFind;
}


template<typename CHARSET, typename DATA>
inline CwxTrieNode<DATA>* CwxTrieTree<CHARSET, DATA>::addKey(char const* szKey, CWX_UINT32 uiKeyLen)
{
    CWX_UINT32 uiWordLen = 0;
    CWX_UINT32 uiPos = 0;
    CwxTrieNode<DATA>* pNode = &m_root;
    while(uiPos < uiKeyLen)
    {
        uiWordLen = CHARSET::nextChrLen(szKey + uiPos, uiKeyLen - uiPos);
        if (uiWordLen)
        {
           pNode = pNode->add(szKey + uiPos, uiWordLen, m_pool);
        }
        else
        {
            return NULL;
        }
        uiPos += uiWordLen;
    }
    if (pNode != &m_root)
    {
        pNode->endKey();
        m_uiDataNum++;
        return pNode;
    }
    return NULL;
}

template<typename CHARSET, typename DATA>
inline CwxTrieNode<DATA>* CwxTrieTree<CHARSET, DATA>::addCaseKey(char const* szKey, CWX_UINT32 uiKeyLen)
{
    CWX_UINT32 uiWordLen = 0;
    CWX_UINT32 uiPos = 0;
    char szWord[9];
    CwxTrieNode<DATA>* pNode = &m_root;
    while(uiPos < uiKeyLen)
    {
        uiWordLen = CHARSET::upperChr(szKey + uiPos, uiKeyLen - uiPos, szWord);
        if (uiWordLen)
        {
            pNode = pNode->add(szWord, uiWordLen, m_pool);
        }
        else
        {
            return NULL;
        }
        uiPos += uiWordLen;
    }
    if (pNode != &m_root)
    {
        pNode->endKey();
        m_uiDataNum++;
        return pNode;
    }
    return NULL;
}

template<typename CHARSET, typename DATA>
inline bool CwxTrieTree<CHARSET, DATA>::eraseNode(CwxTrieNode<DATA>* pNode)
{
    CWX_ASSERT(!pNode->m_data);
    if (!pNode || (&m_root == pNode)) return true;
    if (!pNode->isKeyEnd()) return false;
    pNode->clrKeyEnd();
    //检查是否有孩子节点
    CWX_UINT8 i=0;
    CWX_UINT8 ucChildNum = 0;
    for (i=0; i<16; i++)
    {
        if (pNode->m_child[i]) return true;
    }
    //no child
    CwxTrieNode<DATA>* pParent = pNode->m_parent;
    while(pParent)
    {
        ucChildNum = 0;
        for(i=0; i<16; i++)
        {
            if (pParent->m_child[i])
            {
                if (pParent->m_child[i] == pNode)
                {
                    pParent->m_child[i] = NULL;
                }
                else
                {
                    ucChildNum++;
                }
            }
        }
        m_pool.free(pNode);
        pNode = pParent;
        pParent = pNode->m_parent;
        if (pNode->isKeyEnd()) return true;///other keyword
        if (ucChildNum) return true;///has other child
    }
    return true;
}


template<typename CHARSET, typename DATA>
inline CwxTrieNode<DATA> const* CwxTrieTree<CHARSET, DATA>::firstKey(char const* szKey, CWX_UINT32& uiKeyLen) const
{
    return m_root.firstKey(szKey, uiKeyLen);
}

template<typename CHARSET, typename DATA>
inline CwxTrieNode<DATA> const* CwxTrieTree<CHARSET, DATA>::firstCaseKey(char const* szKey, CWX_UINT32& uiKeyLen) const
{
    CWX_UINT32 uiWordLen = 0;
    CWX_UINT32 uiPos = 0;
    char szWord[9];
    CwxTrieNode<DATA> const* pNode = &m_root;
    while(uiPos < uiKeyLen)
    {
        uiWordLen = CHARSET::upperChr(szKey + uiPos, uiKeyLen - uiPos, szWord);
        if (uiWordLen)
        {
            pNode = pNode->word(szWord, uiWordLen);
        }
        else
        {
            uiKeyLen = 0;
            return NULL;
        }
        uiPos += uiWordLen;
        if (!pNode) return NULL;
        if (pNode->isKeyEnd())
        {
            uiKeyLen = uiPos;
            return pNode;
        }
    }
    return NULL;
}

template<typename CHARSET, typename DATA>
inline CwxTrieNode<DATA> const* CwxTrieTree<CHARSET, DATA>::lastKey(char const* szKey, CWX_UINT32& uiKeyLen) const
{
    return m_root.lastKey(szKey, uiKeyLen);
}

template<typename CHARSET, typename DATA>
inline CwxTrieNode<DATA> const* CwxTrieTree<CHARSET, DATA>::lastCaseKey(char const* szKey, CWX_UINT32& uiKeyLen) const
{
    CWX_UINT32 uiWordLen = 0;
    CWX_UINT32 uiPos = 0;
    char szWord[9];
    CwxTrieNode<DATA> const* pFindNode = NULL;
    CWX_UINT32 uiFindLen = 0;
    CwxTrieNode<DATA> const* pNode = &m_root;
    while(uiPos < uiKeyLen)
    {
        uiWordLen = CHARSET::upperChr(szKey + uiPos, uiKeyLen - uiPos, szWord);
        if (uiWordLen)
        {
            pNode = pNode->word(szWord, uiWordLen);
        }
        else
        {
            uiKeyLen = uiFindLen;
            return pFindNode;
        }
        uiPos += uiWordLen;
        if (!pNode)
        {
            uiKeyLen = uiFindLen;
            return pFindNode;
        }
        if (pNode->isKeyEnd())
        {
            pFindNode = pNode;
            uiFindLen = uiPos;
        }
    }
    if (pFindNode)
    {
        uiKeyLen = uiFindLen;
        return pFindNode;
    }
    return NULL;
}

template<typename CHARSET, typename DATA>
inline void CwxTrieTree<CHARSET, DATA>::clear()
{
    m_uiDataNum=0;
    memset(&m_root, 0x00, sizeof(m_root));
    m_pool.clear();

}


CWINUX_END_NAMESPACE

