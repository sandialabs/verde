//-------------------------------------------------------------------------
// Filename      : QuadMetric.cc 
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

#include "stdlib.h"

#include "QuadMetric.hpp"

#include "QuadRef.hpp"
#include "VerdeVector.hpp"
#include "VerdeMessage.hpp"
#include "FailedQuad.hpp"

#if defined(USE_VERDICT)
  #include "verdict.h"
#endif

const char* QuadMetric::metricNames[] = 
{ "Shear",
  "Shape", 
  "Relative Size",
  "Shape and Size",
  "Element Area",
  "Aspect Ratio",
  "Skew",
  "Taper",
  "Warpage",
  "Stretch",
  "Maximum Angle",
  "Minimum Angle",
  "Oddy",
  "Condition No.",
  "Jacobian",
  "Scaled Jacobian"};


min_max QuadMetric::range[NUM_QUAD_METRICS] =
{ {0.3, 1.0},                // Shear
  {0.3, 1.0},                // Shape
  {0.5, 1.0},                // Relative Size
  {0.2, 1.0},                // Shape and Size
  {0.0, VERDE_DBL_MAX},      // Element Area
  {1.0, 4.0},                // Aspect Ratio
  {0.0, 0.5},                // Skew
  {0.0, 0.7},                // Taper
  {0.9, 1.0},                // Warpage
  {0.25, 1.0},               // Stretch
  {90.0, 135.0},             // Maximum Angle
  {45.0, 90.0},              // Minimum Angle
  {0.0, 16.0},               // Oddy
  {1.0, 4.0},                // Condition No.
  {0.0, VERDE_DBL_MAX},      // Jacobian
  {0.5, 1.0}                // Scaled Jacobian
};

const min_max QuadMetric::range_backup[NUM_QUAD_METRICS] =
{ {0.3, 1.0},                // Shear
  {0.3, 1.0},                // Shape
  {0.5, 1.0},                // Relative Size
  {0.2, 1.0},                // Shape and Size
  {0.0, VERDE_DBL_MAX},      // Element Area
  {1.0, 4.0},                // Aspect Ratio
  {0.0, 0.5},                // Skew
  {0.0, 0.7},                // Taper
  {0.9, 1.0},                // Warpage
  {0.25, 1.0},               // Stretch
  {90.0, 135.0},             // Maximum Angle
  {45.0, 90.0},              // Minimum Angle
  {0.0, 16.0},               // Oddy
  {1.0, 4.0},                // Condition No.
  {0.0, VERDE_DBL_MAX},      // Jacobian
  {0.5, 1.0}                // Scaled Jacobian
};


QuadMetric *QuadMetric::instance_ = NULL;

QuadMetric::QuadMetric()
{
  totalArea = 0.0;
}


QuadMetric::~QuadMetric()
{
  // clean up memory
  reset_all();
}

QuadMetric *QuadMetric::instance()
{
   if (!instance_)
      instance_ = new QuadMetric();
   return instance_;
}


int QuadMetric::calculate(QuadRef &quad_ref, int which_metric)
{
   VerdeBoolean malformed_found = VERDE_FALSE;

   which_metric = which_metric;  //gets rid of compiler warning

#if defined(USE_VERDICT)

   double node_pos[4][3];
   for ( int ii = 0; ii < 4; ii++ )
   {
      node_pos[ii][0] = quad_ref.node_coordinates(ii).x();
      node_pos[ii][1] = quad_ref.node_coordinates(ii).y();
      node_pos[ii][2] = quad_ref.node_coordinates(ii).z();
   }

   int metric_flag = V_QUAD_ALL - V_QUAD_DISTORTION - V_QUAD_SHEAR_AND_SIZE;
   QuadMetricVals values;

   v_quad_quality(4, node_pos, metric_flag, &values);

   metrics[QUAD_ASPECT] = values.aspect;
   metrics[QUAD_SKEW] = values.skew;
   metrics[QUAD_TAPER] = values.taper;
   metrics[QUAD_WARPAGE] = values.warpage;
   metrics[QUAD_AREA] = values.area;
   metrics[QUAD_STRETCH] = values.stretch;
   metrics[QUAD_ANGLE] = values.largest_angle;
   metrics[QUAD_MIN_ANGLE] = values.smallest_angle;
   metrics[QUAD_MAX_ODDY] = values.oddy;
   metrics[QUAD_MAX_COND] = values.condition;
   metrics[QUAD_MIN_JAC] = values.jacobian;
   metrics[QUAD_MIN_SC_JAC] = values.scaled_jacobian;
   metrics[QUAD_SHEAR] = values.shear;
   metrics[QUAD_SHAPE] = values.shape;
   metrics[QUAD_REL_SIZE] = values.relative_size;
   metrics[QUAD_SHAPE_SIZE] = values.shape_and_size;

   totalArea += values.area;
#else

   // Get positions of each node of the quad
  
   VerdeVector node_pos[4];
   for ( int ii = 0; ii < 4; ii++ )
   {
      node_pos[ii] = quad_ref.node_coordinates(ii);
   }
   
   // calculate the normal for the quad
   
   VerdeVector quad_normal;
   get_quad_normal(node_pos,quad_normal);
  
   // calculate element aspect ratio, skew and taper
   // At this point, the local[] array contains the coordinates of the 
   // quad transformed into an approximate 2D configuration
    
      // Localize coordinates. (Transform quad such that the plane that best 
      // approximates the element lies in the X-Y plane. The local X axis is
      // aligned with the vector from node 0 to node 1)
   VerdeVector local[4];
   localize_quad_coordinates(node_pos, local);
  
    
  if( doTraditional || doDiagnostic )
  {   
      // calculate element aspect ratio, skew and taper
      // At this point, the local[] array contains the coordinates of the 
      // quad transformed into an approximate 2D configuration
   malformed_found = 
       calculate_2d_quad_quality(node_pos, &metrics[QUAD_ASPECT],
                                 &metrics[QUAD_SKEW],
                                 &metrics[QUAD_TAPER],
                                 &metrics[QUAD_WARPAGE],
                                 &metrics[QUAD_AREA],
                                 which_metric );
  }
  
  if(doTraditional)
  { 
   //- calculate element stretch
   metrics[QUAD_STRETCH] = calculate_stretch_metric(node_pos);
  
   //- Calculate maximum interior angle (degrees)
   calculate_angle_metric(node_pos, quad_normal,
                          &metrics[QUAD_MIN_ANGLE],
                          &metrics[QUAD_ANGLE]);

  
   //- Calculate maximum value of Oddy's metric 
   calculate_max_oddy(node_pos, &metrics[QUAD_MAX_ODDY] );

   //- Calculate maximum condition number relative to surface normal
   calculate_max_condition(node_pos, quad_normal,
                           &metrics[QUAD_MAX_COND] );

   //- Calculate minimum jacobian relative to surface normal
   calculate_min_jacobian(node_pos, quad_normal,
                          &metrics[QUAD_MIN_JAC] );

   //- Calculate minimum scaled jacobian relative to surface normal
   calculate_min_scaled_jacobian(node_pos, quad_normal,
                                 &metrics[QUAD_MIN_SC_JAC] );
  }

  if(doAlgebraic)
  {
    //- Calculate shape and size related metrics

    calculate_shear(node_pos, quad_normal, &metrics[QUAD_SHEAR]);
    calculate_shape(node_pos, quad_normal, &metrics[QUAD_SHAPE]);
    calculate_relative_size(node_pos, quad_normal,&metrics[QUAD_REL_SIZE]);
    calculate_shape_size(node_pos, quad_normal, &metrics[QUAD_SHAPE_SIZE]);
  }
 

#endif // USE_VERDICT
  

   // put information in QualitySummary for each metric
   // create any failed elements

   int element_id = quad_ref.original_id();

  if(doAlgebraic)
  {
    for (int i = 0; i < 4; i++)
    {
      summary[i].add_sample(metrics[i],element_id);
      // apply a tolerance to the range of acceptable values
      if (metrics[i] > (range[i].max + metricTolerance) ||
                        metrics[i] < (range[i].min - metricTolerance))
      {
         FailedQuad *bad_elem = new FailedQuad(quad_ref,metrics[i]);
         failedQuads[i].push_back(bad_elem);
         malformed_found = VERDE_TRUE;
      }
    }
  }

  if(doDiagnostic)
  {
    for (int i = 4; i < 5; i++)
    {
      summary[i].add_sample(metrics[i],element_id);
      // apply a tolerance to the range of acceptable values
      if (metrics[i] > (range[i].max + metricTolerance) ||
                        metrics[i] < (range[i].min - metricTolerance))
      {
         FailedQuad *bad_elem = new FailedQuad(quad_ref,metrics[i]);
         failedQuads[i].push_back(bad_elem);
         malformed_found = VERDE_TRUE;
      }
    }
  }

  if(doTraditional)
  {
    for (int i = 5; i < NUM_QUAD_METRICS; i++)
    {
      summary[i].add_sample(metrics[i],element_id);
      // apply a tolerance to the range of acceptable values
      if (metrics[i] > (range[i].max + metricTolerance) ||
                        metrics[i] < (range[i].min - metricTolerance))
      {
         FailedQuad *bad_elem = new FailedQuad(quad_ref,metrics[i]);
         failedQuads[i].push_back(bad_elem);
         malformed_found = VERDE_TRUE;
      }
    }
  }

  return malformed_found;
}

#define SQUARE(x) ((x) * (x))

VerdeBoolean QuadMetric::calculate_2d_quad_quality(VerdeVector nodes[4],
                                           double *aspect, double *skew,
                                           double *taper, double *warpage,
                                           double *area,
                                           int which_metric )
{
  /*
  // Calculate quality of quadrilateral element defined by the 4 nodal
  //  positions in 'node_pos'
  //
  // code lifted almost directly from CON3D subroutine of NUMBERSII
  // Based on article by John Robinson, "CRE Method of element testing
  //    and the Jacobian shape parameters," Eng. Comput., 1987, Vol. 4,
  //    June, pp 113 - 118
  //
  //         E2                                               E4
  //     +----------+          +-----------+       +---------+ |
  //     |          | F3      /           /       /           \
  //     |          |        / A         /       /             \
  //     +----------+       +-----------+       +---------------+
  //
  //      AR = E2/F3        SKEW = SIN(A)            TAPER Y
  */
 
  VerdeVector node_pos[4];
  localize_quad_coordinates(nodes, node_pos);
  
  VerdeBoolean malformed_found = VERDE_FALSE;
  
  // ... Make centroid of element the center of coordinate system
  VerdeVector centroid = node_pos[0];
  centroid += node_pos[1];
  centroid += node_pos[2];
  centroid += node_pos[3];

  centroid /= 4.0;

  node_pos[0] -= centroid;
  node_pos[1] -= centroid;
  node_pos[2] -= centroid;
  node_pos[3] -= centroid;

  // ... Rotate element such that center of side 2-3 and 4-1 define X axis
  VerdeVector xy23 = node_pos[1] + node_pos[2];
  VerdeVector xy41 = node_pos[3] + node_pos[0];
  
  VerdeVector rotate = xy23;
  rotate -= xy41;
  rotate.normalize();

  double cosine = rotate.x();
  double   sine = rotate.y();
  
  double xnew, ynew;
  
  for (int i=0; i < 4; i++) {
    xnew =  cosine * node_pos[i].x() +   sine * node_pos[i].y();
    ynew =   -sine * node_pos[i].x() + cosine * node_pos[i].y();
    node_pos[i].x(xnew);
    node_pos[i].y(ynew);
  }

  // ... Calculate ``Shape function'' parameters - E1, F1, F2 = 0.0

  // ef1 unused
  // VerdeVector ef1 =  node_pos[0] + node_pos[1] + node_pos[2] + node_pos[3];
  VerdeVector ef2 = -node_pos[0] + node_pos[1] + node_pos[2] - node_pos[3];
  VerdeVector ef3 = -node_pos[0] - node_pos[1] + node_pos[2] + node_pos[3];
  // ef4 just used for taper
  // VerdeVector ef4 =  node_pos[0] - node_pos[1] + node_pos[2] - node_pos[3];
  
  if (ef3.y() == 0.0 || ef2.x() == 0.0) {
    malformed_found = VERDE_TRUE;
    // Put 'bad', but 'valid' values in the variables
    *aspect = 0.0;
    *skew   = 1.0;
    *taper  = 0.0;
    *warpage= 0.0;
    *area   = 0.0;
  } else {
    if ( which_metric == -1 ||
	 which_metric == QUAD_ASPECT ) {
      *aspect = ef2.x() / ef3.y();
      *aspect = (*aspect < 1.0 ? 1.0/ *aspect : *aspect);

    }
    
    if ( which_metric == -1 ||
	 which_metric == QUAD_SKEW ) {
      *skew = ef3.x() / ef3.y();
      *skew = *skew / sqrt(*skew * *skew + 1.0);
      *skew = fabs(*skew);
    }
    
    if ( which_metric == -1 ||
	 which_metric == QUAD_TAPER ) {
      VerdeVector ef4 =  node_pos[0] - node_pos[1] + node_pos[2] - node_pos[3];
      //*taper = VERDE_MAX(SQUARE(ef4.y()/ef3.y()), SQUARE(ef4.x()/ef2.x()));
      *taper = VERDE_MAX(fabs(ef4.y()/ef3.y()), fabs(ef4.x()/ef2.x()));
    }

    if ( which_metric == -1 ||
	 which_metric == QUAD_WARPAGE ) {

      VerdeVector xxi, xet, unit_normal[4];

      double min_value = 0;
      double min_mag = VERDE_DBL_MAX;
      for ( int i=0; i<4; i++ )
      {

        xxi = nodes[(i+1)%4] - nodes[i];
        xet = nodes[(i+3)%4] - nodes[i];

        unit_normal[i] = xxi * xet;

        double mag = unit_normal[i].length();
        min_mag = VERDE_MIN( min_mag, mag );

        if (mag > VERDE_DBL_MIN )
            unit_normal[i] /= mag;

      }

      if ( min_mag > VERDE_DBL_MIN ) {
          double dot1 = unit_normal[0] % unit_normal[2];
          double dot2 = unit_normal[1] % unit_normal[3];

          if ( (dot1>VERDE_DBL_MIN) && (dot1>VERDE_DBL_MIN) )
            min_value = pow( VERDE_MIN( dot1, dot2 ), 4 );
      }

      *warpage = min_value;
    }

    if ( which_metric == -1 ||
	 which_metric == QUAD_AREA ) {
      *area = ef2.x() * ef3.y() / 4.0;
      totalArea += *area;
    }
  }
  if (DEBUG_FLAG(40) && which_metric == -1)
    PRINT_DEBUG(VERDE_DEBUG_40,
	      "a=%12.3e, s=%12.3e, t=%12.3e, w=%12.3e, A=%12.3e\n",
	      *aspect, *skew, *taper, *warpage, *area);
  return malformed_found;
}

double QuadMetric::calculate_stretch_metric(VerdeVector node_pos[4])
{
   // stretch metric code designed after FI-GEN (FDI) quality metrics
   // The stretch metric is the ratio of the minimum quad edge length divided
   // by the length of the longest diagonal. The calculated value is normalized
   // by multiplying by sqrt(2.0).

   // compute side, diagonal lengths (all in squares for now)
   //double side01 = VerdeVector(nodes[0], nodes[1]).length_squared();
   //double side12 = VerdeVector(nodes[1], nodes[2]).length_squared();
   //double side23 = VerdeVector(nodes[2], nodes[3]).length_squared();
   // double side30 = VerdeVector(nodes[3], nodes[0]).length_squared();

   //double diag02 = VerdeVector(nodes[0], nodes[2]).length_squared();
   //double diag13 = VerdeVector(nodes[1], nodes[3]).length_squared();

   VerdeVector temp;
   temp = node_pos[1] - node_pos[0];
   double side01 = temp.length_squared();
   temp = node_pos[2] - node_pos[1];
   double side12 = temp.length_squared();
   temp = node_pos[3] - node_pos[2];
   double side23 = temp.length_squared();
   temp = node_pos[0] - node_pos[3];
   double side30 = temp.length_squared();

   temp = node_pos[2] - node_pos[0];
   double diag02 = temp.length_squared();
   temp = node_pos[3] - node_pos[1];
   double diag13 = temp.length_squared();

   // compute stretch or aspect ratio; take square roots, since all
   // lengths are squared.
   const double QUAD_STRETCH_SCALE_FACTOR = sqrt(2.0);
   double stretch = sqrt(VERDE_MIN_4(side01,side12, side23,side30) /
                         VERDE_MAX(diag02, diag13)) * QUAD_STRETCH_SCALE_FACTOR;
   return stretch;
}


void QuadMetric::localize_quad_coordinates(VerdeVector node_pos[4],
                                           VerdeVector local[4])
{
  int i;
  // Copy nodal positions to global VerdeVector array and 
  // transform so center of element at (0,0,0) in global coordinates
  VerdeVector global[4];
  for (i=0; i < 4; i++)  {
    global[i] = node_pos[i];
  }

  VerdeVector center = (global[0] + global[1] + global[2] + global[3]) / 4.0;
  for (i=0; i < 4; i++)  {
    global[i] -= center;
  }
  
  // Calculate 'best-fit' plane approximation to (possibly) warped quad.
  // The quad will be transformed such that the plane normal is the 'z' 
  // axis in 'local' coordinates.
  std::vector<VerdeVector*> global_list(4);
  global_list[0] = &global[0];
  global_list[1] = &global[1];
  global_list[2] = &global[2];
  global_list[3] = &global[3];
  VerdeVector normal = plane_normal(global_list);
  normal.normalize();
	
  // The 'x' axis in local coordinates will be aligned with the vector
  // from node 0 to node 1.
  VerdeVector local_x_axis = global[1] - global[0];
  local_x_axis.normalize();
	
  // The 'y' axis in local coordinates is 'z' cross 'x'
  VerdeVector local_y_axis = normal * local_x_axis;
  local_y_axis.normalize();
	
  // We can now transform from global coordinates to local coordinates
  // using the equation L = G * B
  // where-
  //  L = row vector of local coordinates,
  //  G = row vector of global coordinates, and
  //  B = Basis matrix (3x3) which consists of the three column vectors
  //      local_x_axis, local_y_axis, normal.
	
  // Transform each nodal position to local coordinates.
  for (i=0; i < 4; i++)   {
    local[i].x(global[i] % local_x_axis);
    local[i].y(global[i] % local_y_axis);
    local[i].z(global[i] % normal);
  }
}

void QuadMetric::calculate_angle_metric(VerdeVector node_pos[4],
                            VerdeVector quad_normal,
                            double *min_angle, double *max_angle)
{

   // Loop through all of the corners of the quad.
   *max_angle = 0.0;
   *min_angle = 6.5;  // Larger than 360 degrees
   VerdeVector next, prev;
   double angle;
   for(int i = 0; i < 4; i++)
   {
      next = node_pos[(i+1)%4] - node_pos[i];
      prev = node_pos[(i+3)%4] - node_pos[i];
      angle = quad_normal.vector_angle(next, prev);
      *max_angle = VERDE_MAX(*max_angle, angle);
      *min_angle = VERDE_MIN(*min_angle, angle);
   }

   *max_angle *= 180.0 / VERDE_PI; // Angle in degrees
   *min_angle *= 180.0 / VERDE_PI; // Angle in degrees
}

void QuadMetric::calculate_min_jacobian(VerdeVector node_pos[4],
                                         VerdeVector quad_normal,
                                                double *min_jac )
{
 
   *min_jac = VERDE_DBL_MAX;
 
   VerdeVector first, second;

   for ( int i = 0; i < 4; i++ )
   {
      first = node_pos[i] - node_pos[(i+1)%4];
      second = node_pos[i] - node_pos[(i+3)%4];
  
      double cur_jac = quad_normal % ( first * second );

      *min_jac = VERDE_MIN(*min_jac, cur_jac);
   
   }
}

void QuadMetric::calculate_min_scaled_jacobian(VerdeVector node_pos[4],
                                                VerdeVector quad_normal,
                                                double *min_scaled_jac )
{

   *min_scaled_jac = VERDE_DBL_MAX;
 
   VerdeVector first, second;

   for ( int i=0; i<4; i++ )
   {
      first = node_pos[i] - node_pos[(i+1)%4];
      second = node_pos[i] - node_pos[(i+3)%4];

      double rt_g11 = first.length();
      double rt_g22 = second.length();

      double cur_jac=0.; 
      if ( rt_g11 != 0. && rt_g22 != 0. ) {
         cur_jac = quad_normal % ( first * second ) / rt_g11 / rt_g22;
      }

      *min_scaled_jac = VERDE_MIN(*min_scaled_jac, cur_jac);

   }
}


void QuadMetric::calculate_max_condition(VerdeVector node_pos[4],
                                          VerdeVector quad_normal,
                                          double *max_condition )
{
   *max_condition = 0.;
 
   VerdeVector xxi, xet;

   VerdeVector normal = quad_normal;
   normal.normalize();

   double condition, det;

   for ( int i=0; i<4; i++ )
   {
      //xxi.set( nodes[(i+1)%4], nodes[i] );
      //xet.set( nodes[(i+3)%4], nodes[i] );
      xxi = node_pos[i] - node_pos[(i+1)%4];
      xet = node_pos[i] - node_pos[(i+3)%4];

      det = normal % ( xxi * xet );

      if ( det <  VERDE_DBL_MIN ) { 
         condition = 1.e+06; 
      }
      else
      {
         condition = ( xxi % xxi + xet % xet ) / det;
      }
   
      *max_condition = VERDE_MAX(*max_condition, condition);
   
   }

   *max_condition /= 2;
}




void QuadMetric::calculate_max_oddy(VerdeVector node_pos[4], 
                                    double *max_oddy )
{
 
   *max_oddy = 0.;
 
   VerdeVector first, second;

   double g, g11, g12, g22, cur_oddy;

   for ( int i = 0; i < 4; i++ )
   {
      first = node_pos[i] - node_pos[(i+1)%4];
      second = node_pos[i] - node_pos[(i+3)%4];

      g11 = first % first;
      g12 = first % second;
      g22 = second % second;
      g = g11*g22 - g12*g12;

      if ( g != 0. ) {
         cur_oddy = ( (g11-g22)*(g11-g22) + 4.*g12*g12 ) / 2. / g;
      }
      else {
         cur_oddy = 1.e+06;
      }

      *max_oddy = VERDE_MAX(*max_oddy, cur_oddy);

   }

}


void QuadMetric::print_summary()
{

   PRINT_INFO("   Function Name     Average    Std Dev    "
              "Minimum   (id)  Maximum  (id)\n"
              " -----------------  ---------  ---------  "
              "--------------  -------------\n");
  if(doAlgebraic)
  {
    PRINT_INFO("\nALGEBRAIC METRICS\n");
    for (int i = 0; i < 4; i++)
    {
      PRINT_INFO("% 18s ", metricNames[i]); // NO newline
      summary[i].print(); // Adds newline.
    }
  }
 
  if(doDiagnostic)
  {
    PRINT_INFO("\nDIAGNOSTIC METRICS\n");
    for (int i = 4; i < 5; i++)
    {
      PRINT_INFO("% 18s ", metricNames[i]); // NO newline
      summary[i].print(); // Adds newline.
    }
  }

  if(doTraditional)
  {
    PRINT_INFO("\nTRADITIONAL METRICS\n");
    for (int i = 5; i < NUM_QUAD_METRICS ; i++)
    {
      PRINT_INFO("% 18s ", metricNames[i]); // NO newline
      summary[i].print(); // Adds newline.
    }
  }
}

int QuadMetric::number_failed_quads()
{
   int number = 0;
   
   for (int i = 0; i < NUM_QUAD_METRICS; i++)
   {
      number += failedQuads[i].size();
   }
   return number;
}

void QuadMetric::get_quad_normal(VerdeVector node_pos[4], VerdeVector &normal)
{
   // get the normal at node_pos[0]

   VerdeVector one = node_pos[1] - node_pos[0];
   VerdeVector two = node_pos[3] - node_pos[0];
   normal = one * two;
   normal.normalize();

   // get the normal at node_pos[1]

   one = node_pos[3] - node_pos[2];
   two = node_pos[1] - node_pos[2];
   VerdeVector normal1 = one * two;
   normal1.normalize();

   // check to see that they point in the same direction; an angle larger than
   // 180 could goof up the normal direction.  IF they are the same, we are done

   if (normal % normal1 > 0.0)
   {
     normal += normal1;
     normal *= 0.5;
     return;
   }
   
   // check the third normal if necessary to see which is correct

   one = node_pos[2] - node_pos[1];
   two = node_pos[0] - node_pos[1];
   VerdeVector normal2 = one * two;
   normal2.normalize();

   if (normal % normal2 < 0.0)
   {
     normal += normal2;
     normal *= 0.5;
   }
   else
   {
     normal1 += normal2;
     normal1 *= 0.5;
     normal = normal1;
   }
}
   

   
int QuadMetric::print_failed_quads(VerdeBoolean verbose)
{
  int num_failed = number_failed_quads();
  
  PRINT_INFO("   Number of Failed Quad Metrics: %d\n", num_failed);
  
  // check for verbose output

  if (num_failed)
  {
    
    for (int i = 0; i < NUM_QUAD_METRICS; i++)
    {
      int num_quads = failedQuads[i].size();
      if (num_quads)
      {
        if (verbose)
        {
          // print the first line
          FailedElem *quad = failedQuads[i][0];
          PRINT_INFO("\n Failed Metric      Block    Element     Value "
              "    Min. Valid   Max. Valid \n"
              "----------------   -------   -------   --------- "
              "  ----------   ----------\n");
          PRINT_INFO("%16s   %6d   %7d   %10.7f   %10.7f   %10.7f\n",
              metricNames[i],
              quad->block_id(), quad->original_id(),
              quad->metric_value(),range[i].min, range[i].max);
          // print subsequent lines
          
          for (int j = 1; j < num_quads; j++)
          {
            quad = failedQuads[i][j];
            PRINT_INFO("                   %6d   %7d   %10.7f\n",
                quad->block_id(), quad->original_id(),
                quad->metric_value());
          }
        }  
        PRINT_INFO("      Failed %s: %d  "
            "\t(valid range: %10.3f to %10.3f)\n", metricNames[i],
            failedQuads[i].size(),range[i].min, range[i].max);
        
      }      
    }
  }
  return num_failed;
}

void QuadMetric::failed_quad_list(int index, 
    std::deque<FailedQuad*> &quad_list)
{
  for(unsigned int i=0; i<failedQuads[index].size(); i++)
    quad_list.push_back(failedQuads[index][i]);
}


void QuadMetric::reset()
{
  for(int i = 0; i < NUM_QUAD_METRICS; i++)
  {
    summary[i].reset();
  }
  totalArea = 0.0;
}

void QuadMetric::reset_all()
{
  reset();
  for(int i=0; i<NUM_QUAD_METRICS; i++)
  {
    for(unsigned int j=0; j<failedQuads[i].size(); j++)
      delete failedQuads[i][j];
    failedQuads[i].clear();
  }
  
}

void QuadMetric::reset_range()
{
  memcpy( range, range_backup, sizeof range );
}

void QuadMetric::calculate_shear(VerdeVector node_pos[4], 
                                 VerdeVector &quad_normal,
                                 double *min_shear )
{

// we might want to change it to this

   VerdeVector normal = quad_normal;
   normal.normalize();
   
   double shear;

   *min_shear = 1;

   VerdeVector xxi, xet;

   double deta;
   double g11, g22;

   for ( int i=0; i<4; i++ )
   {
         xxi = node_pos[i] - node_pos[(i+1)%4];
         xet = node_pos[i] - node_pos[(i+3)%4];

         g11 = xxi % xxi;
         g22 = xet % xet;
         deta = normal % ( xxi * xet );

         double den = sqrt( g11 * g22 );
         if ( den > VERDE_DBL_MIN )
         {
            shear = deta / den;
            *min_shear = VERDE_MIN(*min_shear, shear);
         }
         else
           *min_shear = 0;

   }

   *min_shear = VERDE_MAX(*min_shear,0);

/*
// previously as defined below
 
  *min_shear = 1;
 
  int result;
  double w11,w21,w12,w22;
  double gw11,gw12,gw22;
  double qw11,qw21,qw12,qw22;
  VerdeVector xxi, xet;

  get_weight(w11,w21,w12,w22);
  double detw = determinant(w11,w21,w12,w22);

  metric_matrix(w11,w21,w12,w22,gw11,gw12,gw22);

  result = skew_matrix(gw11,gw12,gw22,detw,qw11,qw21,qw12,qw22);
  double detqw = 0;
  
  if(result == VERDE_SUCCESS)
     detqw = determinant(qw11,qw21,qw12,qw22);

  if ( detqw > VERDE_DBL_MIN && result == VERDE_SUCCESS ) {

    double det;
    double q11,q21,q12,q22;
    double g11, g12, g22;
    double x11,x21,x12,x22;
    for ( int i=0; i<4; i++ ) {

      xxi = node_pos[i] - node_pos[(i+1)%4];
      xet = node_pos[i] - node_pos[(i+3)%4];
    
      g11 = xxi % xxi;
      g12 = xxi % xet;
      g22 = xet % xet;
      det = normal % ( xxi * xet );
      if ( det <= VERDE_DBL_MIN ) { 
        *min_shear = 0;
        break;
      }

      result = skew_matrix(g11,g12,g22,det,q11,q21,q12,q22);
      if ( result == VERDE_FAILURE ) { 
        *min_shear = 0;
        break; 
      }

      form_t(q11,q21,q12,q22,qw11,qw21,qw12,qw22,detqw,x11,x21,x12,x22);

      double detx = determinant(x11,x21,x12,x22);
      double normsq = norm_squared(x11,x21,x12,x22);

      double shear=0;
      if ( normsq > VERDE_DBL_MIN ) {
        shear = 2 * detx / normsq;
      }

      *min_shear = VERDE_MIN(*min_shear, shear);
   
    }
  }
  else {
    *min_shear=0;
  }
*/

}

void QuadMetric::calculate_shape(VerdeVector node_pos[4], 
                                 VerdeVector &quad_normal,
                                 double *min_shape )
{
 
  *min_shape = 1;
  
  double shape;
  VerdeVector xxi, xet;

  VerdeVector normal = quad_normal;
  normal.normalize();
  
  double deta;
  double g11, g22;
  
  for ( int i=0; i<4; i++ ) {
    
    xxi = node_pos[i] - node_pos[(i+1)%4];
    xet = node_pos[i] - node_pos[(i+3)%4];
    
    g11 = xxi % xxi;
    g22 = xet % xet;
    deta = normal % ( xxi * xet );
    
    double den = g11 + g22;
    if ( den > VERDE_DBL_MIN ) {
      shape = 2 * deta / den;  
      *min_shape = VERDE_MIN(*min_shape, shape);
    }
    else {
      *min_shape = 0;
    }
    
  }
  
   *min_shape = VERDE_MAX(*min_shape,0);
}

void QuadMetric::calculate_relative_size(VerdeVector node_pos[4], 
                                         VerdeVector &quad_normal,
                                         double *rel_size )
{
 
  *rel_size = 0;
 
  double w11,w21,w12,w22;
  VerdeVector xxi, xet;

  VerdeVector normal = quad_normal;
  normal.normalize();

  get_weight(w11,w21,w12,w22);
  double detw = determinant(w11,w21,w12,w22);


 if ( detw > VERDE_DBL_MIN ) {

      double deta0, deta2;

      xxi = node_pos[1] - node_pos[0];
      xet = node_pos[3] - node_pos[0];

      deta0 = normal % ( xxi * xet );

      xxi = node_pos[2] - node_pos[3];
      xet = node_pos[2] - node_pos[1];

      deta2 = normal % ( xxi * xet );

      double tau = 0.5*( deta0 + deta2 ) / detw;

      if ( tau > VERDE_DBL_MIN ) {
         *rel_size = VERDE_MIN( tau*tau, 1/tau/tau );
      }
   }

/*
  if ( detw > VERDE_DBL_MIN ) {

    double deta;
    for ( int i=0; i<4; i++ ) {

      xxi = node_pos[i] - node_pos[(i+1)%4];
      xet = node_pos[i] - node_pos[(i+3)%4];
    
      deta = quad_normal % ( xxi * xet );

      double tau = deta / detw;

      double size=0;
      if ( tau > VERDE_DBL_MIN ) {
        size = VERDE_MIN( tau, 1/tau );
      }

      *rel_size += size*size;
   
    }
  }
  *rel_size = 0.5*sqrt(*rel_size);
*/
}

void QuadMetric::calculate_shape_size(VerdeVector node_pos[4], 
                                      VerdeVector &quad_normal,
                                      double *rel_shape_size )
{

  double rel_size = 0;
  calculate_relative_size( node_pos, quad_normal, &rel_size );
  
  double shape = 0;
  calculate_shape( node_pos, quad_normal, &shape );

  *rel_shape_size = rel_size * shape;

//how it was done before
/*
  *rel_shape_size = 0;
  double min_shape = 1;
  double rel_size = 0;

  double w11,w21,w12,w22;
  VerdeVector xxi, xet;

  get_weight(w11,w21,w12,w22);
  double detw = determinant(w11,w21,w12,w22);

  if ( detw > VERDE_DBL_MIN ) {

    double shape;
    double deta;
    double a11,a21,a12,a22;
    double g11, g12;
    double t11,t21,t12,t22;
    for ( int i=0; i<4; i++ ) {

      xxi = node_pos[i] - node_pos[(i+1)%4];
      xet = node_pos[i] - node_pos[(i+3)%4];
    
      g11 = xxi % xxi;
      g12 = xxi % xet;
      deta = quad_normal % ( xxi * xet );

      jacobian_matrix(g11,g12,deta,a11,a21,a12,a22);

      form_t(a11,a21,a12,a22,w11,w21,w12,w22,detw,t11,t21,t12,t22);
      double tau = determinant(t11,t21,t12,t22);

      double normsq = norm_squared(t11,t21,t12,t22);

      shape = 2 * tau / normsq;

      double size=0;
      if ( tau > VERDE_DBL_MIN ) {
        size = VERDE_MIN( tau, 1/tau );
      }

      rel_size += size*size;
      min_shape = VERDE_MIN(min_shape, shape);
   
    }
  }
  min_shape = VERDE_MAX(min_shape,0);
  rel_size = 0.5*sqrt(rel_size);
  *rel_shape_size = min_shape * rel_size;
*/

}


double QuadMetric::area(QuadRef &quad_ref)
{

   // Get positions of each node of the quad
  
   VerdeVector node_pos[4];
   for ( int ii = 0; ii < 4; ii++ )
   {
      node_pos[ii] = quad_ref.node_coordinates(ii);
   }

   // get area

   double area;
   double dummy;

   calculate_2d_quad_quality(node_pos,&dummy, &dummy, &dummy, &dummy,
                             &area, QUAD_AREA);

   return area;
}

void QuadMetric::get_weight( double &m11,double &m21,
                             double &m12,double &m22)
{
  if ( elementShape == 1 ) {
     m11=1;
     m21=0;
     m12=0;
     m22=aspectRatio;
   }

  double scale = sqrt(elementSize/(m11*m22-m21*m12));

  m11 *= scale;
  m21 *= scale;
  m12 *= scale;
  m22 *= scale;
     
}

VerdeVector QuadMetric::plane_normal(std::vector<VerdeVector*> &positions)
{
// Newells method for determining approximate plane equation.
// Plane equation is of the form:
// 0 = plane_D +plane_normal.x()*X + plane_normal.y()*Y + plane_normal.z()*Z

  VerdeVector vector_diff;
  VerdeVector vector_sum;
  VerdeVector ref_point = VerdeVector (0.0, 0.0, 0.0);
  VerdeVector tmp_vector;
  VerdeVector normal = VerdeVector(0.0, 0.0, 0.0);
  VerdeVector *vector1, *vector2;

  unsigned int list_size = positions.size();
  for (unsigned int i=0; i<list_size; i++)
  {
    vector1 = positions[i];
    vector2 = positions[(i+1)%list_size];
    vector_diff = (*vector2) - (*vector1);
    ref_point += (*vector1);
    vector_sum = (*vector1) + (*vector2);
    
    tmp_vector.set(vector_diff.y() * vector_sum.z(),
        vector_diff.z() * vector_sum.x(),
        vector_diff.x() * vector_sum.y());
    normal += tmp_vector;
  }
  normal.normalize();
  normal *= -1.0;
  return normal;
}
  
