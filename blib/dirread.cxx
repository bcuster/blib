// File    : dirread.cxx
// Purpose : contains DirRead class member functions
// Updates -
// 19990626 - Begun


#include <sys/types.h>  // POSIX dir stuff:
#include <sys/stat.h>
#include <dirent.h>     //  opendir() closedir() readdir()
#include <unistd.h>     //  stat()
#include <cstring>      // for strcmp()
#include "dirread.h"    // DirRead and DirRec


namespace blib
{


// Function : int DirRead::read(void)
// Purpose  : Reads all the file names from
//            DirRead::dirname into the DirRec
//            list *this.
// Warning  : Calling this function will free all
//            memory used by any DirRecs currently
//            in the *this list.
// Returns  : -1 - failed to read directory
//                 ERRNO set to opendir() error
//            # of files read otherwise
int DirRead::read(void)
{
    DIR *dir;

    if( !dirname.size() )
        dirname = ".";  // default to current-working dir

    dirname += '/';     // needed to indicate a dir name

    // open directory for reading file names
    if( !(dir = opendir(dirname.c_str())) )
        return -1;

    free_all();  // get rid of old file list

    // loop until no more dirents
    dirent *file;
    while( (file = readdir(dir)) )
    {
        // filter out "." and ".."
        if( !strcmp(file->d_name,".") ||
            !strcmp(file->d_name,"..") )
            continue;

        // stat file to get its size & type
        string filename = dirname + file->d_name;
        struct stat *stats = new struct stat;
        if( stat(filename.c_str(), stats) < 0 )
            continue;

        // build new DirRec
        DirRec *rec = new DirRec;
        rec->name = file->d_name;
        rec->size = stats->st_size;
        rec->dir  = S_ISDIR(stats->st_mode);
        delete stats;

        add( rec );  // add new DirRec to list
    } // while

    return size();  // successful run
} // DirRead::read()


// Function : bool DirRead::find(const string &)
// Purpose  : Search the current directory list
//            for a file named 'file'.
// Returns  : true  - file named 'file' in list
//            false - 'file' not found
bool DirRead::find(const string &file) const
{
    for(cdllit<DirRec> i(*this); !i.done(); ++i)
        if( file == i()->name )
            return true;

    return false;
} // DirRead::find()


} // namespace blib

// dirread.cxx

