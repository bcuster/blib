// File     : logfile.h
// Purpose  : logfile class acts like a ofstream, but prepends a time
//            stamp to write to the file.
// Contains : class logfile
// Updates -
// 19990626 - Begun
// 20090527 - appended #endif comment
// 20150423 - abstracted the time-stamp used with #define parameter STAMP_TYPE()


// prototype
namespace blib
{
class blib::logfile;
} // namespace blib



#ifndef LOGFILE_CLASS_DEFINITIONS
#define LOGFILE_CLASS_DEFINITIONS


#include <iostream>     // for cout
#include <fstream>      // for ofstream
#include <string>       // for string
#include "blib.h"       // blib global prototypes, defines, etc
#include "timestamp.h"  // for timestamp


namespace blib
{


// place the name of the type of time stamp function you
// want called below, taken from the const char* returning
// member functions of timestamp class (blib/timestamp.h)
#define  STAMP_TYPE()  full_stamp_2punc()


// Class   : logfile
// Purpose : logfile class acts like a ofstream, but prepends a
//           time stamp to every write.
// Note    : The open file constructor defaults to opening the
//           file for appended writing, whereas ofstream opens
//           it to be written over by default.
// Note    : 1st arg of constructor sets the level of 'debug-reporting'
//           the file should retain, so lines sent with values greater
//           than that level will not be printed to file.
// Example : logfile log(3,"exmp.log");
//           log(2) << "hello ";
//           log(4) << "world";
//           log << ", how are you?" << endl;
//           WILL HAVE OUTPUT: STAMP_TYPE()hello, how are you?
//           where STAMP_TYPE() is a time-stamp set above
//           (giving no debug arg is treated as debug arg=0)
class logfile : public ofstream
{
    private:
//        ofstream    outfile;   // the log file
        ostream     trash;     // dummy for taking output below the current debug level
        stringbuf   trashbuf;
        timestamp   datetime;

    public:
        // level of error reporting to send to outfile
        int  global_debug_report_level;  // this is public since it makes no sense to set it only at construction time

        // constructors & desctructor
        logfile(const int& report_level = 0, const string file = "")
          : trash(&trashbuf), global_debug_report_level(report_level)
        {
          if( file.c_str() ) ofstream::open(file.c_str(), ios::app);  // open file and direct stream to it
        } // logfile()
        ~logfile(void)
        {
          close();
        } // ~logfile()

        // open the log file, if not done at construction time
        void open(const string file)
        {
          if( file.c_str() )
          {
            close();
            ofstream::open(file.c_str(), ios::app);
          } // if
        } // logfile()

        // public inspector for getting current time string
        const char* time_stamp(void) { return datetime.STAMP_TYPE(); }

        // just return the current stream, without adding a time-stamp
        std::ostream& get_stream(int debug_report_level = 0)
        {
          // clear the trash buffer before potentially writing to it again
          string null = "NULL";
          trashbuf.str(null);
          // return the appropriate ostream depending on debug level
          if( debug_report_level > global_debug_report_level ) return trash;
          if( is_open() )
          { // this is what we want to have happen to get the usual output
            return *this;
          } // if
          else
          { // no log file has been opened, send output to stdout
            return cout;
          } // else
        } // get_stream()

        // use operator() to return a ostream copy to write to with <<
        std::ostream& operator()(int debug_report_level = 0)
        {
          // clear the trash buffer before potentially writing to it again
          string null = "NULL";
          trashbuf.str(null);
          // return the appropriate ostream depending on debug level
          if( debug_report_level > global_debug_report_level ) return trash;
          if( is_open() )
          { // this is what we want to have happen to get the usual output
            *this << datetime.STAMP_TYPE();
            return *this;
          } // if
          else
          { // no log file has been opened, send output to stdout
            cout << datetime.STAMP_TYPE();
            return cout;
          } // else
        } // operator()()
}; // class logfile

//NOTE: to make the logfile take other object types as a 1st arg in a outstream line
// (when not using the operator()) besides what's listed below,
// you will need to add a line like one of the below
inline ostream& operator<<(logfile& os, const char obj) { return os() << obj; }
inline ostream& operator<<(logfile& os, const char* obj) { return os() << obj; }
inline ostream& operator<<(logfile& os, const string obj) { return os() << obj; }
inline ostream& operator<<(logfile& os, const int obj) { return os() << obj; }
inline ostream& operator<<(logfile& os, const long obj) { return os() << obj; }
inline ostream& operator<<(logfile& os, const uint obj) { return os() << obj; }
inline ostream& operator<<(logfile& os, const ulong obj) { return os() << obj; }
inline ostream& operator<<(logfile& os, const float obj) { return os() << obj; }
inline ostream& operator<<(logfile& os, const double obj) { return os() << obj; }


} // namespace blib

#endif // LOGFILE_CLASS_DEFINITIONS

// logfile.h

