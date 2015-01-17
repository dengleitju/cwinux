CWINUX_BEGIN_NAMESPACE

inline string& CwxDate::format(time_t tt, char const* format, string& datetime)
{
    char timestamp[512];
    memset(timestamp, 0x00, sizeof(timestamp));
    strftime(timestamp, sizeof(timestamp), format, localtime(&tt));
    datetime = timestamp;
    return datetime;
}


inline string& CwxDate::getDate(string& datetime)
{
    return getDate(time(NULL), datetime);
}

inline time_t CwxDate::trimToHour(time_t ttDatetime)
{
    time_t uiMode = ttDatetime%3600;
    ttDatetime -= uiMode;
    return ttDatetime;
}
inline time_t CwxDate::trimToDay(time_t ttDatetime)
{
    struct tm tmNow;
    time_t ttDate = ttDatetime;
    localtime_r(&ttDate, &tmNow);
    ttDatetime -= (tmNow.tm_hour * 3600 + tmNow.tm_min * 60 + tmNow.tm_sec);
    return ttDatetime;
}
inline int	CwxDate::getYear(time_t tt)
{
    struct tm tm_time;
    localtime_r(&tt, &tm_time);
    return tm_time.tm_year + 1900;
}
inline int	CwxDate::getMonth(time_t tt)
{
    struct tm tm_time;
    localtime_r(&tt, &tm_time);
    return tm_time.tm_mon + 1;
}
inline int	CwxDate::getDay(time_t tt)
{
    struct tm tm_time;
    localtime_r(&tt, &tm_time);
    return tm_time.tm_mday;
}
inline int	CwxDate::getHour(time_t tt)
{
    struct tm tm_time;
    localtime_r(&tt, &tm_time);
    return tm_time.tm_hour;
}
inline int	CwxDate::getMinute(time_t tt)
{
    struct tm tm_time;
    localtime_r(&tt, &tm_time);
    return tm_time.tm_min;
}
inline int	CwxDate::getSecond(time_t tt)
{
    struct tm tm_time;
    localtime_r(&tt, &tm_time);
    return tm_time.tm_sec;
}

inline CWX_UINT64 CwxDate::getTimestamp()
{
    struct timeval tv;
    CWX_UINT64 llTimestamp;
    gettimeofday(&tv, NULL);
    llTimestamp = tv.tv_sec;
    llTimestamp *= 1000000;
    llTimestamp += tv.tv_usec;
    return llTimestamp;
}


CWINUX_END_NAMESPACE

