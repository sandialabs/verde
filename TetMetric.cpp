//-------------------------------------------------------------------------
// Filename      : TetMetric.cc 
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

#include "stdlib.h"
#include "TetMetric.hpp"

#include "NodeRef.hpp"
#include "VerdeVector.hpp"
#include "VerdeMessage.hpp"
#include "VerdeMatrix.hpp"
#include "TetRef.hpp"
#include "FailedTet.hpp"

#if defined(USE_VERDICT)
  #include "verdict.h"
#endif


const char* TetMetric::metricNames[NUM_TET_METRICS] =
{ "Shear",
  "Shape",
  "Relative Size",
  "Shape and Size", 
  "Element Volume",
  "Aspect Ratio",
  "Aspect Ratio Gamma",
  "Condition No.",
  "Jacobian",
  "Scaled Jacobian"};

min_max TetMetric::range[NUM_TET_METRICS] =
{ {0.3, 1.0},                // Shear
  {0.3, 1.0},                // Shape
  {0.5, 1.0},                // Relative Size
  {0.2, 1.01},               // Shape Size
  {0.0, VERDE_DBL_MAX},      // Element Volume
  {1.0, 3.0},                // Aspect Ratio
  {1.0, 3.0},                // Aspect Ratio Gamma
  {1.0, 3.0},                // Condition Number
  {0.0, VERDE_DBL_MAX},      // Jacobian
  {0.5, 1.414},              // Scaled Jacobian
};

const min_max TetMetric::range_backup[NUM_TET_METRICS] =
{ {0.3, 1.0},                // Shear
  {0.3, 1.0},                // Shape
  {0.5, 1.0},                // Relative Size
  {0.2, 1.01},               // Shape Size
  {0.0, VERDE_DBL_MAX},      // Element Volume
  {1.0, 3.0},                // Aspect Ratio
  {1.0, 3.0},                // Aspect Ratio Gamma
  {1.0, 3.0},                // Condition Number
  {0.0, VERDE_DBL_MAX},      // Jacobian
  {0.5, 1.414},              // Scaled Jacobian
};


TetMetric *TetMetric::instance_ = NULL;

TetMetric::TetMetric()
{
  totalVolume = 0.0;
}


TetMetric::~TetMetric()
{
  // need to go through and clean out the failed tets
  reset_all();
}

TetMetric *TetMetric::instance()
{
   if (!instance_)
      instance_ = new TetMetric();
   return instance_;
}


int  TetMetric::calculate (TetRef &tet_ref, int which_metric)
{
   VerdeBoolean malformed_found = VERDE_FALSE;

#if defined(USE_VERDICT)

   which_metric = which_metric;
   double node_pos[4][3];
   int ii;
   for ( ii = 0; ii < 4; ii++ )
   {
      node_pos[ii][0] = tet_ref.node_coordinates(ii).x();
      node_pos[ii][1] = tet_ref.node_coordinates(ii).y();
      node_pos[ii][2] = tet_ref.node_coordinates(ii).z();
   }
 
   int metric_flag = V_TET_ALL - V_TET_DISTORTION;
   TetMetricVals values;

   v_tet_quality(4, node_pos, metric_flag, &values);

   metrics[ASPECT_RATIO] = values.aspect;
   metrics[ASPECT_RATIO_GAMMA] = values.aspect_gamma;
   metrics[TET_VOLUME] = values.volume;
   metrics[TET_CONDITION_NUMBER] = values.condition;
   metrics[TET_JACOBIAN] = values.jacobian;
   metrics[TET_NORM_JACOBIAN] = values.scaled_jacobian;
   metrics[TET_SHEAR] = values.shear;
   metrics[TET_SHAPE] = values.shape;
   metrics[TET_RELSIZE] = values.relative_size;
   metrics[TET_SHAPE_SIZE] = values.shape_and_size;

   totalVolume += values.volume;

#else

   VerdeVector node_pos[4];

   int ii;
   for ( ii = 0; ii < 4; ii++ )
   {
      node_pos[ii] = tet_ref.node_coordinates(ii);
   }

   // All calculations are based solely on the passed-in node coordinates;
   // the underlying tet may or may not exist.

// Tet element aspect ratio formulae taken from V. N. Parthasarathy et al,
// "A comparison of tetrahedron quality measures", Finite Elem. Anal. Des., 
// Vol 15(1993), 255-261; seven formulae of aspect ratio are given:
// 
// beta = CR / (3.0 * IR)   (CR = circumsphere rad, IR = inscribed sphere rad)
// sigma = Smax/(4.898979*IR)  (Smax = max edge length)
// omega = CR/(0.612507*Smax)
// tau = Smax/Smin   (Smin = min edge length)
// alpha = Savg**3 / (8.479670*V)    (V = element volume = 1/6 * 
//                                        det(1 x1 y1 z1, 1 x2 y2 z2,
//                                            1 x3 y3 z3, 1 x4 y4 z4)
// gamma = Srms**3 / (8.479670*V)    (Srms = sqrt(Sum(Si**2)/6), Si = edge length)
// kappa = (complicated, therefore not used)
//
// Below, all these aspect ratios are implemented (except for kappa), but commented
// out (they did work once, really).  The one remaining is the most common one used
// in the literature, which is beta.  The formulae above are normalized using the
// value of the given quantity for an equilateral tet.
// 
// 

   // calculate element aspect ratio

   double one_sixth = 1.0 / 6.0;

#define AREA(v1,v2) ((0.5 * (v1 * v2)).length())


   VerdeVector side0 = node_pos[1] - node_pos[0];
   VerdeVector side1 = node_pos[2] - node_pos[1];
   VerdeVector side2 = node_pos[0] - node_pos[2];
   VerdeVector side3 = node_pos[3] - node_pos[0];
   VerdeVector side4 = node_pos[3] - node_pos[1];
   VerdeVector side5 = node_pos[3] - node_pos[2];

   double area_sum = 0.0;

   area_sum = AREA(side0, -side2) + AREA(side3, side0) + 
       AREA(side4, side1) + AREA(side3, -side2);

   double volume = 0.0;
   double jacobian_value = 0.0;

   // calculate jacobian
   
   jacobian_value = jacobian(side0, side1, side2,
                             side3, side4, side5,
                             &metrics[TET_NORM_JACOBIAN]);
   metrics[TET_JACOBIAN] = jacobian_value;
   volume = one_sixth * jacobian_value;
   metrics[TET_VOLUME] = volume;
   totalVolume += volume;

   if (which_metric == TET_VOLUME)
     return malformed_found;
   
   // aspect ratio - beta (CR / IR)

  if( doTraditional )
  {

   double inscribed_rad = 1.0;
   double circum_rad = 0.0;
  
   if (volume != 0.0 && area_sum != 0.0) {
    
      inscribed_rad = 3.0 * volume / area_sum;


      // calculate circumradius

      VerdeVector top = side3.length_squared() * ( side0 * -side2)
          + side2.length_squared() * (side3 * side0)
          + side0.length_squared() * (-side2 * side3);

      VerdeMatrix bot(3,3);
      bot.set(0,0,side0.x());
      bot.set(0,1,side0.y());
      bot.set(0,2,side0.z());
      bot.set(1,0,-side2.x());
      bot.set(1,1,-side2.y());
      bot.set(1,2,-side2.z());
      bot.set(2,0,side3.x());
      bot.set(2,1,side3.y());
      bot.set(2,2,side3.z());

      top = top / (2.0*bot.determinant());
      circum_rad = top.length();

      if (circum_rad == 0.0) metrics[ASPECT_RATIO] = pow(VERDE_DBL_MAX,0.333);
      else
         metrics[ASPECT_RATIO] = circum_rad / (3.0 * inscribed_rad);
   }
   

/*
  // aspect ratio - theta
  if ( which_metric == -1 || which_metric == ASPECT_RATIO_SIGMA ) {
  metrics[ASPECT_RATIO_SIGMA] = smax / (4.898979 * inscribed_rad);
  }

  // aspect ratio - omega
  if ( which_metric == -1 || which_metric == ASPECT_RATIO_OMEGA ) {
  metrics[ASPECT_RATIO_OMEGA] = circum_rad / (0.612507 * smax);
  }

  // aspect ratio - tau
  if ( which_metric == -1 || which_metric == ASPECT_RATIO_TAU ) {
  metrics[ASPECT_RATIO_TAU] = smax / smin;
  }

  // aspect ratio - alpha
  if ( which_metric == -1 || which_metric == ASPECT_RATIO_ALPHA ) {
  metrics[ASPECT_RATIO_ALPHA] = pow(savg, 3) / (8.47967 * metrics[TET_VOLUME]);
  }

  */

   // aspect ratio - gamma

   if (volume != 0.0) {
      double srms = sqrt(one_sixth *
                         (side0.length_squared() + side1.length_squared() +
                          side2.length_squared() + side3.length_squared() +
                          side4.length_squared() + side5.length_squared()));
      metrics[ASPECT_RATIO_GAMMA] = pow(srms, 3) / (8.47967 * volume);
   }
   else {
      metrics[ASPECT_RATIO_GAMMA] = pow(VERDE_DBL_MAX, 0.333);
   }

   // Condition number
   
   metrics[TET_CONDITION_NUMBER] = condition(side0, side2, side3 );

  }


   // shear, shape, relative size, shape-size

   if(doAlgebraic)
   {
     metrics[TET_SHEAR] = shear(side0, side1, side2, side3, side4, side5);
     metrics[TET_SHAPE] = shape(side0, -side2, side3);
     metrics[TET_RELSIZE] = rel_size(-side0, side2, side3);
     metrics[TET_SHAPE_SIZE] = shape_size(-side0, side2, side3);
   }
  
#endif

   // put information in QualitySummary for each metric
   // create failed elements as necessary

   int element_id = tet_ref.original_id();
 
   if( doAlgebraic )
   { 
     for (int i = 0; i < 4; i++)
     {
       summary[i].add_sample(metric(i),element_id);
       // apply a tolerance to the range of acceptable values
       if (metrics[i] > (range[i].max + metricTolerance) ||
                        metrics[i] < (range[i].min - metricTolerance))
       {
         FailedTet *bad_elem = new FailedTet(tet_ref,metrics[i]);
         failedTets[i].push_back(bad_elem);
         malformed_found = VERDE_TRUE;
       }
     }
   }

   if( doDiagnostic)
   { 
     for (int i = 4; i < 5; i++)
     {
       summary[i].add_sample(metric(i),element_id);
       // apply a tolerance to the range of acceptable values
       if (metrics[i] > (range[i].max + metricTolerance) ||
                        metrics[i] < (range[i].min - metricTolerance))
       {
         FailedTet *bad_elem = new FailedTet(tet_ref,metrics[i]);
         failedTets[i].push_back(bad_elem);
         malformed_found = VERDE_TRUE;
       }
     }
   }

   if( doTraditional)
   { 
     for (int i = 5; i < NUM_TET_METRICS; i++)
     {
       summary[i].add_sample(metric(i),element_id);
       // apply a tolerance to the range of acceptable values
       if (metrics[i] > (range[i].max + metricTolerance) ||
                        metrics[i] < (range[i].min - metricTolerance))
       {
         FailedTet *bad_elem = new FailedTet(tet_ref,metrics[i]);
         failedTets[i].push_back(bad_elem);
         malformed_found = VERDE_TRUE;
       }
     }
   }

   return malformed_found;
}

inline double tet_normalize_jacobian( double &jacobi, 
                                  VerdeVector &v1,
                                  VerdeVector &v2,
                                  VerdeVector &v3 )
{
  double norm_jac = 0.0;
 
  if ( jacobi != 0.0 )
  { 
     double l1, l2, l3, length_product;
     // Note: there may be numerical problems if one is a lot shorter
     // than the others this way. But scaling each vector before the
     // triple product would involve 3 square roots instead of just
     // one.
     l1 = v1.length_squared();
     l2 = v2.length_squared();
     l3 = v3.length_squared();
     length_product = sqrt( l1 * l2 * l3 );
  
    // if some numerical scaling problem, or just plain roundoff,
    // then push back into range [-1,1]. 
     if ( length_product < fabs(jacobi) )
        length_product = fabs(jacobi);
     norm_jac = sqrt(2.0) * jacobi / length_product;

  }
 
  return norm_jac;
}

double TetMetric::jacobian(VerdeVector &side01, VerdeVector &side12, VerdeVector &side20,
                            VerdeVector &side03, VerdeVector &side13, VerdeVector &side23,
                            double *normalized_jacobian)
{
  double jacobi;
  double min_jacobi = VERDE_DBL_MAX;
  double temp_norm_jac, min_norm_jac = VERDE_DBL_MAX;

  // node 0
  jacobi = side03 % ( side01 * (-side20) );
  min_jacobi = VERDE_MIN(jacobi, min_jacobi);

    // compute normalized jacobian
  if (normalized_jacobian) {
    temp_norm_jac = tet_normalize_jacobian( jacobi, side03, side01, side20);
    min_norm_jac = VERDE_MIN(min_norm_jac, temp_norm_jac);
  }
  else // non-scaled jacobian equal at all 4 nodes, equal to 6*volume.
    return min_jacobi;


  // node 1
  jacobi =  side13 % ( side12 * (-side01) );
  min_jacobi = VERDE_MIN(jacobi, min_jacobi);

    // compute normalized jacobian
  if (normalized_jacobian) {
    temp_norm_jac = tet_normalize_jacobian( jacobi, side13, side12, side01);
    min_norm_jac = VERDE_MIN(min_norm_jac, temp_norm_jac);
  }

  // node 2
  jacobi = -side23 % ( (-side12) * side20 );
  min_jacobi = VERDE_MIN(jacobi, min_jacobi);

    // compute normalized jacobian
  if (normalized_jacobian) {
    temp_norm_jac = tet_normalize_jacobian( jacobi, side23, side12, side20);
    min_norm_jac = VERDE_MIN(min_norm_jac, temp_norm_jac);
  }

  // node 3
  jacobi = -side03 % ( (-side23) * (-side13) );
  min_jacobi = VERDE_MIN(jacobi, min_jacobi);

    // compute normalized jacobian
  if (normalized_jacobian) {
    temp_norm_jac = tet_normalize_jacobian( jacobi, side03, side23, side13);
    min_norm_jac = VERDE_MIN(min_norm_jac, temp_norm_jac);
  }

  if ( normalized_jacobian )
    *normalized_jacobian = min_norm_jac;
  
  return min_jacobi;
}



void TetMetric::print_summary()
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
  
     for (int i = 5; i < NUM_TET_METRICS; i++)
     {
       PRINT_INFO("% 18s ", metricNames[i]); // NO newline
       summary[i].print(); // Adds newline.
     }
   }
}

int TetMetric::number_failed_tets()
{
   int number = 0;
   
   for (int i = 0; i < NUM_TET_METRICS; i++)
   {
      number += failedTets[i].size();
   }
   return number;
}

   
int TetMetric::print_failed_tets(VerdeBoolean verbose)
{
  int num_failed = number_failed_tets();
  
  PRINT_INFO("   Number of Failed Tet Metrics: %d\n", num_failed);
  
  if (num_failed)
  {
    
    for (int i = 0; i < NUM_TET_METRICS; i++)
    {
      int num_tets = failedTets[i].size();
      if (num_tets)
      {
        if (verbose)
        {
          // print the first line
          FailedTet *tet = failedTets[i][0];
          PRINT_INFO("\n Failed Metric      Block    Element     Value "
              "    Min. Valid   Max. Valid \n"
              "----------------   -------   -------   --------- "
              "  ----------   ----------\n");
          PRINT_INFO("%16s   %6d   %7d   %10.7f   %10.7f   %10.7f\n",
              metricNames[i],
              tet->block_id(), tet->original_id(),
              tet->metric_value(),range[i].min, range[i].max);
          // print subsequent lines
          for (int j = 1; j < num_tets; j++)
          {
            tet = failedTets[i][j];
            PRINT_INFO("                   %6d   %7d   %10.7f\n",
                tet->block_id(), tet->original_id(),
                tet->metric_value());
          }
        }
        PRINT_INFO("      Failed %s: %d  "
            "\t(valid range: %10.3f to %10.3f)\n", metricNames[i],
            failedTets[i].size(),range[i].min, range[i].max);
      }
    }
  }
  return num_failed;
}

void TetMetric::failed_tet_list(int index, std::deque<FailedTet*> &tet_list)
{
  for(unsigned int i=0; i<failedTets[index].size(); i++)
    tet_list.push_back(failedTets[index][i]);
}

void TetMetric::reset()
{
   for(int i = 0; i < NUM_TET_METRICS; i++)
   {
      summary[i].reset();
   }
   totalVolume = 0.0;
}

void TetMetric::reset_all()
{
  reset();
  for(int i=0; i<NUM_TET_METRICS; i++)
  {
    for(unsigned int j=0; j<failedTets[i].size(); j++)
      delete failedTets[i][j]; 
    failedTets[i].clear();
  }
}

void TetMetric::reset_range()
{
  memcpy(range, range_backup, sizeof range );
}


double TetMetric::condition(VerdeVector &side01, VerdeVector &side20,
			     VerdeVector &side03)
{

  double condition, term1, term2, det;
  double max_condition = -VERDE_DBL_MAX;
  double rt3 = sqrt(3.0);
  double rt6 = sqrt(6.0);

  VerdeVector xxi, xet, xze;

  // condition number is same at each of the 4 nodes, so only calculate at
  // node 0

  xxi = side01;
  xet = (-2*side20-side01)/rt3;
  xze = (3*side03+side20-side01)/rt6;

  term1 = xxi % xxi + xet % xet + xze % xze;
  term2 = ( xxi * xet ) % ( xxi * xet ) + ( xet * xze ) % ( xet * xze ) + ( xze * xxi ) % ( xze * xxi );
  det = xxi % ( xet * xze );

  if ( det <= VERDE_DBL_MIN ) { 
     return 1.e+06;
  }
  else {
    condition = sqrt( term1 * term2 ) / det;
  }

  max_condition = VERDE_MAX( condition, max_condition );

  return max_condition/3.0;
}

void TetMetric::get_weight(VerdeVector &w1, VerdeVector &w2, VerdeVector &w3)
{

   const double rt3 = sqrt(3.0);

   if (elementShape == 1 ) {
      w1.set(1,0,0);
      w2.set(0.5,0.5*rt3,0);
      w3.set(0.5,rt3/6.0,sqrt(2.0)/rt3);
   }

   if (elementShape == 2 ) {
      w1.set(1,0,0);
      w2.set(0,1,0);
      w3.set(0,0,aspectRatio);
   }

   if (elementShape == 3 ) {
      w1.set(1,0,0);
      w2.set(0.5,0.5*rt3,0);
      w3.set(0.5,rt3/6.0,aspectRatio*sqrt(2.0)/rt3);
   }

   double scale = pow(6.*elementSize/determinant(w1,w2,w3),0.3333333333333);

   w1 *= scale;
   w2 *= scale;
   w3 *= scale;
}


double TetMetric::shear(VerdeVector s0, VerdeVector s1, VerdeVector s2,
                        VerdeVector s3, VerdeVector s4, VerdeVector s5)
{

   double min_skew=VERDE_DBL_MAX;
   double skew;

   VerdeVector w1, w2, w3;
   get_weight(w1,w2,w3);
   double detw = determinant(w1,w2,w3);



   VerdeVector neg_s2 = -s2;
   
   if ( detw > VERDE_DBL_MIN && determinant(s0,neg_s2,s3) > VERDE_DBL_MIN ) {

      VerdeVector q1, q2, q3;
      VerdeVector qw1, qw2, qw3;

      form_q(s0,-s2,s3,q1,q2,q3);
      form_q(w1,w2,w3,qw1,qw2,qw3);
      skew = skew_x(q1,q2,q3,qw1,qw2,qw3);
      min_skew = VERDE_MIN( skew, min_skew );

      form_q(s1,-s0,s4,q1,q2,q3);
      form_q(w2-w1,-w1,w3-w1,qw1,qw2,qw3);
      skew = skew_x(q1,q2,q3,qw1,qw2,qw3);
      min_skew = VERDE_MIN( skew, min_skew );

      form_q(s2,-s1,s5,q1,q2,q3);
      form_q(-w2,w1-w2,w3-w2,qw1,qw2,qw3);
      skew = skew_x(q1,q2,q3,qw1,qw2,qw3);
      min_skew = VERDE_MIN( skew, min_skew );

      form_q(s3,s4,s5,q1,q2,q3);
      form_q(w3,w1-w3,w2-w1,qw1,qw2,qw3);
      skew = skew_x(q1,q2,q3,qw1,qw2,qw3);
      min_skew = VERDE_MIN( skew, min_skew );

   }
   else {
      return 0;
   }
   return min_skew;
}

double TetMetric::shape(VerdeVector xxi, VerdeVector xet, VerdeVector xze)
{

   double shape=0;
   const double two_thirds = 2.0/3.0;
   const double rt2 = sqrt(2.0);

   double alp = xxi % (xet *xze);

   if ( alp > VERDE_DBL_MIN) {

     double num = 3 * pow( rt2 * alp, two_thirds );
     double den = 1.5*(xxi%xxi+xet%xet+xze%xze)-(xxi%xet+xet%xze+xze%xxi);

     if ( den > VERDE_DBL_MIN) {
       shape = num / den;
     }
   }
   return shape;


/*
   double shape=0;
   VerdeVector w1, w2, w3;

   get_weight(w1,w2,w3);

   double detw = determinant(w1,w2,w3);

   if ( detw > VERDE_DBL_MIN && determinant(xxi,xet,xze) > VERDE_DBL_MIN ) {

      VerdeVector t1, t2, t3;
      form_t(xxi,xet,xze,w1,w2,w3,t1,t2,t3);
  
      VerdeVector u1, u2, u3;
      inverse(t1,t2,t3,u1,u2,u3);

      double normsq1 = norm_squared(t1,t2,t3);
      double normsq2 = norm_squared(u1,u2,u3);
      double kappa = sqrt( normsq1 * normsq2 );

      if ( kappa > VERDE_DBL_MIN ) {
         shape = 3/kappa;
      }

   }
   return shape;
*/
}

double TetMetric::rel_size(VerdeVector xxi, VerdeVector xet, VerdeVector xze)
{
   double size=0;
   VerdeVector w1, w2, w3;

   get_weight(w1,w2,w3);

   double detw = determinant(w1,w2,w3);
   double deta = determinant(xxi,xet,xze);

   if ( detw > VERDE_DBL_MIN && deta > VERDE_DBL_MIN ) {

      size = pow(deta/detw,2);
      if ( size > 1 ) { size = (double)(1)/size; }

   }
   return size;
}

double TetMetric::shape_size(VerdeVector xxi, VerdeVector xet, VerdeVector xze)
{

  double tet_size  = rel_size( xxi, xet, xze );
  double tet_shape = shape( -xxi, -xet, xze );

  return tet_size * tet_shape;

/*
   double shape=0;
   double ss=0;
   VerdeVector w1, w2, w3;

   get_weight(w1,w2,w3);

   double detw = determinant(w1,w2,w3);
   double deta = determinant(xxi,xet,xze);

   if ( detw > VERDE_DBL_MIN && determinant(xxi,xet,xze) > VERDE_DBL_MIN ) {

      VerdeVector t1, t2, t3;
      form_t(xxi,xet,xze,w1,w2,w3,t1,t2,t3);
  
      VerdeVector u1, u2, u3;
      inverse(t1,t2,t3,u1,u2,u3);

      double normsq1 = norm_squared(t1,t2,t3);
      double normsq2 = norm_squared(u1,u2,u3);
      double kappa = sqrt( normsq1 * normsq2 );

      if ( kappa > VERDE_DBL_MIN ) {
         shape = 3/kappa;
      }

      double size = deta/detw;
      if ( size > 1 ) { size = (double)(1)/size; }
      ss = size * shape;
   }
   return ss;
*/
}

double TetMetric::volume(TetRef &tet_ref)
{

  // calc volume

  #if defined(USE_VERDICT)

   double node_pos[4][3];
   int ii;
   for ( ii = 0; ii < 4; ii++ )
   {
      node_pos[ii][0] = tet_ref.node_coordinates(ii).x();
      node_pos[ii][1] = tet_ref.node_coordinates(ii).y();
      node_pos[ii][2] = tet_ref.node_coordinates(ii).z();
   }
 
    return v_tet_volume(4, node_pos); 

  #else

    calculate(tet_ref,TET_VOLUME);
  
    return metrics[TET_VOLUME];
 
  #endif
  
}
