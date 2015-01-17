CWINUX_BEGIN_NAMESPACE

inline bool CwxChrType::isAlnum(int c)
{
    return isalnum(c)?true:false;
}

inline bool CwxChrType::isAlpha(int c)
{
    return isalpha(c)?true:false;
}

inline bool CwxChrType::isAscii(int c)
{
    return isascii(c)?true:false;
}

inline bool CwxChrType::isBlank(int c)
{
    return isblank(c)?true:false;
}

inline bool CwxChrType::isCntrl(int c)
{
    return iscntrl(c)?true:false;
}

inline bool CwxChrType::isDigit(int c)
{
    return isdigit(c)?true:false;
}

inline bool CwxChrType::isGraph(int c)
{
    return isgraph(c)?true:false;
}

inline bool CwxChrType::isLower(int c)
{
    return islower(c)?true:false;
}

inline bool CwxChrType::isPrint(int c)
{
    return isprint(c)?true:false;
}

inline bool CwxChrType::isPunct(int c)
{
    return ispunct(c)?true:false;
}

inline bool CwxChrType::isSpace(int c)
{
    return isspace(c)?true:false;
}

inline bool CwxChrType::isUpper(int c)
{
    return isupper(c)?true:false;
}

inline bool CwxChrType::isXdigit(int c)
{
    return isxdigit(c)?true:false;
}

CWINUX_END_NAMESPACE
