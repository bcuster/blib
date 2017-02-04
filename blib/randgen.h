// File     : randgen.h
// Purpose  : to provide a simple interface to the rand()/srand() uitilies
//            or to PRNGs written for child classes
//
// Update Log -
//
// 20090526 - Begun
// 20101013 - Overhauled RandGen to support multiple psudeo-random number generating algorithms
//            via inheritance of RandGen as a base class and added a two-tiered seeding method.
// 20110312 - Added macros to turn on/off single seeding for all RandGen objects
// 20110711 - Completed RandGenCMWC class
// 20150318 - Corrected some comments.
// 20150426 - Added 'const' to inspectors that were missing it.


// prototypes
namespace blib
{
class RandGen;
class RandGenCMWC;
} // namespace blib


#ifndef RANDGEN_CLASS_DEFINITIONS
#define RANDGEN_CLASS_DEFINITIONS


#include <cstdlib>     // for rand() and srand()
#include <sys/time.h>  // for gettimeofday()
#include <unistd.h>    // for gettimeofday()
#include <time.h>      // for placing date stamp
#include "blib.h"      // blib globals
#include "dll.h"       // dllist is used in RandGenCMWC

namespace blib
{


// comment out the next line if you want to enable seeding for each call to RandGen::RandGen()
#define  SEED_RANDOM_GENERATOR_ONCE   1    // Note: this is not thread friendly!
/****************************************************************************************
    Code for SEED_RANDOM_GENERATOR_ONCE Option
 *** THE FOLLOWING LINES OF CODE MUST BE CUT-N-PASTED INTO -ONE- .cxx FILE
 *** -AFTER- THE #include <randgen.h>
 *** IF YOU WANT TO TURN THE SEED_RANDOM_GENERATOR_ONCE OPTION ON
 *** IF YOU DON'T USE THE CODE YOU WILL HAVE "UNDEFINED REFERENCE" ERRORS AT LINK TIME
 *** (this is because it relies on static members, which essentially are global variables)
 *** TURNING ON THE OPTION CAUSES ALL RandGen INSTANCES TO GENERATE FROM ONE SEED
 *** THIS IS THEORETICALLY PREFERABLE FOR A STRONG PRNG LIKE THE CMWC ALGORITHM
 *** BUT THIS IS NOT THREAD FREINDLY
#ifdef  SEED_RANDOM_GENERATOR_ONCE  // these lines pertain to blib/randgen.h
    ulong RandGen::seed_num = 0;  // saved so seed can be read by RandGen::seed_used()
    bool  RandGen::seeding_done = false;
    // initial value indicates to seed on 1st construction of a RandGen
    //  after that there will be no more calls to stdio.h seed()
    //  by RandGen class, so all random numbers should come from
    //  a single stream
    dllist<uint> RandGenCMWC::lag_list;   // the previous CMWC_r values of generator
    uint         RandGenCMWC::c = 0;      // the carry (or quotient of each iteration)
#endif
******************************************************************************************/


// algorithm parameters
#define  RG_BLEED_NUMBER  10000  // ignore the 1st this many numbers of the rand() sequence
#define  CMWC_a  15455296        // 'a' multiplier used by CMWC algorithm in ab^r + 1
#define  CMWC_r  42658           // 'r' exponent used by CMWC algorithm in ab^r + 1


// Class   : RandGen
// Purpose : To provide a simple interface to psuedo-random number generating algorithms.
//           RandGen will use the stdlib rand() for RandGen::random(), but any inheriting class
//           that overloads RandGen::seed() and RandGen::random() will be expected to use those
//           methods for their obvious purposes unique to the algorithm being implemented.
//           A call to RandGen::initialize_seeder() should be made before a call to
//           RandGen*::seed(), it will seed and prep the stdlib srand() function, and the
//           subsequent values of stdlib rand() are used to generate values of calls to
//           RandGen::next_seed(), which should be called to obtain any seeds needed by
//           the child's RandGen*::seed() method.
//           Utlimately RandGen::next_rand() is called by the user to obtain the
//           random sequece, for it calls the child's RandGen*::random() for a random number
//           and then processes it according to parameters given by the user at construction
//           time (to give random numbers in a range with certain odds).
class RandGen
{
    private:
        uint  range;               // number of numbers to choose randomly from
        uint  start;               // starting number of this random choice range
        const uint*  odds;         // range sized array, which contains the relative
                                   //   "counting odds" that each number in start-to-(start+range-1)
                                   //   should be returned when next_rand() is called
              uint   denominator;  // sum of elts of odds array
              int    sign;         // used to randomly select sign of seed sent to srand()

        #ifdef  SEED_RANDOM_GENERATOR_ONCE
          static ulong  seed_num;      // used by initialize_seeder(), saved so seed can be read by seed_used()
          static bool   seeding_done;  // initial value should indicate to seed on 1st construction
                      //  after that there will be no more calls to stdio.h seed() by RandGen class,
                      //  so all random numbers should come from a single stream
          // note! You must initialize with 'bool RandGen::seeding_done = false;' and 'bool RandGen::seed_num = 0;' in a .cxx file
        #else
          ulong  seed_num;   // used by initialize_seeder(), saved so seed can be read by seed_used()
        #endif


    protected:
        // inspectors
        virtual uint next_seed(void) const { return rand(); }  // use to seed PRNG algorithms implemented in child classes
        virtual uint random(void) const { return rand(); }     // dummy function for overloading


    public:
        // constructor
        RandGen(const uint& r = RAND_MAX, const uint& s = 0, const uint* o = 0)    // note RAND_MAX is the maximum value rand() returns, set in cstdlib
          : range(r), start(s), odds(o)
        { // will cause random numbers to be generated, fed out by next_rand(),
          // in the range [s,s+r-1]
          // (so r is the #-of-#s in the range, and s is the 1st # in the range)
          // with odds of hitting each number in that range
          // determined by o (a uint array that must be exactly r uints long to avoid segfault)
          // for example if r=3,s=1 then o={1,2,1} gives P(1)=1/4,P(2)=2/4,P(3)=1/4
          // o=0 results in uniform probability

            // get the milliseconds since last GMT second
            timeval usecs;
            gettimeofday(&usecs, (struct timezone*) 0);

            // make the sign -1 if the current millisecond is odd, +1 if even
            sign = 1;
            if( usecs.tv_usec % 2 ) sign = -1;

            // compute the sum of odds array, ie the denominator of probabilities
            denominator = 0;
            if( odds ) for(uint i = 0; i < range; i++) denominator += odds[i];            

            #ifdef  SEED_RANDOM_GENERATOR_ONCE
                if( !seeding_done )
                {
                    initialize_seeder();
                    seeding_done = true;
                } // if
            #else
                // seed here to avoid segfaults, but should reinitialize latter for best seed choice
                initialize_seeder();
            #endif  // SEED_RANDOM_GENERATOR_ONCE
        } // RandGen()

        virtual ~RandGen(void) {} // RandGen destructor


        // mutators

        // Name:    initialize_seeder()
        // Purpose: Seed & prep the stdlib rand() function random sequence,
        //          for use in seeding better random number generators.
        void initialize_seeder(void)
        {   // seed the rand() int sequence with: 1st-32-bits + 2nd-32-bits
            //  of the 64 bit product described as follows--
            //  (cast-to-unsigned)((-1)^(millisec at RandGen() call time)x(cast-to-signed)(seconds since 1970)) x (Process execution cycles) x (current millisec)
            timeval time;  // get the milliseconds since last GMT second
            gettimeofday(&time, (struct timezone*) 0);
            unsigned long long const32_1s = 0xffffffff;  // bit-AND, &, with this gives modulo 2^32
            unsigned long long tics = (unsigned long long)clock();
            unsigned long long product = 0;
            if( !tics ) tics = (unsigned long long) sign;  // check that cycles doesn't report 0 (could happen it seed runs early in program execution)
            if( !time.tv_usec ) time.tv_usec = 1001;  // don't allow 0 to occur (would give 1/1000 chance to 0, too much for one seed!)
            product = tics * (unsigned long long)time.tv_usec * (unsigned long long)(sign * (int)time.tv_sec);
            seed_num = product & const32_1s;
            seed_num += (product >> 32 ) & const32_1s;
            srand( seed_num );

            // Bleed off the first RG_BLEED_NUMBER elements of the rand() sequence,
            // in order to "prep" the rand() generator past the beginning part of
            // its sequence, which is often not as uniform in distribution.
            for(uint i = 0; i < RG_BLEED_NUMBER; i++) rand();

            // flip sign to alternate halfs of long type's bytes between initialize_seeder() calls
            sign *= -1;
        } // initialize_seeder()

        // Initialize the random sequence.
        // For base RandGen this is already done by initialize_seeder().
        virtual void seed(void) { initialize_seeder(); }   // dummy function for overloading


        // inspectors

        uint Range(void) const { return range; }
        uint Start(void) const { return start; }
        ulong seed_used(void) const { return seed_num; }

        // Return the next element of RanGen*::random() sequence,
        //  after applying range and odds parameters.
        //  Will return an integer from interval [start,range+start-1],
        //  with uniform distribution if no odds given, or modified
        //  to agree with odds if it is given.
        uint next_rand(void)
        {
            // if odds wasn't filled, then use even (uniform) odds
            if( !denominator ) return (this->random() % range) + start;

            // otherwise the i-th number in the range gets the i-th odds of occurrence

            // get the next sequence element from random()
            uint rand_elt = this->random() % denominator;

            // determine which subrange it falls into and return result
            uint low_sub_range = 0, high_sub_range = 0;
            for(uint i = 0; i < range; i++)
            {
                high_sub_range += odds[i];
                if( low_sub_range <= rand_elt && rand_elt < high_sub_range )
                    return i + start;
                low_sub_range = high_sub_range;
            } // for
            // Note: If the odds array is actually larger than range elements
            //   this will be ignored, and only the first range elements will form
            //   the odds of occurrence.  If the odds array is smaller than range
            //   then you have a bug without warning, where whatever is in memory
            //   beyond the end of array will contribute to odds.

            // Note: The flow of execution should never get to here; if it does
            //   it is because something happened to overwrite the odds array
            //   or the denominator member between *this construction and
            //   the call to next_rand(). 0 (the first element of range) will
            //   be returned and no warning will be given.
            return 0;
        } // RandGen::next_rand()
        
        // Return a floating pt # from 0 to 1, by returning the
        //  value generated by next_rand() divided by the range.
        //  Will return an integer from interval [start,range+start-1],
        //  with uniform distribution if no odds given, or modified
        //  to agree with odds if it is given.
        double next_rand_from_0_to_1(void)
        {
            return double(next_rand() - start) / double(range - 1);
        } // RandGen::next_rand_from_0_to_1()
}; // class RandGen


#ifdef  SEED_RANDOM_GENERATOR_ONCE

// Class   : RandGenCMWC
// Purpose : Implements the Complimentary-Multiply-With-Carry (CMWC) algorithm as devised by
//           George Marsaglia (MWC) and latter Couture and L'Ecuyer (CMWC).
//           This version seeds on the first instance and then never again,
//           so all RandGenCMWC objects feed one psuedo-random sequence.
//           Note, this is NOT THREAD SAFE!
class RandGenCMWC : public RandGen
{
    private:
        static dllist<uint> lag_list;   // the previous r values of generator
        static uint         c;          // the carry (or quotient of each iteration)
        // note! You must initialize these static variables
        //  with 'dllist<uint> RandGenCMWC::lag_list;', etc. in a .cxx file
        //  because class static varialbes are just global varialbes
        //  formally associated to the class

    protected:
        virtual uint random(void)
        { // generate next element of CMWC sequence
            unsigned long long const32_1s = 0xffffffff;  // bit-AND, &, with this gives modulo 2^32
            uint* lag_tmp = lag_list.first();
            unsigned long long tmp1 = (unsigned long long)(CMWC_a)*(*lag_tmp) + c;  // perform integer product and sum
            uint tmp2 = tmp1 & const32_1s;   // copy the lower 32 bits of the product, this is the remainder of dividing by 2^32
            uint x = 0xffffffff - tmp2;      // next random is subtraction from 2^32 - 1 (the compliment)
            c = (tmp1 >> 32) & const32_1s;   // copy the upper 32 bits of the product, this is the new carry
            // shift list forward
            lag_list.pop();
            *lag_tmp = x;  // add the new sequence element onto list
            lag_list.add( lag_tmp );
            return x;
        } // RandGenCMWC::random()

    public:
        RandGenCMWC(const uint& range = RAND_MAX, const uint& s = 0, const uint* o = 0)
          : RandGen(range,s,o) // RandGenCMWC constructor
        { if( !lag_list.size() ) seed(); }  // only seed once

        virtual ~RandGenCMWC(void) {} // RandGenCMWC destructor


        virtual void seed(void)
        { // draw r+1 seeds from RandGen::next_seed()
            for(uint i = 0; i < CMWC_r; i++)
            { // biuld list of r initial sequence seeds
                uint* tmp = new uint;
                *tmp = next_seed();
                lag_list.add( tmp );
            } // for
            c = next_seed();  // +1 more seed for initial carry

            // Bleed off the first RG_BLEED_NUMBER elements to prep generator
            for(uint i = 0; i < RG_BLEED_NUMBER; i++) random();
        } // RandGenCMWC::seed()
}; // class RandGenCMWC

#else  // SEED_RANDOM_GENERATOR_ONCE

// Class   : RandGenCMWC
// Purpose : Implements the Complimentary-Multiply-With-Carry (CMWC) algorithm as devised by
//           George Marsaglia (MWC) and latter Couture and L'Ecuyer (CMWC).
//           This version reseeds the generator every for each instance.
class RandGenCMWC : public RandGen
{
    private:
        dllist<uint> lag_list;   // the previous r values of generator
        uint         c;          // the carry (or quotient of each iteration)

    protected:
        virtual uint random(void)
        { // generate next element of CMWC sequence
            unsigned long long const32_1s = 0xffffffff;  // bit-AND, &, with this gives modulo 2^32
            uint* lag_tmp = lag_list.first();
            unsigned long long tmp1 = (unsigned long long)(CMWC_a)*(*lag_tmp) + c;  // perform integer product and sum
            uint tmp2 = tmp1 & const32_1s;   // copy the lower 32 bits of the product, this is the remainder of dividing by 2^32
            uint x = 0xffffffff - tmp2;      // next random is subtraction from 2^32 - 1 (the compliment)
            c = (tmp1 >> 32) & const32_1s;   // copy the upper 32 bits of the product, this is the new carry
            // shift list forward
            lag_list.pop();
            *lag_tmp = x;  // add the new sequence element onto list
            lag_list.add( lag_tmp );
            return x;
       } // RandGenCMWC::random()

    public:
        RandGenCMWC(const uint& range = RAND_MAX, const uint& s = 0, const uint* o = 0, const uint& r_ = CMWC_r)
          : RandGen(range,s,o) { seed(r_); } // RandGenCMWC constructor

        virtual ~RandGenCMWC(void)  // RandGenCMWC destructor
        { lag_list.free_all(); }

        virtual void seed(void)
        { // draw r+1 seeds from RandGen::next_seed()
            for(uint i = 0; i < CMWC_r; i++)
            { // biuld list of r initial sequence seeds
                uint* tmp = new uint;
                *tmp = next_seed();
                lag_list.add( tmp );
            } // for
            c = next_seed();  // +1 more seed for initial carry

            // Bleed off the first RG_BLEED_NUMBER elements to prep generator
            for(uint i = 0; i < RG_BLEED_NUMBER; i++) random();
        } // RandGenCMWC::seed()
}; // class RandGenCMWC

#endif // SEED_RANDOM_GENERATOR_ONCE


} // namespace blib

#endif // RANDGEN_CLASS_DEFINITIONS

// randgen.h

