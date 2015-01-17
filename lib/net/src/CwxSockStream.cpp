#include "CwxSockStream.h"

CWINUX_BEGIN_NAMESPACE

CwxSockStream::CwxSockStream()
{

}

CwxSockStream::CwxSockStream(CWX_HANDLE handle)
{
    setHandle(handle);
}

CwxSockStream::~CwxSockStream()
{

}

CWINUX_END_NAMESPACE
