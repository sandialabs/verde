//-------------------------------------------------------------------------
// Filename      : TetMetric.hpp 
//
// Purpose       : This class performs metric calculations on Tet Elements.
//
// Special Notes : 
//
// Creator       : Ray Meyers
//
// Date          : 11/10/98
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------

#ifndef TET_METRIC_HPP
#define TET_METRIC_HPP

#include <deque>

#include "QualitySummary.hpp"
#include "Metric.hpp"

enum tetMetrics {TET_SHEAR, TET_SHAPE, TET_RELSIZE, TET_SHAPE_SIZE,
		 TET_VOLUME, ASPECT_RATIO, ASPECT_RATIO_GAMMA, 
                 TET_CONDITION_NUMBER, TET_JACOBIAN,
                 TET_NORM_JACOBIAN, NUM_TET_METRICS};

class TetRef;
class FailedTet;

//! performs metric calculations on Tet Elements
class TetMetric: public Metric
{  
   //! singleton pointer
   static TetMetric *instance_;
   
   //! Return the minimum of the jacobian at the center and nodes of
   //! the tet.  If normalized_jacobian is specified, return in it
   //! the minimum of ( jacobian normalized by the product of the edge
   //! lengths at center and at nodes ).
   double jacobian(VerdeVector &side01, VerdeVector &side12,
                   VerdeVector &side20, VerdeVector &side03,
                   VerdeVector &side13, VerdeVector &side23,
                   double *normalized_jacobian = NULL );

   //! returns the maximum condition number over the 4 vertices
   double condition(VerdeVector &side01, VerdeVector &side20, 
                    VerdeVector &side03);
   
   //! array of metric values used in computations
   double metrics[NUM_TET_METRICS];
   
   //! array of character names, on for each metric
   static const char* metricNames[NUM_TET_METRICS];

   //! holds summary data for the metric from all elements processed
   QualitySummary summary[NUM_TET_METRICS];

   //! contains a list of tetes with values outside valid range for metric
   std::deque<FailedTet*> failedTets[NUM_TET_METRICS];

   //! contains valid ranges for each metric
   static min_max range[NUM_TET_METRICS];

   //! contains valid backup ranges for each metric
   static const min_max range_backup[NUM_TET_METRICS];
   
   //! holds the volume of all elements processed
   double totalVolume;

   //! gets weighted vectors for several metrics calculations
   void get_weight( VerdeVector &w1, VerdeVector &w2, VerdeVector &w3 );

   //! returns the minimum shear over the 4 verticies
   double shear(VerdeVector s0, VerdeVector s1, VerdeVector s2,
               VerdeVector s3, VerdeVector s4, VerdeVector s5);

   //! returns the shape metric for the tet 
   double shape(VerdeVector xxi, VerdeVector xet, VerdeVector xze);

   //! returns the shape metric for the tet 
   double rel_size(VerdeVector xxi, VerdeVector xet, VerdeVector xze);

   //! returns the shape_size metric for the tet
   double shape_size(VerdeVector xxi, VerdeVector xet, VerdeVector xze);


public:

   //! constructor
   TetMetric();

   //! destructor
   ~TetMetric ();

   //! calculates metrics for the tet represented by the 4 nodeRefs
   int calculate(TetRef &tet_ref,int which_metric = -1);
   
   //! calculates metrics for the tet represented by the 4 vectors
   int calculate(VerdeVector node_position[4], int which_metric = -1);

   //! Access function.
   double metric(const int i) const;

   //! print summary of gathered metrics
   void print_summary();

   //! access to singleton pointer
   static TetMetric *instance();

   //! prints a summary of tets which failed metric tests
   int print_failed_tets(VerdeBoolean verbose);

   //! returns the number of metrics defined
   int number_metrics();

   //! returns the failed tet list for metric at index
   void failed_tet_list(int index, std::deque<FailedTet*> &tet_list);

   //! counts the total number of tets in failedTets lists
   int number_failed_tets();

   //! returns the string name of metric at index
   const char *metric_name(int index);

   //! resets the tool for a new set of values
   void reset();

   //! resets the tool and deletes failed tets
   void reset_all();

   //! reset ranges back to hard-coded defaults
   void reset_range();

   //! resest the total volume to zero
   void reset_volume();

   //! returns the volume of all tets processed
   double volume();
   
   //! set min value of range parameter
   void set_min(tetMetrics metric, double value);

   //! set max value of range parameter
   void set_max(tetMetrics metric, double value);
   
   //! get min value of range parameter
   double get_min(tetMetrics metric);

   //! get max value of range parameter
   double get_max(tetMetrics metric);

   //! special function used to make most efficient initial pass of finding
   //! average size of tets needed for main calculate loop
   double volume(TetRef &tet_ref);
  
};

// inline functions

inline double TetMetric::metric(const int i) const
{return metrics[i];}

inline int TetMetric::number_metrics()
{
   return NUM_TET_METRICS;
}

inline const char *TetMetric::metric_name(int index)
{
   return metricNames[index];
}

inline double TetMetric::volume()
{
   return totalVolume;
}

inline void TetMetric::set_min(tetMetrics metric, double value)
{
   range[metric].min = value;
}

inline void TetMetric::set_max(tetMetrics metric, double value)
{
   range[metric].max = value;
}


inline double TetMetric::get_min(tetMetrics metric)
{
	return   range[metric].min;
}
 
inline double TetMetric::get_max(tetMetrics metric)
{
	return   range[metric].max;
} 

inline void TetMetric::reset_volume()
{
   totalVolume = 0.0;
}

#endif

