CWINUX_BEGIN_NAMESPACE
inline bool CwxFile::createMissDir(char const* path, int mode)
{
    if (false == isDir(path))
    {
        return  (0 == mkdir(path, mode));
    }
    return true;
}

inline bool CwxFile::createDir(char const* path, int mode)
{
    return  (0 == mkdir(path, mode));
}


inline bool CwxFile::isFile(char const* strFile)
{
    struct stat info;
    return (0 == stat(strFile, &info))?S_ISREG(info.st_mode):false;
}
inline bool CwxFile::isDir(char const*  strFile)
{
    struct stat info;
    return (0 == stat(strFile, &info))?S_ISDIR(info.st_mode):false;
}
inline off_t CwxFile::getFileSize(char const*  strFile)
{
    struct stat info;
    return (0 == stat(strFile, &info))?info.st_size:-1;
}
inline time_t CwxFile::getFileMTime(char const*  strFile)
{
    struct stat info;
    return (0 == stat(strFile, &info))?info.st_mtime:0;
}
inline bool CwxFile::moveFile(char const*  src, char const*  dest)
{
    if (isFile(dest))  rmFile(dest);
    return (0 == rename(src, dest));
}
inline bool CwxFile::rmFile(char const*  strFile)
{
  return (0 == unlink(strFile));
}
inline bool CwxFile::rmDir(char const*  strPath)
{
    return  (0 == rmdir(strPath));
}
inline bool CwxFile::changeDir(char const*  strPath)
{
    return (0 == chdir(strPath));
}

inline bool CwxFile::lock(int fd)
{
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence =SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    do
    {
        if (-1 == fcntl(fd, F_SETLK,  reinterpret_cast<long>(&lock)))
        {
            if (EINTR == errno)
            {
                continue;
            }
            return false;
        }
    }while(0);
    return true;
}

inline bool CwxFile::unlock(int fd)
{
    struct flock lock;
    lock.l_type = F_UNLCK;
    lock.l_whence =SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    return (-1 == fcntl(fd, F_SETLKW,  reinterpret_cast<long>(&lock)))?false:true;
}

CWINUX_END_NAMESPACE
