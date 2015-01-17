##  Process this file with automake to create Makefile.in

includedir =
pkgconfigdir = 

lib_LTLIBRARIES = libcwinux.la
libcwinux_la_CPPFLAGS = -W -Wall

libcwinux_la_SOURCES = cwx_msg_header.c\
			cwx_package.c\
			cwx_package_ex.c\
			cwx_package_writer.c\
			cwx_package_writer_ex.c\
			cwx_package_reader.c\
			cwx_package_reader_ex.c\
			cwx_socket.c\
			cwx_crc32.c\
			cwx_md5.c
libcwinux_la_LDFLAGS = 

cwinuxincludedir=$(prefix)/include
cwinuxinclude_HEADERS=cwx_config.h\
			cwx_msg_header.h\
			cwx_package.h\
			cwx_package_ex.h\
			cwx_package_reader.h\
			cwx_package_reader_ex.h\
			cwx_package_writer.h\
			cwx_package_writer_ex.h\
			cwx_socket.h\
			cwx_crc32.h\
			cwx_md5.h

	
INCLUDES= -I$(srcdir)

## Clean up template repositories, etc.
clean-local:
	-rm -f *~ *.bak *.rpo *.sym lib*.*_pure_* core core.*
