##  Process this file with automake to create Makefile.in

includedir =
pkgconfigdir = 

lib_LTLIBRARIES = libcwxapp.la
libcwxapp_la_CPPFLAGS = -W -Wall

libcwxapp_la_SOURCES = \
	CwxAppConfig.cpp\
	CwxAppHandler4Msg.cpp\
	CwxAppHandler4UnixConn.cpp\
	CwxAppTcpAcceptor.cpp\
	CwxAppConnInfo.cpp\
	CwxAppHandler4Notice.cpp\
	CwxAppHandlerCache.cpp\
	CwxAppTcpConnector.cpp\
	CwxAppFramework.cpp\
	CwxAppHandler4Signal.cpp\
	CwxAppListenMgr.cpp\
	CwxAppUnixAcceptor.cpp\
	CwxAppHandler4Base.cpp\
	CwxAppHandler4Stdio.cpp\
	CwxAppNoticePipe.cpp\
	CwxAppUnixConnector.cpp\
	CwxAppHandler4IoEvent.cpp\
	CwxAppHandler4TcpConn.cpp\
	CwxAppProcessMgr.cpp\
	CwxAppHandler4IoMsg.cpp\
	CwxAppHandler4Time.cpp\
	CwxAppReactor.cpp\
        CwxAppEpoll.cpp\
	CwxAppHandler4Channel.cpp\
	CwxAppChannel.cpp

libcwxapp_la_LDFLAGS = 

cwxappincludedir=$(prefix)/include/app
cwxappinclude_HEADERS=\
	../include/CwxAppAioWindow.h\
	../include/CwxAppHandler4IoEvent.h\
	../include/CwxAppHandler4UnixConn.inl\
	../include/CwxAppProcessMgr.h\
	../include/CwxAppHandler4IoMsg.h\
	../include/CwxAppHandlerCache.h\
	../include/CwxAppReactor.h\
	../include/CwxAppHandler4Msg.h\
	../include/CwxAppHandlerCache.inl\
	../include/CwxAppReactor.inl\
	../include/CwxAppConfig.h\
	../include/CwxAppHandler4Msg.inl\
	../include/CwxAppListenMgr.h\
	../include/CwxAppConnInfo.h\
        ../include/CwxAppEpoll.h\
        ../include/CwxAppEpoll.inl\
	../include/CwxAppHandler4Notice.h\
	../include/CwxAppConnInfo.inl\
	../include/CwxAppHandler4Signal.h\
	../include/CwxAppMacro.h\
	../include/CwxAppTcpAcceptor.h\
	../include/CwxAppUnixAcceptor.h\
	../include/CwxAppForkMgr.h\
	../include/CwxAppHandler4Stdio.h\
	../include/CwxAppTcpConnector.h\
	../include/CwxAppUnixConnector.h\
	../include/CwxAppFramework.h\
	../include/CwxAppHandler4TcpConn.h\
	../include/CwxAppFramework.inl\
	../include/CwxAppHandler4TcpConn.inl\
	../include/CwxAppNoticePipe.h\
	../include/CwxAppHandler4Base.h\
	../include/CwxAppHandler4Time.h\
	../include/CwxAppNoticePipe.inl\
	../include/CwxAppHandler4Base.inl\
	../include/CwxAppHandler4UnixConn.h\
	../include/CwxAppChannel.h\
	../include/CwxAppChannel.inl\
	../include/CwxAppHandler4Channel.h\
	../include/CwxAppHandler4Channel.inl

	
INCLUDES=-I$(srcdir)/../include -I$(srcdir)/../../cmn/include -I$(srcdir)/../../net/include -I$(thrift_dir) -I$(gflags_dir)

doc:
	doxygen ../doc/app.dxyg

## Clean up template repositories, etc.
clean-local:
	-rm -f *~ *.bak *.rpo *.sym lib*.*_pure_* core core.*
