#ifndef __CWX_HOST_INFO_H__
#define __CWX_HOST_INFO_H__
/*
版权声明：
本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxHostInfo.h
@brief 定义cwinux架构Host对象定义
@author cwinux@gmail.com
@version 0.1
@date 2009-10-10
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxType.h"
#include "CwxStl.h"
#include "CwxStlFunc.h"

CWINUX_BEGIN_NAMESPACE

/**
@class CwxHostInfo
@brief Host数据对象，定义HOST的常用信息
*/
class CWX_API CwxHostInfo
{
public:
  enum{
    DEF_MIN_INTERNAL = 2,
    DEF_MAX_INTERNAL = 16,
    DEF_RAW_DATA_LEN = 2048
  };
public:
  ///构造默认函数
  CwxHostInfo() {
    reset();
  }
  ///构造函数
  CwxHostInfo(string const& strHost,        
    string const& strUser,
    CWX_UINT16 unPort)
  {
    m_strHost = strHost;
    m_strUser = strUser;
    m_unPort = unPort;
    m_bKeepAlive = true;
    m_bRawData = false;
    m_uiRawDataLen = DEF_RAW_DATA_LEN;
    m_unMinInternal = DEF_MIN_INTERNAL;
    m_unMaxInternal = DEF_MAX_INTERNAL;
  }
  ///拷贝构造函数
  CwxHostInfo(const CwxHostInfo& obj){
    m_strHost = obj.m_strHost;
    m_strUnixPathFile = obj.m_strUnixPathFile;
    m_strUser = obj.m_strUser;
    m_strPasswd = obj.m_strPasswd;
    m_unPort = obj.m_unPort;
    m_unGroupId = obj.m_unGroupId;
    m_uiSvrId = obj.m_uiSvrId;
    m_uiHostId = obj.m_uiHostId;
    m_uiConnId = obj.m_uiConnId;
    m_bKeepAlive = obj.m_bKeepAlive;
    m_bRawData = obj.m_bRawData;
    m_uiRawDataLen = obj.m_uiRawDataLen;
    m_unMinInternal = obj.m_unMinInternal;
    m_unMaxInternal = obj.m_unMaxInternal;
  }
  ///赋值操作
  CwxHostInfo& operator=(const CwxHostInfo& obj){
    if(this != & obj){
      m_strHost = obj.m_strHost;
      m_strUnixPathFile = obj.m_strUnixPathFile;
      m_strUser = obj.m_strUser;
      m_strPasswd = obj.m_strPasswd;
      m_unPort = obj.m_unPort;
      m_unGroupId = obj.m_unGroupId;
      m_uiSvrId = obj.m_uiSvrId;
      m_uiHostId = obj.m_uiHostId;
      m_uiConnId = obj.m_uiConnId;
      m_bKeepAlive = obj.m_bKeepAlive;
      m_bRawData = obj.m_bRawData;
      m_uiRawDataLen = obj.m_uiRawDataLen;
      m_unMinInternal = obj.m_unMinInternal;
      m_unMaxInternal = obj.m_unMaxInternal;
    }
    return *this;
  }
  ///清空主机信息
  void reset(){
    m_strHost.erase();
    m_strUnixPathFile.erase();
    m_strUser.erase();
    m_strPasswd.erase();
    m_unPort = 0;
    m_unGroupId = 0;
    m_uiSvrId = 0;
    m_uiHostId = 0;
    m_uiConnId = 0;
    m_bKeepAlive = true;
    m_bRawData = false;
    m_uiRawDataLen = DEF_RAW_DATA_LEN;
    m_unMinInternal = DEF_MIN_INTERNAL;
    m_unMaxInternal = DEF_MAX_INTERNAL;
  }

public:
  ///获取主机的名字或unix-domain file
  string const& getHostName() const {
    return m_strHost;
  }
  ///设置主机的名字或unix-domain file
  void setHostName(string const& strHost){ 
    m_strHost = strHost; 
  }
  ///获取unix domain path file
  string const& getUnixDomain() const { 
    return m_strUnixPathFile;
  }
  ///设置unix domain path file
  void setUnixDomain(string const& strPathFile) { 
    m_strUnixPathFile = strPathFile;
  }
  ///获取主机的用户名
  string const& getUser() const{
    return m_strUser;
  }
  ///设置主机的用户名
  void setUser(string const& strUser) {
    m_strUser = strUser;
  }
  ///获取主机的口令
  string const& getPasswd() const {
    return m_strPasswd;
  }
  ///设置主机的口令
  void setPassword(string const& strPasswd){
    m_strPasswd = strPasswd;
  }
  ///获取主机的端口号
  CWX_UINT16 getPort() const { 
    return m_unPort;
  }
  ///设置主机的端口号
  void setPort(CWX_UINT16 unPort){ 
    m_unPort = unPort;
  }
  ///获取主机的分组ID
  CWX_UINT16 getGroupId() const { 
    return m_unGroupId;
  }
  ///设置主机的分组ID
  void setGroupId(CWX_UINT16 unGroupId) { 
    m_unGroupId = unGroupId;
  }
  ///获取主机的服务ID
  CWX_UINT32 getSvrId() const { 
    return m_uiSvrId;
  }
  ///设置主机的服务ID
  void setSvrId(CWX_UINT32 uiSvrId) {
    m_uiSvrId = uiSvrId;
  }
  ///获取主机的HOST ID
  CWX_UINT32 getHostId() const { 
    return m_uiHostId;
  }
  ///设置主机的Host ID
  void setHostId(CWX_UINT32 uiHostId) {
    m_uiHostId = uiHostId;
  }
  ///获取主机的连接ID
  CWX_UINT32 getConnId() const {
    return m_uiConnId;
  }
  ///设置主机的连接ID
  void setConnID(CWX_UINT32 uiConnId) {
    m_uiConnId = uiConnId;
  }
  ///获取是否执行keep-alive
  bool isKeepAlive() const {
    return m_bKeepAlive;
  }
  ///设置是否执行keep-alive
  void setKeepAlive(bool bKeepAlive) { 
    m_bKeepAlive = bKeepAlive;
  }
  ///获取是否采用raw数据的格式通信
  bool isRawData() const {
    return m_bRawData;
  }
  ///设置是否采用raw数据的格式通信
  void setRawData(bool bRawData){
    m_bRawData = bRawData;
  }
  ///获取raw数据的每次最大接收长度
  CWX_UINT32 getRawDataLen() const {
    return m_uiRawDataLen;
  }
  ///设置raw数据的每次最大接收长度
  void setRawDataLen(CWX_UINT32 uiLen) {
    m_uiRawDataLen = uiLen;
  }
  ///获取主动连接失效时的最小重连间隔
  CWX_UINT16 getMinInternal() const {
    return m_unMinInternal;
  }
  ///设置主动连接失效时的最小重连间隔
  void setMinInternal(CWX_UINT16 unInternal) {
    m_unMinInternal = unInternal;
  }
  ///获取主动连接失效时的最大重连间隔
  CWX_UINT16 getMaxInternal() const {
    return m_unMaxInternal;
  }
  ///设置主动连接失效时的最大重连间隔
  void setMaxInternal(CWX_UINT16 unInternal) {
    m_unMaxInternal = unInternal;
  }
private:
  string			    m_strHost;///<主机的IP
  string             m_strUnixPathFile; ///<unix-domain file
  string			    m_strUser;///<主机的用户名
  string              m_strPasswd; ///<主机用户的口令
  CWX_UINT16         m_unPort;///<主机的主端口号
  CWX_UINT16         m_unGroupId;///<主机所属的分组ID
  CWX_UINT32         m_uiSvrId;///<主机所属的服务类型ID
  CWX_UINT32         m_uiHostId;///<主机的host ID
  CWX_UINT32         m_uiConnId;///<主机的链接ID
  bool               m_bKeepAlive; ///<是否keep-alive
  bool               m_bRawData; ///<是否是Raw数据
  CWX_UINT32         m_uiRawDataLen; ///<raw 数据的最大接收长度
  CWX_UINT16         m_unMinInternal; ///<最小的重连间隔
  CWX_UINT16         m_unMaxInternal; ///<最大的重连间隔
};


CWINUX_END_NAMESPACE

#include "CwxPost.h"

#endif

