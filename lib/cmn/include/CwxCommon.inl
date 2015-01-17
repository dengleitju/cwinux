CWINUX_BEGIN_NAMESPACE

inline size_t CwxCommon::copyStr(char* dest, char const* src, size_t len)
{
    size_t copy_len = 0;
    if (dest)
    {
        if (!src)
        {
            dest[0] = 0x00;
        }
        else
        {
            copy_len= strlen(src);
            if (copy_len>len) copy_len = len;
            memcpy(dest, src, copy_len);
            dest[copy_len] = 0x00;
        }
    }
    return copy_len;
}

inline size_t CwxCommon::copyStr(char* dest, size_t dest_len, char const* src, size_t src_len)
{
    size_t len = dest_len>src_len?src_len:dest_len;
    memcpy(dest, src, len);
    dest[len] = 0x00;
    return len;
}

inline bool CwxCommon::keyValue(string const& src, pair<string, string>& kv)
{
    string::size_type end = src.find('=');
    if (string::npos != end)
    {
        kv.first = src.substr(0, end);
        kv.second = src.substr(end + 1);
        CwxCommon::trim(kv.first);
        CwxCommon::trim(kv.second);
        return true;
    }
    return false;
}

inline int CwxCommon::split(string const& src, list<string>& value, char ch)
{
    string::size_type begin = 0;
    string::size_type end = src.find(ch);
    int num=1;
    value.clear();
    while(string::npos != end)
    {
        value.push_back(src.substr(begin, end - begin));
        begin = end + 1;
        end = src.find(ch, begin);
        num++;
    }	
    value.push_back(src.substr(begin));	
    return num;
}

inline bool CwxCommon::findKey(list< pair<string, string> > const& values, string const& name, pair<string, string>& item)
{
    list< pair<string, string> >::const_iterator iter=values.begin();
    while(iter != values.end())
    {
        if (strcmp(iter->first.c_str(), name.c_str()) == 0)
        {
            item = (*iter);
            return true;
        }
        iter++;
    }
    return false;
}

inline bool CwxCommon::findCaseKey(list< pair<string, string> > const & values, string const& name, pair<string, string>& item)
{
    list< pair<string, string> >::const_iterator iter=values.begin();
    while(iter != values.end())
    {
        if (strcasecmp(iter->first.c_str(), name.c_str()) == 0)
        {
            item = (*iter);
            return true;
        }
        iter++;
    }
    return false;
}


inline bool CwxCommon::findKey(list< pair<char*, char*> > const & values, char const* name, pair<char*, char*>& item)
{
    list< pair<char*, char*> >::const_iterator iter=values.begin();
    while(iter != values.end())
    {
        if (strcmp(iter->first, name) == 0)
        {
            item = (*iter);
            return true;
        }
        iter++;
    }
    return false;
}

inline bool CwxCommon::findCaseKey(list< pair<char*, char*> > const & values, char const* name, pair<char*, char*>& item)
{
    list< pair<char*, char*> >::const_iterator iter=values.begin();
    while(iter != values.end())
    {
        if (strcasecmp(iter->first, name) == 0)
        {
            item = (*iter);
            return true;
        }
        iter++;
    }
    return false;
}

inline char* CwxCommon::memMove(char *dest, char const *src, size_t n)
{
    return (char*)memmove(dest, src, n);
}


inline string& CwxCommon::replaceAll(string& strSrc, string const& str1, string const& str2)
{
    string::size_type pos=0;
    while(true)
    {
        pos = strSrc.find(str1, pos);
        if (pos == string::npos) return strSrc;
        strSrc.replace(pos, str1.length(), str2);
        pos += str2.length();
    }
    return strSrc;
}

inline char* CwxCommon::lower(char* src)
{
    for (size_t i=0; i<strlen(src); i++)
    {
        if ((src[i]>='A') && (src[i]<='Z'))
        {
            src[i] += 'a' - 'A';
        }
    }
    return src;
}
inline char* CwxCommon::upper(char* src)
{
    for (size_t i=0; i<strlen(src); i++)
    {
        if ((src[i]>='a') && (src[i]<='z'))
        {
            src[i] += 'A' - 'a';
        }
    }
    return src;
}

inline char* CwxCommon::ipInt2Doc(CWX_UINT32 ip, char* szIp)
{
    sprintf(szIp, "%u.%u.%u.%u", ip&0xFF, (ip>>8)&0xFF , (ip>>16)&0xFF, ip>>24);
    return szIp;
}

///输出long long unsigned数值的字符串
inline char const* CwxCommon::toString(CWX_UINT64 ullNum, char* szBuf, int base)
{
    char const* szFormat=(16==base)?"%" PRIx64:"%" PRIu64;
    sprintf(szBuf, szFormat, ullNum);
    return szBuf;
}
///输出long long数值的字符串
inline char const* CwxCommon::toString(CWX_INT64 llNum, char* szBuf, int base)
{
    char const* szFormat=(16==base)?"%" PRIx64:"%" PRId64;
    sprintf(szBuf, szFormat, llNum);
    return szBuf;
}



CWINUX_END_NAMESPACE
