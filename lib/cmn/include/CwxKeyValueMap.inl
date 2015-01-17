
CWINUX_BEGIN_NAMESPACE

inline CwxKeyValueMap::CwxKeyValueMap(CwxKeyValueMap const& obj)
{
    m_bCaseSensive = obj.m_bCaseSensive;
    m_index = new MAP(CwxCaseStringLess2(obj.isCaseSensive()));
    *m_index = *obj.m_index;
}

inline inline CwxKeyValueMap& CwxKeyValueMap::operator=(CwxKeyValueMap const& obj)
{
    if (this != &obj)
    {
        if (m_bCaseSensive != obj.m_bCaseSensive)
        {
            delete m_index;
            m_index = new MAP(CwxCaseStringLess2(obj.isCaseSensive()));
        }
        else
        {
            m_index->clear();
        }
        *m_index = *obj.m_index;
    }
}

inline void CwxKeyValueMap::swap(CwxKeyValueMap& obj)
{
    m_index->swap(*obj->m_index);
}

inline MAP& CwxKeyValueMap::map()
{
    return &m_index;
}

inline bool CwxKeyValueMap::isCaseSensive() const
{
    return m_bCaseSensive;
}

CWINUX_END_NAMESPACE

