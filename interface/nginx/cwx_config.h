#ifndef __CWX_CONFIG_H__
#define __CWX_CONFIG_H__
/*
版权声明：
    本软件遵循GNU GPL V3（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/

/**
@file cwx_config.h
@brief apache module的数据定义
@author cwinux@gmail.com
@version 0.1
@date 2009-10-12
@warning
@bug
*/

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include <nginx.h>


///CWX_UINT64 数据类型定义
#define CWX_UINT64   uint64_t

///CWX_UINT32 数据类型定义
#define CWX_UINT32   uint32_t

///CWX_UINT16 数据类型定义
#define CWX_UINT16   uint16_t
///CWX_UINT8 数据类型定义
#define CWX_UINT8   uint8_t
///定义bool类型
#ifndef bool
#define bool unsigned char
#endif
///定义true
#ifndef true
#define true 1
#endif
///定义false
#ifndef false
#define false 0
#endif

///缺省输出respone
#define CWX_DEF_SHOW 0
///缺省输出delay
#define CWX_DEF_DELAY 1
///缺省为持久连接
#define CWX_DEF_PERSISTANT 1
///缺省的查询超时为10s
#define CWX_DEF_QUERY_TIMEOUT 10000
///缺省的连接超时为1s
#define CWX_DEF_CONN_TIMEOUT  1000
///缺省的回复超时为9s
#define CWX_DEF_REPLY_TIMEOUT 9000
///缺省的实效连接重试间隔为2s
#define CWX_DEF_RESTORE_TIMEOUT 2000
///缺省的最少空闲持久连接数为2
#define CWX_DEF_MIN_IDLE_CONN 2
///缺省的最多空闲持久连接数为4
#define CWX_DEF_MAX_IDLE_CONN 4

#define CWX_VERSION "1.0"


typedef struct {
    ngx_http_upstream_conf_t   upstream;
    bool                     show;
    bool                     delay;
    ngx_str_t                content_type;
} ngx_http_cwinux_loc_conf_t;



///key/value item
typedef struct CWX_KEY_VALUE{
    char*          m_key; ///<key的名字
    char*          m_lowerkey; ///<key的小写
    char*          m_value; ///<key的内容
    struct CWX_KEY_VALUE*  m_next; ///<下一个key
}CWX_KEY_VALUE;


#endif
