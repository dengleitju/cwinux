#ifndef __CWX_MSG_HEADER_H__
#define __CWX_MSG_HEADER_H__
/*
版权声明：
    本软件遵循GNU GPL V3（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/

/**
@file cwx_msg_header.h
@brief 定义cwinux module与后台服务通信数据包的包头及操作
@author cwinux@gmail.com
@version 0.1
@date 2009-10-12
@warning
@bug
*/
#include "cwx_config.h"

///定义通信包头的长度
#define  CWINUX_MSG_HEAD_LEN   14
//定义attr
///定义系统消息的属性
#define  CWINUX_MSG_ATTR_SYS_MSG (1<<0)
///定义数据压缩的属性
#define  CWINUX_MSG_ATTR_COMPRESS (1<<1)
///定义通信包头
typedef struct CWX_MSG_HEADER_S{
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
char const* cwx_pack_head(CWX_MSG_HEADER_S const*  header, char* szHead);
/**
*@brief 将收到的消息头，解成消息头对象。
*@param [in] szHead 消息包头的buf
*@param [out] header 消息包头
*@return false:校验码错误；true:解包正确
*/
bool cwx_unpack_head(char const* szHead, CWX_MSG_HEADER_S*  header);


#endif

