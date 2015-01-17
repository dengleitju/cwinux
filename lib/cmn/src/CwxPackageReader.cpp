#include "CwxPackageReader.h"

CWINUX_BEGIN_NAMESPACE

///构造函数，
CwxPackageReader::CwxPackageReader(bool bCaseSensive)
:m_bCaseSensive(bCaseSensive)
{
    m_pKeyValue = NULL;
    m_pKeyIndex = NULL;
    m_uiTotalKeyNum = 0;
    m_uiKeyNum = 0;
    m_szPackMsg = NULL;
    m_uiPackBufLen = 0;
    m_bIndex = false;
    memset(m_szErr, 0x00, ERR_MSG_LEN+1);
}
///析构
CwxPackageReader::~CwxPackageReader()
{
    if (m_pKeyIndex) delete m_pKeyIndex;
    if (m_pKeyValue) delete [] m_pKeyValue;
}

//NULL, not exist, otherwise, it's the first key
CwxKeyValueItem const* CwxPackageReader::getKey(char const* szKey, bool bSubKey) const
{
    char const* pNextSub = NULL;
    if (bSubKey){
        pNextSub = strchr(szKey, '.');
    }
    if (!bSubKey || !pNextSub){
        if (m_bIndex){
            if (m_pKeyIndex){
                CWX_PACKAGE_KEY_MAP_ITER iter = m_pKeyIndex->find(szKey);
                if (iter != m_pKeyIndex->end()){
                    return iter->second;
                }
            }
        }else{
            for (CWX_UINT32 i=0; i<m_uiKeyNum; i++){
                if (m_bCaseSensive){
                    if (strcmp(szKey, m_pKeyValue[i].m_szKey) == 0){
                        return &m_pKeyValue[i];
                    }
                }else{
                    if (strcasecmp(szKey, m_pKeyValue[i].m_szKey) == 0){
                        return &m_pKeyValue[i];
                    }
                }
            }
        }
    }else{
        string key(szKey, szKey - pNextSub);
        pNextSub++;
        CwxKeyValueItem const* pItem = this->getKey(key.c_str(), false);
        if (!pItem) return NULL;
        if (!pItem->m_bKeyValue) return NULL;
        CwxPackageReader reader(m_bCaseSensive);
        if (!reader.unpack(pItem->m_szData, pItem->m_uiDataLen, false, m_bCaseSensive)) return NULL;
        pItem = reader.getKey(pNextSub, true);
        if (pItem){
            m_tmpKey = *pItem;
            return &m_tmpKey;
        }
        return NULL;
    }
    return NULL;
}

void CwxPackageReader::reset()
{
    if (m_pKeyValue){
        memset(m_pKeyValue, 0x00, sizeof(CwxKeyValueItem) * m_uiTotalKeyNum);
    }
    if (m_pKeyIndex) delete m_pKeyIndex;
    m_pKeyIndex = NULL;
    m_uiKeyNum = 0;
    m_szPackMsg = 0;
    m_uiPackBufLen = 0;
    m_bIndex = false;
    memset(m_szErr, 0x00, ERR_MSG_LEN+1);
}


//false:错误；true:解包正确。通过GetError()获取失败的原因。
bool CwxPackageReader::unpack(char const* szMsg, CWX_UINT32 uiMsgLen, bool bIndex, bool bCaseSensive)
{
    int  iRet = 0;
    reset();
    m_uiPackBufLen = uiMsgLen;
    m_szPackMsg = szMsg;
    m_bIndex = bIndex;
    m_bCaseSensive = bCaseSensive;
    if (0 == uiMsgLen) return true;

    CwxCharCaseLess2 cmp(m_bCaseSensive);
    m_pKeyIndex = new CWX_PACKAGE_KEY_MAP(cmp);
    
    iRet = CwxPackage::getKeyValueNum(szMsg, uiMsgLen);
    if (0 > iRet)
    {
        strcpy(this->m_szErr, "It's not a valid package");
        return false;
    }
    m_uiKeyNum = iRet;
    if (m_uiTotalKeyNum < m_uiKeyNum)
    {
        if (m_pKeyValue) delete [] m_pKeyValue;
        m_uiTotalKeyNum = (m_uiKeyNum + KEY_VALUE_ALIGN -1)/KEY_VALUE_ALIGN;
        m_uiTotalKeyNum *= KEY_VALUE_ALIGN;
        m_pKeyValue = new CwxKeyValueItem[m_uiTotalKeyNum];
        memset(m_pKeyValue, 0x00, sizeof(CwxKeyValueItem) * m_uiTotalKeyNum);
    }
    CWX_UINT32 uiPos = 0;
    CWX_UINT32 uiIndex = 0;
    while(uiPos < m_uiPackBufLen)
    {
        if (uiIndex == m_uiKeyNum)
        {
            CwxCommon::snprintf(this->m_szErr, ERR_MSG_LEN, "The package should be kv[%u], but exceed.", m_uiKeyNum);
            CWX_ASSERT(0);
            return false;
        }
        if (-1 == (iRet = CwxPackage::getNextKey(this->m_szPackMsg + uiPos,
            m_uiPackBufLen - uiPos,
            this->m_pKeyValue[uiIndex])))
        {
            CwxCommon::snprintf(this->m_szErr, ERR_MSG_LEN, "Can't get the %dth key/value", uiIndex);
            return false;
        }
        if (0 == iRet) break; //finish
        if (m_bIndex) (*m_pKeyIndex)[this->m_pKeyValue[uiIndex].m_szKey] = &this->m_pKeyValue[uiIndex];
        uiIndex ++;
        uiPos += iRet;
        if (uiPos == uiMsgLen) return true; //finish.
    }
    return true;
}

CWINUX_END_NAMESPACE
