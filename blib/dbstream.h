// File     : dbstream.h
// Purpose  : define DBStream classes
//
// Update Log -
//
// 199905014 - Began iDBStream, oDBStream, DBStreamRec


// prototypes
namespace blib
{
struct DBStreamRec;
class  iDBStream;
class  oDBStream;
} // namespace blib


#ifndef DBSTREAM_CLASS_DEFINITION
#define DBSTREAM_CLASS_DEFINITION


#include <iostream.h>      // stream file stuff
#include <string>          // class string used in DBStreamRec
#include "blib.h"          // blib global typedefs, defines, etc
#include "dll.h"           // for dllist  (doubly linked list)


namespace blib
{


#define  MAX_DBS_RECORD_SIZE  3000


// Struct  : DBStreamRec
// Purpose : contains data required for a DBStream record
struct DBStreamRec
{
    char            type;	// record type
    dllist<string>  fields;	// field values of this record


    // destructor
    ~DBStreamRec(void) { fields.free_all(); }

    // operators
    bool dll_lessthan(const DBStreamRec *a) const
        { return type < a->type; }
}; // struct DBStreamRec


// Classes : iDBStream and oDBStream
// Purpose : contains functions for reading and writing
//           DBStreamRecs from/to a character stream
// Note    : The format for DBStream records is very simple,
//           and follows these rules:
//
//           1) Every record is began and ended on 1 line.
//           2) The first two bytes of each record are
//              reserved, the first byte of which indicates
//              the type of record, the second a white space.
//           3) Fields withing the record/line are deliminated
//              byte tabs, as many fields will be parsed as
//              there are tabs in the line.
//           4) A line begining with the character ';' will
//              be considered a comment and ignored.
//           5) Records cann't exceed MAX_DBS_RECORD_SIZE bytes.
//
//           So the idea is that a DBStream format file will
//           have as many record types as you want, each
//           line being a new record.  Each record starts
//           with the type indicator followed by a space
//           followed by as many tab separated fields as
//           you want.  Here's an example:
//
//           ; This is a comment
//           c c type record 	;-)
//           w w type record
//           w this is the first field	this is the second field
//           w hello	45	54	67676
class iDBStream
{
    private:
        istream  &stream;

    public:
        // constructor
        iDBStream(istream &s) : stream(s) {}

        // mutators
        bool read(DBStreamRec &);     		// read record from stream
}; // class iDBStream

class oDBStream
{
    private:
        ostream  &stream;

    public:
        // constructor
        oDBStream(ostream &s) : stream(s) {}

        // mutators
        bool write(const DBStreamRec &);	// write record to stream
}; // class oDBStream


} // namespace blib


#endif

// dbstream.h

