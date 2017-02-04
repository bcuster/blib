// File     : timestamp.cxx
// Purpose  : some definitions for the timestamp class
//
// Update Log -
//
// 20150809 - Begun, added the const name arrarys


#include "timestamp.h"     // timestamp class


namespace blib
{


//
// *** Day of the Week and Month Character Strings ***
//
const char* timestamp::weekday_full_strings[8] =
{
  "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday",
  "EveryDay"  // N/A or to indicate an arbitrary day (for applications like scheduling)
};
const char* timestamp::weekday_short_strings[8] =
{
  "Sun", "Mon",	"Tue", "Wed",	"Thr", "Fri", "Sat",
  "Evy"  // N/A or to indicate an arbitrary day (for applications like scheduling)
};
const char* timestamp::month_full_strings[13] =
{
  "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December",
  "EveryMonth"  // N/A or to indicate an arbitrary month (for applications like scheduling)
};
const char* timestamp::month_short_strings[13] =
{
  "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
  "Evy"  // N/A or to indicate an arbitrary month (for applications like scheduling)
};


} // namespace blib

// timestamp.cxx

