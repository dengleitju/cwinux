
CWINUX_BEGIN_NAMESPACE
inline void CwxGbkUnicodeMap::utf16ChrToUtf8(CWX_UINT16 unUtf16, char* szUtf8, CWX_UINT8& unUtf8Len)
{
    /*
    UCS-2编码(16进制) UTF-8 字节流(二进制) 
    0000 - 007F       0xxxxxxx 
    0080 - 07FF       110xxxxx 10xxxxxx 
    0800 - FFFF       1110xxxx 10xxxxxx 10xxxxxx 
    */
    if (unUtf16 < 0x80)
    {//single
        unUtf8Len = 1;
        szUtf8[0] =(char)unUtf16;
    }
    else if (unUtf16 < 0x800)
    {
        szUtf8[0] =(char) 0xC0 | ((unUtf16>>6)&0x1F);
        szUtf8[1] =(char) 0x80 | (unUtf16 & 0x3F);
        unUtf8Len = 2;
    }
    else
    {
        szUtf8[0] =(char) 0xE0 | ((unUtf16>>12)&0xF);
        szUtf8[1] =(char) 0x80 | ((unUtf16>>6)& 0x3F);
        szUtf8[2] =(char) 0x80 | (unUtf16 & 0x3F);
        unUtf8Len = 3;
    }

}
inline void CwxGbkUnicodeMap::utf8ChrToUtf16(char const* szUtf8, CWX_UINT16 unUtf8en, CWX_UINT16& unUtf16)
{
    if (1 == unUtf8en)
    {//single
        unUtf16 = szUtf8[0];
    }
    else if (2 == unUtf8en)
    {
        unUtf16 = (szUtf8[0]&0x1F)<<6;
        unUtf16 += szUtf8[1]&0x3F;
    }
    else
    {
        unUtf16 = (szUtf8[0]&0x0F)<<12;
        unUtf16 += (szUtf8[1]&0x3F)<<6;
        unUtf16 += (szUtf8[2]&0x3F);
    }
}

inline CWX_UINT16 CwxGbkUnicodeMap::gbkToUtf16 (CWX_UINT16 unGbk)
{
    return m_gbkUtf16Map[unGbk];
}

inline CWX_UINT16 CwxGbkUnicodeMap::utf16ToGbk(CWX_UINT16 unUtf16)
{
    return m_utf16GbkMap[unUtf16];
}


CWINUX_END_NAMESPACE
