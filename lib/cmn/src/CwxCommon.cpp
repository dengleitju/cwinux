#include "CwxCommon.h"

CWINUX_BEGIN_NAMESPACE

int CwxCommon::split(string const& src, list< pair<string, string> >& value, char ch)
{
    string::size_type begin = 0;
    string::size_type end = src.find(ch);
    pair<string, string> item;
    value.clear();
    int num = 0;
    while(string::npos != end)
    {
        if (CwxCommon::keyValue(src.substr(begin, end - begin), item))
        {
            value.push_back(item);
            num++;
        }
        begin = end + 1;
        end = src.find(ch, begin);
    }
    if (CwxCommon::keyValue(src.substr(begin, end - begin), item))
    {
        value.push_back(item);
        num++;
    }
    return num;
}

string& CwxCommon::trim(string& value, char const* chrs)
{
    size_t start = 0;
    size_t end = 0;
    size_t len = value.length();
    size_t i=0, j=0, chr_len=chrs?strlen(chrs):0;
    bool bContinue = false;
    for ( i = 0; i<len; i++)
    {
        if (value[i] != ' ')
        {
            if (!chr_len) break;
            bContinue = false;
            for (j=0; j<chr_len; j++)
            {
                if (value[i] == chrs[j])
                {
                    bContinue = true;
                    break;
                }
            }
            if (!bContinue) break;
        }
    }
    start = i;
    for ( i = len; i>0; i--)
    {
        if (value[i-1] != ' ')
        {
            if (!chr_len) break;
            bContinue = false;
            for (j=0; j<chr_len; j++)
            {
                if (value[i-1] == chrs[j])
                {
                    bContinue = true;
                    break;
                }
            }
            if (!bContinue) break;
        }
    }
    end = i;
    if (end - start != len) value = value.substr(start, end-start);
    return value;
}

string& CwxCommon::rtrim(string& value, char const* chrs)
{
    size_t len = value.length();
    size_t i=0, j=0, chr_len=chrs?strlen(chrs):0;
    bool bContinue = false;
    for ( i = len; i>0; i--)
    {
        if (value[i-1] != ' ')
        {
            if (!chr_len) break;
            bContinue = false;
            for (j=0; j<chr_len; j++)
            {
                if (value[i-1] == chrs[j])
                {
                    bContinue = true;
                    break;
                }
            }
            if (!bContinue) break;
        }
    }
    if (i<len) value = value.substr(0, i);
    return value;
}

string& CwxCommon::ltrim(string& value, char const* chrs)
{
    size_t len = value.length();
    size_t i=0, j=0, chr_len=chrs?strlen(chrs):0;
    bool bContinue = false;
    for ( i = 0; i<len; i++)
    {
        if (value[i] != ' ')
        {
            if (!chr_len) break;
            bContinue = false;
            for (j=0; j<chr_len; j++)
            {
                if (value[i] == chrs[j])
                {
                    bContinue = true;
                    break;
                }
            }
            if (!bContinue) break;
        }
    }
    if (i>0) value = value.substr(i, len-i);
    return value;
}

char* CwxCommon::trim(char* value, char const* chrs)
{
    size_t len = strlen(value);
    size_t chr_len = chrs?strlen(chrs):0;
    size_t i=0, j=0;
    bool bContinue = false;

    for (i=len; i>0; i--)
    {//trim right
        if (value[i-1] != ' ')
        {
            if (!chr_len) break;
            bContinue = false;
            for (j=0; j<chr_len; j++)
            {
                if (value[i] == chrs[j])
                {
                    bContinue = true;
                    break;
                }
            }
            if (!bContinue) break;
        }
        value[i-1] = 0x00;
    }
    len = i;
    for (i=0; i<len; i++)
    {//left
        if (value[i] != ' ')
        {
            if (!chr_len) break;
            bContinue = false;
            for (j=0; j<chr_len; j++)
            {
                if (value[i] == chrs[j])
                {
                    bContinue = true;
                    break;
                }
            }
            if (!bContinue) break;
        }
    }
    if (i != 0)
    {
        memMove(value, value+i, len - i);
        value[len-i] = 0x00;
    }
    return value;
}

char* CwxCommon::rtrim(char* value, char const* chrs)
{
    size_t len = strlen(value);
    size_t chr_len = chrs?strlen(chrs):0;
    size_t i=0, j=0;
    bool bContinue = false;

    for (i=len; i>0; i--)
    {//trim right
        if (value[i-1] != ' ')
        {
            if (!chr_len) break;
            bContinue = false;
            for (j=0; j<chr_len; j++)
            {
                if (value[i] == chrs[j])
                {
                    bContinue = true;
                    break;
                }
            }
            if (!bContinue) break;
        }
        value[i-1] = 0x00;
    }
    return value;
}

char* CwxCommon::ltrim(char* value, char const* chrs)
{
    size_t len = strlen(value);
    size_t chr_len = chrs?strlen(chrs):0;
    size_t i=0, j=0;
    bool bContinue = false;
    for (i=0; i<len; i++)
    {
        if (value[i] != ' ')
        {
            if (!chr_len) break;
            bContinue = false;
            for (j=0; j<chr_len; j++)
            {
                if (value[i] == chrs[j])
                {
                    bContinue = true;
                    break;
                }
            }
            if (!bContinue) break;
        }
    }
    if (i != 0)
    {
        memMove(value, value+i, len - i);
        value[len - i] = 0x00;
    }
    return value;
}

char* CwxCommon::replaceAll(char const* src, char* dest, size_t& dest_len, char const* from, char const* to)
{
    int from_len = strlen(from);
    int to_len = strlen(to);
    int left_len = dest_len;
    int src_len = 0;
    int copy_len = 0;
    char const* psrc = src;
    char const* pnext = NULL;

    do
    {
        pnext = strstr(psrc, from);
        if (NULL == pnext)
        {
            src_len = strlen(psrc);
            copy_len = left_len > src_len?src_len:left_len;
            memcpy(dest + dest_len - left_len, psrc, copy_len);
            left_len -= copy_len;
            break;
        }
        if (pnext - psrc >= left_len)
        {
            copy_len = left_len;
            memcpy(dest + dest_len - left_len, psrc, copy_len);
            left_len -= copy_len;
            break;
        }
        else
        {
            copy_len = pnext - psrc;
            memcpy(dest + dest_len - left_len, psrc, copy_len);
            left_len -= copy_len;
        }
        //replace
        if (to_len >= left_len)
        {
            copy_len = left_len;
            memcpy(dest + dest_len - left_len, to, copy_len);
            left_len -= copy_len;
            break;
        }
        else
        {
            copy_len = to_len;
            memcpy(dest + dest_len - left_len, to, copy_len);
            left_len -= copy_len;
        }
        psrc = pnext + from_len;

    }while(1);
    dest_len -= left_len;
    dest[dest_len] = 0x00;
    return dest;
}


char* CwxCommon::hexToString(char const* src, char* dest, size_t& dest_len)
{
    size_t i = 0;
    size_t pos = 0;
    size_t src_len = strlen(src);
    memset(dest, 0x00, dest_len);
    for (i=0; (i<src_len/2)&&(i<dest_len); i++)
    {
        if ((src[pos]>='0') && (src[pos]<='9'))
        {
            dest[i] = src[pos] - '0';
        }
        else if ((src[pos]>='a') && (src[pos]<='f'))
        {
            dest[i] = src[pos] - 'a' + 10;
        }
        else if ((src[pos]>='A') && (src[pos]<='F'))
        {
            dest[i] = src[pos] - 'A' + 10;
        }
        else
        {
            return NULL;
        }
        dest[i] <<=4;
        pos++;
        if ((src[pos]>='0') && (src[pos]<='9'))
        {
            dest[i] = src[pos] - '0';
        }
        else if ((src[pos]>='a') && (src[pos]<='f'))
        {
            dest[i] = src[pos] - 'a' + 10;
        }
        else if ((src[pos]>='A') && (src[pos]<='F'))
        {
            dest[i] = src[pos] - 'A' + 10;
        }
        else
        {
            return NULL;
        }
        pos++;
    }
    dest[i] = 0x00;
    return dest;
}



CWX_UINT32 CwxCommon::ipDot2Int(char const* szIp)
{
    struct in_addr in;
    if (0 == inet_aton(szIp, &in)) return 0;
    return (CWX_UINT32)in.s_addr;
}

int CwxCommon::snprintf(char *buf, size_t maxlen, const char *format, ...)
{
    va_list ap;
    va_start (ap, format);
    int result = vsnprintf(buf, maxlen, format, ap);
    va_end (ap);
    return result;
}

CWINUX_END_NAMESPACE


