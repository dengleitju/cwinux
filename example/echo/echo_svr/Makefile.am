##  Process this file with automake to create Makefile.in

includedir =
pkgconfigdir =

BUILD_DATE=$(shell date +%F)


bin_PROGRAMS = cwx_echo
cwx_echo_CPPFLAGS = -D_BUILD_DATE="$(BUILD_DATE)" -W -Wall
cwx_echo_SOURCES = \
	CwxEchoApp.cpp\
	CwxEchoConfig.cpp\
	cwx_echo.cpp\
	CwxEchoEventHandler.cpp
cwx_echo_LDADD=
cwx_echo_LDFLAGS =-L@cwinux_dir@/lib
LIBS=-lcwxapp -lcwxnet -lcwxcmn -lz -lpthread -ldl -lrt
INCLUDES=-I@cwinux_dir@/include/app -I@cwinux_dir@/include/cmn -I@cwinux_dir@/include/net 

## Clean up template repositories, etc.
clean-local:
	-rm -f *~ *.bak *.rpo *.sym lib*.*_pure_* core core.*
