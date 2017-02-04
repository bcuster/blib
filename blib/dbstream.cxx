// File     : dbstream.cxx
// Purpose  : function definitions for class DBStream
//
// Update Log -
//
// 19990514 - Begun


#include  "dbstream.h"       // DBStream, DBStreamRec


namespace blib
{


//******************************************//
//*        DBStream Member Functions       *//
//******************************************//

// Function : bool oDBStream::write(const DBStreamRec &rec)
// Purpose  : to place the data in 'rec' onto 'stream'
//            in the format explained in the header for
//            class DBStream
// Note     : if no fields are present in 'rec' a blank
//            record will still be written
// Warning  : this function assumes that the prior record
//            on the stream was carriage return terminated
//            so no CR is passed onto the stream before
//            writing 'rec'
// Returns  : true   - successful write
//            false  - failed to write to the stream
bool oDBStream::write(const DBStreamRec &rec)
{
    // write the type
    stream << rec.type;

    // write the fields
    for(cdllit<string> i(rec.fields); !i.done(); ++i)
        stream << *i() << '\t';

    stream << endl;

    return true;
} // oDBStream::write()


// Function : bool iDBStream::read(DBStreamRec &rec)
// Purpose  : extract a record from 'stream', assuming
//            it to be in DBStream format, and place
//            the data into 'rec'
// Note     : any line read which begins with a ';'
//            will be ignored as a comment
// Warning  : records will be assumed not to use more
//            than MAX_DBS_RECORD_SIZE bytes, if a line
//            exeeds this limit record paring will become
//            flawed without indication to caller
// Warning  : 'rec' will be overwritten with the data
//            of the nect record on the stream
// Returns  : true   - successful read
//            false  - failed to read the stream
bool iDBStream::read(DBStreamRec &rec)
{
    rec.fields.free_all();

    char parse_space[MAX_DBS_RECORD_SIZE];

    // read the line, and
    //  parse the record type,
    //  while ignoring comments
    do
    {   // check if the stream is ended
        if( stream.eof() ) return false;
        stream.getline(parse_space, MAX_DBS_RECORD_SIZE);
        rec.type = parse_space[0];
    } while( !parse_space[0] || parse_space[0] == ';' );

    // parse the fields
    for( char *tok = strtok(parse_space + 2, "\t");
               tok;
               tok = strtok(NULL, "\t") )
    {
        string *t = new string;
        *t = tok;
        rec.fields.add( t );
    } // for

    return true;
} // iDBStream::read()


} // namespace blib


// dbstream.cxx

