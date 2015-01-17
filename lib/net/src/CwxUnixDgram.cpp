#include "CwxUnixDgram.h"

CWINUX_BEGIN_NAMESPACE

CwxUnixDgram::CwxUnixDgram()
{

}

CwxUnixDgram::CwxUnixDgram(CWX_HANDLE handle):CwxSockDgram(handle)
{
}
CwxUnixDgram::~CwxUnixDgram(void)
{

}

CWINUX_END_NAMESPACE
