#ifndef __CWX_CRC32_H__
#define __CWX_CRC32_H__


#include <stddef.h>
#include <stdint.h>
#include <ctype.h>
#include <endian.h>
#include <zlib.h>
#include "cwx_config.h"

/**
*@file  cwx_dd5.h
*@brief  crc32签名
*@author cwinux@gmail.com
*@version 0.1
*@date    2011-05-16
*@warning 无
*@bug   
*/
#ifdef __cplusplus
extern "C" {
#endif

#define  CWX_CRC32_MASK_DELTA  0xa282ead8ul

// Return the crc32c of concat(A, data[0,n-1]) where init_crc is the
// crc32c of some string A.  Extend() is often used to maintain the
// crc32c of a stream of data.
CWX_UINT32 cwx_crc32_extend(CWX_UINT32 init_crc, const char* data, size_t n);

// Return the crc32c of data[0,n-1]
inline CWX_UINT32 cwx_crc32_value(const char* data, size_t n)
{
    return cwx_crc32_extend(0, data, n);
}


#ifdef __cplusplus
}
#endif

#endif
