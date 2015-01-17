CWINUX_BEGIN_NAMESPACE

///设置UNIX-DOMAIN的路径文件
inline void CwxAppHandler4UnixConn::setConnectPathFile(char const* szPathFile)
{
    m_strConnectPathFile = szPathFile;
}
///获取UNIX-DOMAIN的路径文件
inline string const& CwxAppHandler4UnixConn::getConnectPathFile() const
{
    return m_strConnectPathFile;
}


CWINUX_END_NAMESPACE

