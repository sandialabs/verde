//-------------------------------------------------------------------------
// Filename      : WedgeMetric.hpp 
//
// Purpose       : This class performs metric calculations on Wedge Elements.
//
// Special Notes : 
//
// Creator       : Ray Meyers
//
// Date          : 03/10/99
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------

#ifndef WEDGE_METRIC_HPP
#define WEDGE_METRIC_HPP

#include <deque>

#include "QualitySummary.hpp"
#include "Metric.hpp"

enum wedgeMetrics {WEDGE_VOLUME,NUM_WEDGE_METRICS};

class WedgeRef;
class FailedWedge;
class VerdeVector;


//! This class performs metric calculations on Wedge Elements.
class WedgeMetric: public Metric
{
   //! pointer to singleton of class  
   static WedgeMetric *instance_;
   
   //double jacobian(VerdeVector &side01, VerdeVector &side12,
   //                VerdeVector &side20, VerdeVector &side03,
   //                VerdeVector &side13, VerdeVector &side23,
   //                double *normalized_jacobian = NULL );
   //- Return the minimum of the jacobian at the center and nodes of
   //- the wedge.  If normalized_jacobian is specified, return in it
   //- the minimum of ( jacobian normalized by the product of the edge
   //- lengths at center and at nodes ).

   //double condition(VerdeVector &side01, VerdeVector &side20, 
   //                 VerdeVector &side03);
   // returns the maximum condition number over the 4 vertices
   

   //! array of metric values used in computations
   double metrics[NUM_WEDGE_METRICS];
   
   //! array of character names, on for each metric
   static const char* metricNames[NUM_WEDGE_METRICS];

   //! holds summary data for the metric from all elements processed
   QualitySummary summary[NUM_WEDGE_METRICS];

   //! contains a list of wedgees with values outside valid range for metric
   std::deque<FailedWedge*> failedWedges[NUM_WEDGE_METRICS];

   //! contains valid ranges for each metric
   static min_max range[NUM_WEDGE_METRICS];

   //! contains hard-code default ranges for each metric
   static const min_max range_backup[NUM_WEDGE_METRICS];

   //! counts the total number of wedges in failedWedges lists
   int number_failed_wedges();

   //! holds the volume of all elements processed
   double totalVolume;

   //! gets volume of a tet portion of the wedge
   double tet_volume (VerdeVector &node_pos_0,
                      VerdeVector &node_pos_1,
                      VerdeVector &node_pos_2,
                      VerdeVector &node_pos_3);

   //! gets the volume of a wedge
   double volume (VerdeVector node_pos[5]);

public:

   //! Constructor
   WedgeMetric();

   //! Destructor
   ~WedgeMetric ();

   //! calculates metrics for the wedge represented by the 4 nodeRefs
   int calculate(WedgeRef &wedge_ref);
   
   //! calculates metrics for the wedge represented by the 6 vectors
   int calculate(VerdeVector node_position[6], int which_metric = -1);

   //! Access function.
   double metric(const int i) const;

   //! print summary of gathered metrics
   void print_summary();


   //! access to singleton pointer
   static WedgeMetric *instance();

   //! prints a summary of wedges which failed metric tests
   int print_failed_wedges(VerdeBoolean verbose);

   //! returns the number of metrics defined
   int number_metrics();

   //! returns the failed wedge list for metric at index
   void failed_wedge_list(int index, std::deque<FailedWedge*> &wedge_list);

   //! returns the string name of metric at index
   const char *metric_name(int index);

   //! resets the tool for a new set of values
   void reset();

   // resets the tool and deletes failed wedges
   void reset_all();
  
   // reset ranges back to hard-code defaults
   void reset_range();
   
   //! resets the total volume to zero
   void reset_volume();
 
   //! returns the volume of all wedges processed
   double volume();
   
   //! set min value of range parameter
   void set_min(wedgeMetrics metric, double value);

   //! set max value of range parameter
   void set_max(wedgeMetrics metric, double value);
   
   //! get min value of range parameter
   double get_min(wedgeMetrics metric);

   //! get max value of range parameter
   double get_max(wedgeMetrics metric);
   
};

// inline functions

inline double WedgeMetric::metric(const int i) const
{return metrics[i];}

inline int WedgeMetric::number_metrics()
{
   return NUM_WEDGE_METRICS;
}

inline const char *WedgeMetric::metric_name(int index)
{
   return metricNames[index];
}

inline double WedgeMetric::volume()
{
   return totalVolume;
}

inline void WedgeMetric::set_min(wedgeMetrics metric, double value)
{
   range[metric].min = value;
}

inline void WedgeMetric::set_max(wedgeMetrics metric, double value)
{
   range[metric].max = value;
}


inline double WedgeMetric::get_min(wedgeMetrics metric)
{
	return range[metric].min;
}
 
inline double WedgeMetric::get_max(wedgeMetrics metric)
{
	return range[metric].max;
} 

inline void WedgeMetric::reset_volume()
{
   totalVolume = 0.0;
}

#endif

