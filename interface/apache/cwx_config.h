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

#include "httpd.h"
#include "http_config.h"
#include "http_core.h"
#include "http_protocol.h"
#include "apr_lib.h"
#include "apr_hash.h"
#include "ap_config.h"
#include "apr_strings.h"
#include "apr_thread_mutex.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

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

///如果不是多线程环境，则不进行临界区保护
#if ! APR_HAS_THREADS
///锁的数据类型
#define apr_thread_mutex_t int
///创建锁
#define apr_thread_mutex_create(a,b,c)
///获取锁
#define apr_thread_mutex_lock(a)
///释放锁
#define apr_thread_mutex_unlock(a)
#endif

#define CWX_VERSION "1.0"

//日志输出
#ifdef _DEBUG
#define CWX_LOG(x) cwx_log x
#define CWX_ERR(x) cwx_log x
#else
#define CWX_LOG(x)
#define CWX_ERR(x) cwx_log x
#endif

struct CWX_HOST;
struct CWX_CONFIG;
struct CWX_SERVICE;

///key/value item
typedef struct CWX_KEY_VALUE{
    char*          m_key; ///<key的名字
    char*          m_lowerkey; ///<key的小写
    char*          m_value; ///<key的内容
    struct CWX_KEY_VALUE*  m_next; ///<下一个key
}CWX_KEY_VALUE;

///负载均衡与response key的数据结构
typedef struct CWX_KEY_ITEM{
    apr_hash_t*         m_bkey_hash; ///<参与负载均衡的key的hash，若为空，则采用m_ex_bkey_hash
    apr_hash_t*         m_ex_bkey_hash; ///<不参与负载均衡的key的hash，若与m_bkey_hash都为空，表示全部参与参与负载均衡。
    CWX_KEY_VALUE*      m_header; ///<http response的key列表，若为空，则表示没有输出的key
}CWX_KEY_ITEM;

///服务类型及消息类型的头
typedef struct CWX_SVR_MSG_KEY{
    char*               m_svr_name; ///<服务名
    char*               m_msg_type; ///<消息类型
    bool                m_exclude;
    CWX_KEY_VALUE*       m_key_value; ///<对应的key/value值
    struct CWX_SVR_MSG_KEY*     m_next; ///<下一个
}CWX_SVR_MSG_KEY;


///用户的配置信息数据结构
typedef struct CWX_USER_CONFIG{
    int           m_show; ///<是否将后台服务的key/value对外输出。-1:没有设置，0：false；1：true
    int           m_delay; ///<是否输出cwinux-delay的header。-1:没有设置，0：false；1：true
    int           m_persistant; ///<是否为持久连接。-1:没有设置，0：false；1：true
    int           m_query_timeout; ///<查询的超时ms数。-1：没有设置，否则为设置的数值。
    int           m_conn_timeout; ///<连接的超时ms数。-1：没有设置，否则为设置的数值。
    int           m_reply_timeout; ///<回复的超时ms数。-1：没有设置，否则为设置的数值。
    int           m_restore_timeout; ///<恢复的时间间隔，单位为ms。-1：没有设置，否则为设置的数值。
    int           m_min_idle_conn; ///<最少的空闲持久连接数量。-1：没有设置，否则为设置的数值。
    int           m_max_idle_conn; ///<最多的空闲持久连接数量。-1：没有设置，否则为设置的数值。
}CWX_USER_CONFIG;

typedef struct CWX_SOCKET{
    int                m_fd;   ///<连接的socket
    struct CWX_SOCKET*  m_next; ///<下一个连接
}CWX_SOCKET;

///定义链接对象的数据结构
typedef struct CWX_CONNECT{
    struct CWX_CONFIG*  m_config; ///<module的config
    apr_pool_t*         m_pool; ///<request's pool
    struct CWX_KEY_ITEM  m_key_item; ///<key的配置
    struct CWX_HOST*    m_host; ///<连接的HOST对象
    CWX_SOCKET*         m_socket;   ///<连接的socket
    char*              m_svr_name; ///<服务名字
    char*              m_msg_type; ///<消息类型
    request_rec*        m_request; ///<http request
    char*              m_args; ///<http request's args
    CWX_KEY_VALUE*      m_args_head; ///<url  args's head
    CWX_KEY_VALUE*      m_args_tail; ///<url's args's tail
    struct CWX_SERVICE*        m_service; ///<connect's service object
    int                m_def_host_index; ///<connect's init host
    int                m_cur_host_index; ///<current connect's host index
}CWX_CONNECT;

///主机信息
typedef struct CWX_HOST{
  char*              m_svr_name;///<host的服务名
  int                m_index; ///<主机的索引
  char*              m_ip;///<host的ip
  CWX_UINT16         m_port;///<host的服务的port
  struct CWX_USER_CONFIG     m_user_config;///<host的用户配置，将merge service与default值。
  bool               m_is_valid; ///<是否有效
  CWX_UINT64          m_invalid_time; ///<失效的时间。
  struct in_addr      m_host; ///<主机的addr struct
  apr_thread_mutex_t*  m_lock;  ///<主机的锁
  CWX_SOCKET*         m_idle_socket_head; ///<空闲连接的头;
  int                m_idle_socket_num; ///<空闲连接的数量。
  int                m_used_socket_num; ///<被使用的连接的数量
  struct CWX_SERVICE*  m_service; ///<主机所属的service;
  struct CWX_CONFIG*   m_config; ///< apache module的config
  struct CWX_HOST*    m_next; ///<下一个主机;
} CWX_HOST;

///服务数据对象
typedef struct CWX_SERVICE{
   char*                    m_svr_name;///<服务名
   struct CWX_USER_CONFIG    m_user_config;///<service的用户配置，将merge default值。
   struct CWX_KEY_ITEM       m_key_item; ///<key的配置
   struct CWX_HOST**         m_hosts; ///<host array
   int                      m_host_num; ///<host num;
   apr_hash_t*               m_uri_key_hash; ///<url's key config hash
   struct CWX_CONFIG*        m_config; ///< apache module的config
} CWX_SERVICE;

///apache module的config
typedef struct CWX_CONFIG {
  apr_pool_t*         m_pool; ///<process mempool
  apr_thread_mutex_t*  m_lock;  ///<全局的锁
  apr_hash_t*          m_svr_hash; ///<svr的hash
  int                 m_svr_num; ///服务的数量
  struct CWX_USER_CONFIG   m_user_config;///<service的用户配置，将merge default值。
  struct CWX_KEY_ITEM     m_key_item; ///<key的配置
  struct CWX_HOST*     m_hosts; ///<host chain's head
  int                 m_host_num; ///<host num;
  struct CWX_SVR_MSG_KEY*   m_balance; ///<balance's config
  struct CWX_SVR_MSG_KEY*   m_header; ///<header's config
  bool                    m_init; ///<系统是否初始化
}CWX_CONFIG;
#endif
