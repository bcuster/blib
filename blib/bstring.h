// File    : bstring.h
// Purpose : blib type bstring -- wrapper of class std::string
//           made to simplify use of atoi, sscanf, etc with assignment operators
//           and so make implicit conversions easy for lots of types!
// Updates -
//   20150507 - begun



// prototype
namespace blib
{
class  bstring;
} // namespace blib


#ifndef BSTRING_CLASS_DEFINITION
#define BSTRING_CLASS_DEFINITION


#include <cstdlib>  // atoi(), etc
#include <cstdio>   // sprintf(), etc
#include <string>   // class std::string
#include "blib.h"   // blib global typedefs, defines, etc


using std::string;


namespace blib
{


// NOTE: BSTRING_MAX_DIGITS must be set to the largest # of digits
//   needed to accurately represent any number (long,double,etc).
#define  BSTRING_MAX_DIGITS         30
#define  BSTRING_STR_VALUE(arg)     #arg
#define  BSTRING_TO_STRING(name)    BSTRING_STR_VALUE(name)
#define  BSTRING_MAX_DIGITS_STRING  BSTRING_TO_STRING(BSTRING_MAX_DIGITS)
// the 3 macros above are the only way to get a macro to represent a char string version of the value of another macro
// see: http://stackoverflow.com/questions/195975/how-to-make-a-char-string-from-a-c-macros-value


// Class   : bstring
// Purpose : extension of string to allow for simple coding of manipulations with numeric types
class bstring : public std::string
{
  public:
    // default constructor & destructor
    bstring(void)  = default;
    ~bstring(void) = default;

    // copy constructors  (implicit conversion from argument to *this)
    bstring(const string& val)
    { this->string::operator=(val); }
    bstring(const char* val)
    { this->string::operator=(val); }
    bstring(const int val)
    {
      char buf[BSTRING_MAX_DIGITS+1];
      snprintf(buf, BSTRING_MAX_DIGITS+1, "%d", val);
      this->string::operator=(buf);
    }
    bstring(const uint val)
    {
      char buf[BSTRING_MAX_DIGITS+1];
      snprintf(buf, BSTRING_MAX_DIGITS+1, "%u", val);
      this->string::operator=(buf);
    }
    bstring(const long val)
    {
      char buf[BSTRING_MAX_DIGITS+1];
      snprintf(buf, BSTRING_MAX_DIGITS+1, "%ld", val);
      this->string::operator=(buf);
    }
    bstring(const ulong val)
    {
      char buf[BSTRING_MAX_DIGITS+1];
      snprintf(buf, BSTRING_MAX_DIGITS+1, "%lu", val);
      this->string::operator=(buf);
    }
    bstring(const float val)
    {
      char buf[BSTRING_MAX_DIGITS+10];
      snprintf(buf, BSTRING_MAX_DIGITS+10, "%." BSTRING_MAX_DIGITS_STRING "g", val);
      this->string::operator=(buf);
    }
    bstring(const double val)
    {
      char buf[BSTRING_MAX_DIGITS+10];
      snprintf(buf, BSTRING_MAX_DIGITS+10, "%." BSTRING_MAX_DIGITS_STRING "lg", val);
      this->string::operator=(buf);
    }

    // type-cast operators  (implicit conversion from *this to return value)
    operator const char*() const { return c_str(); }
    operator int() const { return atoi(c_str()); }
    operator uint() const { return uint(strtoul(c_str(),NULL,10)); }
    operator long() const { return atol(c_str()); }
    operator ulong() const { return strtoul(c_str(),NULL,10); }
    operator float() const { return float(atof(c_str())); }
    operator double() const { return atof(c_str()); }
};  // class bstring


// arithmetical operations
inline int operator+(const string& str, const int value)
{ return int(bstring(str)) + value; }
inline int operator-(const string& str, const int value)
{ return int(bstring(str)) - value; }
inline int operator*(const string& str, const int value)
{ return int(bstring(str)) * value; }
inline int operator/(const string& str, const int value)
{ return int(bstring(str)) / value; }
inline bool operator==(const string& str, const int value)
{ return int(bstring(str)) == value; }
inline int operator+(const int value, const string& str)
{ return value + int(bstring(str)); }
inline int operator-(const int value, const string& str)
{ return value - int(bstring(str)); }
inline int operator*(const int value, const string& str)
{ return value * int(bstring(str)); }
inline int operator/(const int value, const string& str)
{ return value / int(bstring(str)); }
inline bool operator==(const int value, const string& str)
{ return value == int(bstring(str)); }

inline uint operator+(const string& str, const uint value)
{ return uint(bstring(str)) + value; }
inline uint operator-(const string& str, const uint value)
{ return uint(bstring(str)) - value; }
inline uint operator*(const string& str, const uint value)
{ return uint(bstring(str)) * value; }
inline uint operator/(const string& str, const uint value)
{ return uint(bstring(str)) / value; }
inline bool operator==(const string& str, const uint value)
{ return uint(bstring(str)) == value; }
inline uint operator+(const uint value, const string& str)
{ return value + uint(bstring(str)); }
inline uint operator-(const uint value, const string& str)
{ return value - uint(bstring(str)); }
inline uint operator*(const uint value, const string& str)
{ return value * uint(bstring(str)); }
inline uint operator/(const uint value, const string& str)
{ return value / uint(bstring(str)); }
inline bool operator==(const uint value, const string& str)
{ return value == uint(bstring(str)); }

inline long operator+(const string& str, const long value)
{ return long(bstring(str)) + value; }
inline long operator-(const string& str, const long value)
{ return long(bstring(str)) - value; }
inline long operator*(const string& str, const long value)
{ return long(bstring(str)) * value; }
inline long operator/(const string& str, const long value)
{ return long(bstring(str)) / value; }
inline bool operator==(const string& str, const long value)
{ return long(bstring(str)) == value; }
inline long operator+(const long value, const string& str)
{ return value + long(bstring(str)); }
inline long operator-(const long value, const string& str)
{ return value - long(bstring(str)); }
inline long operator*(const long value, const string& str)
{ return value * long(bstring(str)); }
inline long operator/(const long value, const string& str)
{ return value / long(bstring(str)); }
inline bool operator==(const long value, const string& str)
{ return value == long(bstring(str)); }

inline ulong operator+(const string& str, const ulong value)
{ return ulong(bstring(str)) + value; }
inline ulong operator-(const string& str, const ulong value)
{ return ulong(bstring(str)) - value; }
inline ulong operator*(const string& str, const ulong value)
{ return ulong(bstring(str)) * value; }
inline ulong operator/(const string& str, const ulong value)
{ return ulong(bstring(str)) / value; }
inline bool operator==(const string& str, const ulong value)
{ return ulong(bstring(str)) == value; }
inline ulong operator+(const ulong value, const string& str)
{ return value + ulong(bstring(str)); }
inline ulong operator-(const ulong value, const string& str)
{ return value - ulong(bstring(str)); }
inline ulong operator*(const ulong value, const string& str)
{ return value * ulong(bstring(str)); }
inline ulong operator/(const ulong value, const string& str)
{ return value / ulong(bstring(str)); }
inline bool operator==(const ulong value, const string& str)
{ return value == ulong(bstring(str)); }

inline float operator+(const string& str, const float value)
{ return float(bstring(str)) + value; }
inline float operator-(const string& str, const float value)
{ return float(bstring(str)) - value; }
inline float operator*(const string& str, const float value)
{ return float(bstring(str)) * value; }
inline float operator/(const string& str, const float value)
{ return float(bstring(str)) / value; }
inline bool operator==(const string& str, const float value)
{ return float(bstring(str)) == value; }
inline float operator+(const float value, const string& str)
{ return value + float(bstring(str)); }
inline float operator-(const float value, const string& str)
{ return value - float(bstring(str)); }
inline float operator*(const float value, const string& str)
{ return value * float(bstring(str)); }
inline float operator/(const float value, const string& str)
{ return value / float(bstring(str)); }
inline bool operator==(const float value, const string& str)
{ return value == float(bstring(str)); }

inline double operator+(const string& str, const double value)
{ return double(bstring(str)) + value; }
inline double operator-(const string& str, const double value)
{ return double(bstring(str)) - value; }
inline double operator*(const string& str, const double value)
{ return double(bstring(str)) * value; }
inline double operator/(const string& str, const double value)
{ return double(bstring(str)) / value; }
inline bool operator==(const string& str, const double value)
{ return double(bstring(str)) == value; }
inline double operator+(const double value, const string& str)
{ return value + double(bstring(str)); }
inline double operator-(const double value, const string& str)
{ return value - double(bstring(str)); }
inline double operator*(const double value, const string& str)
{ return value * double(bstring(str)); }
inline double operator/(const double value, const string& str)
{ return value / double(bstring(str)); }
inline bool operator==(const double value, const string& str)
{ return value == double(bstring(str)); }


} // namespace blib

#endif // BSTRING_CLASS_DEFINITION

// bstring.h

