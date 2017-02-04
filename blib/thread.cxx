// File     : thread.cxx
// Purpose  : function definitions for class Thread
//
// Update Log -
//
// 20100802 - Begun


#include  <iostream>      // for debugging
#include  "thread.h"   // Thread definition


namespace blib
{


//***************************************//
//*       Thread Member Functions       *//
//***************************************//


// Function : Thread::Thread(void)
// Purpose  : Creates a pthread, represented by *this.
Thread::Thread(void*(*EntryPoint)(void*))
{
     dead = false;

//     cout << "\nT::Thread(): this == " << this << ", this->dead == " << this->dead << ", &this->dead == " << &this->dead << flush; 

     errcode = pthread_create(&thread,    //  pthread struct
	                             NULL,    //  default thread attributes
		                   EntryPoint,    //  execution start point
//		           Thread::EntryPoint,    //  execution start point
		                        this);    //  send this pointer so EntryPoint can run this->execute()

    // it would be good to interface will somekind of global exception handling here,
    //  such as printing a non-zero errcode to a system-debug file, etc.
    if( errcode ) dead = true;  // for now just mark the thread still-born
} // Thread::Thread()


// Function : Thread::~Thread()
// Purpose  : Kill *this thread.
//            The descructor should never be called except by the
//            calling thread, not the thread represented by *this
//            or any other b/c the allocation of *this was done
//            by the caller, so this should work okay for multithreading.
//            But pthread_join() actually makes the calling thread
//            idel until the thread for termination exits on
//            its own, so this can hang the program!
Thread::~Thread(void)  // virtual
{
    if( !errcode )  // check that a thread exists to kill
        errcode = pthread_join(thread, NULL);   //  kill thread represened by *this

    // should do some exception handling here for non-zero errcode
} // Thread::~Thread()

/*
// Function : void* Thread::EntryPoint(void*)
// Purpose  : This is the static function wrapper for calling execute().
// Returns  : 0  - always (isn't void just for pthread_create() prototype)
void* Thread::EntryPoint(void* pthis)  // static
{
//    Thread* p = (Thread*)pthis;
      p->execute();
//    p->run();
//    p->dead = true;
//    ((Thread*)pthis)->execute();
    ((Thread*)pthis)->dead = true;
    return 0;  // must return something b/c pthread_create() prototypes void* return value
} // Thread::EntryPoint()

void Thread::run(void)
{
//    execute();
    this->execute();
} // run()


// Function : virtual void Thread::execute(void)
// Purpose  : The activity/code for the thread should be executed here.
//            This function should be overloaded by child classes.
void Thread::execute(void)  // virtual
{
    cout << "\nNOT Counting a walk now.\n";
    // DO NOT terminate your overloaded execute() with pthread_exit()
    //  because then *this->dead will not be set true by EntryPoint()
    // just let the function terminate at code end or with return
} // Thread::execute()
*/

// Function : virtual void Thread::kill(void)
// Purpose  : This is meant to immedately terminate thread *this.
//            This function can be overloaden if child class
//            needs to do something before submitting to kill-request.
int Thread::kill(void)  // virtual
{
    return pthread_cancel(thread);
} // Thread::kill()


} // namespace blib

// thread.cxx

