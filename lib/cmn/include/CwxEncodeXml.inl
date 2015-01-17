CWINUX_BEGIN_NAMESPACE

inline CwxEncodeXml::CwxEncodeXml(bool bGbk, bool bCaseSensive)
:m_bGbk(bGbk),m_bCaseSensive(bCaseSensive)
{
    m_pRepl = NULL;
}
///析构函数
inline CwxEncodeXml::~CwxEncodeXml()
{
    if (m_pRepl) delete m_pRepl;
}

inline bool CwxEncodeXml::init(map<string, string> const& append, map<string, string> const& remove)
{
    if (m_pRepl) delete m_pRepl;
    if (m_bGbk)
    {
        m_pRepl = new CwxReplMachine<CwxCharsetGbk>(100);
    }
    else
    {
        m_pRepl = new CwxReplMachine<CwxCharsetUtf8>(100);
    }
    if (!addStr("<", "&lt;", append, remove)) return false;
    if (!addStr(">", "&gt;", append, remove)) return false;
    if (!addStr("&", "&amp;", append, remove)) return false;
    if (!addStr("'", "&#39;", append, remove)) return false;
    char szMask[2];
    memset(szMask, 0x00, 2);
    for (int i=1; i<32; i++)
    {
        szMask[0] = i;
        if ((9 != i)/*\t*/ || (10 == i) /*\r*/ || (13 == i) )/*\n*/
        {
            if (!addStr(szMask, "", append, remove)) return false;
        }
    }
    map<string, string>::const_iterator iter=append.begin();
    map<string, string> empty;
    while(iter != append.end())
    {
        if (!addStr(iter->first.c_str(), iter->second.c_str(), empty, empty)) return false;
        iter++;
    }
    return true;
}

inline bool CwxEncodeXml::encode(char const* szIn, CWX_UINT32& uiInLen, char* szOut, CWX_UINT32& uiOutLen) const
{
    if (m_bCaseSensive)
    {
        return m_pRepl->replace(szIn, uiInLen, szOut, uiOutLen);
    }
    return m_pRepl->caseReplace(szIn, uiInLen, szOut, uiOutLen);
}

inline bool CwxEncodeXml::addStr(char const* src, char const* dst, map<string, string> const& append, map<string, string> const& remove)
{
    if ((append.find(src) != append.end()) || (remove.find(src) != remove.end())) return true;

    if (m_bCaseSensive)
    {
       return m_pRepl->addStr(src, strlen(src), dst, strlen(dst));
    }
    return m_pRepl->addCaseStr(src, strlen(src), dst, strlen(dst));
}

///返回是否GBK
inline bool CwxEncodeXml::isGbk() const
{
    return m_bGbk;
}
///返回是否大小写敏感
inline bool CwxEncodeXml::isCaseSensive() const 
{ 
    return m_bCaseSensive;
}


CWINUX_END_NAMESPACE

