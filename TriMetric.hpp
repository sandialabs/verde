//-------------------------------------------------------------------------
// Filename      : TriMetric.hpp 
//
// Purpose       : This class performs metric calculations on Tri Elements.
//
// Special Notes : 
//
// Creator       : Ray Meyers
//
// Date          : 11/10/98
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------

#ifndef TRI_METRIC_HPP
#define TRI_METRIC_HPP

#include <deque>

#include "Metric.hpp"
#include "QualitySummary.hpp"
#include "VerdeVector.hpp"

enum triMetrics {TRI_SHAPE, TRI_REL_SIZE, TRI_SHAPE_SIZE, TRI_AREA, 
		 TRI_ASPECT, TRI_ANGLE, TRI_MIN_ANGLE, TRI_CONDITION_NUMBER, 
                 TRI_MIN_SC_JAC, TRI_SHEAR, NUM_TRI_METRICS};

class TriRef;
class FailedTri;

//! class performs metric calculations on Tri Elements.
class TriMetric : public Metric
{  

public:

   //! constructor
   TriMetric();

   //! destructor
   ~TriMetric ();

   //! calculates metrics for the tri represented by the 8 nodeRefs
   int calculate(TriRef &tri_ref, int which_metric = -1);
   
   //! Access function.
   double metric(const int i) const;

   //! print summary of gathered metrics
   void print_summary();

   //! prints a summary of tris which failed metric tests
   int print_failed_tris(VerdeBoolean verbose);

   //! access function for the singleton pointer
   static TriMetric *instance();

   //! returns the number of metrics defined
   int number_metrics();

   //! returns the failed tri list for metric at index
   void failed_tri_list(int index, std::deque<FailedTri*> &tri_list);

   //! counts the total number of tris in failedTris lists
   int number_failed_tris();

   //! returns the string name of metric at index
   const char *metric_name(int index);

   //! resets the tool for a new set of values
   void reset();

   //! resets ranges back to hard-coded defaults
   void reset_range();

   // resets the tool as well as deletes failed tris
   void reset_all();

   void reset_area();
   //! resets the total area to zero

   //! returns area of all tris processed
   double area();
   
   //! set min value of range parameter
   void set_min(triMetrics metric, double value);

   //! set max value of range parameter
   void set_max(triMetrics metric, double value);

   //! get min value of range parameter
   double get_min(triMetrics metric);
   
   //! get max value of range parameter
   double get_max(triMetrics metric);
   
   //! special function used to make most efficient initial pass of finding
   //! average size of quads needed for main calculate loop
   double area(TriRef &tri_ref);

private:
  
   //! pointer to class singleton 
   static TriMetric *instance_;
   
   //! array to hold values calculated for current element
   double metrics[NUM_TRI_METRICS];
   
   //! contains character names of each metric
   static const char* metricNames[NUM_TRI_METRICS];

   //! contains summary information for each metric
   QualitySummary summary[NUM_TRI_METRICS];

   //! contains a list of tris with values outside valid range for metric
   std::deque<FailedTri*> failedTris[NUM_TRI_METRICS];

   //! contains valid ranges for each metric
   static min_max range[NUM_TRI_METRICS];

   //! contains hard-coded backup ranges for each metric
   static const min_max range_backup[NUM_TRI_METRICS];

   //! total area for the elements processed
   double totalArea;

   //! scratch variables, for efficiency
   double w11,w21,w12,w22;
   double gw11,gw12,gw22;
   double qw11,qw21,qw12,qw22;
   double g11,g12,g21,g22;
   double q11,q12,q21,q22;
   double x11,x12,x21,x22;
   double a11,a12,a21,a22;

   //! get weights for metric calculations
   void get_weight( double &w11,double &w21,double &w12,double &w22);

   void calculate_2d_tri_quality(VerdeVector node_pos[3],
                                 double *aspect, double *area,
                                 int which_metric );
   
   void calculate_angle_metric(VerdeVector node_pos[3],
                               double *min_angle, double *max_angle);
   
   void calculate_min_scaled_jacobian(VerdeVector node_pos[3],
                               double *min_jac );

   void calculate_max_condition(VerdeVector node_pos[3],
                               double *max_condition);
   
   void calculate_shear(VerdeVector node_pos[3], 
                       double *min_skew );

   void calculate_shape(VerdeVector node_pos[3], 
                       double *min_shape );

   void calculate_relative_size(VerdeVector node_pos[3], 
                               double *min_size );

   void calculate_shape_size(VerdeVector node_pos[3], 
                            double *min_ss );

};

// inline functions

inline double TriMetric::metric(const int i) const
{return metrics[i];}

inline int TriMetric::number_metrics()
{
   return NUM_TRI_METRICS;
}

inline const char *TriMetric::metric_name(int index)
{
   return metricNames[index];
}

inline double TriMetric::area()
{
   return totalArea;
}

inline void TriMetric::set_min(triMetrics metric, double value)
{
   range[metric].min = value;
}

inline void TriMetric::set_max(triMetrics metric, double value)
{
   range[metric].max = value;
}


inline double TriMetric::get_min(triMetrics metric)
{
	return range[metric].min;
}
 
inline double TriMetric::get_max(triMetrics metric)
{
	return range[metric].max;
}

inline void TriMetric::reset_area()
{
   totalArea = 0.0;
}

#endif

