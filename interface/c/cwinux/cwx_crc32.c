#include "cwx_crc32.h"



CWX_UINT32 cwx_crc32_extend(CWX_UINT32 crc, const char* buf, size_t size) 
{
    return crc32( crc, (unsigned char*)buf, size );
}

