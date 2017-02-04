/*****************************************************************************/
/*** Software: AMOS v0.01 (c)1998 All Rights Reserved by Bradley B. Custer ***/
/*****************************************************************************/

// File    : dll.h
// Purpose : contains templates for a doublely-linked list ADT
//           dllist is the container class for dllitem elements
//
// Update Log -
//
// 19980228 - Begun
// 19980907 - added free_all(), operator[](), empty()
// 19981009 - added constdllit<T>
// 19981016 - added first() and last()
// 19981022 - added push() and remove_last()
// 19981027 - added sort() with lessthan() and swap()
// 19981117 - added pop()
// 19990503 - added done() for dlliterator & constdllit::finished()
//            changed name of dlliterator to dllit
//            and constdllit to cdllit
// 20090526 - appended #endif comment
// 20120408 - added dllist<T>::add_copy(), pop_delete() and a const ref constructor of dllitem to go with it
// 20120411 - added dllit<T>::remove_delete()
// 20150721 - rewrote all the remove-node methods of dllist to return bools (had returned new length of list but that's ambiguous with 0 for item not-found)
// 20160603 - added dllist<T>::append_and_purge()
// 20160621 - added bool dllit<T>::remove(const T*) and bool dllit<T>::remove_delete(T*)
// 20160630 - changed dllit<T>::at_start() to return true if length==0
// 20160916 - rewrote dllist<T>::match2_in_list(const string&, const string&) as dllist<T>::match_in_list(const string&, const string&)
//            since it a different signature than dllist<T>::match_in_list(const string&), duh!
// 20160917 - added dllist<T>::match_in_list(const double&)


#ifndef DOUBLELY_LINKED_LIST_TEMPLATE
#define DOUBLELY_LINKED_LIST_TEMPLATE


#include <string>
#include <assert.h>  // assert()
#include "blib.h"    // blib defines


using std::string;


namespace blib
{


// prototypes
template <class T> class dllitem;
template <class T> class dllist;
template <class T> class dllit;
template <class T> class cdllit;


// Template : class dllitem
// Purpose  : contains pointer to node value 
//            and next & prior pointers
// Note     : at construction time, 'a' needs to already
//            have been allocated; be careful not to
//            free it latter, or you'll have a dangling
//            pointer for value here
template <class T> class dllitem
{
    private:
        T*           value;  // points to value of this element
        dllitem<T>*  next;
        dllitem<T>*  prior;

        friend class dllist<T>;
        friend class dllit<T>;
        friend class cdllit<T>;

    protected:
        // constructor
        dllitem(T* a) : value(a), next(0), prior(0) {}
        dllitem(const T& a) : next(0), prior(0)
        { value = new T; *value = a; }
}; // template class dllitem


// Template: class dllist
// Purpose : doublely-linked list container for dllitem elements
// Warning : when the list is purged, deleted, or has any node removed
//           the space pointed to by dllitem<T>.value IS NOT DELETED
//           dllist does nothing to the value pointers, it is just
//           a linked list of them, wouldn't matter if they were
//           all zero
//           HOWEVER, a method is provided for deleteding each element
//           in the list, if you actually want to free the space
//           pointed to by the value pointers, free_all()
//           (but note: free for value pointers is not an array delete)
template <class T> class dllist
{
    private:
        dllitem<T> *head;  // pointer to first element
        uint length;       // length of list

        friend class dllit<T>;
        friend class cdllit<T>;

        bool lessthan_opr(const dllitem<T> *, const dllitem<T> *) const;  // uses T::operator<() to decide (a < b)
        bool lessthan_dll(const dllitem<T> *, const dllitem<T> *) const;  // uses T::dll_lessthan() to decide (a < b)
        bool lessthan(const dllitem<T> *, const dllitem<T> *,             // uses the 3rd argument to decide (a < b)
                      bool (*)(const T *, const T *)) const;
        // exhanges list positions of passed & passed->next
        void swap(dllitem<T> *);

    public:
        // constructors
        dllist() : head(0), length(0) {}
        dllist(const dllist<T> &a) : head(0), length(0) { operator=(a); }
        // destructor
        ~dllist() { purge(); }  // this only frees the list points, does not delete values pointed to

        // mutators
        void operator=(const dllist<T> &);   // assign *this to a copy of arguement's list pointers, that point to the same values as the argument's pointers
        void operator+=(const dllist<T> &);  // add copy of argument to end of *this, only ptrs are copied, not values, so can't delete elements of arg without destroying this*
        void operator-=(const dllist<T> &);  // remove elements in arg. from *this
        void copy(const dllist<T> &);        // purge() *this, copy instances from arg, new T objects values built
        void pUnion(const dllist<T> &);      // make *this = *this U argument, new pointers point to argument's values, no new objects build
        void Union(const dllist<T> &);       // make *this = *this U argument, new pointers point to newly built objects copied with T::operator=()
        void append_and_purge(dllist<T> &);  // places arguement's actual pointer list onto end of *this, thus leaving arguement in a purge()d state
        void prepend_and_purge(dllist<T> &); // places arguement's actual pointer list onto beginning of *this, thus leaving arguement in a purge()d state
        void add_list(const dllist<T> &a) { *this += a; }
        void push_list(const dllist<T> &);   // add copy of argument to beginning of *this (alt. to opr+=), only ptrs are copied, not values, so can't delete elements of arg without destroying this*
        uint push(T *);                      // adds new list element/pointer at beginning of list, does NOT build a new T
        uint add(T *);                       // adds new list element/pointer at end of list, does NOT build a new T
        uint add_copy(const T&);             // adds new list element and builds a new T it points to, using T::operator=()
        uint add_num(T *, uint);             // add a node in i-th position, does NOT build a new T
        bool pop(void);                      // removes first element from list, does not delete T objects
        bool pop_delete(void);               // removes first element from list, and DELETES T object it points to
        bool remove(const T*);               // removes first oldvalue from list comparing pointer values, does not delete T objects
        bool remove_delete(T*);              // removes first oldvalue from list comparing pointer values, DOES DELETE T object pointed to by argument!
        bool remove(const T&);               // removes first oldvalue from list comparing with T::operator==(), does not delete any found T object
        bool remove_delete(const T&);        // removes first oldvalue from list comparing with T::operator==(), DOES DELETE any found T object
        bool remove_last(void);              // removes last element from list, does not delete T objects
        bool remove_last_delete(void);       // removes last element from list, and DELETES T object it points to
        bool remove_num(uint);               // removes i-th node from list, does not delete T objects
        bool remove_num_delete(uint);        // removes i-th node from list, and DELETES T object it points to
        void purge(void);                    // removes every element from the list, does not delete T objects
        void free_all(void);                 // deletes every element in the list, DELETES the T objects
        uint sort(void);                     // sorts the list greast-to-least with T::operator<(const T*)
        uint sort_dll(void);                 // sorts the list greast-to-least with T::dll_lessthan(const T*)
        uint sort(bool (*)(const T*, const T*));

        // inspectors
        bool identical(const dllist<T>&) const;    // identity by *value == *value
        bool operator==(const dllist<T>&) const;   // identity by value  == value
        bool operator!=(const dllist<T>& a) const  // identity by value  != value
            { return !(*this == a); }
        bool in_list(const T*) const;              // report if argument is in list, comparing pointer values
        T* ref_in_list(const T&) const;            // return 1st match to argument in list, comparing with T::operator==()
        T* match_in_list(const string&) const;                 // return 1st match to string key in list, comparing with "bool T::matches(const string&) const"
        T* match_in_list(const string&, const string&) const;  // return 1st match to string keys in list, comparing with "bool T::matches(const string&, const string&) const"
        T* match_in_list(const int) const;                     // report if a match to int key is in list, comparing with T::matches(const int)
        T* match_in_list(const uint) const;                    // report if a match to uint key is in list, comparing with T::matches(const uint)
        T* match_in_list(const double) const;                  // return 1st match to double key in list, comparing with "bool T::matches(const double&) const"
        T& operator[](uint) const;                 // return reference to ith node
        T* get_num(const uint) const;              // return pointer to ith node
        T* first(void) const                       // return value of head node
            { if( length ) return head->value; else return 0; }
        T* last(void) const                        // return value of last node
            { if( length ) return head->prior->value; else return 0; }
        T* rand(void) const;                       // return randomly selected node
        void seed(void) const;                     // seed the random generator
        bool empty(void) const                     // report if list is empty
            { if( length ) return false; else return true; }
        uint size(void) const { return length; }    // report size of list
}; // template class dllist


// Template : class dllit
// Purpose  : iterator class for dllist
// Note     : to use dllit a dllist must have been declared,
//            and must have AT LEAST ONE ELEMENT, or ptr will be NULL
template <class T> class dllit
{
    private:
        dllitem<T> *ptr;       // pointer to current iteration
        dllist<T>  &list;      // list being iterated
        bool       step_made;  // flags whether iteration has begun
        uint       i;          // maintains iterative position

        friend class dllist<T>;

    public:
        // constructor
        dllit(dllist<T> &L) : ptr(L.head), list(L), step_made(false), i(0) {}
        dllit(dllist<T> &L, const uint &s)
            : ptr(L.head), list(L), step_made(false), i(0)
            { for(uint  j = 0; j < s; j++) ++(*this); }

        // mutators
        void start(void)           // start iteration over again
            { ptr = list.head; step_made = false; i = 0; }
        void start_at(const uint& s)
            { start(); for(uint  j = 0; j < s; j++) ++(*this); } 
        T *operator++(void)        // increment element being pointed to
            { if(ptr) ptr = ptr->next; step_made = true;
              if(ptr == list.head) i = 0; else i++;
              if(ptr) return ptr->value; return 0; }
        T *operator--(void)        // decrement element being pointed to
            { if(ptr) ptr = ptr->prior; step_made = true;
              if(ptr == list.head->prior) i = list.length - 1; else i--;
              if(ptr) return ptr->value; return 0; }
        T *operator=(T *a)         // assign value of element pointed to
            { if( ptr ) ptr->value = a;
              else { list.add(a); start(); } return a; }
        uint add_before(T *);      // adds node before iterator position
        uint add_after(T *);       // adds node after iterator position
        uint remove(void);         // remove current iteration from list, does NOT DELETE value object
        uint remove_delete(void);  // remove current iteration from list, WILL DELETE value object
        bool remove(const T*);     // removes first oldvalue from list comparing pointer values, does not delete T object; this method should be used when deleting elements while in middle of iteration, so iterator isn't confused about its position, using dllist<T>::remove_delete() could cause seg-fault if what is removed is pointed to by iterator
        bool remove_delete(T*);    // removes first oldvalue from list comparing pointer values, DOES DELETE T object; this method should be used when deleting elements while in middle of iteration, so iterator isn't confused about its position, using dllist<T>::remove_delete() could cause seg-fault if what is removed is pointed to by iterator

        // inspectors
        uint num(void) const       // return iteration position
            { return i; }
        T *operator()(void) const  // inspect value interator is pointing to
            { if(ptr) return ptr->value; return 0; }
        bool at_start(void) const  // is iterator pointing to first element?
            { if( list.length ) return ptr == list.head; return false; }
        bool at_end(void) const    // is iterator pointing to last element?
            { if( list.length ) return ptr == list.head->prior; return true; }
        bool finished(void) const  // has a full list iteration occured ? 
            { if( list.length ) 
              { if( step_made ) return ptr == list.head; else return false; }
              return true; }
        bool done(void) const      // second name for finished()
            { return finished(); }

        // The step_made flag is used within finished() so as to allow for() style
        // pre-test loop iteration with the dllist<T>.
        // Note: It only works with forward, and not backward, iteration;
        // a pre-test loop will not work using at_start() or at_end() as the
        // loop tests (the first will not process any nodes, and the second
        // will never process the last node), hence the finished() test which
        // checks if an iteration has occured and then checks at_start().
        // But if iteration proceeds backward the first list element will be
        // processed first, then the last, then 2nd-to-last, because the
        // constructor sets position to the first node; so if order matters
        // this doesn't work. If the last element must be processed 1st and
        // position is manually initially set to the last element then the first
        // list element will never be processed at all, using finished() for
        // a loop pre-test.
        // finished() can be used in a for() statement as follows:
        // for(dllit<T> i(list); !i.finished(); ++i) ;
        // The alternative, to this for() example, is:
        // if( !list.empty() ) // must pre-verify that the list has nodes
        // {   dllit<T> i(list);
        //     do { .. ++i(); } while( !i.at_start() );
        // }                   // and then use a post-test loop
        // which is rather more cumbersome.
}; // template class dllit


// Template : class cdllit
// Purpose  : const iterator class for dllist
// Note     : to use cdllit a dllist must have been declared,
//            and must have AT LEAST ONE ELEMENT, or ptr will be NULL
template <class T> class cdllit
{
    private:
        dllitem<T> *ptr;       // pointer to current iteration
        dllitem<T> *head;      // pointer to head of list
        bool       step_made;  // flags whether iteration has begun
        uint       i;          // maintains iterative position
        uint       length;     // list length

        friend class dllist<T>;

    public:
        // constructor
        cdllit(const dllist<T> &L)
            : ptr(L.head), head(L.head), step_made(false), i(0), length(L.length) {}
        cdllit(const dllist<T> &L, const uint &s)
            : ptr(L.head), head(L.head), step_made(false), i(0), length(L.length)
            { for(uint  j = 0; j < s; j++) ++(*this); }

        // mutators
        void start(void)           // start iteration over again
            { ptr = head; step_made = false; i = 0; }
        void start_at(const uint& s)
            { start(); for(uint  j = 0; j < s; j++) ++(*this); } 
        T *operator++(void)        // increment element being pointed to
            { if(ptr) ptr = ptr->next; step_made = true;
              if(ptr == head) i = 0; else i++;
              if(ptr) return ptr->value; return 0; }
        T *operator--(void)        // decrement element being pointed to
            { if(ptr) ptr = ptr->prior; step_made = true;
              if(ptr == head->prior) i = length - 1; else i--;
              if(ptr) return ptr->value; return 0; }

        // inspectors
        uint num(void) const       // return iteration position
            { return i; }
        T *operator()(void) const  // inspect value interator is pointing to
            { if(ptr) return ptr->value; return 0; }
        bool at_start(void) const  // is iterator pointing to first element?
            { if( length ) return ptr == head; return false; }
        bool at_end(void) const    // is iterator pointing to last element?
            { if( length ) return ptr == head->prior; return true; }
        bool finished(void) const  // has a full list iteration occured ? 
            { if( length ) 
              { if( step_made ) return ptr == head; else return false; }
              return true; }
        bool done(void) const      // second name for finished()
            { return finished(); }

        // The step_made flag is used within finished() so as to allow for() style
        // pre-test loop iteration with the dllist<T>.
        // Note: It only works with forward, and not backward, iteration;
        // a pre-test loop will not work using at_start() or at_end() as the
        // loop tests (the first will not process any nodes, and the second
        // will never process the last node), hence the finished() test which
        // checks if an iteration has occured and then checks at_start().
        // But if iteration proceeds backward the first list element will be
        // processed first, then the last, then 2nd-to-last, because the
        // constructor sets position to the first node; so if order matters
        // this doesn't work. If the last element must be processed 1st and
        // position is manually initially set to the last element then the first
        // list element will never be processed at all, using finished() for
        // a loop pre-test.
        // finished() can be used in a for() statement as follows:
        // for(cdllit<T> i(list); !i.finished(); ++i) ;
        // The alternative, to this for() example, is:
        // if( !list.empty() ) // must pre-verify that the list has nodes
        // {   cdllit<T> i(list);
        //     do { .. ++i(); } while( !i.at_start() );
        // }                   // and then use a post-test loop
        // which is rather more cumbersome.
}; // template class cdllit


} // namespace blib

#include "dll.cxx"   // included b/c dll is a set of template classes, not compilable itself

#endif // DOUBLELY_LINKED_LIST_TEMPLATE

// dll.h

