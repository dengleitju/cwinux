#ifndef __CWX_STREAM_H__
#define __CWX_STREAM_H__
/*
版权声明：
    本软件遵循GNU GPL V3（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/



/**
@file cwx_stream.h
@brief apache module的通信接口定义
@author cwinux@gmail.com
@version 0.1
@date 2009-10-12
@warning
@bug
*/

#include "cwx_config.h"

///获取当前的时间，UINT64
CWX_UINT64 cwx_time_of_day();
/**
*@brief tcp连接。
*@param [in] conn  请求的连接环境对象。
*@param [in] millisecond 连接的超时ms数
*@return -1：失败；否则为连接的handle
*/
int cwx_stream_connect(CWX_CONNECT* conn, CWX_UINT32 millisecond);
/**
*@brief 从连接上读取数据。
*@param [in] fd  连接的handle。
*@param [in] buf 存放读取数据的buf
*@param [in] length 读取的数据的长度
*@param [in] millisecond 读取的超时ms数
*@return -1：读取失败；0：表示超时；否则为读取的字节数，应该与length相等
*/
int cwx_stream_read(CWX_CONNECT* conn, void *buf, CWX_UINT32 length, CWX_UINT32 millisecond);
/**
*@brief 往连接上写数据。
*@param [in] fd  连接的handle。
*@param [in] buf 发送数据的buf
*@param [in] length 发送数据的长度
*@param [in] millisecond 发送的超时ms数
*@return -1：发送失败；0：表示超时；否则为发送的字节数，应该与length相等
*/
int cwx_stream_write(CWX_CONNECT* conn, void *buf, CWX_UINT32 length, CWX_UINT32 millisecond);
/**
*@brief 关闭tcp连接。
*@param [in] fd  连接的handle。
*@return -1：关闭失败；0：关闭成功
*/
int cwx_stream_close(int fd);
#endif
