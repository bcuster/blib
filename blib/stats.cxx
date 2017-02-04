// File     : stats.cxx
// Purpose  : function code for blib stats library classes
//
//
// Update Log -
//
// 20120409 - Begun


namespace blib
{


// this little marco is about the best we can do for a template type
#define  abs_val(a)  (a < 0 ? -1*a : a)


// type specific math functions
template<class U> inline U sqrt(const U x) { return sqrt<U>(x); }
template<> inline int         sqrt<int        >(const int         x) { return int  (sqrt(double(x))); }
template<> inline uint        sqrt<uint       >(const uint        x) { return uint (sqrt(double(x))); }
template<> inline long        sqrt<long       >(const long        x) { return long (sqrt(double(x))); }
template<> inline ulong       sqrt<ulong      >(const ulong       x) { return ulong(sqrt(double(x))); }
template<> inline float       sqrt<float      >(const float       x) { return sqrt(x); }
template<> inline double      sqrt<double     >(const double      x) { return sqrt(x); }
template<> inline long double sqrt<long double>(const long double x) { return sqrt(x); }

template<class U> inline bool isnan(const U x) { return isnan<U>(x); }
template<> inline bool isnan<float      >(const float       x) { return std::isnan(x); }
template<> inline bool isnan<double     >(const double      x) { return std::isnan(x); }
template<> inline bool isnan<long double>(const long double x) { return std::isnan(x); }

template<class U> inline bool isnormal(const U x) { return isnormal<U>(x); }
template<> inline bool isnormal<float      >(const float       x) { return std::isnormal(x); }
template<> inline bool isnormal<double     >(const double      x) { return std::isnormal(x); }
template<> inline bool isnormal<long double>(const long double x) { return std::isnormal(x); }


// calculate ave & ave^2 for x-axis, must be done before calc_stats_*(),
// done at construction if list.size() > 0
template<class U>
void OneVarStats<U>::calc_x_stats(void)
{
  if( list.empty() ) return;
  // test if *this constructed to default to compute sub-list stats
  ave_x    = 0;
  ave_x_sq = 0;
  if( start > 0 || (end > 0 && end < list.size()) )
  { // computing stats on part of the list
    for(uint i = start; i < end; ++i)
    {
      ave_x    += i;
      ave_x_sq += i*i;
    } // for
    uint size = end - start;
    ave_x    /= size;
    ave_x_sq /= size;
  } // if
  else
  { // computing stats on whole list
    // compute average x
    //   (x values are 1,2,3,... corresponding to list nodes, use this to calculate sum of 1-to-list.size() easily)
    if( list.size() % 2 )
    { // odd number of elements
      U middle = (U(list.size())/2) - 0.5;
      ave_x = middle*(list.size()+1);
      ave_x += middle + 1;
    } // if
    else // even number of elements
      ave_x = (U(list.size())/2)*(list.size()+1);
    // compute average of x squared
    for(uint i = 1; i < list.size() + 1; ++i) ave_x_sq += i*i;
    ave_x    /= list.size();
    ave_x_sq /= list.size();
  } // else
} // OneVarStats<U>::calc_x_stats()

// calculate least squares linear regression and stddev
template<class U>
void OneVarStats<U>::calc_stats_all(void)
{
  if( list.empty() ) return;

  // do one loop through the data values & compute counts
    ave_y    = 0;  // average y (the actual data values)
  U ave_y_sq = 0;  // average y squared
  U ave_xy   = 0;  // average of x*y
  for(cdllit<U> i(list); !i.done(); ++i)
  {
    ave_y    += *i();
    ave_y_sq += (*i())*(*i());
    ave_xy   += (i.num()+1)*(*i());
  } // for
  // take ratio to compute averages
  ave_y      /= list.size();
  ave_y_sq   /= list.size();
  ave_xy     /= list.size();
  // linear regression values
  slp  = (ave_xy - ave_x*ave_y)/(ave_x_sq - ave_x*ave_x);
  icpt = ave_y - slp*ave_x;
  // standard deviation in the data values
  stdev_y = sqrt<U>(ave_y_sq - ave_y*ave_y);

  // another loop through the data to get error, using the computed slope
    reg_err    = 0;  // regression error (average absolute distance from the regression line)
  U ave_err_sq = 0;  // average error squared (for error's std dev)
  U tmp        = 0;  // used for computing error
  for(cdllit<U> i(list); !i.done(); ++i)
  {
    tmp         = *i() - slp*(i.num()+1) - icpt;
    reg_err    += abs_val(tmp);
    ave_err_sq += tmp*tmp;
  } // for
  // take ratio to compute averages
  reg_err    /= list.size();
  ave_err_sq /= list.size();
  // standard deviation in the distance from the regression line
  reg_err_stdev = sqrt<U>(ave_err_sq - reg_err*reg_err);
} // OneVarStats<U>::calc_stats_all()

// calculate on subset of list, given start & end nodes
template<class U>
void OneVarStats<U>::calc_stats_sub(const uint s, const uint e)
{ // s is the start position, e is s+num-to-iterate, ie. 0-starting end position
  if( list.empty()        ||
      s > list.size() - 1 ||
      e > list.size()     ||
      s >= e ) return;

  // do one loop through the data values & compute counts
    ave_y    = 0;  // average y (the actual data values)
  U ave_y_sq = 0;  // average y squared
  U ave_xy   = 0;  // average of x*y
  cdllit<U> i(list);
  i.start_at(s);
  do
  {
    ave_y      += *i();
    ave_y_sq   += (*i())*(*i());
    ave_xy     += (i.num()+1)*(*i());
    ++i;
  } while( i.num() <= e && i.num() );
  // take ratio to compute averages
  uint size = e - s;
  ave_y    /= size;
  ave_y_sq /= size;
  ave_xy   /= size;
  // linear regression values
  slp  = (ave_xy - ave_x*ave_y)/(ave_x_sq - ave_x*ave_x);
  icpt = ave_y - slp*ave_x;
  // standard deviation in the data values
  stdev_y = sqrt<U>(ave_y_sq - ave_y*ave_y);

  // another loop through the data to get error, using the computed slope
    reg_err    = 0;  // regression error (average absolute distance from the regression line)
  U ave_err_sq = 0;  // average error squared (for error's std dev)
  U tmp        = 0;  // used for computing error
  i.start_at(s);
  do
  {
    tmp         = *i() - slp*(i.num()+1) - icpt;
    reg_err    += abs_val(tmp);
    ave_err_sq += tmp*tmp;
    ++i;
  } while( i.num() <= e && i.num() );
  // take ratio to compute averages
  reg_err    /= size;
  ave_err_sq /= size;
  // standard deviation in the distance from the regression line
  reg_err_stdev = sqrt<U>(ave_err_sq - reg_err*reg_err);
} // OneVarStats<U>::calc_stats_sub()


// calculate least squares linear regression and stddev
template<class T, class U>
void TwoVarStats<T,U>::calc_stats_all(void)
{
  if( list.empty() ) return;

  size = list.size();
  U Bessel_corr_size = (size > 1 ? size - 1 : 1);  // Bessel's Correction (but double check at least 2 data pts)

  // do one loop through the data values & compute counts and high & low values
  ave_x = 0;  // average x (the actual data values)
  ave_y = 0;  // average y (the actual data values)
  highest_x = list.first()->x();
  lowest_x  = list.first()->x();
  highest_y = list.first()->y();
  lowest_y  = list.first()->y();
  for(cdllit<T> i(list); !i.done(); ++i)
  {
    ave_x += i()->x();
    ave_y += i()->y();

    if( i()->x() > highest_x ) highest_x = i()->x();
    if( i()->x() < lowest_x  ) lowest_x  = i()->x();
    if( i()->y() > highest_y ) highest_y = i()->y();
    if( i()->y() < lowest_y  ) lowest_y  = i()->y();
  } // for
  ave_x /= size;
  ave_y /= size;

  // loop again to compute variances, to avoid numerical instablility
  U var_x = 0;
  U var_y = 0;
  covar   = 0;
  for(cdllit<T> i(list); !i.done(); ++i)
  {
    var_x += (i()->x() - ave_x)*(i()->x() - ave_x);
    var_y += (i()->y() - ave_y)*(i()->y() - ave_y);
    covar += (i()->x() - ave_x)*(i()->y() - ave_y);
  } // for
  var_x /= Bessel_corr_size;
  var_y /= Bessel_corr_size;
  covar /= Bessel_corr_size;
  // linear regression values
  slp  = covar / var_x;
  icpt = ave_y - slp*ave_x;
  // standard deviation in the data values
  stdev_x = sqrt<U>(var_x);
  stdev_y = sqrt<U>(var_y);
  // Pearson's correlation coefficient
  correl_coeff = covar / sqrt<U>(var_x*var_y);

  // another loop through the data to get error, using the computed slope
    reg_err    = 0;  // regression error (average absolute distance from the regression line)
  U ave_err_sq = 0;  // average error squared (for error's std dev)
  U tmp        = 0;  // used for computing error
  for(cdllit<T> i(list); !i.done(); ++i)
  {
    tmp         = i()->y() - slp*(i()->x()) - icpt;
    reg_err    += abs_val(tmp);
    ave_err_sq += tmp*tmp;
  } // for
  // take ratio to compute averages
  reg_err    /= size;
  ave_err_sq /= Bessel_corr_size;
  // standard deviation in the distance from the regression line  (not worrying about another loop for variance on reg err)
  reg_err_stdev = sqrt<U>(ave_err_sq - reg_err*reg_err);
} // TwoVarStats<T,U>::calc_stats_all()


// calculate on subset of list, given start & end nodes
template<class T, class U>
void TwoVarStats<T,U>::calc_stats_sub(const uint s, const uint e)
{ // s is the start position, e is s+num-to-iterate, ie. 0-starting end position
  if( list.empty()        ||
      s > list.size() - 1 ||
      e > list.size()     ||
      s >= e ) return;

  size = e - s;
  U Bessel_corr_size = (size > 1 ? size - 1 : 1);  // Bessel's Correction (but double check at least 2 data pts)

  // do one loop through the data values & compute counts
  ave_x = 0;  // average x (the actual data values)
  ave_y = 0;  // average y (the actual data values)
  cdllit<T> i(list);
  i.start_at(s);
  highest_x = i()->x();
  lowest_x  = i()->x();
  highest_y = i()->y();
  lowest_y  = i()->y();
  do
  {
    ave_x += i()->x();
    ave_y += i()->y();

    if( i()->x() > highest_x ) highest_x = i()->x();
    if( i()->x() < lowest_x  ) lowest_x  = i()->x();
    if( i()->y() > highest_y ) highest_y = i()->y();
    if( i()->y() < lowest_y  ) lowest_y  = i()->y();

    ++i;
  } while( i.num() < e && i.num() );
  // take ratio to compute averages
  ave_x /= size;
  ave_y /= size;

  // loop again to compute variances, to avoid numerical instablility
  U var_x = 0;
  U var_y = 0;
  covar   = 0;
  i.start_at(s);
  do
  {
    var_x += (i()->x() - ave_x)*(i()->x() - ave_x);
    var_y += (i()->y() - ave_y)*(i()->y() - ave_y);
    covar += (i()->x() - ave_x)*(i()->y() - ave_y);
    ++i;
  } while( i.num() <= e && i.num() );
  var_x /= Bessel_corr_size;
  var_y /= Bessel_corr_size;
  covar /= Bessel_corr_size;
  // linear regression values
  slp  = covar / var_x;
  icpt = ave_y - slp*ave_x;
  // standard deviation in the data values
  stdev_x = sqrt<U>(var_x);
  stdev_y = sqrt<U>(var_y);
  // Pearson's correlation coefficient
  correl_coeff = covar / sqrt<U>(var_x*var_y);

  // another loop through the data to get error, using the computed slope
    reg_err    = 0;  // regression error (average absolute distance from the regression line)
  U ave_err_sq = 0;  // average error squared (for error's std dev)
  U tmp        = 0;  // used for computing error
  i.start_at(s);
  do
  {
    tmp         = i()->y() - slp*(i()->x()) - icpt;
    reg_err    += abs_val(tmp);
    ave_err_sq += tmp*tmp;
    ++i;
  } while( i.num() <= e && i.num() );
  // take ratio to compute averages
  reg_err    /= size;
  ave_err_sq /= Bessel_corr_size;
  // standard deviation in the distance from the regression line
  reg_err_stdev = sqrt<U>(ave_err_sq - reg_err*reg_err);
} // TwoVarStats<T,U>::calc_stats_sub()


// rounds up to 1st whole number, if x>0     (ex. 1.45 -> 2)
// rounds down to 1st whole number, if x<0   (ex. -1.45 -> -2)
template<class U>
U ProbabilityDistributions<U>::round(U x)
{
  if( !isnormal<U>(x) ) return x;   // if x is NaN just return it

  U sign   = (x < 0 ? -1 : 1);
  x        = sign*x;          // x = abs(x)
  U result = std::floor(x);   // result = whole number part of abs(x)

  for(;;)
  {
    x -= std::floor(x);  // remove all digits left of the decimal point, so now x = 0.???...
    if( x >= 0.5 ) return sign*(result + 1);
    if( x <  0.4 ) return (!result ? 0 : sign*result);
    // x = 0.4???..., so we loop again to look at next digit
    x *= 10;  // pushed decial pt 1 digit right, so now x = 4.???...
  } // for
} // ProbabilityDistributions<U>::round()

// rounds x to place digits above or below 1's digit
//  (place=0 is treated as rounding to 1's digit whole #'s)
// ex. if place=1 then 15.445 will be rounded to 20
//     if place=-2 then 15.445 will be rounded to 15.45
// rounds up, if x>0     (ex. 15.445,1 -> 20)
// rounds down, if x<0   (ex. -15.445,2 -> -20)
template<class U>
U ProbabilityDistributions<U>::round(U x, const int place)
{
  if( !place ) return round(x);

  if( place > 0 ) // round to 10's digit or greater
    return round( x / pow(10,place) ) * pow(10,place);
  else // round to 10th's digit or smaller
    return round( x * pow(10,-1*place) ) / pow(10,-1*place);
} // ProbabilityDistributions<U>::round()

// like math.h frexp() lib function, but for base 10
template<class U>
U ProbabilityDistributions<U>::frexp10(U arg, int* exp)
{
   *exp = (arg == 0) ? 0 : int(1 + std::log10( abs_val(arg) ));
   return arg * pow(10, -(*exp));
} // ProbabilityDistributions<U>::frexp10()

// returns the number of digits in x
// ex. x=0 -> 0, x=7.1 -> 2, x=-34 -> 2, x=1000 -> 4, x=48890.035 -> 8, x=0.035 -> 3, x=-0.035 -> 3
template<class U>
inline int ProbabilityDistributions<U>::number_of_digits(U x)
{
  return number_of_digits_left_of_decimal(x) + number_of_digits_right_of_decimal(x);
} // ProbabilityDistributions<U>::number_of_digits()

// returns the number of digits left of the decimal in x
// ex. x=0 -> 0, x=7.1 -> 1, x=-34 -> 2, x=1000 -> 4, x=48890.035 -> 5, x=0.035 -> 0, x=-0.035 -> 0
template<class U>
int ProbabilityDistributions<U>::number_of_digits_left_of_decimal(U x)
{
  if( !x ) return 0;
  if( x < 0 ) x *= -1;

  uint num_digits = 0;
  x = std::floor(x);
  while( x ) { num_digits++; x /= 10; x = std::floor(x); }

  return num_digits;
} // ProbabilityDistributions<U>::number_of_digits_left_of_decimal()

// returns the number of digits right of the decimal in x
// ex. x=0 -> 0, x=7.1 -> 1, x=-34 -> 0, x=1000 -> 0, x=48890.035 -> 3, x=0.035 -> 3, x=-0.035 -> 3
template<class U>
int ProbabilityDistributions<U>::number_of_digits_right_of_decimal(U x)
{
  if( !x ) return 0;
  if( x < 0 ) x *= -1;

  uint max_num_digits = MAXIMUM_DIGITS_ALLOWED_FOR_COUNTS - number_of_digits_left_of_decimal(x);

  uint zero_count = 0, nine_count = 0, num_digits = 0;
  for(;;)
  {
    x -= std::floor(x);
    if( !x ) break;
    x *= 10;
    double nxt_floor = std::floor(x);
    if( !nxt_floor ) zero_count++;
    else zero_count = 0;
    if( zero_count >= NUMBER_OF_09S_TO_TREAT_AS_ZERO )
    {
      num_digits -= zero_count-1;
      break;
    } // if
    if( nxt_floor == 9 ) nine_count++;
    else nine_count = 0;
    if( nine_count >= NUMBER_OF_09S_TO_TREAT_AS_ZERO )
    {
      num_digits -= nine_count-1;
      break;
    } // if
    num_digits++;
    if( num_digits >= max_num_digits ) break;
  } // for

  return num_digits;
} // ProbabilityDistributions<U>::number_of_digits_right_of_decimal()

// returns the place of the largest digit of x, where 1's place =0
// ex. x=0 -> 0, x=7.1 -> 0, x=-34 -> 2, x=1000 -> 3, x=48890.035 -> 4, x=0.035 -> -2, x=-0.035 -> -2
template<class U>
int ProbabilityDistributions<U>::largest_digit(U x)
{
  if( !x ) return 0;
  if( x < 0 ) x *= -1;

  if( x >= 1 && x < 10 ) return 0;

  int place = 0;

  if( x >= 10 )
  {
    place = -1;
    do { x /= 10; place++; } while( x >= 1 );
  } // if
  else
    do { x *= 10; place--; } while( x < 1 );

  return place;
} // ProbabilityDistributions<U>::largest_digit()

// returns the place of the smallest non-zero digit of x, where 1's place =0
// ex. x=0 -> 0, x=7.1 -> -1, x=-34 -> 0, x=1000 -> 3, x=48890.035 -> -3, x=0.035 -> -3, x=-0.035 -> -3
template<class U>
int ProbabilityDistributions<U>::smallest_digit(U x)
{
  if( !x ) return 0;
  if( x < 0 ) x *= -1;

  int place = 0;
  unsigned long long left_of_decimal = std::floor(x);
  int num_to_right = number_of_digits_right_of_decimal(x);
  if( num_to_right )
    return -1 * num_to_right;
  else // since we check for x=0, we now know left_of_decimal != 0
    while( !(left_of_decimal % 10) ) { left_of_decimal /= 10; place++; }

  return place;
} // ProbabilityDistributions<U>::smallest_digit()


template<class U>
U ProbabilityDistributions<U>::factorial(const uint n)
{
  if( n == 0 ) return 1;

  U result = U(n);
  for(U factor = U(n) - 1; factor > 1; factor--)
    result *= factor;

  return result;
} // ProbabilityDistributions<U>::factorial()

// n Choose r -- # of ways to choose r objs from a set of n objs
template<class U>
U ProbabilityDistributions<U>::combinations_n_choose_r(const uint n, const uint r_1)
{
  U down_to;
  U denominator;
  uint r_2 = n - r_1;
  if( r_1 >= r_2 )
  { // r_1 greatest
    down_to = U(r_1);
    denominator = factorial(r_2);
  } // if
  else
  { // r_2 greatest
    down_to = U(r_2);
    denominator = factorial(r_1);
  } // else

  U result = 1;
  for(U factor = n; factor > down_to; factor--)
    result *= factor;
  result /= denominator;

  return result;
} // ProbabilityDistributions<U>::combinations_n_choose_r()

// n Place r -- # of ways to place r identical objs into n distinct boxes
template<class U>
U ProbabilityDistributions<U>::combinations_n_place_r(const uint n, const uint r)
{
  return combinations_n_choose_r(n - 1 + r, r);
} // ProbabilityDistributions<U>::combinations_n_place_r()

// count the # of ways to choose three subsets of r_1 objs, r_2 objs
// and r_3 objs from a superset of r_1+r_2+r_3 objs
template<class U>
U ProbabilityDistributions<U>::combinations_n_choose_r1_r2_r3
(const uint r_1, const uint r_2, const uint r_3)
{
  U down_to;
  U denominator;
  if( r_1 >= r_2 )
  {
    if( r_1 >= r_3 )
    { // r_1 greatest
      down_to = U(r_1);
      denominator = factorial(r_2);
      denominator *= factorial(r_3);
    } // if
    else
    { // r_3 greatest
      down_to = U(r_3);
      denominator = factorial(r_1);
      denominator *= factorial(r_2);
    } // else
  } // if
  else
  {
    if( r_2 >= r_3 )
    { // r_2 greatest
      down_to = U(r_2);
      denominator = factorial(r_1);
      denominator *= factorial(r_3);
    } // if
    else
    { // r_3 greatest
      down_to = U(r_3);
      denominator = factorial(r_1);
      denominator *= factorial(r_2);
    } // else
  } // else

  U result = 1;
  for(U factor = r_1 + r_2 + r_3; factor > down_to; factor--)
    result *= factor;
  result /= denominator;

  return result;
} // ProbabilityDistributions<U>::combinations_n_choose_r1_r2_r3()

// Count the number of placements of max objs into a stair box-scheme
// given by the levels array (which should point to a uint array with max elements).
// A "stair-box-scheme" says which boxes each obj can be placed in, and how many objs
// each box can hold; this is the same as a pyramid-scheme except it is also assumed
// that each level has no more than 1 less box then the previous, so only 1 or 0 less.
// For example: uint levels[4] = {5,5,4,3,2}; is permissible.
// This restriction vastly reduces calculation time.
// This example is for: ***** note this is isomorphic to ***** or other orderings.
//                      *****                            *****
//                       ****                             ****
//                        ***                             ***
//                         **                              **
template<class U>
U ProbabilityDistributions<U>::count_stair_placements(const uint* levels, const uint max)
{
  // degenerate cases
  if( max == 0 )
    return 1;
  if( max == 1 )
    return levels[0];

  // count all placements with given sizes of boxes
  U result = count_sized_placements(levels, max);

  // now loop to decrement over-packings/placements
  for(uint i = max; i > 2; i--)  // i represents the # of objs going forward into smaller boxes
  {
    if( levels[i-1] == levels[i-2] ) break;  // test for end of stairs
    
    // build the box scheme to count over-packings on
    uint another_levels[i];
    for(uint j = 0; j < i-1; j++)
      another_levels[j] = levels[j] - levels[max-1] - (max - i);
    another_levels[i-1] = 1;

    result -= count_sized_placements(another_levels, i);
    result++;  // add back the count for case of i objs all in box of size i in another_levels (which isn't really suppose to exist, but had to be built to keep another_levels elements equal to i)
  } // for

  return result;
} // ProbabilityDistributions<U>::count_stair_placements()

// Count the number of placements of max objs into a pyramid box-scheme
// given by the levels array (which should point to a uint array with max elements).
// A "pyramid-box-scheme" says which boxes each obj can be placed in, and how many objs
// each box can hold. For example: uint levels[4] = {6,5,4,2};
// describes 6 boxes & 4 objs, 2 boxes can take up to 4 objs, 4 up to 3, 5 up to 2, all 6 at
// least take 1, the 4th obj must go into one of the 2 taking 4, the 3rd obj must go into
// one of the 4 taking 3, the 2nd must go into one of the 5 taking 2, and of course the 1st
// can go into any of the 6 boxes.
// This example is for the "pyramid":  ****** note this is isomorphic to ****** or other orderings.
//                                     *****                              *****
//                                     ****                               ****
//                                     **                                  **
template<class U>
U ProbabilityDistributions<U>::count_pyramid_placements(const uint* levels, const uint max)
{
  // base cases
  if( max == 0 )
    return 1;
  if( max == 1 )
    return levels[0];
  if( max == 2 )
    return combinations_n_place_r(levels[1], 2) + (levels[0] - levels[1])*levels[1];

  // counts case of putting all objs into largest boxes
  U result = combinations_n_place_r(levels[max-1], max);

  // now loop to count placing objs into smaller boxes (1 more each loop)
  for(uint i = 1; i < max; i++)  // i represents the # of objs going into smaller boxes
  {
    // build the box scheme to recurse on
    uint another_levels[i];
    for(uint j = 0; j < i; j++)
      another_levels[j] = levels[j] - levels[max-1];

    // do recursion and tally count
    result += combinations_n_place_r(levels[max-1], max - i) *
              count_pyramid_placements(another_levels, i);
  } // for

  return result;
} // ProbabilityDistributions<U>::count_pyramid_placements()

// Count the number of placements of max objs into a sized-box-scheme
// given by the levels array (which should point to a uint array with max elements).
// A "sized-box-scheme" says how many objs each box can hold (what the boxes' sizes' are).
// For example: uint levels[4] = {6,5,4,2};
// describes 6 boxes & 4 objs, 2 boxes can take up to 4 objs, 4 up to 3, 5 up to 2, all 6 at
// least take 1.
// If each box is a column, it can be drawn:  ****** or also ****** or other orderings.
//                                            *****           *****
//                                            ****            ****
//                                            **                **
template<class U>
U ProbabilityDistributions<U>::count_sized_placements(const uint* levels, const uint max)
{
  U result = combinations_n_place_r(levels[0], max);

  if( max <= 1 ) return result;

  for(uint i = 0; i < max-1; i++)  // i represents the sizes of the boxes being recounted
    for(uint j = 0; j < max-1 - i; j++)  // j represents the # of objs being redistributed from each box of size i, from 0 obj (when taking away the case of packing all max objs into the undersized box) to the difference between how many objs boxes of size i can hold and the max # of objs (which the largest boxes hold)
      result -= U(levels[i] - levels[i+1]) * combinations_n_place_r(levels[0], j);

  return result;
} // ProbabilityDistributions<U>::count_sized_placements()

// Trinomial Distribution Probability Density Function
// calculate probability of getting r_1,r_2,r_3 events of respective types
// out of r_1+r_2+r_3 total events, if prob of event 1 is p_1, 2 is p_2, 3 is 1-p_1-p_2
template<class U>
U ProbabilityDistributions<U>::trinomial_distribution_PDF
(const uint r_1, const uint r_2, const uint r_3, const U p_1, const U p_2)
{
  U probability = combinations_n_choose_r1_r2_r3(r_1, r_2, r_3);
  probability  *= pow(p_1, r_1);
  probability  *= pow(p_2, r_2);
  probability  *= pow(1 - p_1 - p_2, r_3);

  return probability;
} // ProbabilityDistributions<U>::trinomial_distribution_PDF()

// Random Walk with Null-Step Probability Density Function
// calculate probability of a random walk arriving at position x (along Z),
// if the walk starts at 0 and runs n iterations and the probability of
// moving right (+) is p_1, prob of taking NO STEP is p_2, and the
// prob of moving left (-) is 1-p_1-p_2
template<class U>
U ProbabilityDistributions<U>::random_walk_null_step_PDF
(const uint n, const int x, const U p_1, const U p_2)
{
  if( !n || x < -1*int(n) || x > int(n) ) return 0;  // nonsense input

  U probability = 0;

  const uint top = n + 1;  // limit of iteration, set so addition isn't done each time
  if( x < 0 )  // know that r_3 must be at least |x|
    for(uint r_3 = abs(x); r_3 < top; r_3++)
    {
      uint r_1 = r_3 + x;
      uint r_2 = n - r_1 - r_3;
      probability += trinomial_distribution_PDF(r_1, r_2, r_3, p_1, p_2);
    } // for
  else  // know that r_1 must be at least x
    for(uint r_1 = x; r_1 < top; r_1++)
    {
      uint r_3 = r_1 - x;
      uint r_2 = n - r_1 - r_3;
      probability += trinomial_distribution_PDF(r_1, r_2, r_3, p_1, p_2);
    } // for
  
  return probability;
} // ProbabilityDistributions<U>::random_walk_null_step_PDF()

// Random Walk with Null-Step Cumulative Density Function
// calculate probability of a random walk arriving at position >= x,
// (usually CDF means <= x, but the opposite is our convention)
// if the walk starts at 0 and runs n iterations and the probability of
// moving right (+) is p_1, prob of taking NO STEP is p_2, and the
// prob of moving left (-) is 1-p_1-p_2
template<class U>
U ProbabilityDistributions<U>::random_walk_null_step_CDF
(const uint n, const int x, const U p_1, const U p_2)
{
  if( !n || x < -1*int(n) || x > int(n) ) return 0;  // nonsense input

  U probability = 0;

  const int top = int(n + 1);  // limit of iteration, set so addition isn't done each time

  for(int x_prime = x; x_prime < top; x_prime++)
    probability += random_walk_null_step_PDF(n, x_prime, p_1, p_2);
  
  return probability;
} // ProbabilityDistributions<U>::random_walk_null_step_CDF()

// Random Walk with Null-Step Probability of Hitting Time <= n
// calculates the probability of reaching point a within n iterations
// if the walk starts at 0 and runs n iterations and the probability of
// moving right (+) is p_1, prob of taking NO STEP is p_2, and the
// prob of moving left (-) is 1-p_1-p_2
// This is the sum of probs for every possible path of n steps
// that reached x.
template<class U>
U ProbabilityDistributions<U>::random_walk_null_step_prob_to_hit_by
(const uint& n, const int& a, const U& p_1, const U& p_2)
{
  // nonsense input
  if( n < 1 ) return 0;
  if( a == 0 ) return 1;
  if( a < -1*int(n) || a > int(n) ) return 0;

  U probability = 0;
  U p_3 = 1 - p_1 - p_2;

  for(int to_a = abs(a), away_a = 0; to_a + away_a <= int(n);)
  {
    // calculate case of to_a increments
    to_a++;
    // count # of ways to distribute away_a steps among to_a a-steps, hitting a on last step
    // this is isomorphic to counting obj placements into a sized-box-scheme
    uint levels1[away_a];
    for(int i = 1; i <= away_a; i++)
      levels1[i-1] = to_a - i;

    double count = 1;
    if( away_a > 0 )
      count = count_sized_placements(levels1, away_a);

    // add in the case of no NULL-steps
    probability += count * pow(p_1, to_a) * pow(p_3, away_a);

    // iterate # of possible NULL-steps and add in to probability for each actual to_a/to_b/NULL step count
    uint top = n - to_a - away_a;  // no loops if n = to_a + away_a (all steps used already)
    for(uint N = 1; N <= top; N++)
      probability += count * combinations_n_place_r(to_a + away_a, N) *
                     pow(p_1, to_a) * pow(p_2, N) * pow(p_3, away_a);
  
    // calculate case of away_a increments
    to_a--; away_a++;
    if( away_a < to_a )
    {
      uint levels2[away_a];
      for(int i = 1; i <= away_a; i++)
        levels2[i-1] = to_a - i;

      count = 1;
      if( away_a > 0 )
        count = count_sized_placements(levels2, away_a);

      // add in the case of no NULL-steps
      probability += count * pow(p_1, to_a) * pow(p_3, away_a);

      // iterate # of possible NULL-steps and add in to probability for each actual to_a/to_b/NULL step count
      top = n - to_a - away_a;  // no loops if n = to_a + away_a (all steps used already)
      for(uint N = 1; N <= top; N++)
        probability += count * combinations_n_place_r(to_a + away_a, N) *
                       pow(p_1, to_a) * pow(p_2, N) * pow(p_3, away_a);
    } // if

    // calculate case of both to_a and away_a increment
    to_a++;
    uint levels3[away_a];
    for(int i = 1; i <= away_a; i++)
      levels3[i-1] = to_a - i;

    count = 1;
    if( away_a > 0 )
      count = count_sized_placements(levels3, away_a);

    // add in the case of no NULL-steps
    probability += count * pow(p_1, to_a) * pow(p_3, away_a);

    // iterate # of possible NULL-steps and add in to probability for each actual to_a/to_b/NULL step count
    top = n - to_a - away_a;  // no loops if n = to_a + away_a (all steps used already)
    for(uint N = 1; N <= top; N++)
      probability += count * combinations_n_place_r(to_a + away_a, N) *
                     pow(p_1, to_a) * pow(p_2, N) * pow(p_3, away_a);
  } // for

  return probability;
} // ProbabilityDistributions<U>::random_walk_null_step_prob_to_hit_by()

// Random Walk with Null-Step Probability of Hitting a before b within steps <= n
// calculates the probability of reaching point a before reaching point b
// if the walk starts at 0, between a and b, within n iterations, so if a is
// positive b must be negative and vice versa, also the probability of
// moving right (+) is p_1, prob of taking NO STEP is p_2, and the
// prob of moving left (-) is 1-p_1-p_2
// This is the sum of probs for every possible path of n steps
// that reached a before b.
template<class U>
U ProbabilityDistributions<U>::random_walk_null_step_prob_to_a_before_b_by
(const uint n, const int a, const int b, const U p_1, const U p_2)
{
  // nonsense input
  if( n < 1 ) return 0;
  if( a == 0 ) return 1;
  if( b == 0 ) return 0;
  if( a < -1*int(n) || a > int(n) ) return 0;
  if( b < -1*int(n) || b > int(n) ) return 1;
  if( (a < 0 && b < a ) || (a > 0 && b > a ) ) return 1;
  if( (b < 0 && a < b ) || (b > 0 && a > b ) ) return 0;

  U probability = 0;
  U p_a, p_b;
  if( a > 0 )
  { // a is on the right side of 0
    p_a = p_1;
    p_b = 1 - p_1 - p_2;
  } // if
  else
  { // a is on the left side of 0
    p_a = 1 - p_1 - p_2;
    p_b = p_1;
  } // else

  uint abs_b = abs(b);
  int to_b = 0;
  for(int to_a = abs(a); to_a + to_b <= int(n); to_a++, to_b++)
  { // iterate through #s of steps toward b, # toward a must increase for each new b-step
    // count # of ways to distribute to_b b-steps among to_a a-steps, hitting a on last step, without hitting b first
    // this is isomorphic to counting obj placements into a pyramid-box-scheme for the "bottom-end" boxes
    // and a sized-box-scheme for the "top-end" boxes
    // so build the two levels arrays for such a scheme, then submit cnting on it
    int c = 2; // assign 2 to start, in case c isn't assigned (when to_b=0)
    uint levels_top[to_b], levels_bot[to_b-1];
    for(int i = 1; i <= to_b; i++)
    { // top-end boxes levels array built first
      c = to_a - i - (i >= int(abs_b) ? 1 + i - abs_b : 0);
      levels_top[i-1] = c;
    } // for

    if( c <= 0 ) break;

    if( to_b >= int(abs_b) )
    { // if any bottom-end boxes are needed, then modify top-end levels array too
      levels_bot[0] = to_b - abs_b + (abs_b > 1 ? 1 : 0);  // = to_b - (abs_b-1) (unless abs_b=1, in which case abs_b-1=0, but must decrement to_b by at least 1)
      levels_top[0] -= levels_bot[0];
      if( abs_b == 1 )  // treated differently because must decrement even when to_b=1
        for(int i = 1; i < to_b-1; i++)
        { // build bottom-end boxes array and adjust levels for top-end
          levels_bot[i] = levels_bot[0] - i;  // make stairs starting from beginning
          levels_top[i] -= levels_bot[i];
        } // for
      else //  abs_b > 1  -- the usual case
        for(int i = 1; i < to_b-1; i++)
        { // build bottom-end boxes array and adjust levels for top-end
          levels_bot[i] = levels_bot[0] - (i+1 < int(abs_b) ? 0 : 2 + i - abs_b);  // decrement by 1 more than the difference between i+1 and abs_b (i is 0-starting, so once i+1=abs_b decrement by 1 more each iteration until i+1=to_b, makes stairs)
          levels_top[i] -= levels_bot[i];
        } // for
    } // if

    double count = 1;
    if( to_b > 0 )
    { // only time count = 1 is for the case of all to_a steps
      if( to_b >= int(abs_b) && levels_bot[0] )
      { // do count with some in bottom boxes
        count = 0;
        for(int i = 0; i < to_b; i++)  // i represents the # of objs distributed to the bottom boxes
//          count += count_stair_placements(levels_bot, i) *
          count += count_pyramid_placements(levels_bot, i) *
                   count_sized_placements(levels_top, to_b - i);
      } // if
      else  // no bottom boxes
        count = count_sized_placements(levels_top, to_b);    
    } // if
/*
    double count = 1;
    if( max > 0 )
      count = count_sized_placements(levels, max);

    cout << "to_a=" << to_a
         << " to_b=" << to_b
         << " count=" << count
         << " c=" << c
         << " prob=" << probability
         << endl;

    if( count == 209 )
    {
      count = 208;

    for(; count > 0; count--)
    {
    U prob = probability;
    // add in the case of no NULL-steps
    prob += count * pow(p_1, to_a) * pow(p_3, to_b);
    // iterate # of possible NULL-steps and add in to probability for each actual to_a/to_b/NULL step count
    uint top = n - to_a - to_b;  // no loops if n = to_a + to_b (all steps used already)
    uint boxes = to_a + to_b - 1;
    for(uint N = 1; N <= top; N++)
      prob += count * combinations_n_choose_r(boxes + N, N) *
                     pow(p_1, to_a) * pow(p_2, N) * pow(p_3, to_b);
    cout << "count=" << count << " prob=" << prob << endl;
    }
    break;
    
    }
*/
    // add in the case of no NULL-steps
    probability += count * pow(p_a, to_a) * pow(p_b, to_b);

    // iterate # of possible NULL-steps and add in to probability for each actual to_a/to_b/NULL step count
    uint top = n - to_a - to_b;  // no loops if n = to_a + to_b (all steps used already)
    for(uint N = 1; N <= top; N++)
      probability += count * combinations_n_place_r(to_a + to_b, N) *
                     pow(p_a, to_a) * pow(p_2, N) * pow(p_b, to_b);

    if( c <= 1 ) break;
  } // for

//  cout << "to_b=" << to_b << "  ";

  return probability;
} // ProbabilityDistributions<U>::random_walk_null_step_prob_to_a_before_b_by()

template<class U>
U ProbabilityDistributions<U>::random_walk_null_step_prob_to_a_before_b_starting_x_by
(const uint n, const int x, const int a, const int b, const U p_1, const U p_2)
{
//  cout << "x=" << x << endl;

  if( x == a ) return 1;
  if( n == 0 ) return 0;
  if( x == b ) return 0;

  U P_u = p_1*random_walk_null_step_prob_to_a_before_b_starting_x_by(n-1, x+1, a, b, p_1, p_2);
  U P_n = p_2*random_walk_null_step_prob_to_a_before_b_starting_x_by(n-1, x, a, b, p_1, p_2);
  U P_d = (1-p_1-p_2)*random_walk_null_step_prob_to_a_before_b_starting_x_by(n-1, x-1, a, b, p_1, p_2);
//  cout << "n=" << n << " x=" << x << " P_u=" << P_u << " P_n=" << P_n << " P_d=" << P_d << endl;
  return P_u + P_n + P_d;
} // ProbabilityDistributions<U>::random_walk_null_step_prob_to_a_before_b_starting_x_by()


// Random Walk Probability of Hitting a before b within steps <= n, starting from x
// calculates the probability of reaching point a before reaching point b
// if the walk starts at x, between a and b, within n iterations,
// the probability of moving right (+) is p_1, and the
// prob of moving left (-) is p_2
// If a=b then returns prob of hitting a within steps <= n, starting from x
// The method is to recurse through every possible walk.
template<class U>
U ProbabilityDistributions<U>::random_walk_prob_to_a_before_b_starting_x_by
(const uint n, const int x, const int a, const int b, const U p_1, const U p_2, string &str)
{
//  cout << "x=" << x << endl;

  if( x == a )  return 1;
//  { cout << str << " 1" << endl; return 1; }
  if( n == 0 ) return 0;
//  { cout << str << " 0" << endl; return 0; }
  if( x == b ) return 0;
//  { cout << str << " 0" << endl; return 0; }

  string str_u = str;
  str_u += '+';
  U P_u = p_1*random_walk_prob_to_a_before_b_starting_x_by(n-1, x+1, a, b, p_1, p_2, str_u);
  string str_d = str;
  str_d += '-';
  U P_d = p_2*random_walk_prob_to_a_before_b_starting_x_by(n-1, x-1, a, b, p_1, p_2, str_d);
//  cout << "x=" << x << "  " << "P_u=" << P_u << "  " << "P_d=" << P_d << endl;
  return P_u + P_d;

//  return p_1*random_walk_prob_to_a_before_b_starting_x_by(n-1, x+1, a, b, p_1, p_2)
//       + p_2*random_walk_prob_to_a_before_b_starting_x_by(n-1, x-1, a, b, p_1, p_2);
} // ProbabilityDistributions<U>::random_walk_prob_to_a_before_b_starting_x_by()

// Random Walk Probability of Hitting a before b within steps <= n, starting from x
// calculates the probability of reaching point a before reaching point b
// if the walk starts at 0, between a and b, within n iterations,
// the probability of moving right (+) is p_1, and the
// prob of moving left (-) is p_2
template<class U>
U ProbabilityDistributions<U>::random_walk_prob_to_a_before_b_by
(const uint n, const int a, const int b, const U p_1, const U p_2)
{
  // nonsense input
  if( n < 1 ) return 0;
  if( a == 0 ) return 1;
  if( a < -1*int(n) || a > int(n) ) return 0;
  if( b < -1*int(n) || b > int(n) ) return 1;
  if( (a < 0 && b < a ) || (a > 0 && b > a ) ) return 1;
  if( (b < 0 && a < b ) || (b > 0 && a > b ) ) return 0;

  U probability = 0;
  U p_3 = 1 - p_1 - p_2;

  uint abs_b = abs(b);
  for(int to_a = abs(a), to_b = 0; to_a + to_b <= n; to_a++, to_b++)
  { // iterate through #s of steps toward b, # toward a must increase for each new b-step
    // count # of ways to distribute to_b b-steps among to_a a-steps, hitting a on last step, without hitting b first
    // this is isomorphic to counting obj placements into a pyramid-box-scheme for the "bottom-end" boxes
    // and a sized-box-scheme for the "top-end" boxes
    // so build the two levels arrays for such a scheme, then submit cnting on it
    int c;
    uint levels_top[to_b], levels_bot[to_b-1];
    for(uint i = 1; i <= to_b; i++)
    { // top-end boxes levels array built first
      c = to_a - i - (i >= abs_b ? 1 + i - abs_b : 0);
      levels_top[i-1] = c;
    } // for

    if( c <= 0 ) break;

    if( to_b >= abs_b )
    { // if any bottom-end boxes are needed, then modify top-end levels array too
      uint left_boxes = to_b - abs_b + (abs_b > 1 ? 1 : 0);  // = to_b - (abs_b-1) (unless abs_b=1, in which case abs_b-1=0, but must decrement to_b by at least 1)
      for(uint i = 0; i < to_b; i++)
      { // build bottom-end boxes array and adjust levels for top-end
        levels_top[i] = levels_top[i] - left_boxes + i;  // - (left_boxes-i)
        levels_bot[i] = left_boxes - i;
      } // for
    } // if

    double count = 1;
    if( to_b > 0 )
    { // only time count = 1 is for the case of all to_a steps
      if( to_b >= abs_b )
      { // do count with some in bottom boxes
        count = 0;
        for(uint i = 0; i < to_b; i++)  // i represents the # of objs distributed to the bottom boxes
          count += count_pyramid_placements(levels_bot, i) *
                   count_sized_placements(levels_top, to_b - i);
      } // if
      else  // no bottom boxes
        count = count_sized_placements(levels_top, to_b);    
    } // if

    // add in the probability for this to_a,to_b case
    probability += count * pow(p_1, to_a) * pow(p_3, to_b);

    if( c <= 1 ) break;
  } // for

  return probability;
} // ProbabilityDistributions<U>::random_walk_prob_to_a_before_b_by()


  // The walk needs to have reached a, and can only have done so by
  // having taken at least |a| steps right (or left) toward a, a-steps.
  // After accounting for reaching a the walk can go anywhere with the
  // remaining n-|a| steps, as long as it does not reach b, so there
  // must be less than |a-b| unmatched b-steps, in the remaining
  // n-|a|. But there may not be any unmatched b-steps, there might be
  // a surplus of a-steps beyond matches to b-steps and beyond getting
  // that needed to reach a, but only up to n-|a| of these.
  // So suppose there are x steps (toward a or b) that are
  // matched to complimentary (a or b) steps. So 2x steps altogether
  // that are nullified. Suppose also there are y a-steps that are
  // unmatched and z b-steps that are unmatched, clearly either y or
  // z is 0. The total number of a-steps is x+y, likewise x+z b-steps.
  // The balance of steps, beyond matched and unmatched, must be
  // NULL-steps (staying in place), say N of them.
  // There are three cases:
  // y > 0 : z = 0, n - |a| = y + 2x - |a| + N, y + x - |a| < n - |a| - N
  // z > 0 : y = 0, n - |a| = z + 2x - |a| + N. z < |a-b|
  // y = z = 0 : n - |a| = 2x - |a| + N


// Return every walk as dllist of int dllists.
// Walks are assumed to start at 0, algorithm will return every walk
// that hit's point a>0 before hitting point b<0, within t steps.
// NOTE: The 1st int of each walk dllist DOES NOT represent the 1st step
//   of the walk, it is a report on whether the walk hit a or b by t.
//  If the walk hit a, then the 1st int will be 1.
//  If the walk hit b, then the 1st int will be -1.
//  If the walk did not reach a or b within t steps, then 1st int is 0.
// WARNING: The algorithm is very vulernable to combinatorial explosion.
//  Be sure there is sufficient heap memory available for resulting list.
// Returns: true  - execution completed normally
//          false - recursion limit was reached
template<class U>
bool ProbabilityDistributions<U>::make_jump_walks_that_hit_a_or_b_by_t
(const int a, const int b, const int t, const int num_types, const int* step_types, dllist<jump_walk_struct>& walks)
{
  // test for walk termination states
  if( a <= 0 )  // a > 0 unless stepped past threshold
  { // walk reached a before hitting b
    for(dllit<jump_walk_struct> i(walks); !i.done(); ++i)
      if( !i()->result ) i()->result = 1;
    return true;
  } // if
  if( b >= 0 )  // b < 0 unless stepped past threshold
  { // walk reached b before hitting a
    for(dllit<jump_walk_struct> i(walks); !i.done(); ++i)
      if( !i()->result ) i()->result = 2;
    return true;
  } // if
  if( t == 0 )
  { // walk reached maximum steps without hitting a or b
    for(dllit<jump_walk_struct> i(walks); !i.done(); ++i)
      if( !i()->result ) i()->result = 3;
    return true;
  } // if

  // double check walks list isn't empty
  if( walks.empty() )
  { // seed the list
    jump_walk_struct* new_walk = new jump_walk_struct;
    walks.add( new_walk );
  } // if

  // termination has not occurred so process steps for each walk
  for(dllit<jump_walk_struct> i(walks); !i.done(); ++i)
    if( !i()->result )
    { // initiate a new walk (ie step list) for every possible step type
      dllist<jump_walk_struct> new_list_for_i;

      for(int j = 0; j < num_types; j++)
      { // for each step type, recurse -- generates a new list for the result
        jump_walk_struct* new_walk = new jump_walk_struct;
        new_walk->steps = i()->steps;
        new_walk->steps.push_back( step_types[j] );  // add the j step to the new walk
        dllist<jump_walk_struct> new_list_for_j;
        new_list_for_j.add( new_walk );
        make_jump_walks_that_hit_a_or_b_by_t(a-step_types[j], b-step_types[j], t-1, num_types, step_types, new_list_for_j);
        new_list_for_i += new_list_for_j;  // add the new walks, for step type j, to set of new walks for walks[i]
      } // for

      walks.push_list( new_list_for_i );  // join the list created by recursion for walk walks[i]
      i.remove_delete();  // the list added now replaces walk walks[i]
    } // if

  return true;
} // ProbabilityDistributions<U>::make_jump_walks_that_hit_a_or_b_by_t()


} // namespace blib

// stats.cxx

