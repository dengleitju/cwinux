#ifndef __CWX_ECHO_CLIENT_CONFIG_H__
#define __CWX_ECHO_CLIENT_CONFIG_H__
/*
版权声明：
    本软件遵循GNU GPL V3（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/
#include "CwxHostInfo.h"
#include "CwxCommon.h"
#include "CwxIniParse.h"

CWINUX_USING_NAMESPACE

///echo压力测试的配置文件加载对象
class CwxEchoClientConfig
{
public:
  CwxEchoClientConfig(){
    m_bTcp = true;
    m_unConnNum = 0;
    m_uiDataSize = 0;
    m_bLasting = true;
  }

  ~CwxEchoClientConfig(){}
public:
  //加载配置文件.-1:failure, 0:success
  int loadConfig(string const & strConfFile);
  //输出配置文件
  void outputConfig(string & strConfig);
  //获取加载配置文件的失败错误信息
  char const* getError() { return m_szError; };
public:
  bool                m_bTcp; ///<是否通过tcp进行连接
  string              m_strUnixPathFile;///<若采用unix domain连接，则为连接的path-file
  string              m_strWorkDir;///<工作目录
  CWX_UINT16          m_unConnNum;///<连接的数量
  CWX_UINT32          m_uiDataSize;///<echo的数据包的大小
  bool                m_bLasting;///<是否为持久连接，类似HTTP的keep-alive
  CwxHostInfo         m_listen;///<tcp连接的对方listen地址
  char                m_szError[2048];///<错误消息buf
};

#endif
