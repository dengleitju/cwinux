#ifndef __CWX_DATE_H__
#define __CWX_DATE_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxDate.h
@brief 定义日期格式转换工具类CwxDate
@author cwinux@gmail.com
@version 0.1
@date 2009-10-10
@warning
@bug
*/

#include "CwxPre.h"
#include "CwxType.h"
#include "CwxGlobalMacro.h"
#include "CwxStl.h"
#include "CwxStlFunc.h"

CWINUX_BEGIN_NAMESPACE

/**
@class CwxDate
@brief 实现了对常用日期格式的变换，此类的方法全部为静态方法，不能实例化。<br>
       CwxDate获取的时间，全部是本地时区的时间
*/
class CWX_API CwxDate
{
public:
    /**
    @brief 将tt变量指定的时间，转化为format定义的格式，并返回。
    @param [in] tt 时间变量
    @param [in] format 返回时间格式的format，格式与strftime()一致
    @param [out] datetime format定义的时间格式
    @return 返回datetime变量
    */
    static string& format(time_t tt, char const* format, string& datetime);
    /**
    @brief 将当前的标准时间，转化为YYYY-MM-DD H24:MM:SS的格式，并返回。
    @param [out] datetime 存放YYYY-MM-DD H24:MM:SS格式字符串的string 变量
    @return 返回datetime变量
    */
    static string& getDate(string& datetime);
    /**
    @brief 将tt变量指定的时间，转化为YYYY-MM-DD H24:MM:SS的格式，并返回。
    @param [in] tt 时间变量
    @param [out] datetime 存放YYYY-MM-DD H24:MM:SS格式字符串的string 变量
    @return 返回datetime变量
    */
    static string& getDate(time_t tt, string& datetime);
    /**
    @brief 将YYYY-MM-DD H24:MM:SS的格式时间，转化为time_t的时间并返回。
    @param [in] datetime 格式为YYYY-MM-DD H24:MM:SS格式字符串变量
    @return datetime对应的time_t的时间
    */
    static time_t getDate(string const& datetime);
    /**
    @brief 将tt表示的时间，转化为YYYYMMDDH24MMSS格式字符串并返回。
    @param [in] tt 要转化的时间
    @param [out] datetime 返回的YYYYMMDDH24MMSS格式的字符串
    @return 返回datetime变量
    */
    static string& getDateY4MDHMS2(time_t tt, string& datetime);
    /**
    @brief 将YYYYMMDDH24MMSS格式字符串，转化为time_t的形式并返回。
    @param [in] datetime YYYYMMDDH24MMSS格式的字符串
    @return datetime对应的时间
    */
    static time_t getDateY4MDHMS2(const string& datetime);
    /**
    @brief 将tt的时间值，按小时归一化，及去掉不满1小时的部分
    @param [in] tt 要处理的时间
    @return 返回按小时归一化的time_t
    */
    static time_t trimToHour(time_t  tt);
    /**
    @brief 将tt的时间值，按天归一化，及去掉不满1天的部分
    @param [in] tt 要处理的时间
    @return 返回按天归一化的time_t
    */
    static time_t trimToDay(time_t  tt);
    /**
    @brief 获取time_t中的年份
    @param [in] tt 要获取年份的time_t
    @return 返回tt时间所在的年份
    */
    static int	getYear(time_t tt);
    /**
    @brief 获取time_t中的月份
    @param [in] tt 要获取月份的time_t
    @return 返回tt时间所在的月份
    */
    static int	getMonth(time_t tt);
    /**
    @brief 获取time_t中的所在月份的日期
    @param [in] tt 要获取所在月份的日期的time_t
    @return 返回tt时间所在月份的日期
    */
    static int	getDay(time_t tt);
    /**
    @brief 获取time_t对应日期的小时
    @param [in] tt 要获取小时的time_t
    @return 返回tt时间的小时
    */
    static int	getHour(time_t tt);
    /**
    @brief 获取time_t对应日期的分钟
    @param [in] tt 要获取分钟的time_t
    @return 返回tt对应日期的分钟
    */
    static int	getMinute(time_t tt);
    /**
    @brief 获取time_t对应日期的秒
    @param [in] tt 要获取秒的time_t
    @return 返回tt对应日期的秒
    */
    static int	getSecond(time_t tt);
    /**
    @brief 获取精确到微妙的当前时间，为64位的无符号数
    @return 返回精确到微妙的当前时间
    */
    static CWX_UINT64 getTimestamp();
public:
    // Sat, 01 Jan 2005 12:00:00 +0100
    static const string HTTP_FORMAT;

private:
    ///对象禁止实例化
    CwxDate(){}
};

CWINUX_END_NAMESPACE

#include "CwxDate.inl"
#include "CwxPost.h"

#endif
