// File    : dbfile.cxx
// Purpose : contains DBFile class member functions
// Author  : Bradley Custer
//
// Updates -
//   01/10/99 - Begun
//   20090627 - included <cerrno> and <cstring> for new stdlib compatibility
//   20120412 - added add_rec(), add_if_missing(), update(), find(), InList(), copy_if_found() and get_*()


#include <cstring>        // for strchr()
#include <cerrno>         // for errno macro
#include <stdio.h>        // for fopen(), fclose(), fgetc()
#include <stdlib.h>       // for strtol()
#include <ctype.h>        // for is...() functions
#include "dbfile.h"       // for class DBFile


namespace blib
{


#define ISALNUM(c)  (isalpha(c) || isdigit(c))


// Function : int DBFile::read(char *)
// Purpose  : to
//            1) open the std database file
//            2) read every record in the file
//               into *this
//            3) close the std database file
//            4) report fail/success results
// Usage    : name may be either NULL or point to
//            a char string designating the file
//            to read from
//            if name is NULL then the pre-set file
//            designated by filename will be used
//            instead, if filename hasn't been set,
//            and name is NULL a error is returned
//            if name is not NULL, and it specifies
//            a file which is readable, then filename
//            will be set to name
//
// File Syntax: here's an example of a DBFile file:
//
// [group-name_is_a_name_like_this-or-a.name.like.this]
// field-name_is_a_similar.name= then whatever comes after '=' until '\n' goes into value for that group.field
// anther_field-name$ but if the field name is terminated with a '$'
//     then everything on multiple lines will go into the value
//     for that group.field, until the multi-line value is terminated by
//     a line with nothing but a '.', like this:
// .
// third_field=10
// # but a comment, which is any line started with a '#' and not inside a multi-line value,
// # is good for explaining things like what happens when third_field=10
//
// Returns  : 0     - successful file read
//            errno - unsuccessful file open or read
int DBFile::read(const char* name)
{
    FILE *file;

    // determine file to read from
    //  and open it
    
    if( name )
    {					// try the argument for the file
        file = fopen(name, "r");

        if( file )
            filename = name;
        else
            return errno;
    } // if
    else if( filename.size() )
    {					// try filename for the file
        file = fopen(filename.c_str(), "r");

        if( !file ) return errno;
    } // else if
    else return 1; 			// no file name to open

    // successful file open
    // begin scaning file, and adding DBRecord 

    // parse_state identifies what step the parser is
    //  at in scanning characters for a record
    int  parse_state = 0;

    string  buf;         		// string scan-in buffer
    char    c;           		// last read character
    bool    add_record;  		// flags if ready to add to dllist

    string  current_group;  		// group reading fields for
    string  current_field;  		// field just read
    string  current_value;  		// value just read

	  // file scan loop
    while( (c = fgetc(file)) != EOF )
    {
        // reject all non-printable characters
        if( !isascii(c) ) continue;

        add_record = false;  		// set add initially off

        switch( parse_state )
        {
            case 0 : 			// state 0 - scanning for group
                if( c == '[' )
                {
                    parse_state   = 1;
                    buf           = "";
                } // if
                else if( c == '#' )   // start of a comment
                {
                    parse_state   = 5;
                    current_group = DBRecord_COMMENT_TAG;
                    current_field = DBRecord_COMMENT_TAG;
                    buf           = "";
                } // else
            break;
            case 1 : 			// state 1 - reading group name
                if( c != ']' )
                {
                    if( ISALNUM(c) || c == '_' || c == '-' || c == '.' )
                        buf += c;
                } // if
                else
                {
                    parse_state   = 2;
                    current_group = buf;
                    buf           = "";
                } // else
            break;
            case 2 : 			// state 2 - reading field name
                if( ISALNUM(c) || c == '_' || c == '-' || c == '.' )
                    buf += c;
                else if( c == '=' ) 	// start of single-line string
                {
                    parse_state   = 3;
                    current_field = buf;
                    buf           = "";
                } // else if
                else if( c == '$' ) 	// start of multi-line string
                {
                    parse_state   = 4;
                    current_field = buf;
                    buf           = "";
                } // else if
                else if( c == '[' )   // start of another group name
                {
                    parse_state   = 1;
                    buf           = "";
                } // else
                else if( c == '#' )   // start of a comment
                {
                    parse_state   = 3;
                    current_field = DBRecord_COMMENT_TAG;
                    buf           = "";
                } // else
            break;
            case 3 : 			// state 3 - reading single-line string
                if( c != '\n' )
                    buf += c;
                else
                {
                    parse_state   = 2;
                    current_value = buf;
                    buf           = "";
                    add_record    = true;
                } // else
            break;
            case 4 :			// state 4 - reading multi-line string
            {
                char d; 		// look-ahead character

                if( c != '\n' )
                    buf += c;
                else if( (d = fgetc(file)) == '.' )  // a '.' (by itself on a line alone, after multi-line string) terminates multi-line string
                {
                    parse_state   = 2;
                    current_value = buf;
                    buf           = "";
                    add_record    = true;
                } // else if
                else
                {
                    buf += c;
                    ungetc(d, file); 	// push d back onto stream
                } // else
            } // for destructing d
            break;
            case 5 : 			// state 5 - comment prior to having 1st group name
                if( c != '\n' )
                    buf += c;
                else
                {
                    parse_state   = 0;
                    current_value = buf;
                    buf           = "";
                    add_record    = true;
                } // else
            break;
            default: // error: this should never happen
                parse_state = 0;
        } // switch

        // add a record to the dllist if flagged to
        if( add_record )
        {
            DBRecord *rec = new DBRecord;
            rec->group = current_group;
            rec->field = current_field;
            rec->value = current_value;
            add( rec );
        } // if
    } // while

    // check if we need to add EOF terminated record
    add_record = false;
    if( parse_state == 3 || parse_state == 4 )
    { // state 3 - reading single-line string
    	// state 4 - reading multi-line string    
        current_value = buf;
        add_record    = true;
    } // if

    // add a record to the dllist if flagged to
    if( add_record )
    {
        DBRecord *rec = new DBRecord;
        rec->group = current_group;
        rec->field = current_field;
        rec->value = current_value;
        add( rec );
    } // if

    fclose( file );   // release the file stream

    return 0;  				// successful file read
} // DBFile::read()


// Function : int DBFile::write(const char *)
// Purpose  : to
//            1) open the a file for OVERWRITE
//            2) write every dll record from *this
//               to the file in std database format
//            3) close the std database file
//            4) report fail/success results
// Usage    : name may be either NULL or point to
//            a char string designating the file
//            to write to
//            if name is NULL then the pre-set file
//            designated by filename will be used
//            instead, if filename hasn't been set,
//            and name is NULL a error is returned
//            if name is not NULL, and it specifies
//            a file name which is writeable, then
//            filename will be set to name
// Returns  : 0     - successful file write
//            errno - unsuccessful file open or write
int DBFile::write(const char* name) const
{
    FILE *file;

    // determine file to read from
    //  and open it
    if( name )
    { // try the argument for the file
        file = fopen(name, "w");

        if( !file ) return errno;
    } // if
    else if( filename.size() )
    { // try filename for the file
        file = fopen(filename.c_str(), "w");

        if( !file ) return errno;
    } // else if
    else return 1; // no file name to open

    // successful file open
    // begin writting DBRecord 

    // print any initial comments, then inital group name
    cdllit<DBRecord> i(*this);
    while( i()->field == DBRecord_COMMENT_TAG )
    {
      fprintf(file, "#%s\n", i()->value.c_str());
      ++i;
    } // while

    string current_group;  // buffer for group being written
    current_group = i()->group;
    fprintf(file, "[%s]\n", current_group.c_str());  // need to write 1st heading outside loop so it doesn't have a leading carriage return '\n'

    for(; !i.finished(); ++i)
    {
        // check if group needs updating
        if( i()->group != current_group )
        { // write new group heading
            current_group = i()->group;
            fprintf(file, "\n[%s]\n", current_group.c_str());
        } // if

        // determine if writing single or multi line string or comment
        if( strchr(i()->value.c_str(), '\n') )
            // i()->value is a multi-line string
            fprintf(file, "%s$%s\n.\n",  i()->field.c_str(), i()->value.c_str());
        else if( i()->field == DBRecord_COMMENT_TAG )
            // i()->value is a comment
            fprintf(file, "#%s\n", i()->value.c_str());
        else
            // i()->value is a single-line string
            fprintf(file, "%s=%s\n",  i()->field.c_str(), i()->value.c_str());
    } // for

    fclose( file );   // release the file stream

    return 0;  // successful file write
} // DBFile::write()

// Set the current group
bool DBFile::CurrentLookup(const string &token)
{   
    for(cdllit<DBRecord> i(*this); !i.finished(); ++i)
    {
        if(i()->group != token)		// Loop through until it is found
            continue;
        else
        {
            lookupGroup = i()->group;   // Assign our group
            return true;                // We found it
        } // else
    } // if
    return false;
} // DBFile::CurrentLookup()

// Long version of query
long DBFile::LWhatIs(const string& token) const
{
    for(cdllit<DBRecord> i(*this); !i.finished(); ++i)
        if(i()->group == lookupGroup)
        {
            if(i()->field != token)		// Loop through until it is found
                continue;
            else
                return strtol(i()->value.c_str(), NULL, 10);     // Assign our value
        } // if
    return -1;
} // DBFile::LWhatIs()

// String version of query
string DBFile::CWhatIs(const string& token) const
{
    for(cdllit<DBRecord> i(*this); !i.finished(); ++i)
        if(i()->group == lookupGroup)
        {
            if(i()->field != token)		// Loop through until it is found
                 continue;
            else
                 return i()->value;             // Assign our value
        } // if
    return NULL;
} // DBFile::CWhatIs()

// report if a record with the group.field is in current dllist
//  Returns: 0 if not found, pointer to first matching DBRecord otherwise
DBRecord* DBFile::find(const string& group, const string& field)
{
  for(dllit<DBRecord> i(*this); !i.finished(); ++i)
    if( i()->group == group && i()->field == field )
      return i();
  return 0;
} // DBFile::find()

// const version of find
const DBRecord* DBFile::find(const string& group, const string& field) const
{
  for(cdllit<DBRecord> i(*this); !i.finished(); ++i)
    if( i()->group == group && i()->field == field )
      return i();
  return 0;
} // DBFile::find()


// report if a record with the group.field is in current dllist
//  Returns: false if not found, true otherwise
bool DBFile::InList(const string& group, const string& field) const
{
  for(cdllit<DBRecord> i(*this); !i.finished(); ++i)
    if( i()->group == group && i()->field == field )
      return true;
  return false;
} // DBFile::InList()

// adds a newly allocated string to list with the value of each
//  record in *this DBFile with matching group.field
uint DBFile::add_to_list_all_strings_found(const string& group, const string& field, dllist<string>& list) const
{
  uint num_found = 0;

  for(cdllit<DBRecord> i(*this); !i.finished(); ++i)
    if( i()->group == group && i()->field == field )
    {
      string* str = new string(i()->value);
      list.add( str );
      num_found++;
    } // if

  return num_found;
} // DBFile::add_to_list_all_strings_found()


bool DBFile::add_rec(const string& group, const string& field, const bstring& value)
{
  if( !add( new DBRecord ) ) return false;
  last()->group = group;
  last()->field = field;
  last()->value = value;
  return true;
} // DBFile::add_rec()


// adds the group.field.value only if group.field not found in dllist
//  Returns: true - if added or found, false - attempted add but memory allocation failed
bool DBFile::add_if_missing(const string& group, const string& field, const bstring& value)
{
  if( !InList(group, field) )
    return add_rec(group, field, value);
  return true;
} // DBFile::add_if_missing()


// adds the group.field.value only if group.field not found in dllist, updates value otherwise
//  Returns: true - if added or updated, false - attempted add but memory allocation failed
bool DBFile::update(const string& group, const string& field, const bstring& value)
{
  DBRecord* tmp = find(group, field);
  if( !tmp )
    return bool(add_rec(group, field, value));
  tmp->value = value;
  return true;
} // DBFile::update()


// 1st searches for group.field, if found copies value into 3rd arguement
//  Returns: true - if found, false - not found
bool DBFile::copy_if_found(const string& group, const string& field, string& value) const
{
  const DBRecord* tmp = find(group, field);
  if( !tmp ) return false;
  value = tmp->value;
  return true;
} // DBFile::copy_if_found()

bool DBFile::copy_if_found(const string& group, const string& field, long& value) const
{
  const DBRecord* tmp = find(group, field);
  if( !tmp ) return false;
  value = tmp->value;
  return true;
} // DBFile::copy_if_found()

bool DBFile::copy_if_found(const string& group, const string& field, ulong& value) const
{
  const DBRecord* tmp = find(group, field);
  if( !tmp ) return false;
  value = tmp->value;
  return true;
} // DBFile::copy_if_found()

bool DBFile::copy_if_found(const string& group, const string& field, int& value) const
{
  const DBRecord* tmp = find(group, field);
  if( !tmp ) return false;
  value = tmp->value;
  return true;
} // DBFile::copy_if_found()

bool DBFile::copy_if_found(const string& group, const string& field, uint& value) const
{
  const DBRecord* tmp = find(group, field);
  if( !tmp ) return false;
  value = tmp->value;
  return true;
} // DBFile::copy_if_found()

bool DBFile::copy_if_found(const string& group, const string& field, float& value) const
{
  const DBRecord* tmp = find(group, field);
  if( !tmp ) return false;
  value = tmp->value;
  return true;
} // DBFile::copy_if_found()

bool DBFile::copy_if_found(const string& group, const string& field, double& value) const
{
  const DBRecord* tmp = find(group, field);
  if( !tmp ) return false;
  value = tmp->value;
  return true;
} // DBFile::copy_if_found()


// searches for group.field, if found returns a copy
//  if not found, returns "NULL" if string, 0 if numeric type
string DBFile::get_string(const string& group, const string& field) const
{
  const DBRecord* tmp = find(group, field);
  if( !tmp ) return string("");
  return tmp->value;
} // DBFile::get_string()

const char* DBFile::get_chars(const string& group, const string& field) const
{
  const DBRecord* tmp = find(group, field);
  if( !tmp ) return 0;
  return tmp->value;
} // DBFile::get_chars()

long DBFile::get_long(const string& group, const string& field) const
{
  const DBRecord* tmp = find(group, field);
  if( !tmp ) return 0;
  return tmp->value;
} // DBFile::get_long()

ulong DBFile::get_ulong(const string& group, const string& field) const
{
  const DBRecord* tmp = find(group, field);
  if( !tmp ) return 0;
  return tmp->value;
} // DBFile::get_ulong()

int DBFile::get_int(const string& group, const string& field) const
{
  const DBRecord* tmp = find(group, field);
  if( !tmp ) return 0;
  return tmp->value;
} // DBFile::get_int()

uint DBFile::get_uint(const string& group, const string& field) const
{
  const DBRecord* tmp = find(group, field);
  if( !tmp ) return 0;
  return tmp->value;
} // DBFile::get_uint()

float DBFile::get_float(const string& group, const string& field) const
{
  const DBRecord* tmp = find(group, field);
  if( !tmp ) return 0;
  return tmp->value;
} // DBFile::get_float()

double DBFile::get_double(const string& group, const string& field) const
{
  const DBRecord* tmp = find(group, field);
  if( !tmp ) return 0;
  return tmp->value;
} // DBFile::get_double()


} // namespace blib


// dbfile.cxx

