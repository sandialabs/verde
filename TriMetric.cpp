//-------------------------------------------------------------------------
// Filename      : TriMetric.cc 
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

#include <algorithm>
#include "stdlib.h"

#include "TriMetric.hpp"

#include "TriRef.hpp"
#include "VerdeVector.hpp"
#include "VerdeMessage.hpp"
#include "FailedTri.hpp"

#if defined(USE_VERDICT)
  #include "verdict.h"
#endif


const char* TriMetric::metricNames[NUM_TRI_METRICS] =
{"Shape",
 "Relative Size",
 "Shape and Size",
 "Element Area",
 "Aspect Ratio",
 "Angle",
 "Minimum Angle",
 "Condition No.",
 "Scaled Jacobian",
 "Shear" };

min_max TriMetric::range[NUM_TRI_METRICS] =
{ {0.75, 1.0},               // Shape
  {0.75, 1.0},               // Relative Size
  {0.75, 1.0},               // Shape and Size
  {0.0, VERDE_DBL_MAX},      // Element Area
  {1.0, 1.3},                // Aspect Ratio
  {60.0, 90.0},              // Angle
  {30.0, 60.0},              // Minimum Angle
  {1.0, 1.3},                // Condition No.
  {0.5,1.155},               // Scaled Jacobian
  {0.75, 1.0}               // Shear
};

const min_max TriMetric::range_backup[NUM_TRI_METRICS] =
{ {0.75, 1.0},               // Shape
  {0.75, 1.0},               // Relative Size
  {0.75, 1.0},               // Shape and Size
  {0.0, VERDE_DBL_MAX},      // Element Area
  {1.0, 1.3},                // Aspect Ratio
  {60.0, 90.0},              // Angle
  {30.0, 60.0},              // Minimum Angle
  {1.0, 1.3},                // Condition No.
  {0.5,1.155},               // Scaled Jacobian
  {0.75, 1.0}               // Shear
};


TriMetric *TriMetric::instance_ = NULL;

TriMetric::TriMetric()
{
  totalArea = 0.0;
}


TriMetric::~TriMetric()
{
  // clean up memory
  reset_all();
}

TriMetric *TriMetric::instance()
{
   if (!instance_)
      instance_ = new TriMetric();
   return instance_;
}


int TriMetric::calculate(TriRef &tri_ref, int which_metric)
{
   VerdeBoolean malformed = VERDE_FALSE;


#if defined(USE_VERDICT)

   which_metric = which_metric;

    double node_pos[3][3];
    int ii;
    for ( ii = 0; ii < 3; ii++ )
    {
      node_pos[ii][0] = tri_ref.node_coordinates(ii).x();
      node_pos[ii][1] = tri_ref.node_coordinates(ii).y();
      node_pos[ii][2] = tri_ref.node_coordinates(ii).z();
    }

    TriMetricVals values;
    int metrics_flag = V_TRI_ALL - V_TRI_DISTORTION;

    v_tri_quality(3, node_pos, metrics_flag, &values);

    metrics[TRI_ASPECT] = values.aspect;
    metrics[TRI_AREA] = values.area;
    metrics[TRI_MIN_ANGLE] = values.smallest_angle;
    metrics[TRI_ANGLE] = values.largest_angle;
    metrics[TRI_CONDITION_NUMBER] = values.condition;
    metrics[TRI_MIN_SC_JAC] = values.scaled_jacobian;
    metrics[TRI_SHEAR] = values.shear;
    metrics[TRI_SHAPE] = values.shape;
    metrics[TRI_REL_SIZE] = values.relative_size;
    metrics[TRI_SHAPE_SIZE] = values.shape_and_size;
    
    totalArea += values.area;

#else  

   // Get positions of each node of the tri
  
   VerdeVector node_pos[3];
   for ( int ii = 0; ii < 3; ii++ )
   {
      node_pos[ii] = tri_ref.node_coordinates(ii);
   }
  
   // calculate element aspect ratio

      // calculate element aspect ratio, skew and taper
  if( doAlgebraic || doDiagnostic )  
    calculate_2d_tri_quality(node_pos, &metrics[TRI_ASPECT],
                            &metrics[TRI_AREA], which_metric );
    
 
   //- Calculate maximum interior angle (degrees)
      
  if( doTraditional ) 
  {
    calculate_angle_metric(node_pos, &metrics[TRI_MIN_ANGLE],
                          &metrics[TRI_ANGLE]);

  
    //- Calculate maximum condition number relative to surface normal
   
    calculate_max_condition(node_pos, &metrics[TRI_CONDITION_NUMBER] );

  
    //- Calculate minimum jacobian relative to surface normal

    calculate_min_scaled_jacobian(node_pos, &metrics[TRI_MIN_SC_JAC] );

  }

  if(doAlgebraic)
  {
    //- calculate shear, shape, relative size and shape & size

    calculate_shear(node_pos, &metrics[TRI_SHEAR]);
    calculate_shape(node_pos, &metrics[TRI_SHAPE]);
    calculate_relative_size(node_pos, &metrics[TRI_REL_SIZE]);
    calculate_shape_size(node_pos, &metrics[TRI_SHAPE_SIZE]);
  } 

#endif


   // put information in QualitySummary for each metric

   int element_id = tri_ref.original_id();
  
  if( doAlgebraic )
  {
    for (int i = 0; i < 3; i++)
    {
      summary[i].add_sample(metric(i),element_id);
      // apply a tolerance to the range of acceptable values
      if (metrics[i] > (range[i].max + metricTolerance) ||
                        metrics[i] < (range[i].min - metricTolerance))
      {
         FailedTri *bad_elem = new FailedTri(tri_ref,metrics[i]);
         failedTris[i].push_back(bad_elem);
         malformed = VERDE_TRUE;
      }
    }
  }

  if( doDiagnostic )
  {
    for (int i = 3; i < 4; i++)
    {
      summary[i].add_sample(metric(i),element_id);
      // apply a tolerance to the range of acceptable values
      if (metrics[i] > (range[i].max + metricTolerance) ||
                        metrics[i] < (range[i].min - metricTolerance))
      {
         FailedTri *bad_elem = new FailedTri(tri_ref,metrics[i]);
         failedTris[i].push_back(bad_elem);
         malformed = VERDE_TRUE;
      }
    }
  }

  if( doTraditional )
  {
    for (int i = 4; i < NUM_TRI_METRICS; i++)
    {
      summary[i].add_sample(metric(i),element_id);
      // apply a tolerance to the range of acceptable values
      if (metrics[i] > (range[i].max + metricTolerance) ||
                        metrics[i] < (range[i].min - metricTolerance))
      {
         FailedTri *bad_elem = new FailedTri(tri_ref,metrics[i]);
         failedTris[i].push_back(bad_elem);
         malformed = VERDE_TRUE;
      }
    }
  }

  return malformed;
}

#define SQUARE(x) ((x) * (x))

void TriMetric::calculate_2d_tri_quality(VerdeVector node_pos[3],
                                          double *aspect, double *area,
                                          int which_metric )
{
   // Calculate quality of triangle element defined by the 3 nodal
   //  positions in 'node_pos'
   //

   VerdeVector side1 = node_pos[1] - node_pos[0];
   VerdeVector side2 = node_pos[2] - node_pos[1];
   VerdeVector side3 = node_pos[0] - node_pos[2];

   if (which_metric == -1 || which_metric == TRI_ASPECT ||
       which_metric == TRI_AREA)
   {
      *area = 0.5 * ((side1 * (-side3)).length());
      totalArea += *area;
   }
  

   // use the aspect ratio gamma defined by Parthasarathy et al. (see def'n
   // of tet quality metrics in TetQuality::calculate_element_quality)
   if (which_metric == -1 || which_metric == TRI_ASPECT)
   {
      double srms = (side1.length_squared() + side2.length_squared() + 
                     side3.length_squared()) / 3.0;
    
      // normalize aspect ratio to that of an equilateral triangle
     
      *aspect = srms / (2.30940108* (*area));
   }


}

void TriMetric::calculate_angle_metric(VerdeVector node_pos[3],
                            double *min_angle, double *max_angle)
{

   // Loop through all of the corners of the tri.
   
   *max_angle = 0.0;
   *min_angle = 361.0;  // Larger than 360 degrees

   VerdeVector next,prev;
   
   for(int i=0; i<3; i++)
   {
      next = node_pos[(i+1)%3] - node_pos[i];
      prev = node_pos[(i+2)%3] - node_pos[i];
      //next.set(nodes[i], nodes[(i+1)%3]);

      // VerdeVector prev(nodes[i], nodes[(i+2)%3]);
      //double angle = tri_normal.vector_angle(next, prev);
      double angle = prev.interior_angle(next);
      
      *max_angle = VERDE_MAX(*max_angle, angle);
      *min_angle = VERDE_MIN(*min_angle, angle);
   }

}

void TriMetric::calculate_min_scaled_jacobian(VerdeVector node_pos[3], 
                                        double *min_jac )
{

   *min_jac = VERDE_DBL_MAX;
 
   VerdeVector first, second;

   const double detw=2/sqrt(3.0);
   
   //first = node_pos[(i+1)%3] - node_pos[i];
   //second = node_pos[(i+2)%3], node_pos[i] );
   first = node_pos[0] - node_pos[1];
   second = node_pos[0] - node_pos[2];
      
   
   VerdeVector cross = first * second;

   double cur_jac = cross.length();

   cur_jac /= first.length();
   cur_jac /= second.length();
   cur_jac *= detw;   

   *min_jac = VERDE_MIN(*min_jac, cur_jac);
}

void TriMetric::calculate_max_condition(VerdeVector node_pos[3], 
                                        double *max_condition )
{

   *max_condition = -VERDE_DBL_MAX;
   double condition;
   VerdeVector xxi, xet;
   const double rt3 = sqrt(3.0);

   // for ( int i=0; i<3; i++ ) {  
   // Note: condition number is same at all 3 nodes, so only calculate once


   //xxi.set( nodes[(i+1)%3], nodes[i] );
   //xet.set( nodes[(i+2)%3], nodes[i] );
   xxi = node_pos[0] - node_pos[1];
   xet = node_pos[0] - node_pos[2];
   xet = ( 2*xet - xxi )/rt3;

   double det = ( xxi * xet ).length();

   if ( det <= VERDE_DBL_MIN ) {
      condition = 1.e+06;
   }
   else {

      condition = 0.5*( xxi % xxi + xet % xet ) / det;

   }

   *max_condition = VERDE_MAX(*max_condition, condition);

}

void TriMetric::print_summary()
{

   PRINT_INFO("   Function Name     Average    Std Dev    "
              "Minimum   (id)  Maximum  (id)\n"
              " -----------------  ---------  ---------  "
              "--------------  -------------\n");

  if(doAlgebraic)
  {
    PRINT_INFO("\nALGEBRAIC METRICS\n"); 
    for (int i = 0; i < 3; i++)
    {
      PRINT_INFO("% 18s ", metricNames[i]); // NO newline
      summary[i].print(); // Adds newline.
    }   
  }

  if(doDiagnostic)
  { 
    PRINT_INFO("\nDIAGNOSTIC METRICS\n"); 
    for (int i = 3; i < 4; i++)
    {
      PRINT_INFO("% 18s ", metricNames[i]); // NO newline
      summary[i].print(); // Adds newline.
    }   
  }

  if(doTraditional)
  { 
    PRINT_INFO("\nTRADITIONAL METRICS\n"); 
    for (int i = 4; i < NUM_TRI_METRICS; i++)
    {
      PRINT_INFO("% 18s ", metricNames[i]); // NO newline
      summary[i].print(); // Adds newline.
    }   
  }
}

int TriMetric::number_failed_tris()
{
   int number = 0;
   
   for (int i = 0; i < NUM_TRI_METRICS; i++)
   {
      number += failedTris[i].size();
   }
   return number;
}

   
int TriMetric::print_failed_tris(VerdeBoolean verbose)
{
   int num_failed = number_failed_tris();
   
   PRINT_INFO("   Number of Failed Tri Metrics: %d\n", num_failed);

   // check for verbose output

   if (num_failed)
   {

      for (int i = 0; i < NUM_TRI_METRICS; i++)
      {
         int num_tris = failedTris[i].size();
         if (num_tris)
         {
            if (verbose)
            {
               // print the first line
               FailedTri *tri = failedTris[i][0];
               PRINT_INFO("\n Failed Metric      Block    Element     Value "
                          "    Min. Valid   Max. Valid \n"
                          "----------------   -------   -------   --------- "
                          "  ----------   ----------\n");
               PRINT_INFO("%16s   %6d   %7d   %10.7f   %10.7f   %10.7f\n",
                          metricNames[i],
                          tri->block_id(), tri->index(),
                          tri->metric_value(),range[i].min, range[i].max);
               // print subsequent lines
                       
               for (int j = 1; j < num_tris; j++)
               {
                  tri = failedTris[i][j];
                  PRINT_INFO("                   %6d   %7d   %10.7f\n",
                             tri->block_id(), tri->index(),
                             tri->metric_value());
               }
            }  
            PRINT_INFO("      Failed %s: %d  "
                       "\t(valid range: %10.3f to %10.3f)\n", metricNames[i],
                       failedTris[i].size(),range[i].min, range[i].max);
         }      
      }
   }
   return num_failed;
}

void TriMetric::failed_tri_list(int index, std::deque<FailedTri*> &tri_list)
{
  for(unsigned int i=0; i<failedTris[index].size(); i++)
    tri_list.push_back(failedTris[index][i]);
}


void TriMetric::reset()
{
   for(int i = 0; i < NUM_TRI_METRICS; i++)
   {
      summary[i].reset();
   }
}

void TriMetric::reset_all()
{
  reset();
  for(int i=0; i< NUM_TRI_METRICS; i++)
  {
    for(unsigned int j=0; j<failedTris[i].size(); j++)
      delete failedTris[i][j];
    failedTris[i].clear();
  }
}

void TriMetric::reset_range()
{
  memcpy( range, range_backup, sizeof range );
}

void TriMetric::calculate_shear(VerdeVector node_pos[3], double *min_skew )
{
   int result;
   *min_skew = VERDE_DBL_MAX;
   double skew = VERDE_DBL_MAX;
   VerdeVector xxi, xet, tri_normal;

   get_weight(w11,w21,w12,w22);

   metric_matrix(w11,w21,w12,w22,gw11,gw12,gw22);

   double detw = determinant(w11,w21,w12,w22);

   result = skew_matrix(gw11,gw12,gw22,detw,qw11,qw21,qw12,qw22);

   double detqw = determinant(qw11,qw21,qw12,qw22);


   if ( detqw > VERDE_DBL_MIN && result == VERDE_SUCCESS ) {

    for ( int i=0; i<3; i++ ) {

      xxi = node_pos[i] - node_pos[(i+1)%3];
      xet = node_pos[i] - node_pos[(i+2)%3];
     
      tri_normal = xxi * xet;
      double det = tri_normal.length();
    
      g11 = xxi % xxi;
      g12 = xxi % xet;
      g22 = xet % xet;

      result = skew_matrix(g11,g12,g22,det,q11,q21,q12,q22);
      if ( result == VERDE_FAILURE ) { break; }

      form_t(q11,q21,q12,q22,qw11,qw21,qw12,qw22,detqw,x11,x21,x12,x22);

      double detx = determinant(x11,x21,x12,x22);
      double normsq = norm_squared(x11,x21,x12,x22);

      if ( normsq <= VERDE_DBL_MIN ) {
	 skew = 0;
      }
      else {

         skew = 2*detx/normsq;
      }

      *min_skew = VERDE_MIN(*min_skew, skew);

    }
   }
}

void TriMetric::calculate_shape(VerdeVector node_pos[3], double *shape )
{
   VerdeVector xxi, xet, tri_normal;

   *shape = 0;

   get_weight(w11,w21,w12,w22);

   double detw = determinant(w11,w21,w12,w22);


   if ( detw > VERDE_DBL_MIN ) {
     
      int i=0;

      xxi = node_pos[i] - node_pos[(i+1)%3];
      xet = node_pos[i] - node_pos[(i+2)%3];

      tri_normal = xxi * xet;
      double det = tri_normal.length();
    
      g11 = xxi % xxi;
      g12 = xxi % xet;

      jacobian_matrix(g11,g12,det,a11,a21,a12,a22);

      form_t(a11,a21,a12,a22,w11,w21,w12,w22,detw,x11,x21,x12,x22);
      double detx = determinant(x11,x21,x12,x22);

      double normsq = norm_squared(x11,x21,x12,x22);

      if ( normsq > VERDE_DBL_MIN ) {
         *shape = 2*detx/normsq;
      }

   }
}

void TriMetric::calculate_relative_size( VerdeVector node_pos[3],
                                         double *min_size )
{
   *min_size=0;
   double size;
   VerdeVector xxi, xet, tri_normal;
   double w11, w21, w12, w22;

   get_weight(w11,w21,w12,w22);

   double detw = determinant(w11,w21,w12,w22);

   xxi = node_pos[0] - node_pos[1];
   xet = node_pos[0] - node_pos[2];
   
   tri_normal = xxi * xet;

   double deta = tri_normal.length();

   size = pow( deta/detw, 2 );
  
   *min_size = VERDE_MIN( size, 1.0/size );


/*
   if ( detw > VERDE_DBL_MIN ) {
     
      int i=0;

      xxi = node_pos[i] - node_pos[(i+1)%3];
      xet = node_pos[i] - node_pos[(i+2)%3];

      tri_normal = xxi * xet;
      double det = tri_normal.length();
    
      g11 = xxi % xxi;
      g12 = xxi % xet;
      g22 = xet % xet;

      jacobian_matrix(g11,g12,det,a11,a21,a12,a22);

      double deta = determinant(a11,a21,a12,a22);

      if ( deta > VERDE_DBL_MIN ) {
         size = deta/detw;
         *min_size = VERDE_MIN(size, (double)(1)/size);
      }

   }
*/

}

void TriMetric::calculate_shape_size(VerdeVector node_pos[3], double *min_ss )
{
  double shape, size;

  calculate_shape( node_pos, &shape );
  calculate_relative_size( node_pos, &size );

  *min_ss=0;
  *min_ss = shape * size;


/*
   *min_ss=0;
   double shape, size, min_size;
   VerdeVector xxi, xet, tri_normal;

   get_weight(w11,w21,w12,w22);

   double detw = determinant(w11,w21,w12,w22);

   if ( detw > VERDE_DBL_MIN ) {
     
      int i=0;

      xxi = node_pos[i] - node_pos[(i+1)%3];
      xet = node_pos[i] - node_pos[(i+2)%3];
 
      tri_normal = xxi * xet;
      double det = tri_normal.length();
    
      g11 = xxi % xxi;
      g12 = xxi % xet;
      g22 = xet % xet;

      jacobian_matrix(g11,g12,det,a11,a21,a12,a22);

      double deta = determinant(a11,a21,a12,a22);

      form_t(a11,a21,a12,a22,w11,w21,w12,w22,detw,x11,x21,x12,x22);
      double detx = determinant(x11,x21,x12,x22);

      double normsq = norm_squared(x11,x21,x12,x22);

      if ( normsq > VERDE_DBL_MIN && deta > VERDE_DBL_MIN ) {
         shape = 2*detx/normsq;
         size = deta/detw;
         min_size = VERDE_MIN(size, (double)(1)/size);
         *min_ss = min_size * shape;
      }

   }
*/
}
               
double TriMetric::area(TriRef &tri_ref)
{

   // Get positions of each node of the tri
  
   VerdeVector node_pos[3];
   for ( int ii = 0; ii < 3; ii++ )
   {
      node_pos[ii] = tri_ref.node_coordinates(ii);
   }

   // get area

   double area;
   
   calculate_2d_tri_quality(node_pos,NULL, &area, TRI_AREA);

   return area;
}

void TriMetric::get_weight( double &m11,double &m21,double &m12,double &m22)
{
  if ( elementShape == 1 ) {
     m11=1;
     m21=0;
     m12=0.5;
     m22=0.5*sqrt(3.0);
  }

  if ( elementShape == 2 ) {
     m11=1;
     m21=0;
     m12=0;
     m22=m11*aspectRatio;
  }

  if ( elementShape == 3 ) {
     m11=1;
     m21=0;
     m12=0.5;
     m22=m11*aspectRatio;
  }


  double scale = sqrt(2.*elementSize/(m11*m22-m21*m12));

  m11 *= scale;
  m21 *= scale;
  m12 *= scale;
  m22 *= scale;
}

