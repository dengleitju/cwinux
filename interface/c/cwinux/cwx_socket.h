#ifndef __CWX_SOCKET_H__
#define __CWX_SOCKET_H__
/*
版权声明：
    本软件遵循GNU GPL V3（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/



/**
@file cwx_socket.h
@brief 跨window、unix平台的socket库
@author cwinux@gmail.com
@version 0.1
@date 2009-12-01
@warning
@bug
*/

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include<sys/un.h>
#endif


#ifdef CWX_SOCKET
#undef CWX_SOCKET;
#endif

#ifdef WIN32
#define CWX_SOCKET SOCKET
#define CWX_INVALID_SOCKET INVALID_SOCKET
#else
#define CWX_SOCKET int
#define CWX_INVALID_SOCKET -1
#endif


///获取当前的时间，uint64_t
uint64_t cwx_socket_timeofday();
///初始化库, 主要是为了兼容winsocket 0:成功；-1：失败，
int cwx_socket_init(char* szErr1K);
/**
*@brief tcp连接。
*@param [in] szHost  连接的主机，为主机名或IP地址。
*@param [in] unPort  连接的端口号。
*@param [in] millisecond 连接的超时ms数
*@param [in] szErr1K 出错时的错误buf，可以为NULL，否则空间必须大于1K
*@return CWX_INVALID_SOCKET：失败；否则为连接的handle
*/
CWX_SOCKET cwx_socket_tcp_connect(char const* szHost, unsigned short unPort, unsigned int millisecond, char* szErr1K);

/**
*@brief tcp连接。
*@param [in] szIp  连接的IP地址。
*@param [in] unPort  连接的端口号。
*@param [in] millisecond 连接的超时ms数
*@param [in] szErr1K 出错时的错误buf，可以为NULL，否则空间必须大于1K
*@return CWX_INVALID_SOCKET：失败；否则为连接的handle
*/
CWX_SOCKET cwx_socket_unix_connect(char const* szPath, char* szErr1K);

/**
*@brief 从连接上读取数据。
*@param [in] fd  连接的handle。
*@param [in] buf 存放读取数据的buf
*@param [in] length 读取的数据的长度
*@param [in] millisecond 读取的超时ms数
*@param [in] szErr1K 出错时的错误buf，可以为NULL，否则空间必须大于1K
*@return -1：读取失败；0：表示超时；否则为读取的字节数，应该与length相等
*/
int cwx_socket_read(CWX_SOCKET fd, void *buf, unsigned int length, unsigned int millisecond, char* szErr1K);

/**
*@brief 往连接上写数据。
*@param [in] fd  连接的handle。
*@param [in] buf 发送数据的buf
*@param [in] length 发送数据的长度
*@param [in] millisecond 发送的超时ms数
*@param [in] szErr1K 出错时的错误buf，可以为NULL，否则空间必须大于1K
*@return -1：发送失败；0：表示超时；否则为发送的字节数，应该与length相等
*/
int cwx_socket_write(CWX_SOCKET fd, void *buf, unsigned int length, unsigned int millisecond, char* szErr1K);

/**
*@brief 关闭tcp连接。
*@param [in] fd  连接的handle。
*@return -1：关闭失败；0：关闭成功
*/
int cwx_socket_close(CWX_SOCKET fd);
#endif
