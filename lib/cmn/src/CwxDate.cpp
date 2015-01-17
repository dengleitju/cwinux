
#include "CwxDate.h"

CWINUX_BEGIN_NAMESPACE

const string CwxDate::HTTP_FORMAT = "%a, %d %b %Y %H:%M:%S %z";

string& CwxDate::getDate(time_t tt, string& datetime)
{
    char buf[20];
    struct tm tmNow;
    datetime.clear();
    localtime_r(&tt, &tmNow);
    sprintf(buf, "%.4d-%.2d-%.2d %.2d:%.2d:%.2d",
        tmNow.tm_year + 1900,
        tmNow.tm_mon + 1,
        tmNow.tm_mday,
        tmNow.tm_hour,
        tmNow.tm_min,
        tmNow.tm_sec);
    datetime = buf;
    return datetime;
}
time_t CwxDate::getDate(string const& datetime)
{
    struct tm tmDate;
    memset(&tmDate, 0x00, sizeof(tmDate));
    tmDate.tm_year = atoi(datetime.c_str()) - 1900;
    tmDate.tm_mon = atoi(datetime.c_str()+5) - 1;
    tmDate.tm_mday = atoi(datetime.c_str()+8);
    tmDate.tm_hour = atoi(datetime.c_str()+11);
    tmDate.tm_min = atoi(datetime.c_str()+14);
    tmDate.tm_sec = atoi(datetime.c_str()+17);
    tmDate.tm_isdst = 0;
    return mktime(&tmDate);
}
string& CwxDate::getDateY4MDHMS2(time_t tt, string& datetime)
{
    char buf[20];
    struct tm tmNow;
    datetime.clear();
    localtime_r(&tt, &tmNow);
    sprintf(buf, "%.4d%.2d%.2d%.2d%.2d%.2d",
        tmNow.tm_year + 1900,
        tmNow.tm_mon + 1,
        tmNow.tm_mday,
        tmNow.tm_hour,
        tmNow.tm_min,
        tmNow.tm_sec);
    datetime = buf;
    return datetime;
}

time_t CwxDate::getDateY4MDHMS2(const string& datetime)
{
    struct tm tmDate;
    memset(&tmDate, 0x00, sizeof(tmDate));
    tmDate.tm_year = atoi(datetime.substr(0, 4).c_str()) - 1900;
    tmDate.tm_mon = atoi(datetime.substr(4, 2).c_str()) - 1;
    tmDate.tm_mday = atoi(datetime.substr(6, 2).c_str());
    tmDate.tm_hour = atoi(datetime.substr(8, 2).c_str());
    tmDate.tm_min = atoi(datetime.substr(10, 2).c_str());
    tmDate.tm_sec = atoi(datetime.substr(12, 2).c_str());
    tmDate.tm_isdst = 0;
    return mktime(&tmDate);
}

CWINUX_END_NAMESPACE
