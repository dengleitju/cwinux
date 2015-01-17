#include "CwxGbkUnicodeMap.h"
#include "CwxCharset.h"

CWINUX_BEGIN_NAMESPACE

void CwxGbkUnicodeMap::utf16ToUtf8(char const* src, size_t& src_len, char* dest, size_t& dest_len)
{
    unsigned short u_code;
    char szUtf8[9];
    CWX_UINT8 utf8_len;
    size_t src_pos=0;
    size_t dest_pos=0;
    while(1)
    {
        if (src_pos + 2 > src_len)
        {//exceed the src's range
            break;
        }
        u_code =(unsigned char)src[src_pos];
        u_code <<=8;
        u_code +=(unsigned char)src[src_pos+1];
        src_pos += 2;
        //convert utf16 to utf8
        utf16ChrToUtf8(u_code, szUtf8, utf8_len);
        if (dest_pos + utf8_len > dest_len) break;//exceed the dest's range.
        memcpy(dest + dest_pos, szUtf8, utf8_len);
        dest_pos += utf8_len;
    }
    src_len = src_pos;
    dest_len = dest_pos;
}
void CwxGbkUnicodeMap::utf16ToGbk(char const* szUtf16, CWX_UINT32& uiUtf16Len, char* szGbk, CWX_UINT32& uiGbkLen)
{
    CWX_UINT16 unGbkCode;
    CWX_UINT16 unUniCode;
    CWX_UINT32 uiGbkPos=0;
    CWX_UINT32 uiUtf16Pos=0;
    while(uiUtf16Len > uiUtf16Pos + 1)
    {
        memcpy(&unUniCode, szUtf16 + uiUtf16Pos, 2);
        unGbkCode = m_utf16GbkMap[unUniCode];
        if (unGbkCode)
        {
            //exist.
            if (uiGbkPos + 2 > uiGbkLen) break;//exceed the max length
            memcpy(szGbk + uiGbkPos, &unGbkCode, 2);
            uiUtf16Pos += 2;
            uiGbkPos +=2;

        }
        else
        {//not exist.
            if (unUniCode < 0x80)
            {//single char
                if (uiGbkPos + 1 > uiGbkLen) break;//exceed length
                szGbk[uiGbkPos] = szUtf16[uiUtf16Pos];
                uiGbkPos++;
                uiUtf16Pos+=2;
            }
            else
            {
                break;//error
            }
        }
    }
    uiUtf16Len = uiUtf16Pos;
    uiGbkLen = uiGbkPos;
    szGbk[uiGbkLen] = 0x00;
}


void CwxGbkUnicodeMap::utf8ToUtf16(char const* src, size_t& src_len, char* dest, size_t& dest_len)
{
    unsigned short u_code;
    size_t utf8_len;
    size_t src_pos=0;
    size_t dest_pos=0;
    while(1)
    {
        if (src_pos >= src_len) //end
            break;
        utf8_len = CwxCharsetUtf8::nextChrLen(src + src_pos, src_len - src_pos);
        if ((0 == utf8_len) || (3<utf8_len))
        {//invalid unicode
            break;
        }
        if (src_pos + utf8_len > src_len)
        {//exceed the max length
            break;
        }
        if (dest_pos + 2 > dest_len)
        {//exceed the max length
            break;
        }
        //get utf16 code
        utf8ChrToUtf16(src + src_pos, utf8_len, u_code);
        dest[dest_pos] = (char)(u_code>>8);
        dest[dest_pos+1]=(char)(u_code);
        dest_pos +=2;
        src_pos += utf8_len;
    }
    src_len = src_pos;
    dest_len = dest_pos;
}

void CwxGbkUnicodeMap::utf8ToGbk(char const* szUtf8, CWX_UINT32& uiUtf8Len, char* szGbk, CWX_UINT32& uiGbkLen)
{
    CWX_UINT16 unGbkCode=0;
    CWX_UINT16 unUniCode=0;
    CWX_UINT8 ucUtf8Len=0;
    CWX_UINT32 uiUtf8Pos=0;
    CWX_UINT32 uiGbkPos=0;
    while(uiUtf8Len > uiUtf8Pos)
    {
        ucUtf8Len = CwxCharsetUtf8::nextChrLen(szUtf8 + uiUtf8Pos, uiUtf8Len - uiUtf8Pos);
        if ((0 == ucUtf8Len) || (ucUtf8Len > 3)) break;
        if (1 == ucUtf8Len)
        {
            if (uiGbkPos + 1 > uiGbkLen) break;//exceed the max length
            szGbk[uiGbkPos] = szUtf8[uiUtf8Pos];
            uiUtf8Pos++;
            uiGbkPos++;
        }
        else
        {
            //get utf16 code
            utf8ChrToUtf16(szUtf8 + uiUtf8Pos, ucUtf8Len, unUniCode);
            unGbkCode = m_utf16GbkMap[unUniCode];
            //exist.
            if (uiGbkPos + 2 > uiGbkLen) break;//exceed the max length
            if (0 != unGbkCode)
            {
                szGbk[uiGbkPos] = (char)(unGbkCode>>8);
                szGbk[uiGbkPos+1]=(char)(unGbkCode);
                uiGbkPos +=2;
            }
            uiUtf8Pos += ucUtf8Len;
        }
    }
    uiUtf8Len = uiUtf8Pos;
    uiGbkLen = uiGbkPos;
    szGbk[uiGbkLen] = 0x00;
}

void CwxGbkUnicodeMap::gbkToUtf8(char const* szGbk, CWX_UINT32& uiGbkLen, char* szUtf8, CWX_UINT32& uiUtf8Len)
{
    CWX_UINT16 unGbkCode=0;
    CWX_UINT16 unUnicode=0;
    char szBuf[5];
    CWX_UINT8 ucLen=0;
    CWX_UINT32 uiGbkPos=0;
    CWX_UINT32 uiUtf8Pos=0;
    CWX_UINT16 unLen = 0;

    while(uiGbkLen > uiGbkPos)
    {
        unLen = CwxCharsetGbk::nextChrLen(szGbk + uiGbkPos, uiGbkLen - uiGbkPos);
        if (2 == unLen)
        {//must 2 byte
            unGbkCode =(((CWX_UINT8)szGbk[uiGbkPos])<<8) + ((CWX_UINT8)szGbk[uiGbkPos+1]);
            unUnicode = m_gbkUtf16Map[unGbkCode];
            if (0 == unUnicode) break;
            //convert utf16 to utf8
            utf16ChrToUtf8(unUnicode, szBuf, ucLen);
            if (uiUtf8Pos + ucLen > uiUtf8Len) break;//exceed the dest's range.
            memcpy(szUtf8 + uiUtf8Pos, szBuf, ucLen);
            uiGbkPos += 2;
            uiUtf8Pos += ucLen;
        }
        else if (1 == ucLen)
        {
            if (uiUtf8Pos + 1 >= uiUtf8Len) break; //exceed the dest's range
            szUtf8[uiUtf8Pos] = szGbk[uiGbkPos];
            uiGbkPos++;
            uiUtf8Pos++;
        }
        else
        {
            break;
        }
    }
    uiGbkLen = uiGbkPos;
    uiUtf8Len = uiUtf8Pos;
    szUtf8[uiUtf8Len] = 0x00;
}


void CwxGbkUnicodeMap::gbkToUtf16(char const* szGbk, CWX_UINT32& uiGbkLen, char* szUtf16, CWX_UINT32& uiUtf16Len)
{
    CWX_UINT16 unGbkCode;
    CWX_UINT16 unUniCode;
    CWX_UINT32 uiGbkPos=0;
    CWX_UINT32 uiUtf16Pos=0;
    while(uiGbkLen > uiGbkPos)
    {
        if (CwxCharsetGbk::isCharset(szGbk + uiGbkPos, uiGbkLen - uiGbkPos))
        {//must 2 byte
            memcpy(&unGbkCode, szGbk + uiGbkPos, 2);
            unUniCode = m_gbkUtf16Map[unGbkCode];
            if (unUniCode)
            {
                if (uiUtf16Pos + 2 > uiUtf16Len) break;//exceed the dest's range.
                memcpy(szUtf16 + uiUtf16Pos, &unUniCode, 2);
                uiUtf16Pos += 2;
            }
            uiGbkPos += 2;
        }
        else
        {
            if (uiUtf16Pos + 1 > uiUtf16Len) break;//exceed the dest's range
            szUtf16[uiUtf16Pos] = 0;
            szUtf16[uiUtf16Pos+1] = szGbk[uiGbkPos];
            uiGbkPos++;
            uiUtf16Pos += 2;
        }
    }
    uiGbkLen = uiGbkPos;
    uiUtf16Len = uiUtf16Pos;
    szUtf16[uiUtf16Len] = 0x00;
}



CWINUX_END_NAMESPACE
