//-------------------------------------------------------------------------
// Filename      : PyramidMetric.hpp 
//
// Purpose       : This class performs metric calculations on Pyramid Elements.
//
// Special Notes : 
//
// Creator       : Ray Meyers
//
// Date          : 02/26/99
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------

#ifndef PYRAMID_METRIC_HPP
#define PYRAMID_METRIC_HPP

#include <deque>

#include "QualitySummary.hpp"
#include "Metric.hpp"

enum pyramidMetrics {PYRAMID_VOLUME,NUM_PYRAMID_METRICS};

class PyramidRef;
class FailedPyramid;
class VerdeVector;

//! calculates metrics for pyramids
class PyramidMetric: public Metric
{
   //! singleton pointer  
   static PyramidMetric *instance_;
   
   //double jacobian(VerdeVector &side01, VerdeVector &side12,
   //                VerdeVector &side20, VerdeVector &side03,
   //                VerdeVector &side13, VerdeVector &side23,
   //                double *normalized_jacobian = NULL );
   //- Return the minimum of the jacobian at the center and nodes of
   //- the pyramid.  If normalized_jacobian is specified, return in it
   //- the minimum of ( jacobian normalized by the product of the edge
   //- lengths at center and at nodes ).

   //double condition(VerdeVector &side01, VerdeVector &side20, 
   //                 VerdeVector &side03);
   // returns the maximum condition number over the 4 vertices
   

   //! array of metric values used in computations
   double metrics[NUM_PYRAMID_METRICS];
   
   //! array of character names, on for each metric
   static const char* metricNames[NUM_PYRAMID_METRICS];

   //! holds summary data for the metric from all elements processed
   QualitySummary summary[NUM_PYRAMID_METRICS];

   //! contains a list of pyramides with values outside valid range for metric
   std::deque<FailedPyramid*> failedPyramids[NUM_PYRAMID_METRICS];

   //! contains valid ranges for each metric
   static struct min_max range[NUM_PYRAMID_METRICS];

   //! contains hard-coded default ranges for each metric
   static const struct min_max range_backup[NUM_PYRAMID_METRICS];
   
   //! counts the total number of pyramids in failedPyramids lists
   int number_failed_pyramids();

   //! holds the volume of all elements processed
   double totalVolume;

   //! gets volume of a tet portion of the pyramid
   double tet_volume (VerdeVector &node_pos_0,
                      VerdeVector &node_pos_1,
                      VerdeVector &node_pos_2,
                      VerdeVector &node_pos_3);

   //! gets the volume of a pyramid
   double volume (VerdeVector node_pos[5]);

public:

   //! Constructor
   PyramidMetric();

   //! Destructor
   ~PyramidMetric ();

   //! calculates metrics for the pyramid represented by the 4 nodeRefs
   int calculate(PyramidRef &pyramid_ref);
   
   //! calculates metrics for the pyramid represented by the 4 vectors
   int calculate(VerdeVector node_position[4], int which_metric = -1);

   //! Access function.
   double metric(const int i) const;

   //! print summary of gathered metrics
   void print_summary();


   //! access to singleton pointer
   static PyramidMetric *instance();

   //! prints a summary of pyramids which failed metric tests
   int print_failed_pyramids(VerdeBoolean verbose);

   //! returns the number of metrics defined
   int number_metrics();

   //! returns the failed pyramid list for metric at index
   void failed_pyramid_list(int index, 
       std::deque<FailedPyramid*> &pyramid_list);

   //! returns the string name of metric at index
   const char *metric_name(int index);

   //! resets the tool for a new set of values
   void reset();

   //! resets the tool and deletes failed pyramids
   void reset_all();

   // reset the ranges to hard-code defaults
   void reset_range();

   //! resets the total volume to zero
   void reset_volume();

   //! returns the volume of all pyramids processed
   double volume();
   
   //! set min value of range parameter
   void set_min(pyramidMetrics metric, double value);

   //! set max value of range parameter
   void set_max(pyramidMetrics metric, double value);
   
   //! get min value of range parameter
   double get_min(pyramidMetrics metric);

   //! get max value of range parameter
   double get_max(pyramidMetrics metric);
   
};

// inline functions

inline double PyramidMetric::metric(const int i) const
{return metrics[i];}

inline int PyramidMetric::number_metrics()
{
   return NUM_PYRAMID_METRICS;
}

inline const char *PyramidMetric::metric_name(int index)
{
   return metricNames[index];
}

inline double PyramidMetric::volume()
{
   return totalVolume;
}

inline void PyramidMetric::set_min(pyramidMetrics metric, double value)
{
   range[metric].min = value;
}

inline void PyramidMetric::set_max(pyramidMetrics metric, double value)
{
   range[metric].max = value;
}

inline double PyramidMetric::get_min(pyramidMetrics metric)
{
	return   range[metric].min;
}
 
inline double PyramidMetric::get_max(pyramidMetrics metric)
{
	return   range[metric].max;
} 

inline void PyramidMetric::reset_volume()
{
   totalVolume = 0.0;
}

#endif

