#include "CwxPackageWriter.h"

CWINUX_BEGIN_NAMESPACE

///构造函数，
CwxPackageWriter::CwxPackageWriter(CWX_UINT32 uiBufLen)
{
    m_uiKeyNum = 0;
    m_szPackMsg = new char[uiBufLen];
    m_uiPackBufLen = uiBufLen;
    m_uiCurPackPos = 0;
    memset(m_szErr, 0x00, ERR_MSG_LEN+1);
}
///析构
CwxPackageWriter::~CwxPackageWriter()
{
    if (m_szPackMsg) delete [] m_szPackMsg;
}

void CwxPackageWriter::reset()
{
    m_uiKeyNum = 0;
    m_uiCurPackPos = 0;
    memset(m_szErr, 0x00, ERR_MSG_LEN+1);
}

///Add a new key.true: success; false:failure, get err-msg by GetError().
bool CwxPackageWriter::addKeyValue(char const* szKey, CWX_UINT16 unKeyLen, char const* szData, CWX_UINT32 uiDataLen, bool bKeyValue){
    if (this->m_uiPackBufLen < this->m_uiCurPackPos + CwxPackage::getKvLen(unKeyLen, uiDataLen)){
        CWX_UINT32 uiNewSize = m_uiPackBufLen * 2 + CwxPackage::getKvLen(unKeyLen, uiDataLen);
        char* pNew = new char[uiNewSize];
        if (!pNew){
            CwxCommon::snprintf(this->m_szErr, ERR_MSG_LEN, "Failure to malloc buf, size=%u", uiNewSize);
            return false;
        }
        if (m_uiCurPackPos){
            memcpy(pNew, m_szPackMsg, m_uiCurPackPos);
        }
        m_uiPackBufLen = uiNewSize;
        delete [] m_szPackMsg;
        m_szPackMsg = pNew;
    }
    int iRet = 0;
    if (0 ==(iRet = CwxPackage::appendKey(this->m_szPackMsg + this->m_uiCurPackPos,
        this->m_uiPackBufLen - this->m_uiCurPackPos,
        szKey,
        unKeyLen,
        szData,
        uiDataLen,
        bKeyValue)))
    {
        CwxCommon::snprintf(this->m_szErr, ERR_MSG_LEN, "No memory to append the key: %s", szKey);
        return false;
    }
    this->m_uiKeyNum ++;
    this->m_uiCurPackPos += iRet;
    return true;
}



CWINUX_END_NAMESPACE
