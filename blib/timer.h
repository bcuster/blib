/*****************************************************************************/
/*** Software: AMOS v0.01 (c)1998 All Rights Reserved by Bradley B. Custer ***/
/*****************************************************************************/

// File     : timer.h
// Purpose  : to provide a way of monitoring real time
// Contains : class time_timer and some globals
// Updates -
// 19990626 - Begun
// 20090527 - appended #endif comment
// 20120320 - added members passed_secs(), passed_usecs()


// prototype
namespace blib
{
class time_timer;
} // namespace blib


#ifndef TIME_TIMER_CLASS_DEFINITIONS
#define TIME_TIMER_CLASS_DEFINITIONS


#include <time.h>          // for time()
#include <sys/time.h>      // for gettimeofday()
#include <sys/types.h>     // for select()
#include <unistd.h>        // for select()
#include "blib.h"          // blib globals


namespace blib
{


#define usecs_to_double(x, y)   \
    double(x) + (double(y) / 1000000)
#define TIMEVAL_TO_DOUBLE( x )  \
    double(x.tv_sec) + (double(x.tv_usec) / 1000000)

inline double current_time_double(void)
{
  timeval cur_time;
  gettimeofday(&cur_time, 0);
  return TIMEVAL_TO_DOUBLE(cur_time);
} // current_time_double()

inline double seconds_since_past_time(const double& past_time)
{
  return current_time_double() - past_time;
} // seconds_since_past_time()


// Class   : time_timer
// Purpose : to privide a way of monitoring real time
class time_timer
{
    private:
        timeval start_time;     // seconds and microseconds of current time

    public:
        // constructor
        time_timer(void) { start_timing(); }

        // initiate the timer
        void start_timing(void)
        { // get the system time
            gettimeofday(&start_time, 0);
        } // start_timing()

        // wait num_secs seconds from the time taken at start_time
        //  if num_secs seconds since start_time has already passed
        //  then no waiting is done
        void wait_for(double num_secs) const
        { // usecs are accounted for as parts of a second in each double
          sleep( int(num_secs) );
          usleep( int( (num_secs - int(num_secs))*100000 ) );
        } // wait_for()
 
        // explicit name identifying seconds in call to wait_for()
        void wait_for_secs(int num_secs) const
        { sleep(num_secs); }
 
        // wait num_usecs micro-seconds from the time taken at start_time
        void wait_for_usecs(int num_usecs) const
        { usleep( num_usecs ); }
 
        // report the current time as a double
        //  with passed microseconds treated as millionths in the double returned
        double current_time(void) const
        {
            timeval cur_time;
            gettimeofday(&cur_time, 0);

            return TIMEVAL_TO_DOUBLE(cur_time);
        } // current_time()

        // report the time start_timing() was called, as a double
        //  with passed microseconds treated as millionths in the double returned
        double started_at(void) const
        {
            return TIMEVAL_TO_DOUBLE(start_time);
        } // started_at()

        // report number of seconds since start_timing()
        //  with passed microseconds treated as millionths in the double returned
        double passed(void) const
        {
            timeval cur_time;
            gettimeofday(&cur_time, 0);

            return double(cur_time.tv_sec - start_time.tv_sec) + double(cur_time.tv_usec - start_time.tv_usec)/1000000;
        } // passed()

        // report just number of seconds since start_timing()
        //  as an unsigned long int, without adjustment for usecs that have passed
        long just_secs_passed(void) const
        {
            timeval cur_time;
            gettimeofday(&cur_time, 0);

            return cur_time.tv_sec - start_time.tv_sec;
        } // just_secs_passed()

        // report the number of microseconds since start_timing()
        //  as an unsigned long int, ONLY ACCURATE UP TO the maximum value of that data type!
        long just_usecs_passed(void) const
        {
            timeval cur_time;
            gettimeofday(&cur_time, 0);

            return cur_time.tv_usec - start_time.tv_usec;
        } // just_usecs_passed()

        // report the number of microseconds since start_timing()
        //  as an unsigned long int, ONLY ACCURATE UP TO the maximum value of that data type!
        long usecs_passed(void) const
        {
            timeval cur_time;
            gettimeofday(&cur_time, 0);

            return 1000000*(cur_time.tv_sec - start_time.tv_sec) + cur_time.tv_usec - start_time.tv_usec;
        } // usecs_passed()

        // true if num_secs seconds have passed since start_timing()
        bool passed_secs(double num_secs) const
        {
            timeval cur_time;
            gettimeofday(&cur_time, 0);
            double passed_secs = TIMEVAL_TO_DOUBLE(cur_time) - TIMEVAL_TO_DOUBLE(start_time);

            return ( num_secs <= passed_secs );
        } // passed_secs()

        // true if num_usecs microseconds have passed since start_timing()
        bool passed_usecs(double num_usecs) const
        {
            timeval cur_time;
            gettimeofday(&cur_time, 0);
            double passed_secs = TIMEVAL_TO_DOUBLE(cur_time) - TIMEVAL_TO_DOUBLE(start_time);

            return ( num_usecs/1000000 <= passed_secs );
        } // passed_usecs()
        // true if num_usecs microseconds have passed since start_timing()

        long get_secs(void) const
        {
            return start_time.tv_sec;
        } // get_secs()

        long get_usecs(void) const
        {
            return start_time.tv_usec;
        } // get_usecs()
}; // class time_timer


} // namespace blib

#endif // TIME_TIMER_CLASS_DEFINITIONS

// timer.h

