// File     : thread.h
// Purpose  : define Thread class, a wrapper for the POSIX pthread library
//            One needs to link in pthread library, so for example:
//              gcc -o threads threads-demo.c -lpthread
//
// Update Log -
// 20100802 - Began Thread user interface class


// prototypes
namespace blib
{
class  Thread;
} // namespace blib


#ifndef THREAD_CLASS_DEFINITION
#define THREAD_CLASS_DEFINITION


namespace blib
{


/* Linux with glibc:
 *   _REENTRANT to grab thread-safe libraries
 *   _POSIX_SOURCE to get POSIX semantics
 */
#ifdef __linux__
#  define _REENTRANT
#  define _POSIX_SOURCE
#endif
/* Hack for LinuxThreads */
#ifdef __linux__
#  define _P __P
#endif
/*** the two #ifdefs above are in the pthread example for Linux I have, I don't know why. ***/


#include <pthread.h>   // POSIX thread library
#include "blib.h"      // blib global typedefs, defines, etc


// Classes : Thread
// Purpose : A wrapper for the POSIX pthread library.
// Useage  : This class should not be used directly,
//           but only by inheritance.
//           This class only abstracts the creation and
//           destruction of threads.
//           The member function execute() is called by
//           EntryPoint() which is the thread-start point jumped
//           to by pthread_create() which is called by the
//           constructor; so execute() is virtual meant to just
//           be a place holder for the child class' execute()
//           which should be written with the application in mind.
//           On the other hand EntryPoint() is static and only
//           serves the purpose of abstracting the execution
//           start point via a Thread* call to execute(),
//           which is what makes the thread object-wrapper work,
//           execute() should be overloaded by child classes.
//           Further note, the system resources of the thread are
//           allocated by constructor and deallocated by destructor.
//           The destructor calls pthread_join() which causes the
//           calling/parent thread to idel until the child thread
//           (represented by *this) completes execution, this
//           ocurrs when execute() returns.
class Thread
{
    private:
        pthread_t  thread;    // pthread thread struct for *this
        int        errcode;   // pthread_create() return value

    protected:
        // methods
/*
        static void* EntryPoint(void*);
        void run(void);
        virtual void execute(void);  // The child class execution point, overload this!
                                     // Once execute() returns the thread execution is
                                     // terminated. But its resources will not be
                                     // deallocated until the destructor runs, which
                                     // should be called by the parent thread.
*/
    public:
        bool       dead;      // flag to indicate if the thread is running

        // constructor & destructor
        Thread(void*(*)(void*));          // thread created here, pthread_create()
        virtual ~Thread(void);  // thread resources deallocated here, pthread_join()

        // mutators
        int kill(void);         // immediately stop thread execution

        // inspectors
        const int& error_code(void) const { return errcode; };
        bool finished(void) const { return dead; };
}; // class Thread


} // namespace blib

#endif // THREAD_CLASS_DEFINITION

// thread.h

