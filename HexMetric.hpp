//-------------------------------------------------------------------------
// Filename      : HexMetric.hpp 
//
// Purpose       : This class performs metric calculations on Hex Elements.
//
// Special Notes : 
//
// Creator       : Ray Meyers
//
// Date          : 11/10/98
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------

#ifndef HEX_METRIC_HPP
#define HEX_METRIC_HPP

#include <deque>

#include "Metric.hpp"
#include "QualitySummary.hpp"
#include "VerdeVector.hpp"

//! possible hex metrics
enum hexMetrics {HEX_SHEAR, HEX_SHAPE, HEX_RELSIZE, HEX_SHAPE_SIZE, 
                 HEX_VOLUME, HEX_ASPECT, HEX_SKEW, HEX_TAPER, 
		 HEX_STRETCH, HEX_DIAGONALS, HEX_CHARDIM, HEX_ODDY, 
		 HEX_CONDITION, HEX_JACOBIAN, HEX_NORM_JACOBIAN, 
                 NUM_HEX_METRICS};

class HexRef;
class FailedHex;

//! calculates metrics for hexes
class HexMetric: public Metric
{  

public:

   //! Constructor
   HexMetric();

   //! Destructor
   ~HexMetric ();

   //! calculates metrics for the hex represented by the 8 nodeRefs
   int calculate(HexRef &hex_ref, int which_metric = -1);
   
   //! calculate the metrics for the hex represented by the 8 vectors
   int calculate(VerdeVector node_position[8], int which_metric = -1);

   //! Access function.
   double metric(const int i) const;

   //! special function used to make most efficient initial pass of finding
   //! average size of hexes needed for main calculate loop
   double volume(HexRef &hex_ref);
   
   //! print summary of gathered metrics
   void print_summary();

   //! prints a summary of hexes which failed metric tests
   int print_failed_hexes(VerdeBoolean verbose);

   //! access function for the singleton pointer
   static HexMetric *instance();

   //! returns the number of metrics defined
   int number_metrics();

   //! returns the failed hex list for metric at index
   void failed_hex_list(int index, std::deque<FailedHex*> &hex_list);

   //! counts the total number of hexes in failedHexes lists
   int number_failed_hexes();

   //! returns the string name of metric at index
   const char *metric_name(int index);

   //! resets the tool for a new set of values
   void reset();

   //! reset ranges back to hard-coded defaults
   void reset_range();

   //! returns the volume of all hexes processed
   double volume();

   //! set min value of range parameter
   void set_min(hexMetrics metric, double value);

   //! set max value of range parameter
   void set_max(hexMetrics metric, double value);

   //! get min value of range parameter
   double get_min(hexMetrics metric);

   //! get max value of range parameters
   double get_max(hexMetrics metric);

   //! resets the tool as well as deletes failed hexes
   void reset_all();

   //! resets the volume stored
   void reset_volume();

   

private:
  
   //! singleton pointer 
   static HexMetric *instance_;

   //! needed in addition to aspectRatio, only for Hexes
   double aspectRatio2;

   //! utility function for HexMetric
   void localize_hex_coordinates(VerdeVector node_pos[]);
   void calculate_hex_efg(const VerdeVector node_pos[], VerdeVector efg[]);
   double characteristic_length(VerdeVector position[]);
   double oddy_comp( VerdeVector &xxi, VerdeVector &xet, VerdeVector &xze );
   double condition_comp( VerdeVector &xxi, VerdeVector &xet, VerdeVector &xze);
   void get_weight( VerdeVector &w1, VerdeVector &w2, VerdeVector &w3 );
   void calc_jac_oddy_and_condition(VerdeVector node_pos[],
                                    double &jacobian, double &norm_jacobian,
                                    double &oddy, double &condition);
   
   
   //! array to hold values calculated for current element
   double metrics[NUM_HEX_METRICS];

   //! holds the volume of all elements processed
   double totalVolume;
   
   //! contains character names of each metric
   static const char* metricNames[NUM_HEX_METRICS];

   //! contains summary information for each metric
   QualitySummary summary[NUM_HEX_METRICS];

   //! contains a list of hexes with values outside valid range for metric
   std::deque<FailedHex*> failedHexes[NUM_HEX_METRICS];

   //! contains valid ranges for each metric
   static min_max range[NUM_HEX_METRICS];

   //! contains hard-code default ranges for each metric
   static const min_max range_backup[NUM_HEX_METRICS];

   //! Shear metric on a hex
   double calculate_shear(VerdeVector position[]);

   //! Shape metric on a hex
   double calculate_shape(VerdeVector position[]);

   //! Relative Size metric on a hex
   double calculate_relative_size(VerdeVector position[]);
  
};

// inline functions

inline double HexMetric::metric(const int i) const
{return metrics[i];}

inline double HexMetric::volume()
{return totalVolume;}

inline int HexMetric::number_metrics()
{
   return NUM_HEX_METRICS;
}

inline const char *HexMetric::metric_name(int index)
{
   return metricNames[index];
}

inline void HexMetric::set_min(hexMetrics metric, double value)
{
   range[metric].min = value;
}

inline void HexMetric::set_max(hexMetrics metric, double value)
{
   range[metric].max = value;
}

inline double HexMetric::get_min(hexMetrics metric)
{
	return range[metric].min;
}

inline double HexMetric::get_max(hexMetrics metric)
{
	return range[metric].max;
}

inline void HexMetric::reset_volume()
{
   totalVolume = 0.0;
}


#endif

