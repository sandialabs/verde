#include "QualitySummary.hpp"
#include "VerdeMessage.hpp"
#include "VerdeDefines.hpp"
#include <math.h>

 void QualitySummary::add_sample(const double sample, int element_id)
{
  // set a tolerance so that differences in machine calcs do not change min/max ids
  const double tolerance = 1e-08;
  numSamples++;

  double delta = (sample - meanValue);
  meanValue += delta / (double) numSamples;

  double delta2 = sample*sample - meanSumSquares;
  meanSumSquares += delta2/(double)numSamples;  

  if (numSamples == 1) {
    maxValue   = sample;
    maxID = element_id;
    minValue   = sample;
    minID = element_id;
  }
  else if (sample - tolerance > maxValue) {
    maxValue   = sample;
    maxID = element_id;
  }
  else if (sample + tolerance < minValue) {
    minValue   = sample;
    minID = element_id;
  }
}

 void QualitySummary::reset()
{
   meanValue  = 0.0;
   meanSumSquares = 0.0;
   minValue   = VERDE_DBL_MAX;
   maxValue   = 0.0;  // All metrics are greater than zero.
   numSamples = 0;
   minID = -1;
   maxID = -1;
}

 QualitySummary::QualitySummary()
{
  reset();
}

/*!
gets the format precision for a printf format for a double.
For example for the format %12.xf, where x is the precision,
this routine returns the correct precision such that any number
will be formatted within the 12 character field (unless it is simply
too large, where it will overflow, or too small where it gets zero)
*/
int precision(int total_size, double number)
{
  // find the size of the number left of the decimal, put it in places

  int places;
  if( fabs(number) < 1.0e-20 )
    places = 0;
  else
    places = (int) floor(log10(fabs(number)));
  
  // if the left hand size is less than zero, make it zero

  if (places < 0)
  {
    places = 0;
  }
  
  // now, get the real places by subtracting the entire field width

  places = total_size - places;
  
  // if places is negative, make it zero

  if (places <= 0)
  {
    places = 1;
  }
  return places;
}

void QualitySummary::print() const
{

  // format the strings using the precision function to find how to
  // format them

  double mean_val = mean();
  double std_dev_val = std_dev();
  double min_val = min_value();
  double max_val = max_value();
  
  char format[80];
  sprintf(format,"%% 7.%df  %% 5.%df  %% 7.%df (%%d) %% 7.%df (%%d)\n",
          precision(7,mean_val),precision(5,std_dev_val),
          precision(7,min_val),precision(7,max_val));
  
  PRINT_INFO(format,
	     mean_val, std_dev_val,
	     min_val, minID,
	     max_val, maxID);
}

double QualitySummary::std_dev() const 
{

//  if (numSamples <= 1)
//     return 0.0;
  
//   double result = sqrt(1.0/((double)numSamples - 1.0));
//   double temp = fabs(sumSquares - (double) numSamples * meanValue * meanValue);
//   result *= temp;
//   return result;
  
 if (numSamples <= 1)
    return 0.0;
  
  double result = 1.0/(numSamples - 1.0);
  double temp = fabs((meanSumSquares - meanValue * meanValue)*numSamples);
  result *= temp;
  result = sqrt(result);
  
  return result;

  // return numSamples <= 1 ? 0.0 : sqrt(1.0/(numSamples - 1.0) *
//		       fabs(sumSquares - numSamples * meanValue * meanValue));
}
