
CWINUX_BEGIN_NAMESPACE
inline CwxEscapeUtil* CwxEscapeUtil::instance()
{
    if (!m_pInstance)
    {
        CwxMutexGuard<CwxMutexLock> lock(&m_lock);
        if (!m_pInstance)
        {
            m_pInstance = new CwxEscapeUtil();
            m_pInstance->init();
        }
    }
    return m_pInstance;
}

inline bool CwxEscapeUtil::isEncode(CWX_UINT8 chr, CWX_UINT16 unEscapeType) const
{
    return (m_chrMap[chr]&unEscapeType) !=0;
}

inline char* CwxEscapeUtil::c2x(CWX_UINT8 what, CWX_UINT8 prefix, char* out) const
{
    static const char c2x_table[] = "0123456789ABCDEF";
    if (prefix) *out++ = prefix;
    *out++ = c2x_table[what >> 4];
    *out++ = c2x_table[what & 0xf];
    *out=0;
    return out;
}

inline CWX_UINT8 CwxEscapeUtil::x2c(char const* szStr) const
{
    CWX_UINT8 chr=0;
    for (CWX_UINT8 i=0; i<2; i++)
    {
        chr <<=4;
        if (('0' <=*szStr ) && ('9' >= *szStr))
        {
            chr += *szStr - '0';
        }
        else if (('A' <=*szStr ) && ('F' >= *szStr))
        {
            chr += *szStr - 'A';
        }
        else if(('a' <=*szStr ) && ('f' >= *szStr))
        {
            chr += *szStr - 'a';
        }
        else
        {
            break;
        }
    }
    return chr;
}


inline bool CwxEscapeUtil::urlEncode(char const* szUrl, char* szOut, CWX_UINT32& uiOutLen) const
{
    CWX_UINT32 uiEncodeLen = 0;
    CWX_UINT8 chr;
    while((chr=*szUrl))
    {
        if (isEncode(chr, URL_ESCAPE))
        {
            if (uiEncodeLen + 4 >= uiOutLen) return false;
            szOut = c2x(chr, '%', szOut);
            uiEncodeLen+=3;
        }
        else
        {
            if (uiEncodeLen + 2 >= uiOutLen) return false;
            *szOut++ =chr;
            uiEncodeLen ++;
        }
        szUrl++;
    }
    szOut[uiEncodeLen] = 0x00;
    uiOutLen = uiEncodeLen;
    return true;
}

inline bool CwxEscapeUtil::urlDecode(char const* szUrl, char* szOut, CWX_UINT32& uiOutLen) const
{
    CWX_UINT32 uiDecodeLen = 0;
    while(*szUrl)
    {
        if (uiDecodeLen + 1 >= uiOutLen) return false;
        if ('%' == *szUrl)
        {
            szUrl++;
            *szOut++ = x2c(szUrl);
            szUrl+=2;
        }
        else
        {
            *szOut++ = *szUrl;
            szUrl++;
        }
        uiDecodeLen++;
    }
    szOut[uiDecodeLen] = 0x00;
    uiOutLen = uiDecodeLen;
    return true;
}

inline void CwxEscapeUtil::urlEncode(char const* szUrl, string& strOut) const
{
    CWX_UINT8 chr;
    char szEncode[4];
    strOut = "";
    while((chr=*szUrl))
    {
        if (isEncode(chr, URL_ESCAPE))
        {
            strOut += c2x(chr, '%', szEncode);
        }
        else
        {
            strOut +=chr;
        }
        szUrl++;
    }
}

inline void CwxEscapeUtil::urlDecode(char const* szUrl, string & strOut) const
{
    strOut = "";
    char szDecode[2];
    memset(szDecode, 0x00, 2);
    while(*szUrl)
    {
        if ('%' == *szUrl)
        {
            szUrl++;
            szDecode[0]=x2c(szUrl);
            strOut += szDecode;
            szUrl+=2;
        }
        else
        {
            szDecode[0]= *szUrl;
            strOut += szDecode;
            szUrl++;
        }
    }
}


CWINUX_END_NAMESPACE

