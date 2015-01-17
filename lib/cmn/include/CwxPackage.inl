/**
*@file CwxPackage.inl
*@brief CwxPackage类的Inline实现
*@author cwinux@gmail.com
*@version 1.0
*@date  2009-06-05
*@warning  nothing
*@bug    
*/
CWINUX_BEGIN_NAMESPACE

//-1：包的格式非法；0:不存在；>0：Key/Value的长度。
inline int CwxPackage::getKeyByIndex(char const *szMsg, CWX_UINT32 uiMsgLen, CWX_UINT32 uiIndex, CwxKeyValueItem& item)
{
    CWX_UINT32 uiPos = 0;
    int len=0;
    if (!uiMsgLen) return 0;
    for (CWX_UINT32 i=0; i<=uiIndex; i++)
    {
        len = getNextKey(szMsg + uiPos, uiMsgLen - uiPos, item);
        if (-1 == len) return -1;
        if (0 == len) return 0;
        uiPos += len;
    }
    return len;
}

//-1：包的格式非法；0:不存在；>0：Key/Value的长度。
inline int CwxPackage::getKeyByName(char const *szMsg, CWX_UINT32 uiMsgLen, char const* szKey, CwxKeyValueItem& item, bool bCaseSensive)
{
    CWX_UINT32 uiPos = 0;
    int len=0;
    for ( ; uiPos<uiMsgLen; )
    {
        len = getNextKey(szMsg + uiPos, uiMsgLen - uiPos, item);
        if (-1 == len) return -1;
        if (0 == len) return 0;
        if (bCaseSensive)
        {
            if (strcmp(item.m_szKey, szKey)==0) return len;
        }
        else
        {
            if (strcasecmp(item.m_szKey, szKey)==0) return len;
        }
        uiPos += len;
    }
    return 0;
}

//-1：无效的package，0：没有发现，1：删除了一个KEY
inline int CwxPackage::removeKey(char *szMsg, CWX_UINT32& uiMsgLen, CWX_UINT16 unIndex)
{
    CWX_UINT32 i;
    CWX_UINT32 uiPos = 0;
    int len=0;
    CwxKeyValueItem item;
    for ( i=0; i<=unIndex; i++)
    {
        len = getNextKey(szMsg + uiPos, uiMsgLen - uiPos, item);
        if (-1 == len) return -1;
        if (0 == len) return 0;
        uiPos += len;
    }
    CwxCommon::memMove(szMsg + uiPos - len, szMsg+uiPos, uiMsgLen-uiPos);
    uiMsgLen -= len;
    return 1;
}

inline int CwxPackage::dump(char const* szMsg, CWX_UINT32 uiMsgLen, char* szOutBuf, CWX_UINT32& uiOutBufLen, char const* szTab, char const* szKeyBegin, char const* szKeyEnd, CwxEscape const* pEscape)
{
    int len=dumpEx(szMsg, uiMsgLen, szOutBuf, uiOutBufLen, szTab, 1, szKeyBegin, szKeyEnd, pEscape);
    if (0>len) return len;
    szOutBuf[len] = 0x00;
    return len;
}

//true:有效的包；false：无效的包.
inline bool CwxPackage::isValidPackage(char const *szMsg, CWX_UINT32 uiMsgLen)
{
    CWX_UINT32 uiPos = 0;
    int len=0;
    CwxKeyValueItem item;
    for ( ; uiPos<uiMsgLen ; )
    {
        len = getNextKey(szMsg + uiPos, uiMsgLen - uiPos, item);
        if (-1 == len) return false;
        if (0 == len) return true;
        uiPos += len;
    }
    return true;
}

///获取package的key的数量
inline int CwxPackage::getKeyValueNum(char const* szMsg, CWX_UINT32 uiMsgLen)
{
    int iKeyNum = 0;
    CWX_UINT32 uiPos = 0;
    int len=0;
    CwxKeyValueItem item;
    for ( ; uiPos<uiMsgLen ; )
    {
        len = getNextKey(szMsg + uiPos, uiMsgLen - uiPos, item);
        if (-1 == len) return -1;
        if (0 == len) return iKeyNum;
        uiPos += len;
        iKeyNum++;
    }
    return iKeyNum;
}



///通过Key的长度及data的长度，获取打包后的Key/value长度。
inline CWX_UINT32 CwxPackage::getKvLen(CWX_UINT16 unKeyLen, CWX_UINT32 uiDataLen)
{
    return 8 + unKeyLen + uiDataLen ;
}

///通过key/value的长度及key的长度，获取data的长度
inline CWX_UINT32 CwxPackage::getDataLen(CWX_UINT32 uiKeyValueLen, CWX_UINT16 unKeyLen)
{
    return uiKeyValueLen - unKeyLen - 8;
}

///返回key在key/value中的偏移
inline CWX_UINT16 CwxPackage::getKeyOffset()
{
    return 6;
}


CWINUX_END_NAMESPACE
