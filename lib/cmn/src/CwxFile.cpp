#include "CwxFile.h"
CWINUX_BEGIN_NAMESPACE
bool CwxFile::getDirFile(const string& strPath, list<string>& files)
{
    files.clear();
    DIR* dp;
    struct dirent* dir;
    if (NULL == (dp = opendir(strPath.c_str())))  return false;
    while(NULL != (dir = readdir(dp)))
    {
        if (strcmp(dir->d_name, ".") == 0 ||
            strcmp(dir->d_name, "..") == 0) 
            continue;
        if (isDir((strPath + dir->d_name).c_str())) continue;
        files.push_back(dir->d_name);
    }
    closedir(dp);
    return true;

}
string& CwxFile::getLastDirName(const string& dir, string& last)
{
    string::size_type pos = dir.find_last_of('/');
    if (string::npos == pos)
    {
        last = dir;
    }
    else
    {
        last = dir.substr(++pos);
    }
    return last;
}
bool CwxFile::readTxtLine(FILE* fd, string& line)
{
    char szBuf[1024];
    string msg;
    line.erase();
    memset(szBuf, 0x00, 1024);
    while(1)
    {
        if (fgets(szBuf, 1023, fd) == NULL)
        {
            if (!feof(fd))	return false;
            return true;
        }
        if (szBuf[strlen(szBuf)-1] == '\n'){//line end
            szBuf[strlen(szBuf)-1] = 0x00;
            line += szBuf;
			if (line.empty()) line="";
            break;
        }
        line += szBuf;
    }
    return true;
}
bool CwxFile::readTxtFile(string const& strFileName, string& strData)
{
    off_t offset = CwxFile::getFileSize(strFileName.c_str());
    if (-1  == offset) return false;
    size_t file_size = offset;
    FILE* fd =fopen(strFileName.c_str(), "rb");
    char* szBuf = new char[file_size + 1];
    memset(szBuf, 0x00, file_size + 1);
    if (file_size != fread(szBuf, 1, file_size, fd))
    {
        fclose(fd);
        delete [] szBuf;
        return false;
    }
    strData.assign(szBuf, file_size);
    fclose(fd);
    delete [] szBuf;
    return true;

}
CWINUX_END_NAMESPACE
