/**
*@file CwxPackageReader.inl
*@brief CwxPackageReader类的Inline实现
*@author cwinux@gmail.com
*@version 1.0
*@date  2009-06-05
*@warning  nothing
*@bug    
*/
CWINUX_BEGIN_NAMESPACE


inline CwxKeyValueItem const* CwxPackageReader::getKey(CWX_UINT32 index) const
{
    if (index >= m_uiKeyNum) return NULL;
    return &m_pKeyValue[index];
}

inline bool CwxPackageReader::getKey(char const* szKey, string& value, bool bSubKey) const 
{
    CwxKeyValueItem const* item = getKey(szKey, bSubKey);
    if (item)
    {
        value = item->m_szData;
        return true;
    }
    return false;
}


inline bool CwxPackageReader::getKey(char const* szKey, CWX_UINT32& value, bool bSubKey) const
{
    CwxKeyValueItem const* item = getKey(szKey, bSubKey);
    if (item)
    {
        value = strtoul(item->m_szData, NULL, 10);
        return true;
    }
    return false;
}
inline bool CwxPackageReader::getKey(char const* szKey, CWX_INT32& value, bool bSubKey) const
{
    CwxKeyValueItem const* item = getKey(szKey, bSubKey);
    if (item)
    {
        value = strtoul(item->m_szData, NULL, 10);
        return true;
    }
    return false;
}

///Get data by key for UINT16. false:not exist, true:get one. 
inline bool CwxPackageReader::getKey(char const* szKey, CWX_UINT16& value, bool bSubKey) const
{
    CwxKeyValueItem const* item = getKey(szKey, bSubKey);
    if (item)
    {
        value = strtoul(item->m_szData, NULL, 10);
        return true;
    }
    return false;
}
///Get data by key for INT16. false:not exist, true:get one. 
inline bool CwxPackageReader::getKey(char const* szKey, CWX_INT16& value, bool bSubKey) const
{
    CwxKeyValueItem const* item = getKey(szKey, bSubKey);
    if (item)
    {
        value = strtoul(item->m_szData, NULL, 10);
        return true;
    }
    return false;
}
///Get data by key for UINT8. false:not exist, true:get one. 
inline bool CwxPackageReader::getKey(char const* szKey, CWX_UINT8& value, bool bSubKey) const
{
    CwxKeyValueItem const* item = getKey(szKey, bSubKey);
    if (item)
    {
        value = strtoul(item->m_szData, NULL, 10);
        return true;
    }
    return false;
}
///Get data by key for char. false:not exist, true:get one. 
inline bool CwxPackageReader::getKey(char const* szKey, char& value, bool bSubKey) const
{
    CwxKeyValueItem const* item = getKey(szKey, bSubKey);
    if (item)
    {
        value = strtoul(item->m_szData, NULL, 10);
        return true;
    }
    return false;
}
///Get data by key for INT64. false:not exist, true:get one. 
inline bool CwxPackageReader::getKey(char const* szKey, CWX_INT64& value, bool bSubKey) const 
{
    CwxKeyValueItem const* item = getKey(szKey, bSubKey);
    if (item)
    {
        value = strtoull(item->m_szData, NULL, 10);
        return true;
    }
    return false;
}
///Get data by key for UINT64. false:not exist, true:get one. 
inline bool CwxPackageReader::getKey(char const* szKey, CWX_UINT64& value, bool bSubKey) const
{
    CwxKeyValueItem const* item = getKey(szKey, bSubKey);
    if (item)
    {
        value = strtoull(item->m_szData, NULL, 10);
        return true;
    }
    return false;
}

///获取当前package的Key的数量。
inline CWX_UINT32 CwxPackageReader::getKeyNum() const
{
    return m_uiKeyNum;
}
///return msg's size
inline CWX_UINT32 CwxPackageReader::getMsgSize() const
{
    return m_uiPackBufLen;
}
///return msg
inline char const* CwxPackageReader::getMsg() const 
{
    return m_szPackMsg;
}
///return the errmsg
inline char const* CwxPackageReader::getErrMsg() const 
{
    return m_szErr;
}

inline bool CwxPackageReader::isCaseSensive() const
{
    return m_bCaseSensive;
}

CWINUX_END_NAMESPACE
