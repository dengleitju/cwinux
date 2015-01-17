#include "CwxUnixStream.h"

CWINUX_BEGIN_NAMESPACE

CwxUnixStream::CwxUnixStream()
{

}

CwxUnixStream::CwxUnixStream(CWX_HANDLE handle):CwxSockStream(handle)
{

}

CwxUnixStream::~CwxUnixStream()
{

}

CWINUX_END_NAMESPACE
