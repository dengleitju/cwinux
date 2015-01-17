#include "CwxTimeValue.h"

CWINUX_BEGIN_NAMESPACE
/// Constant "0".
const CwxTimeValue CwxTimeValue::ZERO;
///Constant for maximum time representable.
const CwxTimeValue CwxTimeValue::MAX(LONG_MAX, CWX_MAX_USECS-1);

/// Default Constructor.
CwxTimeValue::CwxTimeValue(void)
{
    this->set(0, 0);
}
/// Constructor.
CwxTimeValue::CwxTimeValue(time_t sec, suseconds_t usecs)
{
    this->set(sec, usecs);
}
/// Construct the CwxTimeValue from a timeval.
CwxTimeValue::CwxTimeValue (const timeval &t)
{
    this->set(t);
}

void CwxTimeValue::normalize (void)
{
    if (this->tv_.tv_usec >= CWX_MAX_USECS)
    {
        this->tv_.tv_sec += this->tv_.tv_usec/CWX_MAX_USECS;
        this->tv_.tv_usec %= CWX_MAX_USECS;
    }
    else if (this->tv_.tv_usec < 0)
    {
        CWX_UINT32 tmp = ((-this->tv_.tv_usec)  + (CWX_MAX_USECS - 1))/CWX_MAX_USECS;
        this->tv_.tv_sec -= tmp;
        this->tv_.tv_usec += tmp + CWX_MAX_USECS;
    }
}

CWINUX_END_NAMESPACE
