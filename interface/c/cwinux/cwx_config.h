#ifndef __CWX_CONFIG_H__
#define __CWX_CONFIG_H__
/*
版权声明：
    本软件遵循GNU GPL V3（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/


/**
@file cwx_config.h
@brief 数据定义
@author cwinux@gmail.com
@version 0.1
@date 2010-10-04
@warning
@bug
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <arpa/inet.h>
#ifdef __cplusplus
extern "C" {
#endif

///CWX_UINT64 数据类型定义
#define CWX_UINT64   uint64_t
///CWX_INT64 数据类型定义
#define CWX_INT64   int64_t
///CWX_UINT32 数据类型定义
#define CWX_UINT32   uint32_t
///CWX_INT32 数据类型定义
#define CWX_INT32   int32_t
///CWX_UINT16 数据类型定义
#define CWX_UINT16   uint16_t
///CWX_INT16 数据类型定义
#define CWX_INT16   int16_t
///CWX_UINT8 数据类型定义
#define CWX_UINT8   uint8_t
///CWX_INT8 数据类型定义
#define CWX_INT8   int8_t

    ///属性的检查
# define CWX_CHECK_ATTR(WORD, ATTR) (((WORD) & (ATTR)) != 0)
    ///属性的设置
# define CWX_SET_ATTR(WORD, ATTR) (WORD |= (ATTR))
    ///属性的清空
# define CWX_CLR_ATTR(WORD, ATTR) (WORD &= ~(ATTR))

    ///位检查
# define CWX_CHECK_BIT(WORD, BIT) (((WORD) & (1<<(BIT))) != 0)
    ///位设置
# define CWX_SET_BIT(WORD, BIT) ((WORD) |= (1<<(BIT)))
    ///位清空
# define CWX_CLR_BIT(WORD, BIT) ((WORD) &= ~((1<<(BIT))))

#ifdef __cplusplus
}
#endif

#endif
