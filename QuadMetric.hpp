//-------------------------------------------------------------------------
// Filename      : QuadMetric.hpp 
//
// Purpose       : This class performs metric calculations on Quad Elements.
//
// Special Notes : 
//
// Creator       : Ray Meyers
//
// Date          : 11/10/98
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------

#ifndef QUAD_METRIC_HPP
#define QUAD_METRIC_HPP

#include <deque>
#include <vector>

#include "Metric.hpp"
#include "QualitySummary.hpp"
#include "VerdeVector.hpp"

enum quadMetrics {QUAD_SHEAR, QUAD_SHAPE, QUAD_REL_SIZE, QUAD_SHAPE_SIZE, 
		  QUAD_AREA, QUAD_ASPECT, QUAD_SKEW, QUAD_TAPER, 
                  QUAD_WARPAGE, QUAD_STRETCH, QUAD_ANGLE, QUAD_MIN_ANGLE,
                  QUAD_MAX_ODDY, QUAD_MAX_COND, QUAD_MIN_JAC, QUAD_MIN_SC_JAC,
                  NUM_QUAD_METRICS};


class QuadRef;
class FailedQuad;


class QuadMetric: public Metric
{  

public:

   //! Constructor
   QuadMetric();

   //! Destructor
   ~QuadMetric ();

   //! calculates metrics for the quad represented by the 8 nodeRefs
   int calculate(QuadRef &quad_ref, int which_metric = -1);

   /*!- special function used to make most efficient initial pass of finding
   // average size of quads needed for main calculate loop !*/
   double area(QuadRef &quad_ref);
   
   //! calculate the metrics for the quad represented by the 8 vectors
   int calculate(VerdeVector node_position[8], int which_metric = -1);

   //! Access function.
   double metric(const int i) const;

   //! print summary of gathered metrics
   void print_summary();

   //! prints a summary of quads which failed metric tests
   int print_failed_quads(VerdeBoolean verbose);

   //! access function for the singleton pointer
   static QuadMetric *instance();

   //! returns the number of metrics defined
   int number_metrics();

   //! returns the failed quad list for metric at index
   void failed_quad_list(int index, std::deque<FailedQuad*> &quad_list);

   //! counts the total number of quads in failedQuads lists
   int number_failed_quads();

   //! returns the string name of metric at index
   const char *metric_name(int index);

   //! resets the tool for a new set of values
   void reset();
   
   //! resets the tool and deletes failed quads
   void reset_all();

   //! resets ranges back to hard-code defaults
   void reset_range();

   //! resets the total area to zero
   void reset_area();
   
   //! returns area of all quads processed
   double area();
   
   //! set min value of range parameter
   void set_min(quadMetrics metric, double value);

   //! set max value of range parameter
   void set_max(quadMetrics metric, double value);

   //! get min value of range parameter
   double get_min(quadMetrics metric);
   
   //! get max value of range parameter
   double get_max(quadMetrics metric);

   
private:
  
   //! singleton pointer 
   static QuadMetric *instance_;
   
   //! array to hold values calculated for current element
   double metrics[NUM_QUAD_METRICS];
   
   //! contains character names of each metric
   static const char* metricNames[NUM_QUAD_METRICS];

   //! contains summary information for each metric
   QualitySummary summary[NUM_QUAD_METRICS];

   //! contains a list of quads with values outside valid range for metric
   std::deque<FailedQuad*> failedQuads[NUM_QUAD_METRICS];

   //! contains valid ranges for each metric
   static min_max range[NUM_QUAD_METRICS];

   //! contains valid ranges for each metric
   static const min_max range_backup[NUM_QUAD_METRICS];

   //! total area for the elements processed 
   double totalArea;

   void get_weight( double &w11,double &w21,double &w12,double &w22);

   VerdeBoolean calculate_2d_quad_quality(VerdeVector nodev[4],
                                 double *aspect, double *skew,
                                 double *taper, double *warpage,
                                 double *area,
                                 int which_metric );
   // Returns TRUE if a malformed element exists

   //! Calculate shape and size related metrics
   void calculate_shear(VerdeVector node_pos[4],
                        VerdeVector &quad_normal,
                        double *shear);
  
   void calculate_shape(VerdeVector node_pos[4],
                        VerdeVector &quad_normal,
                        double *shape);
  
   void calculate_relative_size(VerdeVector node_pos[4],
                                VerdeVector &quad_normal,
                                double *relative_size);
  
   void calculate_shape_size(VerdeVector node_pos[4],
                             VerdeVector &quad_normal,
                             double *shape_size);
  
   
   void localize_quad_coordinates(VerdeVector node_pos[4],
				  VerdeVector local[4]);

   double calculate_stretch_metric(VerdeVector node_pos[4]);
   void calculate_angle_metric(VerdeVector node_pos[4],
                               VerdeVector quad_normal,
                               double *min_angle, double *max_angle);

   void calculate_max_oddy(VerdeVector node_pos[4],
                           double *max_oddy );

   void calculate_max_condition(VerdeVector node_pos[4],
                                VerdeVector quad_normal,
                                double *max_fold );

   void calculate_min_jacobian(VerdeVector node_pos[4],
                               VerdeVector quad_normal,
                               double *min_jac );


   void calculate_min_scaled_jacobian(VerdeVector node_pos[4],
                                      VerdeVector quad_normal,
                                      double *min_scaled_jac );

   void get_quad_normal(VerdeVector node_pos[4], VerdeVector &normal);

   //! gets an approximate normal for the non-planar quad
   VerdeVector plane_normal(std::vector<VerdeVector*> &positions);
  
   //! NOTE: one of these can be removed!!

   
};

// inline functions

inline double QuadMetric::metric(const int i) const
{return metrics[i];}

inline int QuadMetric::number_metrics()
{
   return NUM_QUAD_METRICS;
}

inline const char *QuadMetric::metric_name(int index)
{
   return metricNames[index];
}

inline double QuadMetric::area()
{
   return totalArea;
}

inline void QuadMetric::set_min(quadMetrics metric, double value)
{
   range[metric].min = value;
}

inline void QuadMetric::set_max(quadMetrics metric, double value)
{
   range[metric].max = value;
}

inline double QuadMetric::get_max(quadMetrics metric)
{
	return range[metric].max;
}

inline double QuadMetric::get_min(quadMetrics metric)
{
	return range[metric].min;
}

inline void QuadMetric::reset_area()
{
   totalArea = 0.0;
}

#endif

