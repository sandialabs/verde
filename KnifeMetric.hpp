//-------------------------------------------------------------------------
// Filename      : KnifeMetric.hpp 
//
// Purpose       : This class performs metric calculations on Knife Elements.
//
// Special Notes : 
//
// Creator       : Ray Meyers
//
// Date          : 03/10/99
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------

#ifndef KNIFE_METRIC_HPP
#define KNIFE_METRIC_HPP

#include <deque>

#include "QualitySummary.hpp"
#include "Metric.hpp"

enum knifeMetrics {KNIFE_VOLUME,NUM_KNIFE_METRICS};

class KnifeRef;
class FailedKnife;
class VerdeVector;

//! calcualtes metrics for knives
class KnifeMetric: public Metric
{
   //! singleton pointer  
   static KnifeMetric *instance_;
   
   //double jacobian(VerdeVector &side01, VerdeVector &side12,
   //                VerdeVector &side20, VerdeVector &side03,
   //                VerdeVector &side13, VerdeVector &side23,
   //                double *normalized_jacobian = NULL );
   //- Return the minimum of the jacobian at the center and nodes of
   //- the knife.  If normalized_jacobian is specified, return in it
   //- the minimum of ( jacobian normalized by the product of the edge
   //- lengths at center and at nodes ).

   //double condition(VerdeVector &side01, VerdeVector &side20, 
   //                 VerdeVector &side03);
   // returns the maximum condition number over the 4 vertices
   

   //! array of metric values used in computations
   double metrics[NUM_KNIFE_METRICS];
   
   //! array of character names, on for each metric
   static const char* metricNames[NUM_KNIFE_METRICS];

   //! holds summary data for the metric from all elements processed
   QualitySummary summary[NUM_KNIFE_METRICS];

   //! contains a list of knifees with values outside valid range for metric
   std::deque<FailedKnife*> failedKnives[NUM_KNIFE_METRICS];

   //! contains valid ranges for each metric
   static min_max range[NUM_KNIFE_METRICS];

   //! contains hard-coded ranges for each metric
   static const min_max range_backup[NUM_KNIFE_METRICS];

   //! counts the total number of knives in failedKnifes lists
   int number_failed_knives();

   //! holds the volume of all elements processed
   double totalVolume;

   //! gets volume of a tet portion of the knife
   double tet_volume (VerdeVector &node_pos_0,
                      VerdeVector &node_pos_1,
                      VerdeVector &node_pos_2,
                      VerdeVector &node_pos_3);

   //! gets the volume of a knife
   double volume (VerdeVector node_pos[7]);

public:

   //! Constructor
   KnifeMetric();

   //! Destructor
   ~KnifeMetric ();

   //! calculates metrics for the knife represented by the 4 nodeRefs
   int calculate(KnifeRef &knife_ref);
   
   //! calculates metrics for the knife represented by the 7 vectors
   int calculate(VerdeVector node_position[7], int which_metric = -1);

   //! Access function.
   double metric(const int i) const;

   //! print summary of gathered metrics
   void print_summary();

   //! access to singleton pointer
   static KnifeMetric *instance();

   //! prints a summary of knifes which failed metric tests
   int print_failed_knives(VerdeBoolean verbose);

   //! returns the number of metrics defined
   int number_metrics();

   //! returns the failed knife list for metric at index
   void failed_knife_list(int index, std::deque<FailedKnife*> &knife_list);

   //! returns the string name of metric at index
   const char *metric_name(int index);

   //! resets the tool for a new set of values
   void reset();

   //! resets the tool and deletes failed knives
   void reset_all();

   //! resets the ranges back to hard-code defaults
   void reset_range();

   //! resets the volume to zero
   void reset_volume();

   //! returns the volume of all knifes processed
   double volume();
   
   //! set min value of range parameter
   void set_min(knifeMetrics metric, double value);

   //! set max value of range parameter
   void set_max(knifeMetrics metric, double value);

   //! get min value of range parameter
   double get_min(knifeMetrics metric);

   //! get max value of range parameter
   double get_max(knifeMetrics metric);
   
};

// inline functions

inline double KnifeMetric::metric(const int i) const
{return metrics[i];}

inline int KnifeMetric::number_metrics()
{
   return NUM_KNIFE_METRICS;
}

inline const char *KnifeMetric::metric_name(int index)
{
   return metricNames[index];
}

inline double KnifeMetric::volume()
{
   return totalVolume;
}

inline void KnifeMetric::set_min(knifeMetrics metric, double value)
{
   range[metric].min = value;
}

inline void KnifeMetric::set_max(knifeMetrics metric, double value)
{
   range[metric].max = value;
}

inline double KnifeMetric::get_min(knifeMetrics metric)
{
	return range[metric].min;
}
 
inline double KnifeMetric::get_max(knifeMetrics metric)
{
	return range[metric].max;
} 

inline void KnifeMetric::reset_volume()
{
   totalVolume = 0.0;
}

#endif

