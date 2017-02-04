/*****************************************************************************/
/*** Software: AMOS v0.01 (c)1998 All Rights Reserved by Bradley B. Custer ***/
/*****************************************************************************/

// File     : hash.h
// Purpose  : define HashTable template class
// Contains : class HashTable, class Node
//
// Update Log:
//
// 19980706 - Begun
// 19990628 - added HashTable::search_for_calling()
// 19991116 - refined the remove..() functions
// 20090527 - appended #endif comment


#ifndef HASH_CLASS_DEFINITION
#define HASH_CLASS_DEFINITION


#pragma warning(disable:4786)
#include <string>   // string class
#include "blib.h"   // blib global prototypes, defines, etc


namespace blib
{


// prototypes
template <class T> struct HashNode;
template <class T> class  HashTable;
template <class T> class  chashit;


// if you would like the hash table indexing to be
//  Case-Sensitive uncomment the next line
#define  HASH_INDEX_CASE_SENSITIVE


// Struct  : struct HashNode
// Purpose : is an element of the hash array, obj points to the data
// Note    : class T must have this method 'const char *key(void)'
//           add_to_table() adds the new items at getHashKey( T.key() ) position
template< class T > struct HashNode
{
    T *obj;          // pointer to data for this node
                     // Note: obj -must- be set to 0 when node is freed
                     // HashTable methods determine node usage by obj != 0
    HashNode *next;  // pointer to next node in this element's 
                     // linked list (for collisions)
    // constructors
    HashNode(void) : obj(0), next(0) {}
    HashNode(T *y) : obj(y), next(0) {}
}; // template struct HashNote


// Template: class HashTable
// Purpose : hash table container for HashNode elements
// Note    : class T must have this method 'const char *key(void)'
//           add_to_table() adds the new items at getHashKey( T.key() ) position
template< class T > class HashTable
{
    protected:
        // protected data -  accessable by derived classes
	HashNode<T> *table;        // this will be the hash array itself
    ulong       sizeoftable;   // the number of items in the array
	ulong       collisions;    // number of collisions in table
	ulong       itemcount;     // number of items in table

    friend class chashit<T>;

    public:
	ulong         ihash;         // used as table iterator
        HashNode< T > *ihash_offset; // point to iteration node in collision list

        // constructors & desctructor
        HashTable(void) : table(0), sizeoftable(0), collisions(0), 
            itemcount(0), ihash(0), ihash_offset(0)  {}
	HashTable(ulong size) : table(0), sizeoftable(0),
	    collisions(0), itemcount(0), ihash(0), ihash_offset(0)
	    { init_hashtable(size); }
        ~HashTable(void) { clr(); }


        // mutators
        void init_hashtable(ulong size) 
            // inits table to size Hashnodes
            // warning: clears any previous records
            { clr(); sizeoftable = size; table = new HashNode< T >[size]; }
        void add_to_table(T *);
	void clr(void);        // Destroys all nodes within index, HashTable zeroed
	void free_all(void);   // same as clr() but also DELETES OBJECTS

	bool remove(const T *);        // removes argument from hash table
	bool remove_del(T *);          // same as remove() but also DELETES OBJECTS
	T   *remove(const char *);
	bool remove_del(const char *);

        // inspectors
	int getHashKey(const char *) const;   // the actual hash function
	int getHashKey(const string &x) const 
	    { return getHashKey( x.c_str() ); }

	T *lookup(const char *) const;        // get pointer to objs with key() == x
	T *lookup(const string &x) const
            { return lookup( x.c_str() ); }
        uint search_for_calling(const char *, void (*)(T *)) const;
        uint search_for_calling(const string &w, void (*func)(T *)) const
            { return search_for_calling(w.c_str(), func); }

        ulong size_of_table(void) const       // return the hash table size
            { return sizeoftable; }
        ulong numberofitems(void) const       // return the item counter
            { return itemcount; }
        ulong numberofcollisions(void) const  // return the collision counter
            { return collisions; }
}; // template class HashTable


// Template : class chashit
// Purpose  : const iterator class for HashTable
//            use chashit to iterate a entire hash table
//            in the natural order that nodes are placed
//            into the table by the hashing function
// Example  : for(chashit<obj> i(objtable); ++i;)
//                i()->whatever();
template <class T> class chashit
{
    private:
        T                  *ptr;       // pointer to current T obj
        uint               i;          // maintains iterative count
        HashNode<T>        *offset;    // collision list offset
        uint               ihash;      // maintains table position
        const HashTable<T> &table;     // table to iterate

    public:
        // constructor
        chashit(const HashTable<T> &t) : table(t)
          { start(); }

        // mutators
        void start(void);          // start iteration over again
        T *next(void);             // increment element being pointed to
        T *operator++(void) { return next(); }

        // inspectors
        uint num(void) const       // return iteration position
            { return i; }
        T *operator()(void) const  // inspect value interator is pointing to
            { if(ptr) return ptr; else return 0; }
}; // template class chashit


} // namespace blib

// need to include functions here, because this is a template class ADT
#include "hash.cxx"

#endif // HASH_CLASS_DEFINITION

// hash.h

