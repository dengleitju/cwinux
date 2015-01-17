#ifndef __CWX_FILE_H__
#define __CWX_FILE_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxFile.h
@brief 定义文件系统操作相关的类：CwxFile
@author cwinux@gmail.com
@version 0.1
@date 2009-10-10
@warning
@bug
*/

#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxType.h"
#include "CwxStl.h"
#include <dirent.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>

CWINUX_BEGIN_NAMESPACE

/**
@class CwxFile
@brief 实现了文件系统的常用操作，对象的方法全部为静态方法，对象不能实例化
*/
class CWX_API CwxFile
{
public:
    /**
    @brief 如果path指定的最后一级文件目录不存在，则创建。
    @param [in] path 文件目录
    @param [in] mode 文件的权限模式
    @return true：目录已经存在或创建成功；false：目录创建失败或指定的路径以存在但不是目录。
    */
    static bool createMissDir(char const* path, int mode=S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP);
    /**
    @brief 创建path指定的目录，目录必须不存在。
    @param [in] path 文件目录
    @param [in] mode 文件的权限模式
    @return true：创建成功；false：创建失败。
    */
    static bool createDir(char const* path, int mode=S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP);
    /**
    @brief 判断path对应的路径是否为一个文件。
    @param [in] path 要判断的路径名
    @return true：是文件；false：不是文件或路径不存在。
    */
    static bool isFile(char const* path);
    /**
    @brief 判断path对应的路径是否为一个目录。
    @param [in] path 要判断的路径名
    @return true：是目录；false：不是目录或路径不存在。
    */
    static bool isDir(char const* path);
    /**
    @brief 获取strFile对应的文件的大小。
    @param [in] strFile 文件名
    @return -1：获取失败；否则为文件的大小。
    */
    static off_t getFileSize(char const* strFile);
    /**
    @brief 获取文件的最后修改时间。
    @param [in] strFile 文件名
    @return 0：获取失败；否则为文件的最后修改时间。
    */
    static time_t getFileMTime(char const* strFile);
    /**
    @brief 将src指定的文件，移到dest的路径，dest中包含文件名。
    @param [in] src 要被搬移的文件
    @param [in] dest 新的文件名
    @return true：成功；false：失败。
    */
    static bool moveFile(char const*  src, char const*  dest);
    /**
    @brief 删除strFile对应的文件。
    @param [in] strFile 要删除的文件名
    @return true：成功；false：失败。
    */
    static bool rmFile(char const*  strFile);
    /**
    @brief 删除strPath对应的目录。
    @param [in] strPath 要删除的目录
    @return true：成功；false：失败。
    */
    static bool rmDir(char const*  strPath);
    /**
    @brief 切换当前进程的目录。
    @param [in] strPath 要切换到的目录名
    @return true：成功；false：失败。
    */
    static bool changeDir(char const* strPath);
    /**
    @brief 获取strPath目录下的所有直接文件，不包括子目录。
    @param [in] strPath 要获取文件的目录名
    @param [out] files 文件的列表，列表中的文件不包括路径名
    @return true：成功；false：失败。
    */
    static bool getDirFile(const string& strPath, list<string>& files);
    /**
    @brief 获取/xxx/yyyy/zzzz格式最后一级的名字，即zzzz
    @param [in] strPath 要获取最后一级名字的目录名
    @param [out] strLast 最后一级的名字
    @return 返回最后一级的名字，即strLast。
    */
    static string& getLastDirName(const string& strPath, string& strLast);
    /**
    @brief 从fd对应的文本文件中的当前文件指针位置，读一行数据。
    @param [in] fd 文本文件的文件句柄
    @param [out] strLine 读取的行，若返回成功而strLine.empty()==true，则表示到了文件尾
    @return true：成功；false：失败。
    */
    static bool readTxtLine(FILE* fd, string& strLine);
    /**
    @brief 读取整个文本文件的内容。
    @param [in] strFileName 要读取的文本文件的名字
    @param [out] strData 文本文件的内容
    @return true：成功；false：失败。
    */
    static bool readTxtFile(string const& strFileName, string& strData);
    /**
    @brief 对一个文件加全局的写锁。
    @param [in] fd 具有写权限的文件handle
    @return true：成功；false：失败，errno保存错误的原因。
    */
    static bool lock(int fd);
    /**
    @brief 解锁一个文件上加的全局写锁。
    @param [in] fd 枷锁的文件handle
    @return true：成功；false：失败，errno保存错误的原因。
    */
    static bool unlock(int fd);

private:
    ///私有化的构造函数，禁止实例化
    CwxFile(){}
};
CWINUX_END_NAMESPACE

#include "CwxFile.inl"
#include "CwxPost.h"

#endif
