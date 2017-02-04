// File     : commandi.cxx
// Purpose  : function definitions for class CommandI
//
// Update Log -
//
// 19990623 - Begun
// 20090527 - changed CommandI::execute() to allow "exit" along with "quit" command
// 20090527 - changed CommandIOpt.opt to take ostream argument


#include  "timer.h"      // time_timer class
#include  "commandi.h"   // CommandI definition


namespace blib
{


//******************************************//
//*        CommandI Member Functions       *//
//******************************************//

/*
// Function : CommandI::CommandI(...)
// Purpose  : To build a new CommandI object, taking
//            only the function pointers from the
//            CommandIOpt array, and the char texts
//            from the DBFile.  The DBFile records
//            are matched to the elements of the
//            array assuming they are already in
//            corrisponding order.  If there are more
//            command records in the DBFile than
//            elements of the array, the remaining
//            records will be ignored.
CommandI::CommandI(CommandIOpt array[], DBFile *file,
  istream &i, ostream &o, const char *p)
  : Commands(array), ins(i), outs(i), prompt(p)
{
} // CommandI::CommandI(()
*/

// Function : void* CommandI::execute(void)
// Purpose  : To execute the interface, prompting for
//            commands, executing function pointers
//            when commands are entered, returning
//            if the user enters "quit".
// Returns  : 0  - run failed, data errored some way
//            1  - successful run, "quit" or EOF termination
//            >1 - successful run, termination by executed
//                 option, return code is value returned
//                 by the executed option -- possibly a
//                 pointer to executable or data
void* CommandI::execute(void)
{
    if( !prompt ) return 0;

    dllist<string> args;

    while( !ins.eof() )
    { // cycle prompt-execute until "quit" entered

        outs << prompt;   // prompt

        // get command
        char input[MAX_COMMANDI_INPUT_SIZE];
        ins.getline(input, MAX_COMMANDI_INPUT_SIZE);

        // check for a comment
        if( input[0] == '#' ) continue;

        // echo input to output, if requested
        if( echo_input ) outs << input << endl;

        // tokenize input
        for(char *tok = strtok(input, " \t");
                  tok;
                  tok = strtok(NULL, " \t") )
            args.add( new string(tok) );

        if( args.empty() ) continue;

        // try matching command with "quit"
        if( !strcasecmp(args.first()->c_str(), "quit") ||
            !strcasecmp(args.first()->c_str(), "exit") )
        {
            args.free_all();
            break;
        } // if

        // try matching command with "echo"
        if( !strcasecmp(args.first()->c_str(), "echo") )
        {
            echo_input = !echo_input;
            if( echo_input )
                outs << "echo\nCommand input will be echoed.\n";
            else
                outs << "Command input will not be echoed.\n";
            outs << endl;
            args.free_all();
            continue;
        } // if

        // try matching command with "time"
        if( !strcasecmp(args.first()->c_str(), "time") )
        {
            log_time = !log_time;
            if( log_time )
                outs << "Command execution will be timed.\n";
            else
                outs << "Command execution will not be timed.\n";
            outs << endl;
            args.free_all();
            continue;
        } // if

        // try matching command with "help"
        if( !strcasecmp(args.first()->c_str(), "help") )
        {
           if( args.size() == 1 )
           { // output command list
                outs << "The following commands are available:\n";
                for(int i = 0; Commands[i].command; i++)
                {
                    if( !Commands[i].desc )
                        continue;
                    int pad_len = 18 - strlen(Commands[i].command);
                    char pad[pad_len];
                    for(int j = 0; j < pad_len; j++) pad[j] = ' ';
                    pad[pad_len] = '\0';
                    outs << Commands[i].command << pad
                         << Commands[i].desc    << endl;
                } // for
            } // if
            else
            { // output long desc for second token
                bool found = false;
                for(int i = 0; Commands[i].command; i++)
                {
                    if( !Commands[i].command || !Commands[i].help )
                        continue;
                    if( !strcasecmp(args[1].c_str(), Commands[i].command) )
                    {
                        outs << Commands[i].help;
                        found = true;
                        break;
                    } // if
                } // for
                if( !found )
                    outs << "No help for that command.\n";
            } // else
            outs << endl;
            args.free_all();
            continue;
        } // if

        // try matching command with option array
        void* ret_value = 0;
        bool found      = false;
        for(int i = 0; Commands[i].opt; i++)
            if( Commands[i].command &&
                !strcasecmp(args.first()->c_str(), Commands[i].command) )
            { // command found, execute it
                time_timer timer;

                ret_value = (*Commands[i].opt)(args, outs);

                if( log_time )
                    outs << "Command executed in "
                         << timer.passed()
                         << " seconds.\n" << endl;

                found = true;
                break;
            } // if

        args.free_all();  // free input tokens

        if( !found )
            outs << "That command is not available."
                 << "\nEnter 'help' for a command list.\n"
                 << endl;
        else
            if( ret_value ) return ret_value;
            // if option returned value, terminate
    } // while

    return (void*) 1;
} // CommandI::execute()


} // namespace blib


// commandi.cxx
