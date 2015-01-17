#ifndef __CWX_CRC32_H__
#define __CWX_CRC32_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/


#include <stddef.h>
#include <stdint.h>
#include <zlib.h>
#include <ctype.h>
#include <endian.h>
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxType.h"
/**
*@file  CwxMd5.h
*@brief  此文件定义了CwxCrc32的类，实现CRC32的签名
*@author cwinux@gmail.com
*@version 0.1
*@date    2011-05-16
*@warning 无
*@bug   
*/
CWINUX_BEGIN_NAMESPACE

class CWX_API CwxCrc32
{

public:
    // Return the crc32c of concat(A, data[0,n-1]) where init_crc is the
    // crc32c of some string A.  Extend() is often used to maintain the
    // crc32c of a stream of data.
    static CWX_UINT32 extend(CWX_UINT32 init_crc, const char* data, size_t n);

    // Return the crc32c of data[0,n-1]
    static inline CWX_UINT32 value(const char* data, size_t n)
    {
        return extend(0, data, n);
    }

private:
    CwxCrc32()
    {
    }
};

CWINUX_END_NAMESPACE

#include "CwxPost.h"

#endif
