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

int main(int argc, char** argv) {
  if (3 != argc) {
    printf("using %s  ip  port", argv[0]);
    return 1;
  }
  boost::shared_ptr<TTransport> socket( new TSocket(argv[1], atoi(argv[2])));
  boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
  boost::shared_ptr <TProtocol> protocol(new TBinaryProtocol(transport));
  echo_thrift::EchoClient client(protocol);
  int i = 0;
  echo_thrift::EchoData ret;
  char buf[128];
  while(true) {
    try {
      transport->open();
      sprintf(buf, "aaaaaaaaaaaaaaaaaaaaaaaaaaa,no=%d", i);
      client.Echo(ret, buf);
      transport->close();
    } catch (apache::thrift::TException &tx) {
      printf("Failure to echo, err=%s\n", tx.what());
      ::exit(1);
    }
    i++;
    if (i%10000==0) printf("msg=%s, num=%d\n", ret.data.c_str(), i);
  }
  return 0;

}
