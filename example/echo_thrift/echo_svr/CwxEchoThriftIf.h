#ifndef __CWX_ECHO_THRIFT_IF_H__
#define __CWX_ECHO_THRIFT_IF_H__
/*
版权声明：
    本软件遵循GNU GPL V3（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/
#include "CwxGlobalMacro.h"
#include "CwxCommon.h"

#include "Echo.h"
#include <concurrency/ThreadManager.h>
#include <concurrency/PosixThreadFactory.h>
#include <protocol/TBinaryProtocol.h>
#include <server/TSimpleServer.h>
#include <server/TThreadPoolServer.h>
#include <server/TThreadedServer.h>
#include <transport/TServerSocket.h>
#include <transport/TBufferTransports.h>
#include <transport/TSocket.h>
#include <transport/TTransportUtils.h>

typedef apache::thrift::TException TException;
typedef apache::thrift::TProcessor TProcessor;

typedef apache::thrift::concurrency::Mutex TMutex;
typedef apache::thrift::concurrency::ReadWriteMutex TRwMutex;
typedef apache::thrift::concurrency::Guard TGuard;
typedef apache::thrift::concurrency::RWGuard TRwGuard;
typedef apache::thrift::concurrency::ThreadManager TThreadManager;
typedef apache::thrift::concurrency::PosixThreadFactory TPosixThreadFactory;

typedef apache::thrift::protocol::TBinaryProtocol TBinaryProtocol;
typedef apache::thrift::protocol::TBinaryProtocolFactory ThriftTBinaryProtocolFactory;
typedef apache::thrift::protocol::TProtocol TProtocol;
typedef apache::thrift::protocol::TProtocolFactory TProtocolFactory;

typedef apache::thrift::server::TBinaryProtocolFactory TBinaryProtocolFactory;
typedef apache::thrift::server::TServerTransport TServerTransport;
typedef apache::thrift::server::TSimpleServer TSimpleServer;
typedef apache::thrift::server::TThreadPoolServer TThreadPoolServer;
typedef apache::thrift::server::TThreadedServer TThreadedServer;

typedef apache::thrift::transport::TBufferedTransport TBufferedTransport;
typedef apache::thrift::transport::TFramedTransport TFramedTransport;
typedef apache::thrift::transport::TFramedTransportFactory TFramedTransportFactory;

typedef apache::thrift::transport::TBufferedTransportFactory TBufferedTransportFactory;
typedef apache::thrift::transport::TServerSocket TServerSocket;
typedef apache::thrift::transport::TServerTransport ThriftTServerTransport;
typedef apache::thrift::transport::TSocket TSocket;
typedef apache::thrift::transport::TTransport TTransport;
typedef apache::thrift::transport::TTransportFactory TTransportFactory;
CWINUX_USING_NAMESPACE

class CwxEchoApp;

class CwxEchoThriftIf :public echo_thrift::EchoIf {
public:
  CwxEchoThriftIf(CwxEchoApp* app) {
    m_app = app;
  }
  ~CwxEchoThriftIf() {
  }
public:
  virtual void Echo(echo_thrift::EchoData& _return, const std::string& echo_data);
private:
  CwxEchoApp*    m_app;
};

#endif 
