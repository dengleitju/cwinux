#ifndef __CWX_MSG_HEADER_H__
#define __CWX_MSG_HEADER_H__
/*
版权声明：
    本软件遵循GNU GPL V3（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/


/**
@file cwx_msg_header.h
@brief 定义cwinux 消息头
@author cwinux@gmail.com
@version 1.0
@date 2010-10-04
@warning
@bug
*/
#include "cwx_config.h"

#ifdef __cplusplus
extern "C" {
#endif


///定义通信包头的长度
#define  CWX_MSG_HEAD_LEN   14

///定义系统消息的属性
#define  CWX_MSG_ATTR_SYS_MSG (1<<0)
///定义数据压缩的属性
#define  CWX_MSG_ATTR_COMPRESS (1<<1)

///keep alive的消息类型
#define CWX_MSG_TYPE_KEEPALIVE  1
///keep alive回复的消息类型
#define CWX_MSG_TYPE_KEEPALIVE_REPLY 2

///定义通信包头
typedef struct CWX_MSG_HEADER_S
{
    CWX_UINT8      m_ucVersion;  ///<消息版本号
    CWX_UINT8      m_ucAttr;     ///<消息属性
    CWX_UINT16     m_unMsgType;  ///<消息类型
    CWX_UINT32     m_uiTaskId;   ///<任务号ID
    CWX_UINT32     m_uiDataLen;  ///<发送的数据长度
}CWX_MSG_HEADER_S;


/**
*@brief 将消息头打包成网络字节序的数据包返回
*@param [in] header 进行pack的消息包头。
*@param [out] szHead packed的消息包头buf
*@return 返回szHead
*/
char const* cwx_msg_pack_head(CWX_MSG_HEADER_S const*  header, char* szHead);
/**
*@brief 将收到的消息头，解成消息头对象。
*@param [in] szHead 消息包头的buf
*@param [out] header 消息包头
*@return -1:校验码错误；0:解包正确
*/
int cwx_msg_unpack_head(char const* szHead, CWX_MSG_HEADER_S*  header);

/**
*@brief 是否为keep-alive的消息
*@param [in] header 消息包头。
*@return 1:是；0：不是
*/
int cwx_msg_is_keepalive(CWX_MSG_HEADER_S const*  header);

/**
*@brief 是否为keep-alive的回复消息
*@param [in] header 消息包头。
*@return 1:是；0：不是
*/
int cwx_msg_is_keepalive_reply(CWX_MSG_HEADER_S const*  header);

/**
*@brief 是否系统消息
*@param [in] header 消息包头。
*@return 1:是；0：不是
*/
int cwx_msg_is_sys_msg(CWX_MSG_HEADER_S const*  header);

/**
*@brief 设置系统消息标记
*@param [in] header 消息包头。
*@return void
*/
void cwx_msg_set_sys_msg(CWX_MSG_HEADER_S*  header);

/**
*@brief 设置消息头为keep-alive信息
*@param [in] header 消息包头。
*@return void
*/
void cwx_msg_set_keepalive(CWX_MSG_HEADER_S *  header);

/**
*@brief 设置消息头为keep-alive的reply信息
*@param [in] header 消息包头。
*@return void
*/
void cwx_msg_set_keepalive_reply(CWX_MSG_HEADER_S *  header);


#ifdef __cplusplus
}
#endif

#endif

