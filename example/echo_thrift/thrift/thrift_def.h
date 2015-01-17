#ifndef __THRIFT_DEF__
#define __THRIFT_DEF__

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

#endif