#ifndef __CWX_ECHO_CONFIG_H__
#define __CWX_ECHO_CONFIG_H__

/*
版权声明：
    本软件遵循GNU GPL V3（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/
#include "CwxGlobalMacro.h"
#include "CwxHostInfo.h"
#include "CwxCommon.h"
#include "CwxIniParse.h"
#include "CwxMsgQueue.h"
#include "CwxTss.h"

CWINUX_USING_NAMESPACE

  // echho的tss
class EchoTss:public CwxTss {
public:
  EchoTss():CwxTss(){
    m_curId = 0;
  }
  // 析构函数
  ~EchoTss() {}
public:
  // tss的初始化，0：成功；-1：失败
  int Init(){return 0;}
public:
  CwxMsgQueue         m_queue;
  CWX_UINT64          m_curId;
};

///配置文件加载对象
class CwxEchoConfig
{
public:
    CwxEchoConfig(){
        m_unThreadNum = 0;
    }    
    ~CwxEchoConfig(){}
public:
    //加载配置文件.-1:failure, 0:success
    int loadConfig(string const & strConfFile);
    //输出加载的配置文件信息
    void outputConfig(string & strConfig);
    //获取配置文件加载的失败原因
    char const* getError() { return m_szError; };
public:
    string              m_strWorkDir;///<工作目录
    CWX_UINT16          m_unThreadNum;///<echo服务的echo线程数量
    CwxHostInfo         m_listen;///<tcp的监听ip/port
    char                m_szError[2048];///<错误消息的buf
};

#endif
