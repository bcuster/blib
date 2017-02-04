// File     : function_optomizer.h
// Purpose  : definition of class FunctionOptomizer, a container for
//            data/methods needed to minimize or maximize a function
// Author   : Bradley Custer
//
// Update Log -
//
// 20160501 - Begun


#include <cmath>        // for pow()
#include <limits>       // for std::numeric_limits
#include <cstdlib>      // for srand(), rand()
#include <time.h>       // for time()

#include "blib/dll.h"   // for dllist


using namespace std;
using namespace blib;


//  POINT_CLASS must be any class such that these two methods are defined:
//   void set_coord(const int& i, const FLOAT_TYPE& value);  // i indexes dimension of coord to set
//   const double& get_coord(const int& i) const;  // i indexes dimension of coord to get
//
//  These methods should work for i in range 0 to number_of_dimensions-1.
//  Thus POINT_CLASS represents a point in the space of FLOAT_TYPEs being searched,
//   that the argument function, of minimize() & maximize(), has as domain.


template<class POINT_CLASS, class FLOAT_TYPE>
class FunctionMaximizerPointValuePair {
  public:
    FunctionMaximizerPointValuePair(POINT_CLASS* p, const FLOAT_TYPE& v)
     : point_(p), value_(v) {}
    ~FunctionMaximizerPointValuePair(void)
    { delete point_; }  // NOTE: when this Point-Value-Pair container is constructed we take
    // a pointer to a POINT_CLASS (which is expected to point a valid object!), but
    // at destruction time WE WILL DELETE the POINT_CLASS object!

    const POINT_CLASS& Point(void) { return *point_; }
    const FLOAT_TYPE& Value(void) { return value_; }

    bool operator<(const FunctionMaximizerPointValuePair<POINT_CLASS,FLOAT_TYPE>& a)
    { return value_ < a.value_; }  // dllist::sort() sorts greatest-to-least

  private:
    POINT_CLASS*  point_;
    FLOAT_TYPE    value_;
}; // class FunctionMaximizerPointValuePair


template<class POINT_CLASS, class FLOAT_TYPE>
class FunctionMinimizerPointValuePair {
  public:
    FunctionMinimizerPointValuePair(POINT_CLASS* p, const FLOAT_TYPE& v)
     : point_(p), value_(v) {}
    ~FunctionMinimizerPointValuePair(void)
    { delete point_; }  // NOTE: when this Point-Value-Pair container is constructed we take
    // a pointer to a POINT_CLASS (which is expected to point a valid object!), but
    // at destruction time WE WILL DELETE the POINT_CLASS object!

    const POINT_CLASS& Point(void) { return *point_; }
    const FLOAT_TYPE& Value(void) { return value_; }

    bool operator<(const FunctionMinimizerPointValuePair<POINT_CLASS,FLOAT_TYPE>& a)
    { return value_ > a.value_; }  // dllist::sort() sorts greatest-to-least, so flip inequality to get least-to-greatest

  private:
    POINT_CLASS*  point_;
    FLOAT_TYPE    value_;
}; // class FunctionMinimizerPointValuePair


template<class POINT_CLASS, class FLOAT_TYPE>
class FunctionOptomizer {
  public:
    FunctionOptomizer(const int& n,
                      const POINT_CLASS& low, const POINT_CLASS& high,
                      const FLOAT_TYPE& p, const FLOAT_TYPE& f, const FLOAT_TYPE& t)
      : number_of_dimensions_(n), low_point_(low), high_point_(high),
        unique_point_distance_margin_(p), unique_function_value_margin_(f),
        early_termination_threshold_(t) {}
    virtual ~FunctionOptomizer(void) {}

    virtual void maximize(FLOAT_TYPE (* function)(const POINT_CLASS &),
      dllist<FunctionMaximizerPointValuePair<POINT_CLASS,FLOAT_TYPE>>*) = 0;
    virtual void minimize(FLOAT_TYPE (* function)(const POINT_CLASS &),
      dllist<FunctionMinimizerPointValuePair<POINT_CLASS,FLOAT_TYPE>>*) = 0;

  protected:
    int                 number_of_dimensions_;
    const POINT_CLASS&  low_point_;
    const POINT_CLASS&  high_point_;
    FLOAT_TYPE          unique_point_distance_margin_;
    FLOAT_TYPE          unique_function_value_margin_;
    FLOAT_TYPE          early_termination_threshold_;
}; // class FunctionOptomizer


template<class POINT_CLASS, class FLOAT_TYPE>
class FunctionOptomizerRandomAlg : public FunctionOptomizer<POINT_CLASS,FLOAT_TYPE> {
  public:
    FunctionOptomizerRandomAlg(const int& n,
                      const POINT_CLASS& low, const POINT_CLASS& high,
                      const FLOAT_TYPE& p, const FLOAT_TYPE& f, const FLOAT_TYPE& t) 
      : FunctionOptomizer<POINT_CLASS,FLOAT_TYPE>(n, low, high, p, f, t),
        number_of_passes_(0)
      { seed_random_generator(); }
    virtual ~FunctionOptomizerRandomAlg(void) {}

    void maximize(FLOAT_TYPE (* function)(const POINT_CLASS &),
      dllist<FunctionMaximizerPointValuePair<POINT_CLASS,FLOAT_TYPE>>*);
    void minimize(FLOAT_TYPE (* function)(const POINT_CLASS &),
      dllist<FunctionMinimizerPointValuePair<POINT_CLASS,FLOAT_TYPE>>*);
    int number_of_passes(void) const { return number_of_passes_; }

  private:
    int number_of_passes_;

    void seed_random_generator(void) const { srand(time(NULL)); }
    FLOAT_TYPE random_number(void) const { return FLOAT_TYPE(rand())/FLOAT_TYPE(RAND_MAX); }   // return # in [0,1]
    FLOAT_TYPE random_number(const FLOAT_TYPE& low, const FLOAT_TYPE& high) const
      { return ((high-low)*random_number()) + low; }   // return # in [low,high]
    POINT_CLASS* new_random_point(void) const;
    POINT_CLASS* new_random_point(const POINT_CLASS&, const POINT_CLASS&) const;
}; // class FunctionOptomizerRandomAlg


// NOTE: In the following definitions of FunctionOptomizerRandomAlg member functions,
//  you will see that 'this->' is prepended to any inherited FunctionOptomizer
//  member variable.  This is necessary because FunctionOptomizer is an inherited
//  -template class-.  Template classes must be elaborted in order for the labeles
//  in their naming-scopes to become referencable (or at least that's the best
//  explaination I can give).  So the inherited member variables must be indentifed
//  with 'this->' ... I can't say I fully understand this but this page explains:
// http://stackoverflow.com/questions/1120833/derived-template-class-access-to-base-class-member-data


//#define  NUMBER_OF_PTS_PER_TOP_PT         20
//#define  NUMBER_OF_POINTS_ON_INIT         100
//#define  MAX_NUMBER_OF_PASSES             10
//#define  MAX_NUMBER_OF_TOP_POINTS         10
//#define  PERCENT_VOLUME_RADIUS_OF_SEARCH  0.2

//#define  NUMBER_OF_PTS_PER_TOP_PT         50
//#define  NUMBER_OF_POINTS_ON_INIT         500
//#define  MAX_NUMBER_OF_PASSES             15
//#define  MAX_NUMBER_OF_TOP_POINTS         20
//#define  MAX_TOTAL_OF_POINTS              100
//#define  PERCENT_VOLUME_RADIUS_OF_SEARCH  0.15

#define  NUMBER_OF_PTS_PER_TOP_PT         10
#define  NUMBER_OF_POINTS_ON_INIT         150
#define  MAX_NUMBER_OF_PASSES             10
#define  MAX_NUMBER_OF_TOP_POINTS         20
#define  MAX_TOTAL_OF_POINTS              100
#define  PERCENT_VOLUME_RADIUS_OF_SEARCH  0.15


template<class POINT_CLASS, class FLOAT_TYPE>
POINT_CLASS* FunctionOptomizerRandomAlg<POINT_CLASS,FLOAT_TYPE>::
new_random_point(void) const {
  POINT_CLASS* point = new POINT_CLASS;

  for (int i = 0; i < this->number_of_dimensions_; i++)
    point->set_coord(i, random_number(this->low_point_.get_coord(i), this->high_point_.get_coord(i)));

  return point;
} // FunctionOptomizer::new_random_point()


template<class POINT_CLASS, class FLOAT_TYPE>
POINT_CLASS* FunctionOptomizerRandomAlg<POINT_CLASS,FLOAT_TYPE>::
new_random_point(const POINT_CLASS& low, const POINT_CLASS& high) const {
  POINT_CLASS* point = new POINT_CLASS;

  for (int i = 0; i < this->number_of_dimensions_; i++)
    point->set_coord(i, random_number(low.get_coord(i), high.get_coord(i)));

  return point;
} // FunctionOptomizer::new_random_point()


template<class POINT_CLASS, class FLOAT_TYPE>
void FunctionOptomizerRandomAlg<POINT_CLASS,FLOAT_TYPE>::
maximize(FLOAT_TYPE (* function)(const POINT_CLASS &),
dllist<FunctionMaximizerPointValuePair<POINT_CLASS,FLOAT_TYPE>>* result_list) {

  number_of_passes_ = 0;

  // build initial random list of points throughout space to value
  dllist<POINT_CLASS> points_to_value;
  for (int i = 0; i < NUMBER_OF_POINTS_ON_INIT; i++)
    points_to_value.add( new_random_point() );

  FLOAT_TYPE old_highest_value;  // used to tell when a pass has gotten no further than previous pass
  for (;;) {
    number_of_passes_++;  // 1-start the pass count, so have right power of percent-value for radius

    if (result_list->first())
      old_highest_value = result_list->first()->Value();    // record lowest value of previous pass
    else
      old_highest_value = std::numeric_limits<FLOAT_TYPE>::min();  // set to minimum value for first pass

    // value each of the points in test list
    for (dllit<POINT_CLASS> i(points_to_value); !i.done(); ++i) {
      FLOAT_TYPE value = function(*i());
      if( !std::isnan(value) )
        result_list->add( new FunctionMaximizerPointValuePair<POINT_CLASS,FLOAT_TYPE>(i(), value) );
      else {
        delete i();
        cout << "NAN returned by maximizer function!" << endl;
      } // else
    } // for

    points_to_value.purge();  // wipe points_to_value list but keep POINT_CLASS objs (now stored in result_list)

    // sort values & throw out all but top so-many points
    result_list->sort();

    bool end_of_top_found = false;
    dllit<FunctionMaximizerPointValuePair<POINT_CLASS,FLOAT_TYPE>> i(*result_list);
    while (!i.done()) {
      if (end_of_top_found)
      { // delete the rest of the points if we're already past the top points
        i.remove_delete();
        continue;
      } // if

      if ((i()->Value() < result_list->first()->Value() + this->unique_function_value_margin_ &&
          i.num() >= MAX_NUMBER_OF_TOP_POINTS) ||
          i.num() >= MAX_TOTAL_OF_POINTS) {
         end_of_top_found = true;
        i.remove_delete();
        continue;
      } // if

      // if we reach this point then either
      //  a) i()->Value() is greater than or equal to a margin of the highest value on any point of last pass
      //  b) i()->Value() is one of the MAX_NUMBER_OF_TOP_POINTS largest values from the last pass
      //  so we retain any number of points with values close enough to the largest,
      //  or up to MAX_NUMBER_OF_TOP_POINTS otherwise
      // if we reach this point then i() will be retained for searching around in next pass

      ++i;
    } // while

    if (result_list->first()->Value() < this->early_termination_threshold_ ||  // abort minimizer if 1st run of values are too far from expected target value
        result_list->first()->Value() < old_highest_value + this->unique_function_value_margin_ ||  // done if best value of this pass is no better than a margin of last pass
        number_of_passes_ > MAX_NUMBER_OF_PASSES)  // abort if we've run too many passes
      break;  // we're done, no need to select points for another pass

    // build random list of points to value for next pass
    //  based on randomly choosing points within given radius
    //  of each point retained from last pass
    i.start();
    for(; !i.done(); ++i) {
      POINT_CLASS low, high;

      for (int j = 0; j < this->number_of_dimensions_; j++) {
        FLOAT_TYPE search_radius = pow(PERCENT_VOLUME_RADIUS_OF_SEARCH, number_of_passes_) * (this->high_point_.get_coord(j)-this->low_point_.get_coord(j));
        if ((i()->Point()).get_coord(j)- search_radius < this->low_point_.get_coord(j))  // check low boundary
          low.set_coord(j, this->low_point_.get_coord(j));
        else
          low.set_coord(j, (i()->Point()).get_coord(j) - search_radius);
        if ((i()->Point()).get_coord(j) + search_radius > this->high_point_.get_coord(j))  // check high boundary
          high.set_coord(j, this->high_point_.get_coord(j));
        else
          high.set_coord(j, (i()->Point()).get_coord(j) + search_radius);
      } // for

      for (int j = 0; j < NUMBER_OF_PTS_PER_TOP_PT; j++)
        points_to_value.add( new_random_point(low, high) );
    } // for

    cout << old_highest_value << '\t' << result_list->size() << '\t' << points_to_value.size() << endl;
  } // for
} // FunctionOptomizer::maximize()


template<class POINT_CLASS, class FLOAT_TYPE>
void FunctionOptomizerRandomAlg<POINT_CLASS,FLOAT_TYPE>::
minimize(FLOAT_TYPE (* function)(const POINT_CLASS &),
dllist<FunctionMinimizerPointValuePair<POINT_CLASS,FLOAT_TYPE>>* result_list) {

  number_of_passes_ = 0;

  // build initial random list of points throughout space to value
  dllist<POINT_CLASS> points_to_value;
  for (int i = 0; i < NUMBER_OF_POINTS_ON_INIT; i++)
    points_to_value.add( new_random_point() );

  FLOAT_TYPE old_lowest_value;  // used to tell when a pass has gotten no further than previous pass
  for (;;) {
    number_of_passes_++;  // 1-start the pass count, so have right power of percent-value for radius

    if (result_list->first())
      old_lowest_value = result_list->first()->Value();    // record lowest value of previous pass
    else
      old_lowest_value = std::numeric_limits<FLOAT_TYPE>::max();  // set to maximum value for first pass

    // value each of the points in test list
    for (dllit<POINT_CLASS> i(points_to_value); !i.done(); ++i) {
      FLOAT_TYPE value = function(*i());
      if( !std::isnan(value) )
        result_list->add( new FunctionMinimizerPointValuePair<POINT_CLASS,FLOAT_TYPE>(i(), value) );
      else {
        delete i();
        cout << "NAN returned by minimizer function!" << endl;
      } // else
    } // for

    points_to_value.purge();  // wipe points_to_value list but keep POINT_CLASS objs (now stored in result_list)

    // sort values & throw out all but top so-many points
    result_list->sort();

    bool end_of_top_found = false;
    dllit<FunctionMinimizerPointValuePair<POINT_CLASS,FLOAT_TYPE>> i(*result_list);
    while (!i.done()) {
      if (end_of_top_found)
      { // delete the rest of the points if we're already past the top points
        i.remove_delete();
        continue;
      } // if

      if ((i()->Value() > result_list->first()->Value() + this->unique_function_value_margin_ &&
          i.num() >= MAX_NUMBER_OF_TOP_POINTS) ||
          i.num() >= MAX_TOTAL_OF_POINTS) {
        end_of_top_found = true;
        i.remove_delete();
        continue;
      } // if

      // if we reach this point then either
      //  a) i()->Value() is less than or equal to a margin of the lowest value on any point of last pass
      //  b) i()->Value() is one of the MAX_NUMBER_OF_TOP_POINTS smallest values from the last pass
      //  so we retain any number of points with values close enough to the smallest,
      //  or up to MAX_NUMBER_OF_TOP_POINTS otherwise
      // if we reach this point then i() will be retained for searching around in next pass

      ++i;
    } // while

    if (result_list->first()->Value() > this->early_termination_threshold_ ||  // abort minimizer if 1st run of values are too far from expected target value
        result_list->first()->Value() > old_lowest_value - this->unique_function_value_margin_ ||  // done if best value of this pass is no better than a margin of last pass
        number_of_passes_ > MAX_NUMBER_OF_PASSES)  // abort if we've run too many passes
      break;  // we're done, no need to select points for another pass

    // build random list of points to value for next pass
    //  based on randomly choosing points within given radius
    //  of each point retained from last pass
    i.start();
    for (; !i.done(); ++i) {
      POINT_CLASS low, high;

      for (int j = 0; j < this->number_of_dimensions_; j++) {
        FLOAT_TYPE search_radius = pow(PERCENT_VOLUME_RADIUS_OF_SEARCH, number_of_passes_) * (this->high_point_.get_coord(j)-this->low_point_.get_coord(j));
        if ((i()->Point()).get_coord(j)- search_radius < this->low_point_.get_coord(j))  // check low boundary
          low.set_coord(j, this->low_point_.get_coord(j));
        else
          low.set_coord(j, (i()->Point()).get_coord(j) - search_radius);
        if ((i()->Point()).get_coord(j) + search_radius > this->high_point_.get_coord(j))  // check high boundary
          high.set_coord(j, this->high_point_.get_coord(j));
        else
          high.set_coord(j, (i()->Point()).get_coord(j) + search_radius);
      } // for

      for (int j = 0; j < NUMBER_OF_PTS_PER_TOP_PT; j++)
        points_to_value.add( new_random_point(low, high) );
    } // for

    cout << old_lowest_value << '\t' << result_list->size() << '\t' << points_to_value.size() << endl;
  } // for
} // FunctionOptomizer::minimize()


// function_optomizer.h
