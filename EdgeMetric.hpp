//-------------------------------------------------------------------------
// Filename      : EdgeMetric.hpp 
//
// Purpose       : This class performs metric calculations on Edge Elements.
//
// Special Notes : 
//
// Creator       : Ray Meyers
//
// Date          : 03/10/99
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------

#ifndef EDGE_METRIC_HPP
#define EDGE_METRIC_HPP

#include <deque>

#include "QualitySummary.hpp"
#include "Metric.hpp"

enum edgeMetrics {EDGE_LENGTH,NUM_EDGE_METRICS};

class EdgeRef;
class VerdeVector;
class FailedEdge;

//! performs metric calculations on Edge Elements.
class EdgeMetric: public Metric
{
   //! singleton pointer 
   static EdgeMetric *instance_;

   //! array of metric values used in computations
   double metrics[NUM_EDGE_METRICS];
   
   //! array of character names, on for each metric
   static const char* metricNames[NUM_EDGE_METRICS];

   //! holds summary data for the metric from all elements processed
   QualitySummary summary[NUM_EDGE_METRICS];

   //! contains a list of edgees with values outside valid range for metric
   std::deque<FailedEdge*> failedEdges[NUM_EDGE_METRICS];

   //! contains valid ranges for each metric
   static min_max range[NUM_EDGE_METRICS];

   //! contains hard-code default ranges for each metric
   static const min_max range_backup[NUM_EDGE_METRICS];
   
   //double totalVolume;
   // holds the volume of all elements processed


   //! gets the length of a edge
   double length (VerdeVector node_pos[2]);

       public:

   //! constructor
   EdgeMetric();

   //! destructor
   ~EdgeMetric ();

   //! calculates metrics for the edge represented by the 4 nodeRefs
   int calculate(EdgeRef &edge_ref);
   
   //! calculates metrics for the edge represented by the 6 vectors
   int calculate(VerdeVector node_position[2], int which_metric = -1);

   //! access function.
   double metric(const int i) const;

   //! print summary of gathered metrics
   void print_summary();

   //! access to singleton pointer
   static EdgeMetric *instance();

   //! prints a summary of edges which failed metric tests
   int print_failed_edges(VerdeBoolean verbose);

   //! returns the number of metrics defined
   int number_metrics();

   //! returns the failed edge list for metric at index
   void failed_edge_list(int index, std::deque<FailedEdge*> &edge_list);

   //! counts the total number of edges in failedEdges lists
   int number_failed_edges();

   //! returns the string name of metric at index
   const char *metric_name(int index);

   //! resets the tool for a new set of values
   void reset();

   //! resets the tool and deletes the failed edges
   void reset_all();

   //! reset ranges to hard-code defaults
   void reset_range();

   //! set min value of range parameter
   void set_min(edgeMetrics metric, double value);

   //! set max value of range parameter   
   void set_max(edgeMetrics metric, double value);

   //! gets the min value of range parameter
   double get_min(edgeMetrics metric);

   //! gets the max value of range parameter
   double get_max(edgeMetrics metric);
};

// inline functions

inline double EdgeMetric::metric(const int i) const
{return metrics[i];}

inline int EdgeMetric::number_metrics()
{
   return NUM_EDGE_METRICS;
}

inline const char *EdgeMetric::metric_name(int index)
{
   return metricNames[index];
}

inline void EdgeMetric::set_min(edgeMetrics metric, double value)
{
   range[metric].min = value;
}

inline void EdgeMetric::set_max(edgeMetrics metric, double value)
{
   range[metric].max = value;
}

inline double EdgeMetric::get_min(edgeMetrics metric)
{
	return range[metric].min;
}

inline double EdgeMetric::get_max(edgeMetrics metric)
{
	return range[metric].max;
} 


#endif

