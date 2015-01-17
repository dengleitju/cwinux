#include "CwxAppNoticePipe.h"
CWINUX_BEGIN_NAMESPACE

///构造函数
CwxAppNoticePipe::CwxAppNoticePipe()
{
    m_bPipeEmpty = true;
    m_noticeHead = NULL;
    m_noticeTail = NULL;
    m_pipeReader = CWX_INVALID_HANDLE;
    m_pipeWriter = CWX_INVALID_HANDLE;
}
///析构函数
CwxAppNoticePipe::~CwxAppNoticePipe()
{
    clear();
}

CWINUX_END_NAMESPACE
