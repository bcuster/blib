// File    : dbfile.h
// Purpose : contains DBFile class definition
// Updates -
//   01/10/99 - Begun
//   20090526 - appended #endif comment
//   20090527 - changed to 'char*' formatting (from 'char *')
//   20120412 - added add_rec(), add_if_missing(), update(), find(), InList(), copy_if_found() and get_*()
//   20150506 - fixed copy_if_found() and get_*() functions to be const
//   20150507 - converted all the char*'s over to using bstring's



// prototypes
namespace blib
{
class  DBFile;
struct DBRecord;
} // namespace blib


#ifndef DBFILE_CLASS_DEFINITION
#define DBFILE_CLASS_DEFINITION


#include "blib.h"     // blib global typedefs, bstring, etc
#include "dll.h"      // dllist class
#include "bstring.h"  // bstring class


using std::ostream;


namespace blib
{


// Class   : DBFile
// Purpose : a DBFile will read and write a standard database
//           text file, the format for which is given in the
//           commented documentation above in this file
//           a DBFile acts as a layer between the actual file
//           and the file quering entity
//           all information is passed as a dllist of DBRecord, 
//           which contains the group/field/value for each 
//           database entry
//           DBFile inherets dllist, so the instance of DBFile
//           represents the dllist of file records
class DBFile : public dllist<DBRecord>
{
    private:
        string  filename;              // name of std database file
        string  lookupGroup;

    public:
        // constructors
        DBFile(void) {}                // initialize w/o file read
        DBFile(const string& name)     // initialize & read from file
            { filename = name; read(name.c_str()); }
        // destructor
        ~DBFile(void)                  // free any records read
            { free_all(); }

        // inspectors
        string name(void) const { return filename; }

        // mutators
        int read(const char* name = NULL);    // import data from file
        int read(const string& name) { return read(name.c_str()); }
        void setfile(const string& name)      // set filename
            { filename = name; }

        // easy way to add to list, provide group.field.value as arguements
        bool add_rec(const string&, const string&, const bstring&);
        // update() searches for group.field, replaces value if found, otherwise adds a record
        bool update(const string&, const string&, const bstring&);
        // add_if_missing() 1st searches for group.field, if found nothing is done
        bool add_if_missing(const string&, const string&, const bstring&);

        // auxiliary
        int write(const char* name = NULL) const;   // write data to file
        int write(const string& name) const { return write(name.c_str()); }
        DBRecord* find(const string&, const string&);
        const DBRecord* find(const string&, const string&) const;

        // query
        bool InList(const string&, const string&) const;
        bool CurrentLookup(const string&);    // sets the group being searched in by LWhatIs() and CWhatIs()
        long LWhatIs(const string&) const;    // returns value of 1st field match arguement
        string CWhatIs(const string&) const;  // returns value of 1st field match arguement
        uint add_to_list_all_strings_found(const string&, const string&, dllist<string>&) const;  // note: does not remove any item already in list

        // copy_if_found() 1st searches for group.field, if found copies value into 3rd arguement
        bool copy_if_found(const string&, const string&, string&) const;
        bool copy_if_found(const string&, const string&, long&) const;
        bool copy_if_found(const string&, const string&, ulong&) const;
        bool copy_if_found(const string&, const string&, int&) const;
        bool copy_if_found(const string&, const string&, uint&) const;        
        bool copy_if_found(const string&, const string&, float&) const;
        bool copy_if_found(const string&, const string&, double&) const;

        // get_*() searches for group.field, if found returns a copy, otherwise get "NULL" or 0
        string get_string(const string&, const string&) const;
        const char* get_chars(const string&, const string&) const;
        long get_long(const string&, const string&) const;
        ulong get_ulong(const string&, const string&) const;
        int get_int(const string&, const string&) const;
        uint get_uint(const string&, const string&) const;        
        float get_float(const string&, const string&) const;
        double get_double(const string&, const string&) const;        
};  // class DBFile

inline ostream& operator<<(ostream& os, const DBFile& dbfile)
{ return os << dbfile.name(); }


#define  DBRecord_COMMENT_TAG  "__DBRecord_COMMENT__"

// Struct  : DBRecord
// Purpose : a DBRecord serves as a generic container
//           for one field value of information
// Note    : the one hard coded type of group & field is for comments
//           (lines begun with '#'), they will be coded with
//           field="__DBRecord_COMMENT__"
struct DBRecord
{
    bstring  group;
    bstring  field;
    bstring  value;
};  // struct DBRecord


} // namespace blib

#endif // DBFILE_CLASS_DEFINITION

// dbfile.h

