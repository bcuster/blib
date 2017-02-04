/*****************************************************************************/
/*** Software: AMOS v0.01 (c)1998 All Rights Reserved by Bradley B. Custer ***/
/*****************************************************************************/

// File    : dll.cxx
// Purpose : contains template function members of dllist class (dll.h)
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
// 19991106 - added sort(bool (*)(const T *, const T *))
// 19991211 - added dllit<>::add_before() & add_after()
// 20120408 - added dllist<T>::add_copy(), pop_delete() and some comments
// 20120411 - added dllit<T>::remove_delete()
// 20160603 - added dllist<T>::append_and_purge()
// 20160621 - corrected bug in bool dllit<T>::remove(void) and bool dllit<T>::remove_delete(void); iterator position i was not reset if iterator was pointing to last element
// 20160621 - added bool dllit<T>::remove(const T*) and bool dllit<T>::remove_delete(T*)
// 20160701 - changed dllit<T>::remove(_delete)(void) so that step_made flag set True iff *this not initially pointing to head-node
// 20160916 - rewrote dllist<T>::match2_in_list(const string&, const string&) as dllist<T>::match_in_list(const string&, const string&)
//            since it a different signature than dllist<T>::match_in_list(const string&), duh!
// 20160917 - added dllist<T>::match_in_list(const double&)


// doublely-linked list class header
#include <cstdlib>   // for rand() and srand()
#include <ctime>     // for time()
#include "dll.h"


namespace blib
{


// Template : void dllist<T>::purge(void)
// Purpose  : removes every element from the list.
//            but does NOT DELETE the values pointed to
template <class T> void dllist<T>::purge(void)
{
    dllitem<T> *tmp = head, *next;
    while( length-- )
    {
        next = tmp->next;
        delete tmp;
        tmp = next;
    } // while()
    head = 0; length = 0;
} // template dllist<T>::purge()


// Template : void dllist<T>::free_all(void)
// Purpose  : deletes every element in the list
//            that is frees each elements memory
//            plus removes them from the list
// Note     : the freeing of the value pointers
//            will not work if the dllist<..> type
//            is some kind of array, because the
//            '[]' isn't included in value deletes
template <class T> void dllist<T>::free_all(void)
{
    dllitem<T> *tmp = head, *next;
    while( length-- )
    {
        next = tmp->next;
        delete tmp->value;
        delete tmp;
        tmp = next;
    } // while
    head = 0; length = 0;
} // template dllist<T>::free_all()


// Template : dllist<T>::operator=(const dllist<T> &a)
// Purpose  : copy dllist; *this is purge()d and then
//            turned into a duplicate copy of 'a',
//            pointers are copied NOT instances
template <class T>
void dllist<T>::operator=(const dllist<T> &a)
{
    if( length ) purge();  // clear out current nodes
    if( a.length )
    {
        dllitem<T> *tmp = a.head;
        do
        {   assert( add(tmp->value) );
            tmp = tmp->next;
        } while( tmp != a.head );
    } // if
}  // template dllist<T>::operator=()


// Template : dllist<T>::copy(const dllist<T> &a)
// Purpose  : copy dllist; *this is purge()d and then
//            turned into a list with values pointing to
//            duplicates of the values 'a's nodes point to,
//            T::operator=() is used to COPY INSTANCES
template <class T> void dllist<T>::copy(const dllist<T> &a)
{
    if( length ) purge();  // clear out current nodes
    if( a.length )
    {
        dllitem<T> *tmp = a.head;
        do
        {   assert( add_copy(*tmp->value) );
            tmp = tmp->next;
        } while( tmp != a.head );
    } // if
}  // template dllist<T>::copy()


// Template : dllist<T>::operator+=(const dllist<T> &a)
// Purpose  : add argument to end of *this
template <class T>
void dllist<T>::operator+=(const dllist<T> &a)
{
    if( !length )   // if *this has no nodes
    {               //  just copy every node in 'a'
        *this = a;
        return;
    } // if

    // otherwise, starting adding 'a' to end of *this
    if( a.length )
    {
        dllitem<T> *tmp = a.head;
        do
        {   assert( add(tmp->value) );
            tmp = tmp->next;
        } while( tmp != a.head );
    } // if
} // template dllist<T>::operator+=()


// Template : dllist<T>::push_list(const dllist<T> &a)
// Purpose  : add argument to beginning of *this
//            ie. if *this=[1,2,3] and a=[4,5,6]
//            then after execution *this=[4,5,6,1,2,3]
// Note: it is a shallow copy, values from a are not copied, just pointers
template <class T>
void dllist<T>::push_list(const dllist<T> &a)
{
    if( !length )   // if *this has no nodes
    {               //  just copy every node in 'a'
        *this = a;
        return;
    } // if

    // otherwise, starting pushing 'a' to beginning of *this
    if( a.length )
    {
        dllitem<T> *tmp = a.head;
        do
        {   assert( push(tmp->value) );
            tmp = tmp->next;
        } while( tmp != a.head );
    } // if
} // template dllist<T>::push_list()


// Template : dllist<T>::append_and_purge(dllist<T> &a)
// Purpose  : places arguement a's actual pointer list onto end of *this,
//            by just resetting prior/next of last node, so this is a
//            very fast append, doing no copying of pointers or values,
//            but 'a' is left in a purge()d state
template <class T>
void dllist<T>::append_and_purge(dllist<T> &a)
{
    if( !length )   // if *this has no nodes
    {               //  just assign *this' head node to a's
        head   = a.head;
        length = a.length;
    } // if
    else
    if( a.length )
    {               // otherwise, sew 'a' in at end of *this
        head->prior->next   = a.head;         // reset *this's old last's next (now it's in the middle)
        a.head->prior->next = head;           // reset a's last's next (now it's *this's last)
        dllitem<T> *tmp     = head->prior;    // temporaily store *this's old last, to keep track of new middle
        head->prior         = a.head->prior;  // reset *this's new last to a's last
        a.head->prior       = tmp;            // reset a's head back-ptr to *this's old last, the new middle

        length += a.length;
    } // else if
    
    a.head   = NULL;
    a.length = 0;
} // template dllist<T>::append_and_purge()


// Template : dllist<T>::prepend_and_purge(dllist<T> &a)
// Purpose  : places arguement a's actual pointer list onto beginning of *this,
//            by just resetting prior/next of last node, so this is a
//            very fast prepend, doing no copying of pointers or values,
//            but 'a' is left in a purge()d state
template <class T>
void dllist<T>::prepend_and_purge(dllist<T> &a)
{
    if( !length )   // if *this has no nodes
    {               //  just assign *this' head node to a's
        head   = a.head;
        length = a.length;
    } // if
    else
    if( a.length )
    {               // otherwise, sew 'a' in at end of *this
        head->prior->next   = a.head;         // reset *this's old last's next (now it's in the middle)
        a.head->prior->next = head;           // reset a's last's next (now it's *this's last)
        dllitem<T> *tmp     = head->prior;    // temporaily store *this's old last, to keep track of new middle
        head->prior         = a.head->prior;  // reset *this's new last to a's last
        a.head->prior       = tmp;            // reset a's head back-ptr to *this's old last, the new middle

        head    = a.head;  // this is the only line different from append_and_purge() b/c it's a dll, so only difference between adding at front or back is just where you point the new head to
        length += a.length;
    } // else if

    a.head   = NULL;
    a.length = 0;
} // template dllist<T>::prepend_and_purge()


// Template : dllist<T>::operator-=(const dllist<T> &a)
// Purpose  : remove any elements in 'a' found in *this
//            from *this; the procedure compares pointers
//            and NOT instances
template <class T>
void dllist<T>::operator-=(const dllist<T> &a)
{
    // if *this or 'a' has no nodes
    if( !length || !a.length)  
        return;    // nothing to do

    // otherwise, iterate 'a' looking for matches in *this
    for(cdllit<T> i(a); !i.finished(); ++i)
        remove( i() );

    // Note: because remove() only removes the first
    //  node found which has a value pointer equal to its
    //  argument, we do not account for duplicates here
} // template dllist<T>::operator-=()


// Template : dllist<T>::pUnion(const dllist<T> &a)
// Purpose  : makes *this the Set Union of *this and 'a'
//            comparing the node value pointers, rather
//            then the actual value instances (like Union())
//            the resulting list (*this) will have new nodes
//            pointing to THE SAME values pointed to by 'a's
//            nodes, which where not originally found in *this
// Note     : I'm certain the algorithm used here
//            could be improved
template <class T>
void dllist<T>::pUnion(const dllist<T> &a)
{
    if( !length )                       // if *this has no nodes
    {                                   // just copy every node in 'a'
        *this = a;
        return;
    } // if

    if( a.length )                      // otherwise, verify 'a' has nodes
    {                                   //  and copy over the unqine ones
        dllitem<T> *tmp = a.head;
        do
        {   if( !in_list( tmp->value ) )
                assert( add(tmp->value) );
            tmp = tmp->next;
        } while( tmp != a.head );
    } // if
} // template dllist<T>::pUnion()


// Template : dllist<T>::Union(const dllist<T> &a)
// Purpose  : makes *this the Set Union of *this and 'a'
//            comparing with T::operator==()
// Note     : the modified *this (the result) will
//            contain COPIES of duplicate values found
//            in 'a', so 'a' and its values can be
//            freed without damaging the result
//            the COPIES WILL BE MADE using T::operator=()
template <class T>
void dllist<T>::Union(const dllist<T> &a)
{
    if( !length )                       // if *this has no nodes
    {                                   // just copy every node in 'a'
        copy(a);
        return;
    } // if

    if( a.length )                      // otherwise, verify 'a' has nodes
    {                                   //  and copy over the unqine ones
        dllitem<T> *tmp = a.head;
        do
        {   if( !ref_in_list( *tmp->value ) )
            {
                T *another = new T;
                *another = *tmp->value;
                assert( add(another) );
            } // if
            tmp = tmp->next;
        } while( tmp != a.head );
    } // if
} // template dllist<T>::Union()


// Template : void dllist<T>::sort(void)
// Purpose  : sorts the list by sequentially comparing
//            each node with T::operator<(const T *)
//            this is a special routine which type T
//            must have in order to use this procedure
//            operator<() is expected to return a bool
//            value of 'true' if *this is < its arguemnt
// Returns  : number of comparisons performed for sort
// Note1    : list node values will be NULL checked before
//            attempting use of operator<(), if a nodes
//            value is 0, it is treated as least in list
// Note2    : the algorithm runs in time N^2-2N+2 or O(N^2)
//            at its worst case (when elements start in
//            reverse order), but is O(N) in the best case
template <class T> uint dllist<T>::sort(void)
{
    if( !length )     // if *this has no nodes
        return 0;     //  there's nothing to do

    uint comparisons = 0;

    dllitem<T> *steper = head;
    while( steper != head->prior )
    {
        // perform initial comparison of steped-to node
        //  and the node after it
        comparisons++;
        if( lessthan_opr(steper, steper->next) )
        {
            // exhange positions of the node steper points
            //  to and the node after it
            swap( steper );

            // assign pointer for the nested loop
            dllitem<T> *swaper = steper->prior;

            // nested loop
            // swap swaper back until '<' true
            while( swaper != head )
            {
                comparisons++;
                bool lesstrue = lessthan_opr(swaper, swaper->prior);

                if( !lesstrue )
                    swap( swaper->prior );
                else  // jump from loop as soon as
                    break;  // lessthan_opr() true
            } // while
        } // if
        else // increment comparison node
            steper = steper->next;
    } // while

    return comparisons;
} // template dllist<T>::sort()


// Template : bool dllist<T>::lessthan_opr(a,b)
// Purpose  : uses the T::operator<() member function of
//            type T to decide if 'a' is less than 'b'
//            if one of the dllist<T> values is zero
//            it is considered less than the other
template <class T> bool dllist<T>::lessthan_opr
  (const dllitem<T> *a, const dllitem<T> *b) const
{
    if( !a->value && !b->value )
        return false;  // a == b

    if( !a )
        return true;   // a < b

    if( !b )
        return false;  // a > b

    return *a->value < *b->value;
} // dllist<T>::lessthan_opr


// Template : void dllist<T>::sort_dll(void)
// Purpose  : sorts the list by sequentially comparing
//            each node with T::dll_lessthan(const T *)
//            this is a special routine which type T
//            must have in order to use this procedure
//            dll_lessthan() is expected to return a bool
//            value of 'true' if *this is < its arguemnt
// Returns  : number of comparisons performed for sort
// Note1    : list node values will be NULL checked before
//            attempting use of dll_lessthan(), if a nodes
//            value is 0, it is treated as least in list
// Note2    : the algorithm runs in time N^2-2N+2 or O(N^2)
//            at its worst case (when elements start in
//            reverse order), but is O(N) in the best case
template <class T> uint dllist<T>::sort_dll(void)
{
    if( !length )     // if *this has no nodes
        return 0;     //  there's nothing to do

    uint comparisons = 0;

    dllitem<T> *steper = head;
    while( steper != head->prior )
    {
        // perform initial comparison of steped-to node
        //  and the node after it
        comparisons++;
        if( lessthan_dll(steper, steper->next) )
        {
            // exhange positions of the node steper points
            //  to and the node after it
            swap( steper );

            // assign pointer for the nested loop
            dllitem<T> *swaper = steper->prior;

            // nested loop
            // swap swaper back until '<' true
            while( swaper != head )
            {
                comparisons++;
                bool lesstrue = lessthan_dll(swaper, swaper->prior);

                if( !lesstrue )
                    swap( swaper->prior );
                else  // jump from loop as soon as
                    break;  // lessthan_dll() true
            } // while
        } // if
        else // increment comparison node
            steper = steper->next;
    } // while

    return comparisons;
} // template dllist<T>::sort_dll()


// Template : bool dllist<T>::lessthan_dll(a,b)
// Purpose  : uses the dll_lessthan() member function of
//            type T to decide if 'a' is less than 'b'
//            if one of the dllist<T> values is zero
//            it is considered less than the other
template <class T> bool dllist<T>::lessthan_dll
  (const dllitem<T> *a, const dllitem<T> *b) const
{
    if( !a->value && !b->value )
        return false;  // a == b

    if( !a )
        return true;   // a < b

    if( !b )
        return false;  // a > b

    return a->value->dll_lessthan(b->value);
} // dllist<T>::lessthan_dll


// Template : void dllist<T>::sort(bool (*)())
// Purpose  : Sorts the list by sequentially comparing
//            each node with the comparison functoin,
//            passed as the argument.
//            The function pointed to by 'lt', must
//            return a bool, and take two T*.
// Returns  : number of comparisons performed for sort
// Note1    : list node values will be NULL checked before
//            attempting use of lt(), if a nodes
//            value is 0, it is treated as least in list
// Note2    : the algorithm runs in time N^2-2N+2 or O(N^2)
//            at its worst case (when elements start in
//            reverse order), but is O(N) in the best case
template <class T>
uint dllist<T>::sort(bool (*lt)(const T *, const T *))
{
    if( !length )     // if *this has no nodes
        return 0;     //  there's nothing to do

    uint comparisons = 0;

    dllitem<T> *steper = head;
    while( steper != head->prior )
    {
        // perform initial comparison of steped-to node
        //  and the node after it
        comparisons++;
        if( lessthan(steper, steper->next, lt) )
        {
            // exhange positions of the node steper points
            //  to and the node after it
            swap( steper );

            // assign pointer for the nested loop
            dllitem<T> *swaper = steper->prior;

            // nested loop
            // swap swaper back until '<' true
            while( swaper != head )
            {
                comparisons++;
                bool lesstrue = lessthan(swaper, swaper->prior, lt);

                if( !lesstrue )
                    swap( swaper->prior );
                else  // jump from loop as soon as
                    break;  // lessthan() true
            } // while
        } // if
        else // increment comparison node
            steper = steper->next;
    } // while

    return comparisons;
} // template dllist<T>::sort(bool (*))


// Template : bool dllist<T>::lessthan(a,b, bool (*))
// Purpose  : uses the 'lt' function to decide if 'a'
//            is less than 'b'
//            if one of the dllist<T> values is zero
//            it is considered less than the other
template <class T> bool dllist<T>::lessthan
  (const dllitem<T> *a, const dllitem<T> *b,
   bool (*lt)(const T *, const T *)) const
{
    if( !a->value && !b->value )
        return false;  // a == b

    if( !a )
        return true;   // a < b

    if( !b )
        return false;  // a > b

    return (*lt)(a->value, b->value);
} // dllist<T>::lessthan


// Template : void dllist<T>::swap(ptr)
// Purpose  : swaps list positions of 'ptr' and 'ptr->next'
template <class T> void dllist<T>::swap(dllitem<T> *ptr)
{
    if( head == ptr )        // check if head needs
        head = ptr->next;    //  reseting
    dllitem<T> *tmp   = ptr->prior;
    tmp->next         = ptr->next;
    ptr->prior        = ptr->next;
    ptr->next         = ptr->next->next;
    ptr->prior->next  = ptr;
    ptr->prior->prior = tmp;
    ptr->next->prior  = ptr;
} // dllist<T>::swap


// Template : dllist<T>::operator==(const dllist<T> &a)
// Purpose  : test for identity of two lists, conversing order
//            the test will compare this[].value == a[].value
//            ie. the value pointers will be compared and
//            no call to an operator==() for type T will be made
// Note     : identical() preforms same test using T::operator==()
template <class T>
bool dllist<T>::operator==(const dllist<T> &a) const
{
    if( length != a.length ) return false;
    if( length )
    {   dllitem<T> *pnt   = head;
        dllitem<T> *a_pnt = a.head;
        do
        {   if( pnt->value != a_pnt->value )
                return false;
            pnt   = pnt->next;
            a_pnt = a_pnt->next;
        } while( pnt != head );
    } // if()
    return true;  // lists are identical
}  // template dllist<T>::operator==()


// Template : dllist<T>::identical(const dllist<T> &a)
// Purpose  : test for identity of two lists, conserving order
//            the test will compare *this[].value == *a[].value
//            ie. the value pointers will be compared using
//            a call to an operator==() for type T
// Note     : operator==() preforms same test without using 
//            T::operator==()
template <class T>
bool dllist<T>::identical(const dllist<T> &a) const
{
    if( length != a.length ) return false;
    if( length )
    {   dllitem<T> *pnt   = head;
        dllitem<T> *a_pnt = a.head;
        do
        {   if( !(*pnt->value == *a_pnt->value) )
                return false;
            pnt   = pnt->next;
            a_pnt = a_pnt->next;
        } while( pnt != head );
    } // if()
    return true;  // lists are identical
}  // template dllist<T>::identical()


// Template : uint dllist<T>::add(T *newvalue)
// Purpose  : add newvalue at the end of the linked list
//            only adds list element pointing to arguement,
//            does NOT build a new copy of T
// Returns  : 0 if allocation failed for new node
//            new legnth of list otherwise
template <class T> uint dllist<T>::add(T *newvalue)
{
    dllitem<T> *tmp = new dllitem<T>(newvalue);
    if( !tmp ) return( 0 ); // memory allocation failed
    if( length == 0 )
    {   // if this is the first element,  set it to head
        head       = tmp;
        head->next = tmp;
    } // if
    else
    {   // else add to end
        tmp->prior        = head->prior;
        head->prior->next = tmp;
        tmp->next         = head;
    } // else
    head->prior = tmp;  // set the rear to the new end
    return ++length;
}  // template dllist<T>::add()


// Template : uint dllist<T>::add_copy(T *newvalue)
// Purpose  : add newvalue at the end of the linked list
//            and builds a new T to be pointed to,
//            the new T gets its value from T::opertor=()
// Returns  : 0 if allocation failed for new node
//            new legnth of list otherwise
template <class T> uint dllist<T>::add_copy(const T& newvalue)
{
    dllitem<T> *tmp = new dllitem<T>(newvalue);
    if( !tmp ) return( 0 ); // memory allocation failed
    if( length == 0 )
    {   // if this is the first element,  set it to head
        head       = tmp;
        head->next = tmp;
    } // if
    else
    {   // else add to end
        tmp->prior        = head->prior;
        head->prior->next = tmp;
        tmp->next         = head;
    } // else
    head->prior = tmp;  // set the rear to the new end
    return ++length;
}  // template dllist<T>::add_copy()


// Template : uint dllist<T>::push(T *newvalue)
// Purpose  : add newvalue at beginning of the linked list
// Returns  : 0 if allocation failed for new node
//            new legnth of list otherwise
template <class T> uint dllist<T>::push(T *newvalue)
{
    if( !add( newvalue ) )
        return 0;  // allocation failed
    // adding to the front of a doubly linked list is
    //  just the same as adding to its end, we just
    //  reset the head pointer after adding
    head = head->prior;
    return length;
}  // template dllist<T>::push()


// Template : uint dllist<T>::add_num(T *, int)
// Purpose  : add newvalue at num-th position of the list
//            the old num-th node is moved -down- in the
//            list, so if 'num' is 3 'newvalue' will
//            take position 3, and the previous position
//            3 node will take position 4
//            'num' is taken to be zero starting
// Returns  : 0 if allocation failed for new node
//            new legnth of list otherwise
// Note     : if num <= 0, newvalue becomes head
//            if num >= length, newvalue becomes end
template <class T>
uint dllist<T>::add_num(T *newvalue, uint num)
{
    dllitem<T> *tmp = new dllitem<T>(newvalue);
    if( !tmp ) return( 0 ); // memory allocation failed
    if( length == 0 )
    { // if this is the first element,  set it to head
        head        = tmp;
        head->next  = tmp;
        head->prior = tmp;  // set new rear
    } // if
    else if( num >= length )
    { // add to end
        tmp->prior        = head->prior;
        head->prior->next = tmp;
        tmp->next         = head;
        head->prior       = tmp;  // set new rear
    } // if
    else 
    { // add at num-th position
        // iterate to the num-th node
        dllitem<T> *old = head;
        for(uint i = 0; i < num; i++)
            old = old->next;
        if( old == head ) head = tmp;
        // reassign pointers, to add
        //  newvalue before old
        old->prior->next = tmp;
        tmp->prior       = old->prior;
        old->prior       = tmp;
        tmp->next        = old;
    } // if
    return ++length;
}  // template dllist<T>::add_num()


// Template : bool dllist<T>::remove(const T *oldvalue)
// Purpose  : removes first oldvalue from the linked list
//            does NOT DELETE the T objected pointed to
// Returns  : FALSE if node with value == oldvalue wasn't found
//            TRUE otherwise
template <class T>
bool dllist<T>::remove(const T *oldvalue)
{
    if( length )
    {   dllit<T> i(*this);
        do
        {   if( i() == oldvalue )
            {
                i.remove();
                return true;
            } // if
            ++i;
        } while( !i.at_start() );
    } // if

    return false;  // oldvalue wasn't found
}  // template dllist<T>::remove()


// Template : bool dllist<T>::remove_delete(const T *oldvalue)
// Purpose  : removes first oldvalue from the linked list
//            does DELETE the T objected pointed to
// WARNING  : the T object pointed to by the arguement given
//            by the caller is DELETED (if found), & ptr=0
// Returns  : FALSE if node with value == oldvalue wasn't found
//            TRUE otherwise
template <class T>
bool dllist<T>::remove_delete(T *oldvalue)
{
    if( length )
    {   dllit<T> i(*this);
        do
        {   if( i() == oldvalue )
            {
                delete i();
                oldvalue = 0;
                i.remove();
                return true;
            } // if
            ++i;
        } while( !i.at_start() );
    } // if

    return false;  // oldvalue wasn't found
}  // template dllist<T>::remove_delete()


// Template : bool dllist<T>::remove(const T& oldvalue)
// Purpose  : removes first oldvalue from the linked list,
//            compareing with operator==()
//            does NOT DELETE the T object found
// Returns  : FALSE if node with *i() == oldvalue wasn't found
//            TRUE otherwise
template <class T>
bool dllist<T>::remove(const T& oldvalue)
{
    if( length )
    {   dllit<T> i(*this);
        do
        {   if( *i() == oldvalue )
            {
                i.remove();
                return true;
            } // if
            ++i;
        } while( !i.at_start() );
    } // if

    return false;  // oldvalue wasn't found
}  // template dllist<T>::remove()


// Template : bool dllist<T>::remove(const T& oldvalue)
// Purpose  : removes first oldvalue from the linked list,
//            compareing with operator==()
//            DOES DELETE the T object first found
// Returns  : FALSE if node with *i() == oldvalue wasn't found
//            TRUE otherwise
template <class T>
bool dllist<T>::remove_delete(const T& oldvalue)
{
    if( length )
    {   dllit<T> i(*this);
        do
        {   if( *i() == oldvalue )
            {
                delete i();
                i.remove();
                return true;
            } // if
            ++i;
        } while( !i.at_start() );
    } // if

    return false;  // oldvalue wasn't found
}  // template dllist<T>::remove_delete()


// Template : bool dllist<T>::remove_num(uint)
// Purpose  : removes num-th node from the linked list
//            num is assumed zero starting
//            does NOT DELETE the T objected pointed to
// Returns  : FALSE if num >= length or num < 0
//            TRUE otherwise
template <class T>
bool dllist<T>::remove_num(uint num)
{
    if( !length || num < 0 || num >= length )
        return false;  // out of bounds

    // iterate to num-th position
    dllitem<T> *tmp = head;
    for(uint i = 0; i < num; i++)
        tmp = tmp->next;

    // reassign pointers to remove tmp
    tmp->prior->next = tmp->next;
    tmp->next->prior = tmp->prior;
    // check if head is being removed
    if( head == tmp )
        head = tmp->next;
    delete tmp; // delete old node

    length--;
    return true;
}  // template dllist<T>::remove_num()


// Template : bool dllist<T>::remove_num_delete(uint)
// Purpose  : removes num-th node from the linked list
//            num is assumed zero starting
//            DELETEs the T objected pointed to
// Returns  : FALSE if num >= length or num < 0
//            TRUE otherwise
template <class T>
bool dllist<T>::remove_num_delete(uint num)
{
    if( !length || num < 0 || num >= length )
        return false;  // out of bounds

    // iterate to num-th position
    dllitem<T> *tmp = head;
    for(uint i = 0; i < num; i++)
        tmp = tmp->next;

    // reassign pointers to remove tmp
    tmp->prior->next = tmp->next;
    tmp->next->prior = tmp->prior;
    // check if head is being removed
    if( head == tmp )
        head = tmp->next;
    delete tmp->value;     // delete T object
    delete tmp;            // delete old node

    length--;
    return true;
}  // template dllist<T>::remove_num_delete()


// Template : bool dllist<T>::remove_last(void)
// Purpose  : remove the end of the linked list
//            does NOT DELETE the T objected pointed to
// Returns  : TRUE if successful, and last removed
//            FALSE if there was nothing in the list
template <class T>
bool dllist<T>::remove_last(void)
{
    // nothing to remove?
    if( length == 0 ) return false;

    // otherwise remove head->prior
    dllitem<T> *tmp  = head->prior;
    tmp->prior->next = head;
    head->prior      = tmp->prior;  // set the rear node
    delete tmp;                     // free link node

    length--;
    return true;
}  // template dllist<T>::remove_last()


// Template : bool dllist<T>::remove_last_delete(void)
// Purpose  : remove the end of the linked list
//            DELETEs the T objected pointed to
// Returns  : TRUE if successful, and last removed
//            FALSE if there was nothing in the list
template <class T>
bool dllist<T>::remove_last_delete(void)
{
    // nothing to remove?
    if( length == 0 ) return false;

    // otherwise remove head->prior
    dllitem<T> *tmp  = head->prior;
    tmp->prior->next = head;
    head->prior      = tmp->prior;  // set the rear node
    delete tmp->value;              // free T object
    delete tmp;                     // free link node

    length--;
    return true;
}  // template dllist<T>::remove_last_delete()


// Template : bool dllist<T>::pop(void)
// Purpose  : remove the head of the linked list
// Returns  : TRUE if successful, and head removed
//            FALSE if there was nothing in the list
template <class T> bool dllist<T>::pop(void)
{
    // nothing to remove?
    if( length == 0 ) return false;

    // otherwise remove head
    head->prior->next = head->next;   // set tail-head link
    head->next->prior = head->prior;  // set the rear node
    dllitem<T> *tmp   = head;
    head              = head->next;   // reset head node
    delete tmp;                       // free link node

    length--;
    return true;
}  // template dllist<T>::pop()


// Template : bool dllist<T>::pop_delete(void)
// Purpose  : remove the head of the linked list
//            and DELETES the T objected pointed to
// Returns  : TRUE if successful, and head removed
//            FALSE if there was nothing in the list
template <class T> bool dllist<T>::pop_delete(void)
{
    // nothing to remove?
    if( length == 0 ) return false;

    // otherwise remove head
    head->prior->next = head->next;   // set tail-head link
    head->next->prior = head->prior;  // set the rear node
    dllitem<T> *tmp   = head;
    head              = head->next;   // reset head node
    delete tmp->value;                // free T object
    delete tmp;                       // free link node

    length--;
    return true;
}  // template dllist<T>::pop_delete()


// Template : bool dllist<T>::in_list(const T *a)
// Purpose  : reports whether 'a' is in the list
//            comparing pointer values, not references
// Returns  : true  if 'a' is found
//            false if 'a' is not found
template <class T>
bool dllist<T>::in_list(const T *a) const
{
    if( length )
        for(cdllit<T> i(*this); !i.finished(); ++i)
            if( i() == a ) return true;

    return false;  // 'a' wasn't found
}  // template dllist<T>::in_list()


// Template : T *dllist<T>::ref_in_list(const T &a)
// Purpose  : reports whether 'a' is in the list
//            using T::operator==() for compares
// Returns  : pointer to 'a' value in *this
//            0 if not found
template <class T>
T *dllist<T>::ref_in_list(const T &a) const
{
    if( length )
        for(cdllit<T> i(*this); !i.finished(); ++i)
            if( *i() == a ) return i();

    return 0;  // 'a' wasn't found
}  // template dllist<T>::ref_in_list()


// Template : T *dllist<T>::match_in_list(const string& str)
// Purpose  : reports whether a match is in the list
//            using T::matches(const string& str) to compare
// Returns  : pointer to first matched value in *this
//            0 if not found
template <class T>
T *dllist<T>::match_in_list(const string& str) const
{
    if( length )
        for(cdllit<T> i(*this); !i.finished(); ++i)
            if( i()->matches(str) ) return i();

    return 0;  // match wasn't found
}  // template dllist<T>::match_in_list()


// Template : T *dllist<T>::match_in_list(const string& str1, const string& str2)
// Purpose  : reports whether a match is in the list
//            using T::matches(const string& str1, const string& str2) to compare
// Returns  : pointer to first matched value in *this
//            0 if not found
template <class T>
T *dllist<T>::match_in_list(const string& str1, const string& str2) const
{
    if( length )
        for(cdllit<T> i(*this); !i.finished(); ++i)
            if( i()->matches(str1, str2) ) return i();

    return 0;  // match wasn't found
}  // template dllist<T>::match_in_list()


// Template : T *dllist<T>::match_in_list(const int num)
// Purpose  : reports whether a match is in the list
//            using T::matches(const int num) to compare
// Returns  : pointer to matched value in *this
//            0 if not found
template <class T>
T *dllist<T>::match_in_list(const int num) const
{
    if( length )
        for(cdllit<T> i(*this); !i.finished(); ++i)
            if( i()->matches(num) ) return i();

    return 0;  // match wasn't found
}  // template dllist<T>::match_in_list()


// Template : T *dllist<T>::match_in_list(const uint num)
// Purpose  : reports whether a match is in the list
//            using T::matches(const uint num) to compare
// Returns  : pointer to matched value in *this
//            0 if not found
template <class T>
T *dllist<T>::match_in_list(const uint num) const
{
    if( length )
        for(cdllit<T> i(*this); !i.finished(); ++i)
            if( i()->matches(num) ) return i();

    return 0;  // match wasn't found
}  // template dllist<T>::match_in_list()


// Template : T *dllist<T>::match_in_list(const double num)
// Purpose  : reports whether a match is in the list
//            using T::matches(const double& num) to compare
// Returns  : pointer to first matched value in *this
//            0 if not found
template <class T>
T *dllist<T>::match_in_list(const double num) const
{
    if( length )
        for(cdllit<T> i(*this); !i.finished(); ++i)
            if( i()->matches(num) ) return i();

    return 0;  // match wasn't found
}  // template dllist<T>::match_in_list()


// Template: T &dllist<T>::operator[](const uint)
// Purpose: return reference to the ith element of the list
//          reutrns 0 if i address beyond number of nodes
//          i is assmued to be zero starting
template <class T> T &dllist<T>::operator[](const uint i) const
{
    if( length < 1 || i >= length || i < 0 ) return *(T *)0;

    dllitem<T> *ptr = head;
    for(uint j = 0; j < i; j++) ptr = ptr->next;

    return *ptr->value;
}  // template dllist<T>::operator[]()


// Template: T *dllist<T>::get_num(const uint) const
// Purpose: return pointer to the ith element of the list
//          reutrns 0 if i address beyond number of nodes
//          i is assmued to be zero starting
template <class T> T *dllist<T>::get_num(const uint i) const
{
    if( length < 1 || i >= length || i < 0 ) return 0;

    dllitem<T> *ptr = head;
    for(uint j = 0; j < i; j++) ptr = ptr->next;

    return ptr->value;
}  // template dllist<T>::get_num()


// Template : T *dllist<T>::rand(void)
// Purpose  : to return a randomly selected node value
//            uses rand() from stdlib.h to select
//            a node to return the value of from *this
// Note     : the random generator needs to have been
//            seeded by srand() BEFORE calling rand()
//            calling dllist<T>::seed() will seed it
//            with the current microsecond
// Returns  : pointer to value of randomly selected node
//            0 if no nodes in list
template <class T> T *dllist<T>::rand(void) const
{
    if( !length ) return 0;  // saftey chk

    long pick = std::rand() % length;

    dllitem<T> *ptr = head;
    for(long i = 0; i < pick; i++)
        ptr = ptr->next;

    return ptr->value;
}  // template dllist<T>::rand()


// Template : T *dllist<T>::seed(void)
// Purpose  : seeds the rand() (stdlib.h) number
//            generator by the microseconds obtained
// Note     : this only needs to be called once to begin
//            using dllist<T>::rand() for the entire
//            duration of a program, for all list types
template <class T> void dllist<T>::seed(void) const
{ // seed it off the system clock
    srand(time(NULL));
} // template dllist<T>::seed()


// Template : uint dllit<T>::remove(void)
// Purpose  : removes the node which iterator is pointing to,
//            does NOT DELETE the object of iterator's node position
// Returns  : 0 if list is empty, or if empty as result
//            new legnth of list otherwise
// Notes    : new iterator set to the node AFTER the one removed
//            if iterator points to head, new head will be
//            the next node too
//            step_made flag is set True iff *this' ptr (iteration
//            pointer) is not intially pointing to head node
// Note     : If using dllit<T>::remove(_delete)() in middle
//            of loop iteration, one must be careful of how iterator
//            is positioned/affected by remove, particularly
//            because if head-node is removed the new iteration
//            pointer points to new head-node, which is a termination
//            condition (done()==true) if step_made flag is True,
//            which will be the case if ++i fires. In that case,
//            loop will terminate after 1 iteration pass.
//            Ex: for(dllit<T> i(list); !i.done(); ++i)
//                  if( -something- ) i.remove(_delete)();
//            will have this early-termination bug.
//            The solution is to flag when remove was done, so
//            as to check for when not to increment, since
//            incrementing is redundant at that point.
//            Ex: for(dllit<T> i(list); !i.done();) {
//                  bool remove_happened = false;
//                  if( -something- ) { i.remove(_delete)(); remove_happened = true; }
//                  if(!remove_happened) ++i; }
template <class T> uint dllit<T>::remove(void)
{
    // check for empty list
    if( !list.length ) return( 0 );
    // make holder of next node address
    dllitem<T> *tmp = ptr->next;
    // if removing head, then new head will be previous 2nd node
    bool resetting_head = false;
    if( ptr == list.head ) { list.head = tmp; resetting_head = true; }
    // relink & free memory
    ptr->prior->next = ptr->next;
    ptr->next->prior = ptr->prior;
    delete ptr;
    ptr = tmp;
    // if list has been reduced to 0 length, then 0 pointers
    if( !(--list.length) ) { list.head = ptr = 0; i = 0; }
    else if( i >= list.length ) { i = 0; }  // if iterator was at last element, it now needs to be reassigned to beginning
    if(!resetting_head) step_made = true;
    return( list.length );
} // template dllit<T>::remove()


// Template : uint dllit<T>::remove_delete(void)
// Purpose  : removes the node which iterator is pointing to,
//            it WILL DELETE the object of iterator's node position
// Returns  : 0 if list is empty, or if empty as result
//            new legnth of list otherwise
// Notes    : new iterator set to the node AFTER the one removed
//            if iterator points to head, new head will be
//            the next node too
//            step_made flag is set True iff *this' ptr (iteration
//            pointer) is not intially pointing to head node
// Note     : If using dllit<T>::remove(_delete)() in middle
//            of loop iteration, one must be careful of how iterator
//            is positioned/affected by remove, particularly
//            because if head-node is removed the new iteration
//            pointer points to new head-node, which is a termination
//            condition (done()==true) if step_made flag is True,
//            which will be the case if ++i fires. In that case,
//            loop will terminate after 1 iteration pass.
//            Ex: for(dllit<T> i(list); !i.done(); ++i)
//                  if( -something- ) i.remove(_delete)();
//            will have this early-termination bug.
//            The solution is to flag when remove was done, so
//            as to check for when not to increment, since
//            incrementing is redundant at that point.
//            Ex: for(dllit<T> i(list); !i.done();) {
//                  bool remove_happened = false;
//                  if( -something- ) { i.remove(_delete)(); remove_happened = true; }
//                  if(!remove_happened) ++i; }
template <class T> uint dllit<T>::remove_delete(void)
{
    // check for empty list
    if( !list.length ) return( 0 );
    // make holder of next node address
    dllitem<T> *tmp = ptr->next;
    // if removing head, then new head will be previous 2nd node
    bool resetting_head = false;
    if( ptr == list.head ) { list.head = tmp; resetting_head = true; }
    // relink & free memory
    ptr->prior->next = ptr->next;
    ptr->next->prior = ptr->prior;
    delete ptr->value;
    delete ptr;
    ptr = tmp;
    // if list has been reduced to 0 length, then 0 pointers
    if( !(--list.length) ) { list.head = ptr = 0; i = 0; }
    else if( i >= list.length ) { i = 0; }  // if iterator was at last element, it now needs to be reassigned to beginning
    if(!resetting_head) step_made = true;
    return( list.length );
} // template dllit<T>::remove_delete()


// Template : bool dllit<T>::remove(const T *oldvalue)
// Purpose  : removes first oldvalue from list comparing pointer values,
//            does not delete T object
//            this method should be used when deleting elements while in
//            middle of iteration, so iterator isn't confused about its
//            position, using dllist<T>::remove() could cause
//            seg-fault if what is removed is pointed to by iterator
// Returns  : true -- oldvalue was found and removed from list
//            false -- oldvalue was not found
// Notes    : iterator's position will be reset to compensate for remove
template <class T> bool dllit<T>::remove(const T *oldvalue)
{
    if( list.length )
    {   dllit<T> j(list);
        do
        {   if( j() == oldvalue )
            {
                oldvalue = 0;
                if( j.num() < i )
                {
                  i--;  // oldvalue was found earlier in the list than *this' position
                  j.remove();
                } // if
                else if( j.num() == i )
                  remove();  // using *this' remove() will reset *this' position appropriately
                else // j.num() > i, so *this' position will not be effected
                  j.remove();
                return true;
            } // if
            ++j;
        } while( !j.at_start() );
    } // if

    return false;  // oldvalue wasn't found
} // template dllit<T>::remove()


// Template : bool dllit<T>::remove_delete(T *oldvalue)
// Purpose  : removes first oldvalue from list comparing pointer values,
//            DOES DELETE T object
//            this method should be used when deleting elements while in
//            middle of iteration, so iterator isn't confused about its
//            position, using dllist<T>::remove_delete() could cause
//            seg-fault if what is removed is pointed to by iterator
// Returns  : true -- oldvalue was found and removed from list
//            false -- oldvalue was not found
// Notes    : iterator's position will be reset to compensate for remove
template <class T> bool dllit<T>::remove_delete(T *oldvalue)
{
    if( list.length )
    {   dllit<T> j(list);
        do
        {   if( j() == oldvalue )
            {
                oldvalue = 0;
                if( j.num() < i )
                {
                  i--;  // oldvalue was found earlier in the list than *this' position
                  j.remove_delete();
                } // if
                else if( j.num() == i )
                  remove_delete();  // using *this' remove() will reset *this' position appropriately
                else // j.num() > i, so *this' position will not be effected
                  j.remove_delete();
                return true;
            } // if
            ++j;
        } while( !j.at_start() );
    } // if

    return false;  // oldvalue wasn't found
} // template dllit<T>::remove_delete()


// Template : uint dllit<T>::add_before(T *)
// Purpose  : inserts a node before the iterator's position
// Returns  : 0 node allocation failed
//            new legnth of list otherwise
// Notes    : iterator points to the same node it did before use
template <class T>
inline uint dllit<T>::add_before(T *a)
{
    return list.add_num(a, i++);
} // template dllit<T>::add_before()


// Template : uint dllit<T>::add_after(T *)
// Purpose  : inserts a node after the iterator's position
// Returns  : 0 node allocation failed
//            new legnth of list otherwise
// Notes    : iterator points to the same node it did before use
template <class T>
inline uint dllit<T>::add_after(T *a)
{
    return list.add_num(a, i + 1);
} // template dllit<T>::add_after()


} // namespace blib

// dll.cxx

