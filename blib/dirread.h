// File    : dirread.h
// Purpose : contains DirRead class definition
// Updates -
// 19990626 - Begun
// 20090527 - appended #endif comment


// prototypes
namespace blib
{
class  DirRead;
struct DirRec;
} // namespace blib


#ifndef DIRREAD_CLASS_DEFINITION
#define DIRREAD_CLASS_DEFINITION


#include <string>    // for class string
#include "blib.h"    // blib global typedefs, defines, etc
#include "dll.h"     // for class dllist


namespace blib
{


// Class   : DirRead
// Purpose : Abstracts the directory reading system calls
//           from the program, so that portability is easier.
//           DirRead::read() will fill the DirRead *this with
//           DirRecs, having a name and size of file, for
//           every file in 'dirname'.
class DirRead : public dllist<DirRec>
{
    private:
        string  dirname;              // name of directory

    public:
        // constructors
        DirRead(void) {}              // initialize w/o dir name
        DirRead(const char *name)     // initialize & read from dir
          { read(name); }
        DirRead(const string &name)   // initialize & read from dir
          { read(name); }
        // destructor
        ~DirRead(void)                // free any records read
          { free_all(); }

        // mutators
        int read(void);               // read file names of directory
        int read(const char *name)
          { dirname = name; return read(); }
        int read(const string &name)
          { dirname = name; return read(); }
        void setdir(char *name)             // set dirname
          { dirname = name; }
        void setdir(const string &name)     // set dirname
          { dirname = name; }

        // inspectors
        const string &DirName(void) { return dirname; }
        bool find(const string &) const;
};  // class DirRead


// Struct  : DirRec
// Purpose : contains file information
// Note    : The name refers to the file
//           assuming the DirRead::dirname
//           is prefixed to it.  So if dirname
//           is not the current-working dir,
//           then a fopen() would need:
//           DirRead::DirName() + '/' + DirRec::name
struct DirRec
{
    string         name;   // name of file
    unsigned long  size;   // size of file
    bool           dir;    // file is a directory
};  // struct DirRec


} // namespace blib

#endif //  DIRREAD_CLASS_DEFINITION

// dirread.h

