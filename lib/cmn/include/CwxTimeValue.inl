CWINUX_BEGIN_NAMESPACE

/// Initializes the CwxTimeValue from seconds and useconds.
inline void CwxTimeValue::set (time_t sec, suseconds_t usecs)
{
    tv_.tv_sec = sec;
    tv_.tv_usec = usecs;
    normalize();
}
/// Initializes the CwxTimeValue from a timeval.
inline void CwxTimeValue::set (const timeval &t)
{
    memcpy(&tv_, &t, sizeof(t));
    normalize();
}
///Initializes the CwxTimeValue from microseconds format.
inline void CwxTimeValue::set (CWX_UINT64 usecs)
{
    tv_.tv_sec = usecs/CWX_MAX_USECS;
    tv_.tv_usec = usecs%CWX_MAX_USECS;
}
/// Get seconds.
inline time_t CwxTimeValue::sec (void) const
{
    return tv_.tv_sec;
}
/// Set seconds.
inline void CwxTimeValue::sec(time_t sec)
{
    tv_.tv_sec = sec;
}
/// Get microseconds.
inline suseconds_t CwxTimeValue::usec (void) const
{
    return tv_.tv_usec;
}
/// Set microseconds.
inline void CwxTimeValue::usec(suseconds_t usec)
{
    tv_.tv_usec = usec;
    normalize();
}
/// set current datetime
inline void CwxTimeValue::now()
{
    gettimeofday(&tv_, NULL);
    normalize();
}

///Converts from CwxTimeValue format into microseconds format. 
inline CWX_UINT64 CwxTimeValue::to_usec(void) const
{
    CWX_UINT64 us = tv_.tv_sec;
    us *= CWX_MAX_USECS;
    us += tv_.tv_usec;
    return us;
}

/// Returns the value of the object as a timeval.
inline CwxTimeValue::operator timeval () const
{
    return this->tv_;
}
/// Returns a pointer to the object as a timeval.
inline CwxTimeValue::operator const timeval *() const
{
    return (const timeval *) &this->tv_;
}

/// Add tv to this.
inline CwxTimeValue & CwxTimeValue::operator += (const CwxTimeValue &tv)
{
    this->sec (this->sec () + tv.sec ());
    this->usec (this->usec () + tv.usec ());
    this->normalize ();
    return *this;
}
/// Add tv to this.
inline CwxTimeValue & CwxTimeValue::operator += (time_t tv)
{
    this->sec (this->sec () + tv);
    return *this;
}

/// Assign tv to this
inline CwxTimeValue &CwxTimeValue::operator = (const CwxTimeValue &tv)
{
    memcpy(&tv_, &tv.tv_, sizeof(tv.tv_));
    return *this;
}
/// Assign tv to this
inline CwxTimeValue&CwxTimeValue::operator = (time_t tv)
{
    this->sec(tv);
    this->usec (0);
    return *this;
}

/// Subtract tv to this.
inline CwxTimeValue &CwxTimeValue::operator -= (const CwxTimeValue &tv)
{
    this->sec (this->sec () - tv.sec ());
    this->usec (this->usec () - tv.usec ());
    this->normalize ();
    return *this;
}
/// Substract  tv to this.
inline CwxTimeValue &CwxTimeValue::operator -= (time_t tv)
{
    this->sec (this->sec () - tv);
    return *this;
}

/// Adds two CwxTimeValue objects together, returns the sum.
inline CwxTimeValue operator + (const CwxTimeValue &tv1,
                                const CwxTimeValue &tv2)
{
    CwxTimeValue sum (tv1);
    sum += tv2;
    sum.normalize();
    return sum;
}
/// Subtracts two CwxTimeValue objects, returns the difference.
inline CwxTimeValue operator - (const CwxTimeValue &tv1,
                                const CwxTimeValue &tv2)
{
    CwxTimeValue delta (tv1);
    delta -= tv2;
    delta.normalize();
    return delta;
}
/// True if tv1 <  tv2.
inline bool operator < (const CwxTimeValue &tv1,
                        const CwxTimeValue &tv2)
{
    return tv2>tv1;
}
/// True if tv1 > tv2.
inline bool operator > (const CwxTimeValue &tv1,
                        const CwxTimeValue &tv2)
{
    if (tv1.sec () > tv2.sec ())
        return true;
    else if (tv1.sec () == tv2.sec ()
        && tv1.usec () > tv2.usec ())
        return true;
    return false;
}
/// True if  tv1 <=  tv2.
inline bool operator <= (const CwxTimeValue &tv1,
                         const CwxTimeValue &tv2)
{
    return tv2 >= tv1;
}
/// True if  tv1 >=  tv2.
inline bool operator >= (const CwxTimeValue &tv1,
                         const CwxTimeValue &tv2)
{
    if (tv1.sec () > tv2.sec ())
        return true;
    else if (tv1.sec () == tv2.sec ()
        && tv1.usec () >= tv2.usec ())
        return true;
    return false;
}
/// True if tv1 ==  tv2.
inline bool operator == (const CwxTimeValue &tv1,
                         const CwxTimeValue &tv2)
{
    return tv1.sec () == tv2.sec ()
        && tv1.usec () == tv2.usec ();
}
/// True if  tv1 !=  tv2.
inline  bool operator != (const CwxTimeValue &tv1,
                          const CwxTimeValue &tv2)
{
    return !(tv1 == tv2);
}


inline CwxTimeouter::CwxTimeouter(CwxTimeValue const* timeout):timeout_(timeout)
{
    if (timeout_)
    {
        end_.now();
        end_ += *timeout_;
        left_ = *timeout;
    }
}

inline CwxTimeouter::~CwxTimeouter()
{
}

inline bool CwxTimeouter::timeout()
{
    if (timeout_)
    {
        left_.now();
        left_ = end_ - left_;
        return left_.sec() < 0;
    }
    return false;
}

///剩余时间
inline CwxTimeValue const* CwxTimeouter::left() const
{
    return timeout_?&left_:NULL;
}

inline CwxTimeValue const* CwxTimeouter::getTimeout() const
{
    return timeout_;
}

inline bool CwxTimeouter::isTimer() const
{
    return timeout_ != NULL;
}

CWINUX_END_NAMESPACE
