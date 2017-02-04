// File     : timestamp.h
// Purpose  : to provide a simple interface to the time.h uitilies
// Contains : class timestamp
//
// Update Log -
//
// 19991126 - Begun
// 20090526 - appended #endif comment
// 20150423 - added member func full_stamp_2punc(), that prints stamp in format "YYMMDD HHMMSS.UUUUUU|"
// 20150809 - added Day of Week and Month string arrays, added day_..._of_this_week_to_double() functions


// prototype
namespace blib
{
class timestamp;
} // namespace blib


#ifndef TIMESTAMP_CLASS_DEFINITIONS
#define TIMESTAMP_CLASS_DEFINITIONS


#include <sys/time.h>  // for gettimeofday()
#include <unistd.h>    // for gettimeofday()
#include <time.h>      // for placing date stamp
#include <stdio.h>     // for sprintf()
#include "blib.h"      // blib globals
#include "timer.h"     // for ..._to_double() macros


namespace blib
{


#define  TIMESTAMP_SUNDAY     0
#define  TIMESTAMP_MONDAY     1
#define  TIMESTAMP_TUESDAY    2
#define  TIMESTAMP_WEDNESDAY  3
#define  TIMESTAMP_THURSDAY   4
#define  TIMESTAMP_FRIDAY     5
#define  TIMESTAMP_SATURDAY   6 
#define  TIMESTAMP_EVERYDAY   7

#define  TIMESTAMP_JANUARY     0
#define  TIMESTAMP_FEBRUARY    1
#define  TIMESTAMP_MARCH       2
#define  TIMESTAMP_APRIL       3
#define  TIMESTAMP_MAY         4
#define  TIMESTAMP_JUNE        5
#define  TIMESTAMP_JULY        6
#define  TIMESTAMP_AUGUST      7
#define  TIMESTAMP_SEPTEMBER   8
#define  TIMESTAMP_OCTOBER     9
#define  TIMESTAMP_NOVEMBER    10
#define  TIMESTAMP_DECEMBER    11
#define  TIMESTAMP_EVERYMONTH  12


// Class   : timestamp
// Purpose : to provide a simple interface to the time.h uitilies
class timestamp
{
    private:
        timeval usecs;      // seconds and microseconds of current time
        time_t  realtime;   // seconds since 1970 Jan 1
        tm*     broktime;   // time broken into yy,mm,dd,hh,etc. (note memory for instants pointed to don't need to be released because localtime() returns pointer to memory it declares static and reuses WARNING: this is not thread safe)
        char    buf[50];    // buffer for sprintf()

        // *** Day of the Week and Month Character Strings ***
        static const char* weekday_full_strings[8];
        static const char* weekday_short_strings[8];
        static const char* month_full_strings[13];
        static const char* month_short_strings[13];


    public:
        int timestamp_set_weekday_by_chars_full(const string& str) const
        {
          for(int i = 0; i < 6; i++) if( str == weekday_full_strings[i] ) return i;
          return 7;
        } // timestamp_set_weekday_by_chars_full()
        int timestamp_set_weekday_by_chars_short(const string& str) const
        {
          for(int i = 0; i < 6; i++) if( str == weekday_short_strings[i] ) return i;
          return 7;
        } // timestamp_set_weekday_by_chars_short()
        int timestamp_set_month_by_chars_full(const string& str) const
        {
          for(int i = 0; i < 11; i++) if( str == month_full_strings[i] ) return i;
          return 12;
        } // timestamp_set_month_by_chars_full()
        int timestamp_set_month_by_chars_short(const string& str) const
        {
          for(int i = 0; i < 11; i++) if( str == month_short_strings[i] ) return i;
          return 12;
        } // timestamp_set_month_by_chars_short()

        const char* weekday_full_chars(const int& i) const
        { return weekday_full_strings[i]; }
        const char* weekday_short_chars(const int& i) const
        { return weekday_short_strings[i]; }
        const char* month_full_chars(const int& i) const
        { return month_full_strings[i]; }
        const char* month_short_chars(const int& i) const
        { return month_short_strings[i]; }

        // return a string with the current date and time
        // formated as - [YYYY/MM/DD HH:MM:SS]
        // this is Loebner log file compliant format
        const char* ContestHeaderTimeStamp(void)
        {   // get the system time
            realtime = time( &realtime );
            broktime = localtime( &realtime );

            // format the result
            sprintf(buf,"[20%d/%d/%d %.2d:%.2d:%.2d]",
                         broktime->tm_year - 100,  // start with the year
                         broktime->tm_mon  + 1,    // append month
                         broktime->tm_mday,        // append day of month
                         broktime->tm_hour,        // append hour
                         broktime->tm_min,         // append minutes
                         broktime->tm_sec);        // append seconds

            return buf;
        } // ContestHeaderTimeStamp()

        // return a string with the current date and time
        // formated as - [YY-MM-DD|HH:MM:SS]
        const char* date_time_stamp(void)
        {   // get the system time
            realtime = time( &realtime );
            broktime = localtime( &realtime );

            // format the result
            sprintf(buf,"[%d-%d-%d|%.2d:%.2d:%.2d] ",
                         broktime->tm_year - 100,  // start with the year
                         broktime->tm_mon  + 1,    // append month
                         broktime->tm_mday,        // append day of month
                         broktime->tm_hour,        // append hour
                         broktime->tm_min,         // append minutes
                         broktime->tm_sec);        // append seconds

            return buf;
        } // date_time_stamp()

        // return a string with the current date and time
        // formated as -  YY-MM-DD HH:MM:SS
        const char* date_time_stamp2(void)
        {   // get the system time
            realtime = time( &realtime );
            broktime = localtime( &realtime );

            // format the result
            sprintf(buf,"%d-%d-%d %.2d:%.2d:%.2d",
                         broktime->tm_year - 100,  // start with the year
                         broktime->tm_mon  + 1,    // append month
                         broktime->tm_mday,        // append day of month
                         broktime->tm_hour,        // append hour
                         broktime->tm_min,         // append minutes
                         broktime->tm_sec);        // append seconds

            return buf;
        } // date_time_stamp2()

        // return a string with the current date and time
        // formated as -  YY-MM-DD HH:MM:SS
        const char* date_time_stamp2(time_t some_time)
        {   // set broktime to arguement
            tm time_parts;
            localtime_r(&some_time, &time_parts);

            // format the result
            sprintf(buf,"%d-%d-%d %.2d:%.2d:%.2d",
                       time_parts.tm_year - 100,  // start with the year
                       time_parts.tm_mon  + 1,    // append month
                       time_parts.tm_mday,        // append day of month
                       time_parts.tm_hour,        // append hour
                       time_parts.tm_min,         // append minutes
                       time_parts.tm_sec);        // append seconds

            return buf;
        } // date_time_stamp2()

        // return a string with the current date
        // formated as - [YY-MM-DD]
        const char* date_stamp(void)
        {   // get the system time
            realtime = time( &realtime );
            broktime = localtime( &realtime );

            // format the result
            sprintf(buf,"[%d-%d-%d] ",
                         broktime->tm_year - 100,  // start with the year
                         broktime->tm_mon  + 1,    // append month
                         broktime->tm_mday);       // append day of month

            return buf;
        } // date_stamp()

        // return a string with the current date
        // formated as - Month DD, YYYY
        const char* string_date_stamp(void)
        {   // get the system time
            realtime = time( &realtime );
            broktime = localtime( &realtime );

            // format the result
            sprintf(buf,"%s %d, 20%d",
              month_full_strings[broktime->tm_mon],  // start with month
              broktime->tm_mday,                     // append day of month
              broktime->tm_year - 100);              // append the year

            return buf;
        } // string_date_stamp()

        // return a string with the current date
        // formated as - WeekDay Month DD, YYYY
        const char* string_weekday_date_stamp(void)
        {   // get the system time
            realtime = time( &realtime );
            broktime = localtime( &realtime );
            mktime( broktime );

            // format the result
            sprintf(buf,"%s %s %d, 20%d",
              weekday_full_strings[broktime->tm_wday],  // start with day of the week
              month_full_strings[broktime->tm_mon],     // append month
              broktime->tm_mday,                        // append day of month
              broktime->tm_year - 100);                 // append the year

            return buf;
        } // string_weekday_date_stamp()

        // return a string with the current month
        // formated as - Month
        const char* string_month(void)
        {   // get the system time
            realtime = time( &realtime );
            broktime = localtime( &realtime );

            // format the result
            sprintf(buf,"%s", month_full_strings[broktime->tm_mon]);

            return buf;
        } // string_month()

        // return a string with the current day of the week
        // formated as - WeekDay
        const char* string_weekday(void)
        {   // get the system time
            realtime = time( &realtime );
            broktime = localtime( &realtime );
            mktime( broktime );

            // format the result
            sprintf(buf,"%s", weekday_full_strings[broktime->tm_wday]);

            return buf;
        } // string_weekday()

        // return a string with the current time of day
        // formated as - [HH:MM:SS]
        const char* time_stamp(void)
        {   // get the system time
            realtime = time( &realtime );
            broktime = localtime( &realtime );

            // format the result
            sprintf(buf,"[%.2d:%.2d:%.2d]",
                         broktime->tm_hour,  // append hour
                         broktime->tm_min,   // append minutes
                         broktime->tm_sec);  // append seconds

            return buf;
        } // time_stamp()

        // return a string with the time of day with micro seconds
        // formated as - [HH:MM:SS.UUUUUU]
        const char* micro_time_stamp(void)
        {   // get the system time
            realtime = time( &realtime );
            broktime = localtime( &realtime );
            gettimeofday(&usecs, (struct timezone *) 0);

            // format the result
            sprintf(buf,"[%.2d:%.2d:%.2d.%.6ld] ",
                         broktime->tm_hour,  // append hour
                         broktime->tm_min,   // append minutes
                         broktime->tm_sec,   // append seconds
                         usecs.tv_usec);     // append useconds

            return buf;
        } // micro_time_stamp()

        // return a string with month, day, time, and microsecs
        // formated as - [MMDD|HHMMSS.UUUUUU]
        const char* brief_stamp(void)
        {   // get the system time
            realtime = time( &realtime );
            broktime = localtime( &realtime );
            gettimeofday(&usecs, (struct timezone *) 0);

            // format the result
            sprintf(buf,"[%.2d%.2d|%.2d%.2d%.2d.%.6ld] ",
                         broktime->tm_mon + 1, // append month
                         broktime->tm_mday,    // append day of month
                         broktime->tm_hour,    // append hour
                         broktime->tm_min,     // append minutes
                         broktime->tm_sec,     // append seconds
                         usecs.tv_usec);       // append useconds

            return buf;
        } // brief_stamp()


        // return a string with the current date and time with microseconds
        // formated as - YY/MM/DD HH:MM:SS.UUUUUU|
        const char* full_stamp(void)
        {   // get the system time
            realtime = time( &realtime );
            broktime = localtime( &realtime );
            gettimeofday(&usecs, (struct timezone *) 0);

            // format the result
            sprintf(buf,"%.2d/%.2d/%.2d %.2d:%.2d:%.2d.%.6ld|",
                         broktime->tm_year - 100,  // start with the year
                         broktime->tm_mon  + 1,    // append month
                         broktime->tm_mday,        // append day of month
                         broktime->tm_hour,        // append hour
                         broktime->tm_min,         // append minutes
                         broktime->tm_sec,         // append seconds
                         usecs.tv_usec);           // append useconds

            return buf;
        } // full_stamp()


        // return a string with the current date and time with microseconds
        // formated as - YYMMDDHHMMSSUUUUUU|
        const char* full_stamp_no_punc(void)
        {   // get the system time
            realtime = time( &realtime );
            broktime = localtime( &realtime );
            gettimeofday(&usecs, (struct timezone *) 0);

            // format the result
            sprintf(buf,"%.2d%.2d%.2d%.2d%.2d%.2d%.6ld|",
                         broktime->tm_year - 100,  // start with the year
                         broktime->tm_mon  + 1,    // append month
                         broktime->tm_mday,        // append day of month
                         broktime->tm_hour,        // append hour
                         broktime->tm_min,         // append minutes
                         broktime->tm_sec,         // append seconds
                         usecs.tv_usec);           // append useconds

            return buf;
        } // full_stamp_no_punc()


        // return a string with the current date and time with microseconds
        // formated as - YYMMDD HHMMSS.UUUUUU|
        const char* full_stamp_2punc(void)
        {   // get the system time
            realtime = time( &realtime );
            broktime = localtime( &realtime );
            gettimeofday(&usecs, (struct timezone *) 0);

            // format the result
            sprintf(buf,"%.2d%.2d%.2d %.2d%.2d%.2d.%.6ld|",
                         broktime->tm_year - 100,  // start with the year
                         broktime->tm_mon  + 1,    // append month
                         broktime->tm_mday,        // append day of month
                         broktime->tm_hour,        // append hour
                         broktime->tm_min,         // append minutes
                         broktime->tm_sec,         // append seconds
                         usecs.tv_usec);           // append useconds

            return buf;
        } // full_stamp_2punc()


        // return a double-formated timestamp representing requested day,hour,minute of the current calendar week
        time_t day_time_of_this_week_to_time_t(const int& weekday = 0, const int& hour = 0, const int& minute = 0, const int& second = 0)
        {
            realtime = time( &realtime );       // get seconds since 1970 for current time
            broktime = localtime( &realtime );  // format 1970-secs into local mth,day,etc time stamp
            mktime( broktime );                 // this sets tm::wday (day of the week)
            
            broktime->tm_hour = hour;           // reset time of day
            broktime->tm_min  = minute;
            broktime->tm_sec  = second;

            // reset day according to requested day-of-week by comparing to current day-of-week
            if( broktime->tm_wday < weekday )
            { // time-stamp pushed forward in time
              int days_forward   = weekday - broktime->tm_wday;
              broktime->tm_wday += days_forward;
              broktime->tm_mday += days_forward;
              broktime->tm_yday += days_forward;
            } // if
            else
            if( broktime->tm_wday > weekday )
            { // time-stamp pushed backward in time
              int days_back      = broktime->tm_wday - weekday;
              broktime->tm_wday -= days_back;
              broktime->tm_mday -= days_back;
              broktime->tm_yday -= days_back;
            } // else if
            // else broktime->tm_wday == weekday, so we have nothing to change
            // note: if changing the day overruns the correct month or year then next call to mktime will correct it

            return mktime( broktime );  // return 1970-secs for the adjusted local mth,day,etc time stamp
        } // day_time_of_this_week_to_time_t()

        time_t day_time_of_this_week_to_time_t(const char* weekday, const int& hour = 0, const int& minute = 0, const int& second = 0)
        { return day_time_of_this_week_to_time_t(timestamp_set_weekday_by_chars_full(weekday), hour, minute, second); }

        double day_time_of_this_week_to_double(const int& weekday = 0, const int& hour = 0, const int& minute = 0, const int& second = 0, const int& usecond = 0)
        { return usecs_to_double(day_time_of_this_week_to_time_t(weekday, hour, minute, second), usecond); }

        double day_time_of_this_week_to_double(const char* weekday, const int& hour = 0, const int& minute = 0, const int& second = 0, const int& usecond = 0)
        { return usecs_to_double(day_time_of_this_week_to_time_t(timestamp_set_weekday_by_chars_full(weekday), hour, minute, second), usecond); }
}; // class timestamp


} // namespace blib

#endif // TIMESTAMP_CLASS_DEFINITIONS

// timestamp.h

