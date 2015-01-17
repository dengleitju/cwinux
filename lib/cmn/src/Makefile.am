##  Process this file with automake to create Makefile.in

includedir =
pkgconfigdir = 

lib_LTLIBRARIES = libcwxcmn.la
libcwxcmn_la_CPPFLAGS = -W -Wall

libcwxcmn_la_SOURCES = \
	CwxCharPool.cpp\
	CwxFile.cpp\
	CwxMsgBlock.cpp\
	CwxPackageReader.cpp\
	CwxPackageReaderEx.cpp\
	CwxTaskBoard.cpp\
	CwxTss.cpp\
	CwxCommander.cpp\
	CwxGbkUnicodeMap.cpp\
	CwxMsgHead.cpp\
	CwxPackageWriter.cpp\
	CwxPackageWriterEx.cpp\
	CwxThread.cpp\
	CwxCommon.cpp\
	CwxGbkUnicodeMapDat.cpp\
	CwxMsgQueue.cpp\
	CwxRwIdLocker.cpp\
	CwxThreadPool.cpp\
	CwxThreadPoolThrift.cpp\
	CwxDate.cpp\
	CwxGetOpt.cpp\
	CwxMutexIdLocker.cpp\
	CwxRwLock.cpp\
	CwxThreadPoolEx.cpp\
	CwxEncodeXml.cpp\
	CwxLogger.cpp\
	CwxMutexLock.cpp\
	CwxSingleton.cpp\
	CwxEscapeUtil.cpp\
	CwxMemLineReader.cpp\
	CwxPackage.cpp\
	CwxPackageEx.cpp\
	CwxSingletonMgr.cpp\
	CwxTimeValue.cpp\
	CwxCrc32.cpp\
	CwxIniParse.cpp\
	CwxMd5.cpp\
	CwxFbHash.cpp

libcwxcmn_la_LDFLAGS = 

cwxcmnincludedir=$(prefix)/include/cmn
cwxcmninclude_HEADERS=\
    ../include/CwxBitMap.h\
    ../include/CwxCharPool.h\
    ../include/CwxCharset.h\
    ../include/CwxChrType.h\
    ../include/CwxChrType.inl\
    ../include/CwxCommon.h\
    ../include/CwxCommon.inl\
    ../include/CwxCondition.h\
    ../include/CwxCondition.inl\
    ../include/CwxConfig.h\
    ../include/CwxDate.h\
    ../include/CwxDate.inl\
    ../include/CwxEncodeXml.h\
    ../include/CwxEncodeXml.inl\
    ../include/CwxErrGuard.h\
    ../include/CwxErrGuard.inl\
    ../include/CwxEscape.h\
    ../include/CwxEscapeUtil.h\
    ../include/CwxEscapeUtil.inl\
    ../include/CwxFile.h\
    ../include/CwxFile.inl\
    ../include/CwxFilterMachine.h\
    ../include/CwxFilterMachine.inl\
    ../include/CwxGbkUnicodeMap.h\
    ../include/CwxGbkUnicodeMap.inl\
    ../include/CwxGetOpt.h\
    ../include/CwxGlobalMacro.h\
    ../include/CwxHeap.h\
    ../include/CwxHostInfo.h\
    ../include/CwxKeyValueMap.h\
    ../include/CwxKeyValueMap.inl\
    ../include/CwxLockGuard.h\
    ../include/CwxLruCache.h\
    ../include/CwxMemLineReader.h\
    ../include/CwxMemLineReader.inl\
    ../include/CwxMsgBlock.h\
    ../include/CwxMsgBlock.inl\
    ../include/CwxMsgHead.h\
    ../include/CwxMsgHead.inl\
    ../include/CwxMsgQueue.h\
    ../include/CwxMsgQueue.inl\
    ../include/CwxMutexIdLocker.h\
    ../include/CwxMutexIdLocker.inl\
    ../include/CwxMutexLock.h\
    ../include/CwxMutexLock.inl\
    ../include/CwxPackage.h\
    ../include/CwxPackage.inl\
    ../include/CwxPackageEx.h\
    ../include/CwxPackageEx.inl\
    ../include/CwxPackageReader.h\
    ../include/CwxPackageReader.inl\
    ../include/CwxPackageReaderEx.h\
    ../include/CwxPackageReaderEx.inl\
    ../include/CwxPackageWriter.h\
    ../include/CwxPackageWriter.inl\
    ../include/CwxPackageWriterEx.h\
    ../include/CwxPackageWriterEx.inl\
    ../include/CwxPost.h\
    ../include/CwxPre.h\
    ../include/CwxReplMachine.h\
    ../include/CwxReplMachine.inl\
    ../include/CwxRwIdLocker.h\
    ../include/CwxRwIdLocker.inl\
    ../include/CwxRwLock.h\
    ../include/CwxRwLock.inl\
    ../include/CwxSingleton.h\
    ../include/CwxSingletonMgr.h\
    ../include/CwxStlFunc.h\
    ../include/CwxStl.h\
    ../include/CwxTailMacro.h\
    ../include/CwxTimeValue.h\
    ../include/CwxTimeValue.inl\
    ../include/CwxTrieTree.h\
    ../include/CwxTrieTree.inl\
    ../include/CwxType.h\
    ../include/CwxTypePoolEx.h\
    ../include/CwxTypePool.h\
    ../include/CwxZlib.h\
    ../include/CwxDTail.h\
    ../include/CwxSTail.h\
    ../include/CwxCommander.h\
    ../include/CwxCommander.inl\
    ../include/CwxMinHeap.h\
    ../include/CwxTaskBoard.h\
    ../include/CwxTaskBoard.inl\
    ../include/CwxThread.h\
    ../include/CwxThread.inl\
    ../include/CwxThreadPoolEx.h\
    ../include/CwxThreadPoolEx.inl\
    ../include/CwxThreadPool.h\
    ../include/CwxThreadPool.inl\
    ../include/CwxThreadPoolThrift.h\
    ../include/CwxThreadPoolThrift.inl\
    ../include/CwxTpi.h\
    ../include/CwxTss.h\
    ../include/CwxLogger.h\
    ../include/CwxCrc32.h\
    ../include/CwxIniParse.h\
    ../include/CwxMd5.h\
    ../include/CwxFbHash.h

	
INCLUDES=-I$(srcdir)/../include -I$(thrift_dir) -I$(gflags_dir)

doc:
	doxygen ../doc/cmn.dxyg

## Clean up template repositories, etc.
clean-local:
	-rm -f *~ *.bak *.rpo *.sym lib*.*_pure_* core core.*
