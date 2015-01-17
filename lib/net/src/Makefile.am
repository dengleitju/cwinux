##  Process this file with automake to create Makefile.in

includedir =
pkgconfigdir = 

lib_LTLIBRARIES = libcwxnet.la
libcwxnet_la_CPPFLAGS = -W -Wall

libcwxnet_la_SOURCES = \
	CwxAddr.cpp\
	CwxSockAcceptor.cpp\
	CwxSockDgram.cpp\
	CwxSockStream.cpp\
	CwxUnixConnector.cpp\
	CwxINetAddr.cpp\
	CwxSockBase.cpp\
	CwxSocket.cpp\
	CwxUnixAcceptor.cpp\
	CwxUnixDgram.cpp\
	CwxIpcSap.cpp\
	CwxSockConnector.cpp\
	CwxSockIo.cpp\
	CwxUnixAddr.cpp\
	CwxUnixStream.cpp

libcwxnet_la_LDFLAGS = 

cwxnetincludedir=$(prefix)/include/net
cwxnetinclude_HEADERS=\
	../include/CwxAddr.h\
	../include/CwxAddr.inl\
	../include/CwxINetAddr.h\
	../include/CwxINetAddr.inl\
	../include/CwxIpcSap.h\
	../include/CwxIpcSap.inl\
	../include/CwxNetMacro.h\
	../include/CwxSockAcceptor.h\
	../include/CwxSockAcceptor.inl\
	../include/CwxSockBase.h\
	../include/CwxSockBase.inl\
	../include/CwxSockConnector.h\
	../include/CwxSockConnector.inl\
	../include/CwxSockDgram.h\
	../include/CwxSockDgram.inl\
	../include/CwxSocket.h\
	../include/CwxSocket.inl\
	../include/CwxSockIo.h\
	../include/CwxSockIo.inl\
	../include/CwxSockStream.h\
	../include/CwxSockStream.inl\
	../include/CwxUnixAcceptor.h\
	../include/CwxUnixAcceptor.inl\
	../include/CwxUnixAddr.h\
	../include/CwxUnixAddr.inl\
	../include/CwxUnixConnector.h\
	../include/CwxUnixConnector.inl\
	../include/CwxUnixDgram.h\
	../include/CwxUnixDgram.inl\
	../include/CwxUnixStream.h\
	../include/CwxUnixStream.inl


	
INCLUDES=-I$(srcdir)/../include -I$(srcdir)/../../cmn/include

doc:
	doxygen ../doc/net.dxyg

## Clean up template repositories, etc.
clean-local:
	-rm -f *~ *.bak *.rpo *.sym lib*.*_pure_* core core.*
