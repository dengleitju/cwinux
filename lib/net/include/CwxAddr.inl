CWINUX_BEGIN_NAMESPACE

inline bool CwxAddr::operator == (const CwxAddr &sap) const
{
    return (sap.m_iAddrType == this->m_iAddrType &&
        sap.m_iAddrSize == this->m_iAddrSize   );
}

inline bool CwxAddr::operator != (const CwxAddr &sap) const
{
    return (sap.m_iAddrType != this->m_iAddrType ||
        sap.m_iAddrSize != this->m_iAddrSize   );
}

inline int CwxAddr::getSize (void) const
{
    return this->m_iAddrSize;
}

inline void CwxAddr::setSize (CWX_INT32 iSize)
{
    this->m_iAddrSize = iSize;
}

inline CWX_INT32 CwxAddr::getType (void) const
{
    return this->m_iAddrType;
}

inline void CwxAddr::setType (CWX_INT32 iType)
{
    this->m_iAddrType = iType;
}

// Initializes instance variables.

inline void CwxAddr::baseSet(CWX_INT32 iType, CWX_INT32 iSize)
{
    this->m_iAddrType = iType;
    this->m_iAddrSize = iSize;
}



CWINUX_END_NAMESPACE
