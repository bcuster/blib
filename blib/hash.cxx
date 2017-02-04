/*****************************************************************************/
/*** Software: AMOS v0.01 (c)1998 All Rights Reserved by Bradley B. Custer ***/
/*****************************************************************************/

// File   : hash.cxx
// Purpse : contains templated methods for class HashTable< T >
//
// Update Log -
//
// 19980706 - ported over from mud++ code
// 19990628 - added HashTable::search_for_calling()
// 19991116 - refined the remove..() functions


namespace blib
{


// Function : void chashit< T >::start(void)
// Purpose  : set iterators back to the start of the table
template< class T > inline
  void chashit< T >::start(void)
{
    i         = 0;
    ptr       = 0;
    ihash     = 0;
    offset    = 0;
} // chashit< T >::start()


// Function : T *chashit< T >::next(void)
// Purpose  : to return the next T obj, 
//            from where ihash + offset are pointing
// Note     : iteration does -not- take place in T.key() order
//            it merily walks through the hash array
template< class T >
  T *chashit< T >::next(void)
{
    if( offset )
    { // if we we're in the middle of a linked list, then pick it up there
        ptr    = offset->obj;   // save object for return
        offset = offset->next;  // increment ihash_offset
        i++;
        return ptr;             // return last obj
    } // if

    // start incrementing through array, to find used element
    while( ihash < table.sizeoftable &&
           !(table.table[ ihash++ ].obj) ) ;

    if( ihash < table.sizeoftable ) 
    {  // if we found a node then return its value
        offset = table.table[ihash - 1].next;  // set offset for next call
        ptr    = table.table[ihash - 1].obj;
        i++;
        return ptr;                            // return last obj
    } // if

    // last element in array still could have values to send
    if( ihash == table.sizeoftable &&
        table.table[ table.sizeoftable - 1 ].obj )  
    {
        ihash++;                               // ihash > sizeoftable now
                                               // set offset for next call
        offset = table.table[table.sizeoftable - 1].next;
        ptr    = table.table[table.sizeoftable - 1].obj;
        i++;
        return ptr;                            // return last obj
    } // if

    return 0;  // end of table reached
} // chashit< T >::next()


// Function : template<class T> void HashTable<T>::clr(void)
// Purpose  : to remove all elements in the HashTable, table is zeroed
template< class T > void HashTable< T >::clr(void)
{
    if( sizeoftable > 0 ) // remove HashNode's collision lists
        for(ihash = 0; ihash < sizeoftable; ihash++)
        {
            if( table[ihash].next ) // if a list exists, destory it!
                for(HashNode< T > *ptr = table[ihash].next; ptr;)
                {
                    HashNode< T > *tmp = ptr;  // hold current node for a microsecond
                    ptr = ptr->next;           // reset to next node
                    delete tmp;                // now free what was the current node
                } // for
        } // for

    // free private HashTable data
    if( table )
    { // get rid of all the HashNodes
        delete [] table;
        table    = 0;
    } // if
    sizeoftable  = 0;
    itemcount    = 0;
    collisions   = 0;
    ihash        = 0;
    ihash_offset = 0;
} // clr()


// Function : template<class T> void HashTable<T>::free_all(void)
// Purpose  : to wipe the HashTable, AND FREE ALL OBJECTS MEMORY
//            same as clr(), except that pointed to objs are also deleted
template< class T > void HashTable< T >::free_all(void)
{
    if( sizeoftable > 0 ) // free items pointed to by HashNodes
        for(ihash = 0; ihash < sizeoftable; ihash++)
        {
            if( table[ihash].next ) // if a list exists, destory it!
                for(HashNode< T > *ptr = table[ihash].next; ptr;)
                {
                    delete ptr->obj;           // free this nodes object
                    HashNode< T > *tmp = ptr;  // hold current node for a microsecond
                    ptr = ptr->next;           // reset to next node
                    delete tmp;                // now free what was the current node
                } // for
            if( table[ ihash ].obj )        // if it exists
                delete table[ ihash ].obj;  // free table node's object
        } // for

    // free private HashTable data
    if( table )
    { // get rid of all the HashNodes
        delete [] table;
        table    = 0;
    } // if
    sizeoftable  = 0;
    itemcount    = 0;
    collisions   = 0;
    ihash        = 0;
    ihash_offset = 0;
} // free_all()


// Function : T *HashTable< T >::lookup(const char *x) const
// Prupose  : to return a pointer to the object in the hash table with
//            the key() == x
// Note     : 'const char *key()' must be a method of class T, key() is the
//            index into the table via the hash function
// Return   : T * - if a object with key() == x was found
//            0   - if no such object was found
template <class T> T *HashTable<T>::lookup(const char *x) const
{
    // locate table position by provided key x
    HashNode<T> *ptr = &table[ getHashKey(x) ];

    // if the item is present compare its' key() to x
    if( ptr->obj )
    {

// distinguish between case and incase sensitive tables here
#ifdef HASH_INDEX_CASE_SENSITIVE
        // if they match, we have a winner!
        if( !strcmp(ptr->obj->key(), x) ) return ptr->obj; 
 
        // otherwise search the collison list for a winner
        for(ptr = ptr->next; ptr; ptr = ptr->next)
            if( !strcmp(ptr->obj->key(), x) ) return ptr->obj; 
#else
        // if they match, we have a winner!
        if( !strcasecmp(ptr->obj->key(), x) ) return ptr->obj; 
 
        // otherwise search the collison list for a winner
        for(ptr = ptr->next; ptr; ptr = ptr->next)
            if( !strcasecmp(ptr->obj->key(), x) ) return ptr->obj; 
#endif

    } // if

    return 0; // item was not in table
} // HashTable<T>::lookup()

/*
// Function : T *HashTable< T >::search_for_calling()
// Prupose  : To search the table looking for objects whose
//            key()s are an instance of the 'wild' char string.
//            'wild' is a char string assumed to indicate wild
//            according to the fnmatch() scheme.
//            Every time an instance of 'wild' is found 'func'
//            will be called with a pointer to the instance.
// Return   : 0 - no instances of 'wild' where found
//            # of instances of 'wild' found otherwise
template <class T> uint HashTable<T>::
  search_for_calling(const char *wild, void (*func)(T *)) const
{
    // interate table looking was 'wild' matches
    uint found = 0;
    for(chashit<T> i(*this); ++i;)
        if( !fnmatch(wild, i()->key(), 0) )
        {
            (*func)(i());
            found++;
        } // if

    return found;
} // HashTable<T>::search_for_calling()

*/
// Function : bool HashTable< T >::remove_del(T *x)
// Prupose  : to delete the T obj x, from the hash table
//            AND - FREE IT'S MEMORY -
// Note     : key() must be a method of class T, key() is the
//            index into the table via the hash function
// Returns  : true  - 'x' found in table and deleted
//            false - 'x' not found in table and not deleted
template <class T>
  bool HashTable<T>::remove_del(T *x)
{
    if( remove(x) )
    {
        delete x;
        return true;
    } // if

    return false;
}  // HashTable<T>::remove_del()


// Function : bool HashTable< T >::remove(const T *x)
// Prupose  : to remove the obj x from the hash table
//            this is the same as remove_del(), except that
//            remove() DOES NOT free x's memory (delete x)
// Note     : key() must be a method of class T, key() is the
//            index into the table via the hash function
// Returns  : true  - 'x' found in table and deleted
//            false - 'x' not found in table
template <class T>
  bool HashTable<T>::remove(const T *x)
{
    HashNode<T> *ptr = &table[ getHashKey(x->key()) ]; 

    // if the item is present compare its' key() to x
    if( ptr->obj )
    {   // if they match, we have a winner!
        if( ptr->obj == x )
        {
            if( ptr->next ) // if collision list exists
            {   // increment ihash_offset if necessary
                if( ihash_offset == ptr->next )
                    ihash_offset = ptr->next->next;
                // decrement collision list from its start
                ptr->obj = ptr->next->obj;
                HashNode<T> *tmp = ptr->next;
                ptr->next = ptr->next->next;
                delete tmp;
            } // if
            else
                // otherwise it becomes a blank array element
                ptr->obj = 0;

            itemcount--;
            return true;
        } // if
        else
        {   // otherwise search the collison list for a winner
            HashNode<T> *prior = ptr;
            for(ptr = ptr->next; ptr; prior = ptr, ptr = ptr->next)
                if( ptr->obj == x )
                {
                    if( ptr->next ) // decrement collision list
                        prior->next = ptr->next;
                    else prior->next = 0;
                    delete ptr;
                    itemcount--;
                    return true;
                } // if
        } // else
    } // if

    return false;
}  // HashTable<T>::remove()


// Function : bool HashTable< T >::remove_del(const char *x)
// Prupose  : to delete the T obj x, from the hash table
//            AND - FREE IT'S MEMORY -
// Returns  : true  - 'x' found in table and deleted
//            false - 'x' not found in table
template <class T>
  bool HashTable<T>::remove_del(const char *x)
{
    T *ptr = 0;

    if( (ptr = remove(x)) )
    {
        delete ptr;
        return true;
    } // if

    return false;
}  // HashTable<T>::remove_del()


// Function : T *HashTable< T >::remove(const char *x)
// Prupose  : to remove the obj x from the hash table
//            this is the same as remove_del(), except that
//            remove() DOES NOT free x's memory (delete x)
// Returns  : 0 - 'x' not found in table
//            otherwise address of first T obj found in table
//             with key() == x; that T obj removed from table
template <class T>
  T *HashTable<T>::remove(const char *x)
{
    HashNode<T> *ptr = &table[ getHashKey(x) ]; 

    // if the item is present compare its' key() to x
    if( ptr->obj )
    {   // if they match, we have a winner!
#ifdef HASH_INDEX_CASE_SENSITIVE
        if( !strcmp(ptr->obj->key(), x) )
#else
        if( !strcasecmp(ptr->obj->key(), x) )
#endif
        {
            T *obj = ptr->obj;  // save return value

            if( ptr->next ) // if collision list exists
            {   // increment ihash_offset if necessary
                if( ihash_offset == ptr->next )
                    ihash_offset = ptr->next->next;
                // decrement collision list from its start
                ptr->obj = ptr->next->obj;
                HashNode<T> *tmp = ptr->next;
                ptr->next = ptr->next->next;
                delete tmp;
            } // if
            else
                // otherwise it becomes a blank array element
                ptr->obj = 0;

            itemcount--;
            return obj;
        } // if
        else
        {   // otherwise search the collison list for a winner
            HashNode<T> *prior = ptr;
            for(ptr = ptr->next; ptr; prior = ptr, ptr = ptr->next)
#ifdef HASH_INDEX_CASE_SENSITIVE
                if( !strcmp(ptr->obj->key(), x) )
#else
                if( !strcasecmp(ptr->obj->key(), x) )
#endif
                {
                    if( ptr->next )     // decrement collision list
                        prior->next = ptr->next;
                    else prior->next = 0;
                    T *obj = ptr->obj;  // save return value
                    delete ptr;
                    itemcount--;
                    return obj;
                } // if
        } // else
    } // if

    return 0;
}  // HashTable<T>::remove()


// Function : int HashTable<T>::getHashKey(const char *s) const
// Purpose  : hash function for indexing to the table[] elments
// Note     : this hash function was taken from the ispell utility
//            and obviously indexes character strings
//            -case is NOT preserved in the indexing-
template< class T >
int HashTable<T>::getHashKey(const char *s) const
{

#define HASHSHIFT   5

    register long h = 0;
    register int  i;

//#ifdef ICHAR_IS_CHAR
   for (i = 4;  i--  &&  *s != 0;  ) h = (h << 8) | toupper( *s++ );
//#else // ICHAR_IS_CHAR 
//    for (i = 2;  i--  &&  *s != 0;  ) h = (h << 16) | toupper( *s++ );
//#endif // ICHAR_IS_CHAR 

    while( *s != 0 )
    {
	 // We have to do circular shifts the hard way, since C doesn't
	 // have them even though the hardware probably does.  Oh, well.
	h = (h << HASHSHIFT)
	  | ((h >> (32 - HASHSHIFT)) & ((1 << HASHSHIFT) - 1));
	h ^= toupper( *s++ );
    } // while

    return( (ulong) h % sizeoftable );
}  // HashTable<T>::getHashKey()  


// Function : void HashTable< T >::add_to_table(T *new_obj)
// Purpose  : place a new object (class T) into the hash table
// Note     : T must have method 'const char *key(void)', which will 
//            determine new_obj's position in the table
// Warning  : it is important to remember that -ONLY A POINTER
//            IS BEING STORED IN THE TABLE-
//            therefore, if T is a struct that contains pointers to 
//            other blocks, you must allocate the space and make
//            copies for those pointers as well as allocate a T
//            block which -WILL NOT GO OUT OF SCOPE- as soon as the
//            scope add_to_table() is called from dies
template < class T > 
void HashTable< T >::add_to_table(T *new_obj)
{
    // put new_obj in it's place in the hashtable
    // get its' hash position first
    HashNode< T > *ptr = &table[ getHashKey(new_obj->key()) ]; 

    itemcount++;  // increment the number of items in the table

    // if obj already exists here, goto plan B - linked lists
    if( ptr->obj )
    {
        collisions++;  // increment the collisions counter
        if( ptr->next )
        {
            for(ptr = ptr->next; ptr; ptr = ptr->next)
                if( !ptr->next )
                {   // allocate another node in the linked list
                    ptr->next = new HashNode< T >;
                    ptr->next->obj = new_obj; // add y to end of linked list
                    break;
                } // if
        } // if
        else
        { // allocate first node in new linked list
            ptr->next = new HashNode< T >;
            ptr->next->obj = new_obj; // add y to start of linked list
        } // else
    } // if
    else ptr->obj = new_obj;  // no collision, place in table node 
} // HashTable::add_to_table()


} // namespace blib

// hash.cxx
