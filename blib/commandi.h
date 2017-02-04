// File     : commandi.h
// Purpose  : define CommandI classes
//
// Update Log -
// 19990623 - Began CommandI user interface class
// 20090526 - appended #endif comment
// 20090527 - changed to 'char*' formatting (from 'char *')
// 20090527 - changed CommandIOpt.opt to take ostream argument


// prototypes
namespace blib
{
struct CommandIOpt;
class  CommandI;
} // namespace blib


#ifndef COMMANDI_CLASS_DEFINITION
#define COMMANDI_CLASS_DEFINITION


#include <string>      // string class
#include <cstring>     // for strcmp()
#include <iostream>    // input/output streams
#include "blib.h"      // blib global typedefs, defines, etc
#include "dll.h"       // doubly linked lists
#include "dbfile.h"    // DBStream, DBStreamRec


namespace blib
{


// the following define sets the limit on the
// number of chars read from the user's command
#define  MAX_COMMANDI_INPUT_SIZE  300


// Struct  : CommandIOpt
// Purpose : contains data for a CommandI option
struct CommandIOpt
{
    const char*  command;    // command name
    const char*  desc;       // one line description of command
    const char*  help;       // help text for command
                             // function to run for command
    void* (*opt)(dllist<string> &, ::ostream&);

    // inspectors
    const char* key(void) const { return command; }
    bool dll_lessthan(const CommandIOpt* a) const
        { return strcmp(command, a->command) < 0; }
}; // struct CommandIOpt


// Classes : CommandI
// Purpose : Represents a command-line prompted user interface,
//           by containing data for the options or commands
//           plus rountines to control or run the prompted
//           interface.  Data for the commands comes from
//           an array of CommandIOpt passed to the CommandI
//           constructor.  The strings within the array
//           do not have to be filled in though, and can be
//           loaded from file alternatively.  However the
//           function pointers to call for the commands
//           must come from the hard coded array.
//           The array is expected to be NULL terminated, that
//           is to say, every member of the last CommandIOpt
//           element of the array should be 0. Also, the first
//           command with opt == 0 will termiante the list of
//           executable user-written commands.
// Note    : The specifics of the interface are as follows:
//
//            a) The user will get a string (passed to the
//               constructor) as a prompt.  This will be
//               printed to the screen whenever the system
//               is ready to execute another command.
//            b) Each command will have a name, a one line
//               description shown in the command list, and a
//               longer description as "online help".  The
//               three strings are 'command', 'desc', and
//               'help' in CommandIOpt, respectively.
//            c) The command list will be displayed if the
//               user types "help" as a command.
//            d) The long description of a command will
//               be displayed if the user types
//               "help [command-name]".
//            e) The interface will return to its caller
//               when the user types "quit" as a command,
//               or EOF found, return value will be 1.
//            f) The inferface will return to its caller
//               when an executed option returns a number
//               != 0; which is then returned to caller.
//            g) The functions listed in the option array
//               need to take a dllist<string>& as the
//               argument.  A non-delete-able list of any
//               arguments to the command will be passed,
//               in the dllist (tokenized by ' ' & '\t'),
//               The first element of the dllist will be
//               the command itself.  This list will be
//               deleted when the option returns.
//            h) Commands are assumed NOT case-sensitive.
//            i) The input and output streams used will be
//               constructor arguments.
//            j) If the first character of an input line
//               is '#' the line will be considered a
//               comment and ignored.
class CommandI
{
    private:
        CommandIOpt*  Commands;  // array of commands
        ::istream&    ins;       // stream to take input from 
        ::ostream&    outs;      // stream to write output to
        const char*   prompt;    // command prompt to use

        bool  log_time;     // log time after every command
        bool  echo_input;   // echo inputs to the output

    public:
        // constructors
        CommandI(CommandIOpt* array, ::istream& i, ::ostream& o, const char* p) 
          : Commands(array), ins(i), outs(o), prompt(p), log_time(false), echo_input(false) {}
        CommandI(CommandIOpt*, DBFile*, ::istream&, ::ostream&, const char*);

        // methods
        void* execute(void); // run the interface
}; // class CommandI


} // namespace blib


#endif // COMMANDI_CLASS_DEFINITION

// commandi.h

