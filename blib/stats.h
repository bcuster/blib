// File     : stats.h
// Purpose  : Class definitions for blib statistical library
//
//
// Update Log -
//
// 20120409 - Begun


#ifndef BLIB_STATS_DEFINED
#define BLIB_STATS_DEFINED

#include  <cmath>
#include  <ctgmath>  // isnan()
#include  <vector>   // used by jump_walk_struct
#include  "blib.h"
#include  "dll.h"


namespace blib
{


// some useful math defines
#define  PI_RADIANS  3.141592653589793238462643383279502884197169399375105820974944592307816406286

// next define used in ProbabilityDistributions::number_of_digits_right_of_decimal()
//  determines how many successive 0's or 9's (right of decimal) after which to terminate
//  digit count, this is necessary b/c of inaccuracies in floating point arithmetic that
//  often occur, causing very small non-zero tails to be added to far right, or due to
//  implied rounding in floating point representation
//  ex. if below set to 10, then 34.0003560767400000000000025 is treated as 34.00035607674
//  or 697.3429999999999 is treated as 697.343
#define  NUMBER_OF_09S_TO_TREAT_AS_ZERO  8
// next define gives the largest number of digits ProbabilityDistributions::number_of_digits()
//  will count up to, it is an upper limit on the number of significant figures the type U
//  is allowed to have with accuracy, attempting to input strings of digits longer than
//  this number will complicate floating point arithmetic, compounding inaccuracies
//  note: std::numeric_limits<double>::digits10 is dynamically set to the compiler's max
#define  MAXIMUM_DIGITS_ALLOWED_FOR_COUNTS  16


template<class U> class Pt2Var
{
  private:
    U  X, Y;  // values of this data point

  public:
    Pt2Var<U>(const U x_, const U y_) : X(x_), Y(y_) {}

    // for use by TwoVarStats
    U x(void) const { return X; }
    U y(void) const { return Y; }

    bool operator==(const Pt2Var<U>& a) { return X == a.X && Y == a.Y; }
    bool matches(const U& a) { return X == a; }
}; // Pt2Var<U>

template<class U> class OneVarStats
{
  private:
    const dllist<U>&  list;   // data points
    uint              start;  // 0-starting position in list to start stats at
    uint              end;    // 0-starting +1 position in list to end stats at (ie. will loop with condition while(i < end))

    // resulting stats
    U  ave_y;
    U  ave_x;
    U  ave_x_sq;
    U  stdev_y;
    U  slp;
    U  icpt;
    U  reg_err;
    U  reg_err_stdev;
 
  public:
    OneVarStats<U>(const dllist<U>& l, const uint s = 0, const uint e = 0)
      : list(l), start(s), end(e),
        ave_y(0), ave_x(0), ave_x_sq(0), stdev_y(0), slp(0), icpt(0),
        reg_err(0), reg_err_stdev(0)
      { calc_x_stats(); }

    void calc_stats(void)  // test if *this constructed to default to compute sub-list stats
    {
      if( start > 0 || (end >= start && end < list.size() - 1) )
        calc_stats_sub(start, end);
      else
        calc_stats_all();
    } // calc_stats()
    void calc_x_stats(void);                        // calculate ave & ave^2 for x-axis, must be done before calc_stats_*() called, is done at construction if list.size() > 0
    void calc_stats_all(void);                      // calculate least squares linear regression and stddev
    void calc_stats_sub(const uint, const uint);  // calculate on subset of list, given start & end nodes


    U average(void) const { return ave_y; }
    U stddev(void) const { return stdev_y; }
    U slope(void) const { return slp; }        // regression line slope w.r.t. ordering data by position in list
    U intercept(void) const { return icpt; }   // regression line intercept
    U regression_error(void) const { return reg_err; }  // average absolute distance from regression line
    U regression_error_stddev(void) const { return reg_err_stdev; }  // std dev of the distance from the regression line
}; // OneVarStats<U>

template<class T, class U> class TwoVarStats
{
  private:
    const dllist<T>&  list;   // data points
    uint              start;  // 0-starting position in list to start stats at
    uint              end;    // 0-starting +1 position in list to end stats at (ie. will loop with condition while(i < end))

    // resulting stats
    uint size;
    U  ave_y;
    U  ave_x;
    U  stdev_y;
    U  stdev_x;
    U  highest_x;
    U  lowest_x;
    U  highest_y;
    U  lowest_y;
    U  covar;
    U  correl_coeff;
    U  slp;
    U  icpt;
    U  reg_err;
    U  reg_err_stdev;
 
  public:
    TwoVarStats<T,U>(const dllist<T>& l, const uint s = 0, const uint e = 0)
      : list(l), start(s), end(e), size(0),
        ave_y(0), ave_x(0), stdev_y(0), stdev_x(0),
        highest_x(0), lowest_x(0), highest_y(0), lowest_y(0),
        covar(0), correl_coeff(0), slp(0), icpt(0),
        reg_err(0), reg_err_stdev(0) {}

    void calc_stats(void)  // test if *this constructed to default to compute sub-list stats
    {
      if( start > 0 || (end > 0 && end < list.size()) )
        calc_stats_sub(start, end);
      else
        calc_stats_all();
    } // calc_stats()
    void calc_stats_all(void);                    // calculate least squares linear regression and stddev
    void calc_stats_sub(const uint, const uint);  // calculate on subset of list, given start & end nodes

    bool stats_have_been_calculated(void) const { return size; }   // reports whether a calc_stats_*() has been run
    uint sample_size(void) const { return size; }   // # of data pts used for last call to calc_stats_*()
    U average_x(void) const { return ave_x; }
    U average_y(void) const { return ave_y; }
    U stddev_x(void) const { return stdev_x; }      // note: std dev's and (co)variance are treated as "sample stats" and so computed with Bessel's Correction (divide by n-1 instead of n, to eliminate sampling bias)
    U stddev_y(void) const { return stdev_y; }
    U range_x(void) const { return highest_x-lowest_x; }
    U range_y(void) const { return highest_y-lowest_y; }
    U high_x(void) const { return highest_x; }      // highest value
    U low_x(void) const { return lowest_x; }        // lowest value
    U high_y(void) const { return highest_x; }      // highest value
    U low_y(void) const { return lowest_x; }        // lowest value
    U covariance(void) const { return covar; }
    U correlation_coefficient(void) const { return correl_coeff; }  // Pearson's correlation coefficient  (r^2 stat = (correl coeff)^2)  (-1=all pts lie on neg slope line, 0=no correlation, 1=all pts lie on pos slope line)
    U r_squared(void) const { return correl_coeff*correl_coeff; }   // r^2 statistic  (1=all pts lie on reg line, 0=no correlation)
    U slope(void) const { return slp; }        // linear regression line slope
    U intercept(void) const { return icpt; }   // linear regression line intercept
    U regression_error(void) const { return reg_err; }  // average absolute distance from regression line  (ave sqr of dist from reg line is same as var of y-dist of pts from reg line, this is the # minimized by reg line)
    U regression_error_stddev(void) const { return reg_err_stdev; }  // std dev of the distance from the regression line
}; // TwoVarStats<T,U>

// for use in ProbabilityDistributions::make_jump_walks_that_hit_a_or_b_by_t()
struct jump_walk_struct
{
  int result;  // 0 - result was not determined, 1 - reached a before b, 2 - reached b before a, 3 - walk did not reach a or b within t steps
  vector<int> steps;

  jump_walk_struct(void) : result(0) {}

  bool dll_lessthan(const jump_walk_struct* b)
  { return result < b->result; }
}; // struct jump_walk_struct

template<class U> class ProbabilityDistributions
{
  private:
  public:
    // useful general mathematical functions
    U    round(U);
    U    round(U, const int);
    U    frexp10(U, int*);
    int  number_of_digits(U x);
    int  number_of_digits_left_of_decimal(U x);
    int  number_of_digits_right_of_decimal(U x);
    int  largest_digit(U);
    int  smallest_digit(U);

    // common mathematical functions 
    U factorial(const uint);
    U combinations_n_choose_r(const uint, const uint);
    U combinations_n_place_r(const uint, const uint);
    U combinations_n_choose_r1_r2_r3(const uint, const uint, const uint);
    U count_stair_placements(const uint*, const uint);
    U count_pyramid_placements(const uint*, const uint);
    U count_sized_placements(const uint*, const uint);

    // PDFs
    U trinomial_distribution_PDF(const uint, const uint, const uint, const U, const U);
    U random_walk_null_step_PDF(const uint, const int, const U, const U);
    U random_walk_null_step_CDF(const uint, const int, const U, const U);
    U random_walk_null_step_prob_to_hit_by(const uint, const int, const U, const U);
    U random_walk_null_step_prob_to_a_before_b_by(const uint, const int, const int, const U, const U);
    U random_walk_null_step_prob_to_a_before_b_starting_x_by(const uint, const int, const int, const int, const U, const U);
    U random_walk_prob_to_a_before_b_starting_x_by(const uint, const int, const int, const int, const U, const U, string&);
    U random_walk_prob_to_a_before_b_by(const uint, const int, const int, const U, const U);

    bool make_jump_walks_that_hit_a_or_b_by_t(const int&, const int&, const int&, const int&, const int*, dllist<jump_walk_struct>&);
}; // ProbabilityDistributions<U>


} // namespace blib


#include "stats.cxx"   // included b/c ...Stats is a set of template classes, not compilable itself


#endif // BLIB_STATS_DEFINED

// stats.h

