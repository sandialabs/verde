//-------------------------------------------------------------------------
// Filename      : HexMetric.cc 
//
// Purpose       : This class performs metric calculations on Hex Elements.
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

#include "HexMetric.hpp"

#include "HexRef.hpp"
#include "VerdeVector.hpp"
#include "VerdeMessage.hpp"
#include "FailedHex.hpp"

#if defined(USE_VERDICT) || defined(USE_VERDICT_HEX)
#include "verdict.h"
#endif


const char* HexMetric::metricNames[NUM_HEX_METRICS] =
{ "Shear",
  "Shape", 
  "Relative Size",
  "Shape and Size",
  "Element Volume",
  "Aspect Ratio",
  "Skew",
  "Taper",
  "Stretch",
  "Diagonal Ratio",
  "Dimension",
  "Oddy",
  "Condition No.",
  "Jacobian",
  "Scaled Jacobian"};


min_max HexMetric::range[NUM_HEX_METRICS] =
{ {0.3, 1.0},                // Shear
  {0.3, 1.0},                // Shape
  {0.5, 1.0},                // Relative Size
  {0.2, 1.0},                // Shape and Size
  {0.0, VERDE_DBL_MAX},      // Element Volume
  {1.0, 4.0},                // Aspect Ratio
  {0.0, 0.5},                // Skew
  {0.0, 0.4},                // Taper
  {0.25, 1.0},               // Stretch
  {0.65, 1.0},               // Diagonal Ratio
  {0.0, VERDE_DBL_MAX},      // Characteristic dimension
  {-0.000001, 20.0},         // Oddy
  {0.999999, 8.0},           // Condition No.
  {0.0, VERDE_DBL_MAX},      // Jacobian
  {0.5, 1.01},               // Scaled Jacobian
};

const min_max HexMetric::range_backup[NUM_HEX_METRICS] =
{ {1.0, 4.0},                // Aspect Ratio
  {0.0, 0.5},                // Skew
  {0.0, 0.4},                // Taper
  {0.0, VERDE_DBL_MAX},      // Element Volume
  {0.25, 1.0},               // Stretch
  {0.65, 1.0},               // Diagonal Ratio
  {0.0, VERDE_DBL_MAX},      // Characteristic dimension
  {-0.000001, 20.0},         // Oddy
  {0.999999, 8.0},           // Condition No.
  {0.0, VERDE_DBL_MAX},      // Jacobian
  {0.5, 1.01},               // Scaled Jacobian
  {0.3, 1.0},                //Shear
  {0.3, 1.0},                // Shape.
  {0.5, 1.0},                // Relative Size
  {0.2, 1.0}                 // Shape and Size
};


HexMetric *HexMetric::instance_ = NULL;

HexMetric::HexMetric()
{
  aspectRatio2 = 1.0;
  totalVolume = 0.0;
}

HexMetric::~HexMetric()
{
  // clean up
  reset_all();
}

HexMetric *HexMetric::instance()
{
   if (!instance_)
      instance_ = new HexMetric();
   return instance_;
}


int HexMetric::calculate(HexRef &hex_ref, int which_metric)
{
   // calculate the node position vectors

#if defined(USE_VERDICT) || defined(USE_VERDICT_HEX)


    int malformed_found = VERDE_FALSE;
 
    double coordinates[8][3];
    for ( int ii = 0; ii < 8; ii++ )
    {
       coordinates[ii][0] = hex_ref.node_coordinates(ii).x();
       coordinates[ii][1] = hex_ref.node_coordinates(ii).y();
       coordinates[ii][2] = hex_ref.node_coordinates(ii).z();
    }
 
  if(which_metric == HEX_VOLUME)
  {
    metrics[HEX_VOLUME] = v_hex_volume(8, coordinates);
    totalVolume += metrics[HEX_VOLUME];
    return malformed_found;
  }

    HexMetricVals values;
    int metrics_flag = V_HEX_ALL - V_HEX_DISTORTION - V_HEX_SHEAR_AND_SIZE;
    v_hex_quality(8, coordinates, metrics_flag, &values);

    metrics[HEX_ASPECT] = values.aspect;
    metrics[HEX_SKEW] = values.skew;
    metrics[HEX_TAPER] = values.taper;
    metrics[HEX_VOLUME] = values.volume;
    metrics[HEX_STRETCH] = values.stretch;
    metrics[HEX_DIAGONALS] = values.diagonal;
    metrics[HEX_CHARDIM] = values.dimension;
    metrics[HEX_ODDY] = values.oddy;
    metrics[HEX_CONDITION] = values.condition;
    metrics[HEX_JACOBIAN] = values.jacobian;
    metrics[HEX_NORM_JACOBIAN] = values.scaled_jacobian;
    metrics[HEX_SHEAR] = values.shear;
    metrics[HEX_SHAPE] = values.shape;
    metrics[HEX_RELSIZE] = values.relative_size;
    metrics[HEX_SHAPE_SIZE] = values.shape_and_size;

    totalVolume += values.volume;

#else

   VerdeVector node_pos[9];
   
   for ( int ii = 1; ii < 9; ii++ )
   {
      node_pos[ii] = hex_ref.node_coordinates(ii-1);
   }

   int malformed_found = VERDE_FALSE;
  
   // All calculations are based solely on the passed-in node coordinates;

   localize_hex_coordinates(node_pos);


   // calculate element aspect ratio, skew and taper
   // code lifted almost directly from CON3D subroutine of NUMBERSII
   // Based on article by John Robinson, "CRE Method of element testing
   //  and the Jacobian shape parameters," Eng. Comput., 1987, Vol. 4,
   //  June, pp 113 - 118

   // ... Calculate ``Shape function'' parameters -
   // E1, F1, F2 = 0.0
   // store shape function parameters e, f, g as x, y, z in an array of
   // cubitvectors; that is:
   // e1 = efg[1].x();
   // f1 = efg[1].y();
   // g1 = efg[1].z();
   // etc 

   VerdeVector efg[9];

   calculate_hex_efg(node_pos, efg);

   // element area
   metrics[HEX_VOLUME] = efg[2].x() * efg[3].y() * efg[4].z() / 64.0;
   totalVolume += metrics[HEX_VOLUME];

   if (which_metric == HEX_VOLUME)
     return malformed_found;
   
   if( Metric::doTraditional )
   {
     // aspect ratios
     double aspect_xy = VERDE_MAX (efg[2].x(), efg[3].y()) /
       VERDE_MIN (efg[2].x(), efg[3].y());

     double aspect_xz = VERDE_MAX (efg[2].x(), efg[4].z()) /
       VERDE_MIN (efg[2].x(), efg[4].z());

     double aspect_yz = VERDE_MAX (efg[3].y(), efg[4].z()) /
       VERDE_MIN (efg[3].y(), efg[4].z());

     metrics[HEX_ASPECT] = VERDE_MAX(aspect_xy,
                               VERDE_MAX(aspect_xz, aspect_yz));


     // skew...
     double skewx = fabs(efg[4].y()/efg[4].z()) /
       sqrt((efg[4].y()*efg[4].y()) / (efg[4].z()*efg[4].z()) + 1.0);

     double skewy = fabs(efg[4].x()/efg[4].z()) /
       sqrt((efg[4].x()*efg[4].x()) / (efg[4].z()*efg[4].z()) + 1.0);

     double skewz = fabs(efg[3].x()/efg[3].y()) /
       sqrt((efg[3].x()*efg[3].x()) / (efg[3].y()*efg[3].y()) + 1.0);

     metrics[HEX_SKEW] = VERDE_MAX(skewx, VERDE_MAX(skewy, skewz));


     // taper...
     double taperx = fabs(efg[5].x() / efg[2].x());
     double tapery = fabs(efg[5].y() / efg[3].y());
     double taperz = fabs(efg[5].z() / efg[4].z());
    
     metrics[HEX_TAPER] = VERDE_MAX(taperx, VERDE_MAX(tapery, taperz));



   // calculate stretch and aspect ratio based on side and diagonal lengths
   //
   // aspect ratio = min(p,q,r,s) / max(p,q,r,s)
   // stretch = min(s1,..,s12) / max(p,q,r,s)
   //
   // where s1,..,s12 = edge lengths, (p,q,r,s) = diagonal lengths
  
   // get side and edge lengths
   VerdeVector temp;
   temp = node_pos[7] - node_pos[1];
   double diag17 = temp.length_squared();
   temp = node_pos[8] - node_pos[2];
   double diag28 = temp.length_squared();
   temp = node_pos[5] - node_pos[3];
   double diag35 = temp.length_squared();
   temp = node_pos[6] - node_pos[4];
   double diag46 = temp.length_squared();
      
      //double diag17 = VerdeVector(nodes[0], nodes[6]).length_squared();
      //double diag28 = VerdeVector(nodes[1], nodes[7]).length_squared();
      // double diag35 = VerdeVector(nodes[2], nodes[4]).length_squared();
      //double diag46 = VerdeVector(nodes[3], nodes[5]).length_squared();
    
   double max_diag = VERDE_MAX_4(diag17,diag28,diag35,diag46);
    
   double min_diag = VERDE_MIN_4(diag17,diag28,diag35,diag46);
	
   metrics[HEX_DIAGONALS] = sqrt(min_diag / max_diag);
    
   // lengths of edges      
   double s[13];
   temp = node_pos[2] - node_pos[1];
   s[1] = temp.length_squared();
   temp = node_pos[3] - node_pos[2];
   s[2] = temp.length_squared();
   temp = node_pos[4] - node_pos[3];
   s[3] = temp.length_squared();
   temp = node_pos[1] - node_pos[4];
   s[4] = temp.length_squared();
   temp = node_pos[6] - node_pos[5];
   s[5] = temp.length_squared();
   temp = node_pos[7] - node_pos[6];
   s[6] = temp.length_squared();
   temp = node_pos[8] - node_pos[7];
   s[7] = temp.length_squared();
   temp = node_pos[5] - node_pos[8];
   s[8] = temp.length_squared();
   temp = node_pos[5] - node_pos[1];
   s[9] = temp.length_squared();
   temp = node_pos[6] - node_pos[2];
   s[10] = temp.length_squared();
   temp = node_pos[7] - node_pos[3];
   s[11] = temp.length_squared();
   temp = node_pos[8] - node_pos[4];
   s[12] = temp.length_squared();
         
   //s[1]  = VerdeVector(nodes[0], nodes[1]).length_squared();
   //s[2]  = VerdeVector(nodes[1], nodes[2]).length_squared();
   //s[3]  = VerdeVector(nodes[2], nodes[3]).length_squared();
   //s[4]  = VerdeVector(nodes[3], nodes[0]).length_squared();
   //s[5]  = VerdeVector(nodes[4], nodes[5]).length_squared();
   //s[6]  = VerdeVector(nodes[5], nodes[6]).length_squared();
   //s[7]  = VerdeVector(nodes[6], nodes[7]).length_squared();
   //s[8]  = VerdeVector(nodes[7], nodes[4]).length_squared();
   //s[9]  = VerdeVector(nodes[0], nodes[4]).length_squared();
   //s[10] = VerdeVector(nodes[1], nodes[5]).length_squared();
   //s[11] = VerdeVector(nodes[2], nodes[6]).length_squared();
   //s[12] = VerdeVector(nodes[3], nodes[7]).length_squared();
      
   // compute minimum edge length
   double min_edge = s[1];
   for ( int iside=2; iside < 13; iside++ )
   {
      min_edge = VERDE_MIN (min_edge, s[iside]);
   }

   // compute stretch; take square roots because lengths
   // are squared
   const double HEX_STRETCH_SCALE_FACTOR = sqrt(3.0);
      
   metrics[HEX_STRETCH] = HEX_STRETCH_SCALE_FACTOR *
                      sqrt(min_edge / max_diag);
      
   metrics[HEX_CHARDIM] = characteristic_length(node_pos);
   

   // we will do condition, oddy, and jacobian together because
   // they need the same setup calcs (which take time)

   calc_jac_oddy_and_condition(node_pos,metrics[HEX_JACOBIAN],
                               metrics[HEX_NORM_JACOBIAN],
                               metrics[HEX_ODDY],
                               metrics[HEX_CONDITION]);
   
  }  //end of Traditional metrics
 
   // calculate shear, shape, relative size and shape and size
   if( doAlgebraic )
   {
     metrics[HEX_SHEAR] = calculate_shear(node_pos);
     metrics[HEX_SHAPE] = calculate_shape(node_pos);
     metrics[HEX_RELSIZE] = calculate_relative_size(node_pos);
     metrics[HEX_SHAPE_SIZE] = metrics[HEX_SHAPE]*metrics[HEX_RELSIZE];
   }

#endif  // USE_VERDICT


   // put information in QualitySummary for each metric

   int element_id = hex_ref.original_id();
  

   if( doAlgebraic )
   { 
     for (int i = 0; i < 4; i++)
     {
       summary[i].add_sample(metric(i),element_id);
       // apply a tolerance to the range of acceptable values
       if (metrics[i] > (range[i].max + metricTolerance) ||
                        metrics[i] < (range[i].min - metricTolerance))
       {
         FailedHex *bad_elem = new FailedHex(hex_ref,metrics[i]);
         failedHexes[i].push_back(bad_elem);
         malformed_found = VERDE_TRUE;
       }
     }
   }

   if( doDiagnostic )
   { 
     for (int i = 4; i < 5; i++)
     {
       summary[i].add_sample(metric(i),element_id);
       // apply a tolerance to the range of acceptable values
       if (metrics[i] > (range[i].max + metricTolerance) ||
                        metrics[i] < (range[i].min - metricTolerance))
       {
         FailedHex *bad_elem = new FailedHex(hex_ref,metrics[i]);
         failedHexes[i].push_back(bad_elem);
         malformed_found = VERDE_TRUE;
       }
     }
   }

   if( doTraditional )
   { 
     for (int i = 5; i < NUM_HEX_METRICS; i++)
     {
       summary[i].add_sample(metric(i),element_id);
       // apply a tolerance to the range of acceptable values
       if (metrics[i] > (range[i].max + metricTolerance) ||
                        metrics[i] < (range[i].min - metricTolerance))
       {
         FailedHex *bad_elem = new FailedHex(hex_ref,metrics[i]);
         failedHexes[i].push_back(bad_elem);
         malformed_found = VERDE_TRUE;
       }
     }
   }

   return malformed_found;

}

void HexMetric::localize_hex_coordinates(VerdeVector position[])
{
   // ... Make centroid of element the center of coordinate system
   VerdeVector point_2376 = position[2];
   point_2376 += position[3];
   point_2376 += position[6];
   point_2376 += position[7];

   VerdeVector point_1485 = position[1];
   point_1485 += position[4];
   point_1485 += position[8];
   point_1485 += position[5];
  
   VerdeVector centroid = point_2376;
   centroid += point_1485;
   centroid /= 8.0;
  
   int i;
   for ( i = 1; i < 9; i++)
      position[i] -= centroid;
  
  // ... Rotate element such that center of side 2-3 and 4-1 define X axis
   double DX = point_2376.x() - point_1485.x();
   double DY = point_2376.y() - point_1485.y();
   double DZ = point_2376.z() - point_1485.z();
  
   double AMAGX = sqrt(DX*DX + DZ*DZ);
   double AMAGY = sqrt(DX*DX + DY*DY + DZ*DZ);
   double FMAGX = AMAGX == 0.0 ? 1.0 : 0.0;
   double FMAGY = AMAGY == 0.0 ? 1.0 : 0.0;
  
   double CZ = DX / (AMAGX + FMAGX) + FMAGX;
   double SZ = DZ / (AMAGX + FMAGX);
   double CY = sqrt(DX*DX + DZ*DZ) / (AMAGY + FMAGY) + FMAGY;
   double SY = DY / (AMAGY + FMAGY);
  
   double temp;
  
   for (i = 1; i < 9; i++) {
      temp =  CY * CZ * position[i].x() + CY * SZ * position[i].z() +
          SY * position[i].y();
      position[i].y( -SY * CZ * position[i].x() - SY * SZ * position[i].z() +
                     CY * position[i].y());
      position[i].z( -SZ * position[i].x()      +      CZ * position[i].z());
      position[i].x(temp);
   }
  
   // ... Now, rotate about Y
   VerdeVector delta = -position[1];
   delta -= position[2];
   delta += position[3];
   delta += position[4];
   delta -= position[5];
   delta -= position[6];
   delta += position[7];
   delta += position[8];
    
   DX = delta.x();
   DY = delta.y();
   DZ = delta.z();
    
   AMAGY = sqrt(DY*DY + DZ*DZ);
   FMAGY = AMAGY == 0.0 ? 1.0 : 0.0;
    
   double CX = DY / (AMAGY + FMAGY) + FMAGY;
   double SX = DZ / (AMAGY + FMAGY);
    
   for (i = 1; i < 9; i++) {
      temp =  CX * position[i].y() + SX * position[i].z();
      position[i].z(-SX * position[i].y() + CX * position[i].z());
      position[i].y(temp);
   }
}

void HexMetric::calculate_hex_efg(const VerdeVector position[],
                                  VerdeVector efg[])
{
   // NOTE: There is no const negation operator '-' for a VerdeVector,
   //       That is the reason for changing the order of some of the
   //       calculations below.

   // NOTE: Only entries 2, 3, 4, and 5 are used in quality calculations.
   //       Compute the others only if debug flag 40 is set.
   efg[2] =  position[2];
   efg[2] -= position[1];
   efg[2] += position[3];
   efg[2] -= position[4];
   efg[2] -= position[5];
   efg[2] += position[6];
   efg[2] += position[7];
   efg[2] -= position[8];
    
   efg[3] =  position[3];
   efg[3] -= position[1];
   efg[3] -= position[2];
   efg[3] += position[4];
   efg[3] -= position[5];
   efg[3] -= position[6];
   efg[3] += position[7];
   efg[3] += position[8];
    
   efg[4] =  position[5];
   efg[4] -= position[1];
   efg[4] -= position[2];
   efg[4] -= position[3];
   efg[4] -= position[4];
   efg[4] += position[6];
   efg[4] += position[7];
   efg[4] += position[8];
    
   efg[5] =  position[1];
   efg[5] += position[2];
   efg[5] -= position[3];
   efg[5] -= position[4];
   efg[5] -= position[5];
   efg[5] -= position[6];
   efg[5] += position[7];
   efg[5] += position[8];

   /*
   if (DEBUG_FLAG(40)) {
      efg[1] =  position[1];
      efg[1] += position[2];
      efg[1] += position[3];
      efg[1] += position[4];
      efg[1] += position[5];
      efg[1] += position[6];
      efg[1] += position[7];
      efg[1] += position[8];
    
      efg[6] =  position[1];
      efg[6] -= position[2];
      efg[6] -= position[3];
      efg[6] += position[4];
      efg[6] -= position[5];
      efg[6] += position[6];
      efg[6] += position[7];
      efg[6] -= position[8];
    
      efg[7] =  position[1];
      efg[7] -= position[2];
      efg[7] += position[3];
      efg[7] -= position[4];
      efg[7] += position[5];
      efg[7] -= position[6];
      efg[7] += position[7];
      efg[7] -= position[8];
    
      efg[8] =  position[2];
      efg[8] -= position[1];
      efg[8] -= position[3];
      efg[8] += position[4];
      efg[8] += position[5];
      efg[8] -= position[6];
      efg[8] += position[7];
      efg[8] -= position[8];
   }
   */
}

#define SQR(x) ((x) * (x))

double HexMetric::characteristic_length(VerdeVector position[])
{
  double gradop[9][4];

  double x1 = position[1].x();
  double x2 = position[2].x();
  double x3 = position[3].x();
  double x4 = position[4].x();
  double x5 = position[5].x();
  double x6 = position[6].x();
  double x7 = position[7].x();
  double x8 = position[8].x();
               
  double y1 = position[1].y();
  double y2 = position[2].y();
  double y3 = position[3].y();
  double y4 = position[4].y();
  double y5 = position[5].y();
  double y6 = position[6].y();
  double y7 = position[7].y();
  double y8 = position[8].y();
               
  double z1 = position[1].z();
  double z2 = position[2].z();
  double z3 = position[3].z();
  double z4 = position[4].z();
  double z5 = position[5].z();
  double z6 = position[6].z();
  double z7 = position[7].z();
  double z8 = position[8].z();
               
  double z24 = z2 - z4;
  double z52 = z5 - z2;
  double z45 = z4 - z5;
  gradop[1][1] = ( y2*(z6-z3-z45) + y3*z24 + y4*(z3-z8-z52)
		   + y5*(z8-z6-z24) + y6*z52 + y8*z45 ) / 12.0;

  double z31 = z3 - z1;
  double z63 = z6 - z3;
  double z16 = z1 - z6;
  gradop[2][1] = ( y3*(z7-z4-z16) + y4*z31 + y1*(z4-z5-z63)
                   + y6*(z5-z7-z31) + y7*z63 + y5*z16 ) / 12.0;
  double z42 = z4 - z2;
  double z74 = z7 - z4;
  double z27 = z2 - z7;
  gradop[3][1] = ( y4*(z8-z1-z27) + y1*z42 + y2*(z1-z6-z74)
                   + y7*(z6-z8-z42) + y8*z74 + y6*z27 ) / 12.0;

  double z13 = z1 - z3;
  double z81 = z8 - z1;
  double z38 = z3 - z8;
  gradop[4][1] = ( y1*(z5-z2-z38) + y2*z13 + y3*(z2-z7-z81)
                   + y8*(z7-z5-z13) + y5*z81 + y7*z38 ) / 12.0;

  double z86 = z8 - z6;
  double z18 = z1 - z8;
  double z61 = z6 - z1;
  gradop[5][1] = ( y8*(z4-z7-z61) + y7*z86 + y6*(z7-z2-z18)
                   + y1*(z2-z4-z86) + y4*z18 + y2*z61 ) / 12.0;

  double z57 = z5 - z7;
  double z25 = z2 - z5;
  double z72 = z7 - z2;
  gradop[6][1] = ( y5*(z1-z8-z72) + y8*z57 + y7*(z8-z3-z25)
                   + y2*(z3-z1-z57) + y1*z25 + y3*z72 ) / 12.0;

  double z68 = z6 - z8;
  double z36 = z3 - z6;
  double z83 = z8 - z3;
  gradop[7][1] = ( y6*(z2-z5-z83) + y5*z68 + y8*(z5-z4-z36)
                   + y3*(z4-z2-z68) + y2*z36 + y4*z83 ) / 12.0;

  double z75 = z7 - z5;
  double z47 = z4 - z7;
  double z54 = z5 - z4;
  gradop[8][1] = ( y7*(z3-z6-z54) + y6*z75 + y5*(z6-z1-z47)
                   + y4*(z1-z3-z75) + y3*z47 + y1*z54 ) / 12.0;

  double x24 = x2 - x4;
  double x52 = x5 - x2;
  double x45 = x4 - x5;
  gradop[1][2] = ( z2*(x6-x3-x45) + z3*x24 + z4*(x3-x8-x52)
                   + z5*(x8-x6-x24) + z6*x52 + z8*x45 ) / 12.0;

  double x31 = x3 - x1;
  double x63 = x6 - x3;
  double x16 = x1 - x6;
  gradop[2][2] = ( z3*(x7-x4-x16) + z4*x31 + z1*(x4-x5-x63)
                   + z6*(x5-x7-x31) + z7*x63 + z5*x16 ) / 12.0;

  double x42 = x4 - x2;
  double x74 = x7 - x4;
  double x27 = x2 - x7;
  gradop[3][2] = ( z4*(x8-x1-x27) + z1*x42 + z2*(x1-x6-x74)
                   + z7*(x6-x8-x42) + z8*x74 + z6*x27 ) / 12.0;

  double x13 = x1 - x3;
  double x81 = x8 - x1;
  double x38 = x3 - x8;
  gradop[4][2] = ( z1*(x5-x2-x38) + z2*x13 + z3*(x2-x7-x81)
                   + z8*(x7-x5-x13) + z5*x81 + z7*x38 ) / 12.0;

  double x86 = x8 - x6;
  double x18 = x1 - x8;
  double x61 = x6 - x1;
  gradop[5][2] = ( z8*(x4-x7-x61) + z7*x86 + z6*(x7-x2-x18)
                   + z1*(x2-x4-x86) + z4*x18 + z2*x61 ) / 12.0;

  double x57 = x5 - x7;
  double x25 = x2 - x5;
  double x72 = x7 - x2;
  gradop[6][2] = ( z5*(x1-x8-x72) + z8*x57 + z7*(x8-x3-x25)
                   + z2*(x3-x1-x57) + z1*x25 + z3*x72 ) / 12.0;

  double x68 = x6 - x8;
  double x36 = x3 - x6;
  double x83 = x8 - x3;
  gradop[7][2] = ( z6*(x2-x5-x83) + z5*x68 + z8*(x5-x4-x36)
                   + z3*(x4-x2-x68) + z2*x36 + z4*x83 ) / 12.0;

  double x75 = x7 - x5;
  double x47 = x4 - x7;
  double x54 = x5 - x4;
  gradop[8][2] = ( z7*(x3-x6-x54) + z6*x75 + z5*(x6-x1-x47)
                   + z4*(x1-x3-x75) + z3*x47 + z1*x54 ) / 12.0;

  double y24 = y2 - y4;
  double y52 = y5 - y2;
  double y45 = y4 - y5;
  gradop[1][3] = ( x2*(y6-y3-y45) + x3*y24 + x4*(y3-y8-y52)
                   + x5*(y8-y6-y24) + x6*y52 + x8*y45 ) / 12.0;

  double y31 = y3 - y1;
  double y63 = y6 - y3;
  double y16 = y1 - y6;
  gradop[2][3] = ( x3*(y7-y4-y16) + x4*y31 + x1*(y4-y5-y63)
                   + x6*(y5-y7-y31) + x7*y63 + x5*y16 ) / 12.0;

  double y42 = y4 - y2;
  double y74 = y7 - y4;
  double y27 = y2 - y7;
  gradop[3][3] = ( x4*(y8-y1-y27) + x1*y42 + x2*(y1-y6-y74)
                   + x7*(y6-y8-y42) + x8*y74 + x6*y27 ) / 12.0;

  double y13 = y1 - y3;
  double y81 = y8 - y1;
  double y38 = y3 - y8;
  gradop[4][3] = ( x1*(y5-y2-y38) + x2*y13 + x3*(y2-y7-y81)
                   + x8*(y7-y5-y13) + x5*y81 + x7*y38 ) / 12.0;

  double y86 = y8 - y6;
  double y18 = y1 - y8;
  double y61 = y6 - y1;
  gradop[5][3] = ( x8*(y4-y7-y61) + x7*y86 + x6*(y7-y2-y18)
                   + x1*(y2-y4-y86) + x4*y18 + x2*y61 ) / 12.0;

  double y57 = y5 - y7;
  double y25 = y2 - y5;
  double y72 = y7 - y2;
  gradop[6][3] = ( x5*(y1-y8-y72) + x8*y57 + x7*(y8-y3-y25)
                   + x2*(y3-y1-y57) + x1*y25 + x3*y72 ) / 12.0;

  double y68 = y6 - y8;
  double y36 = y3 - y6;
  double y83 = y8 - y3;
  gradop[7][3] = ( x6*(y2-y5-y83) + x5*y68 + x8*(y5-y4-y36)
                   + x3*(y4-y2-y68) + x2*y36 + x4*y83 ) / 12.0;

  double y75 = y7 - y5;
  double y47 = y4 - y7;
  double y54 = y5 - y4;
  gradop[8][3] = ( x7*(y3-y6-y54) + x6*y75 + x5*(y6-y1-y47)
                   + x4*(y1-y3-y75) + x3*y47 + x1*y54 ) / 12.0;

               
  //     calculate element volume and characteristic element aspect ratio
  //     (used in time step and hourglass control) -
  
  double volume =  position[1].x() * gradop[1][1]
    + position[2].x() * gradop[2][1]
    + position[3].x() * gradop[3][1]
    + position[4].x() * gradop[4][1]
    + position[5].x() * gradop[5][1]
    + position[6].x() * gradop[6][1]
    + position[7].x() * gradop[7][1]
    + position[8].x() * gradop[8][1];
  double aspect = .5*SQR(volume) /
    ( SQR(gradop[1][1]) + SQR(gradop[2][1])
      + SQR(gradop[3][1]) + SQR(gradop[4][1])
      + SQR(gradop[5][1]) + SQR(gradop[6][1])
      + SQR(gradop[7][1]) + SQR(gradop[8][1])
      + SQR(gradop[1][2]) + SQR(gradop[2][2])
      + SQR(gradop[3][2]) + SQR(gradop[4][2])
      + SQR(gradop[5][2]) + SQR(gradop[6][2])
      + SQR(gradop[7][2]) + SQR(gradop[8][2])
      + SQR(gradop[1][3]) + SQR(gradop[2][3])
      + SQR(gradop[3][3]) + SQR(gradop[4][3])
      + SQR(gradop[5][3]) + SQR(gradop[6][3])
      + SQR(gradop[7][3]) + SQR(gradop[8][3]) );

  return sqrt(aspect);
}

inline double normalize_jacobian( double &jacobi, 
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
      norm_jac = jacobi / length_product;
   }
  
   return norm_jac;
}


   


double HexMetric::oddy_comp( VerdeVector &xxi, VerdeVector &xet, VerdeVector &xze )
{
  const double third=1./3.;  
  
  double g11 = xxi % xxi;
  double g12 = xxi % xet;
  double g13 = xxi % xze;
  double g22 = xet % xet; 
  double g23 = xet % xze;
  double g33 = xze % xze;
  double rt_g = xxi % ( xet * xze );

  double oddy_metric;
  if ( rt_g > 0. ) {

     double norm_G_squared = g11*g11 + 2.*g12*g12 + 2.*g13*g13 +
                          g22*g22 + 2.*g23*g23 + g33*g33;
     double norm_J_squared = g11 + g22 + g33;

     oddy_metric = ( norm_G_squared - third*norm_J_squared*norm_J_squared ) / pow( rt_g, 4.*third );

  }
  else {
     oddy_metric = 1.e+06;
  }

  return oddy_metric;

}


void HexMetric::calc_jac_oddy_and_condition(VerdeVector position[],
                                            double &jacobian,
                                            double &norm_jacobian,
                                            double &oddy,
                                            double &condition)
{

   double current_jacobian;
   jacobian = VERDE_DBL_MAX;
   double current_norm_jac;
   norm_jacobian = VERDE_DBL_MAX;
   oddy = 0.0;
   double current_oddy;
   condition = 0.;
   double current_condition;
   
   VerdeVector xxi, xet, xze;

   //  Jacobian at mid-point:

   xxi = position[2]
       + position[3]
       + position[6]
       + position[7]
       - position[1]
       - position[4]
       - position[5]
       - position[8];

   xet = position[3]
       + position[4]
       + position[7]
       + position[8]
       - position[1]
       - position[2]
       - position[5]
       - position[6];

   xze = position[5]
       + position[6]
       + position[7]
       + position[8]
       - position[1]
       - position[2]
       - position[3]
       - position[4];

   // norm jacobian and jacobian
   
   current_jacobian = xxi % ( xet * xze );
   current_norm_jac = normalize_jacobian( current_jacobian,
                                           xxi, xet, xze);
   if (current_norm_jac < norm_jacobian)
   {
      norm_jacobian = current_norm_jac;
   }

   current_jacobian /= 64.0;
   if ( current_jacobian < jacobian ) { jacobian = current_jacobian; }

   // oddy

   current_oddy = oddy_comp( xxi, xet, xze );
   if ( current_oddy > oddy ) { oddy = current_oddy; }

   // condition

   current_condition = condition_comp( xxi, xet, xze );
   if ( current_condition > condition )
   {
      condition = current_condition;
   }
   
   // J(0,0,0):

   xxi = position[2] - position[1];
   xet = position[4] - position[1];
   xze = position[5] - position[1];

   // norm jacobian and jacobian
   
   current_jacobian = xxi % ( xet * xze );
   current_norm_jac = normalize_jacobian( current_jacobian,
                                           xxi, xet, xze);
   if (current_norm_jac < norm_jacobian)
   {
      norm_jacobian = current_norm_jac;
   }

   if ( current_jacobian < jacobian ) { jacobian = current_jacobian; }

   // oddy

   current_oddy = oddy_comp( xxi, xet, xze );
   if ( current_oddy > oddy ) { oddy = current_oddy; }

   // condition

   current_condition = condition_comp( xxi, xet, xze );
   if ( current_condition > condition )
   {
      condition = current_condition;
   }

   // J(1,0,0):

   xxi = position[2] - position[1];
   xet = position[3] - position[2];
   xze = position[6] - position[2];

   // norm jacobian and jacobian
   
   current_jacobian = xxi % ( xet * xze );
   current_norm_jac = normalize_jacobian( current_jacobian,
                                           xxi, xet, xze);
   if (current_norm_jac < norm_jacobian)
   {
      norm_jacobian = current_norm_jac;
   }

   if ( current_jacobian < jacobian ) { jacobian = current_jacobian; }

   // oddy

   current_oddy = oddy_comp( xxi, xet, xze );
   if ( current_oddy > oddy ) { oddy = current_oddy; }

   // condition

   current_condition = condition_comp( xxi, xet, xze );
   if ( current_condition > condition )
   {
      condition = current_condition;
   }
  

   // J(0,1,0):

   xxi = position[3] - position[4];
   xet = position[4] - position[1];
   xze = position[8] - position[4];

   // norm jacobian and jacobian
   
   current_jacobian = xxi % ( xet * xze );
   current_norm_jac = normalize_jacobian( current_jacobian,
                                           xxi, xet, xze);
   if (current_norm_jac < norm_jacobian)
   {
      norm_jacobian = current_norm_jac;
   }

   if ( current_jacobian < jacobian ) { jacobian = current_jacobian; }

   // oddy

   current_oddy = oddy_comp( xxi, xet, xze );
   if ( current_oddy > oddy ) { oddy = current_oddy; }

   // condition

   current_condition = condition_comp( xxi, xet, xze );
   if ( current_condition > condition )
   {
      condition = current_condition;
   }
  

   // J(0,0,1):

   xxi = position[6] - position[5];
   xet = position[8] - position[5];
   xze = position[5] - position[1];

   // norm jacobian and jacobian
   
   current_jacobian = xxi % ( xet * xze );
   current_norm_jac = normalize_jacobian( current_jacobian,
                                           xxi, xet, xze);
   if (current_norm_jac < norm_jacobian)
   {
      norm_jacobian = current_norm_jac;
   }

   if ( current_jacobian < jacobian ) { jacobian = current_jacobian; }

   // oddy

   current_oddy = oddy_comp( xxi, xet, xze );
   if ( current_oddy > oddy ) { oddy = current_oddy; }

   // condition

   current_condition = condition_comp( xxi, xet, xze );
   if ( current_condition > condition )
   {
      condition = current_condition;
   }

   // J(1,1,0):

   xxi = position[3] - position[4];
   xet = position[3] - position[2];
   xze = position[7] - position[3];

   // norm jacobian and jacobian
   
   current_jacobian = xxi % ( xet * xze );
   current_norm_jac = normalize_jacobian( current_jacobian,
                                           xxi, xet, xze);
   if (current_norm_jac < norm_jacobian)
   {
      norm_jacobian = current_norm_jac;
   }

   if ( current_jacobian < jacobian ) { jacobian = current_jacobian; }

   // oddy

   current_oddy = oddy_comp( xxi, xet, xze );
   if ( current_oddy > oddy ) { oddy = current_oddy; }

   // condition

   current_condition = condition_comp( xxi, xet, xze );
   if ( current_condition > condition )
   {
      condition = current_condition;
   }

   // J(1,0,1):

   xxi = position[6] - position[5];
   xet = position[7] - position[6];
   xze = position[6] - position[2];

   // norm jacobian and jacobian
   
   current_jacobian = xxi % ( xet * xze );
   current_norm_jac = normalize_jacobian( current_jacobian,
                                           xxi, xet, xze);
   if (current_norm_jac < norm_jacobian)
   {
      norm_jacobian = current_norm_jac;
   }

   if ( current_jacobian < jacobian ) { jacobian = current_jacobian; }

   // oddy

   current_oddy = oddy_comp( xxi, xet, xze );
   if ( current_oddy > oddy ) { oddy = current_oddy; }

   // condition

   current_condition = condition_comp( xxi, xet, xze );
   if ( current_condition > condition )
   {
      condition = current_condition;
   }
  
   // J(0,1,1):

   xxi = position[7] - position[8];
   xet = position[8] - position[5];
   xze = position[8] - position[4];

   // norm jacobian and jacobian
   
   current_jacobian = xxi % ( xet * xze );
   current_norm_jac = normalize_jacobian( current_jacobian,
                                           xxi, xet, xze);
   if (current_norm_jac < norm_jacobian)
   {
      norm_jacobian = current_norm_jac;
   }

   if ( current_jacobian < jacobian ) { jacobian = current_jacobian; }

   // oddy

   current_oddy = oddy_comp( xxi, xet, xze );
   if ( current_oddy > oddy ) { oddy = current_oddy; }

   // condition

   current_condition = condition_comp( xxi, xet, xze );
   if ( current_condition > condition )
   {
      condition = current_condition;
   }
   
   // J(1,1,1):

   xxi = position[7] - position[8];
   xet = position[7] - position[6];
   xze = position[7] - position[3];

   // norm jacobian and jacobian
   
   current_jacobian = xxi % ( xet * xze );
   current_norm_jac = normalize_jacobian( current_jacobian,
                                           xxi, xet, xze);
   if (current_norm_jac < norm_jacobian)
   {
      norm_jacobian = current_norm_jac;
   }

   if ( current_jacobian < jacobian ) { jacobian = current_jacobian; }

   // oddy

   current_oddy = oddy_comp( xxi, xet, xze );
   if ( current_oddy > oddy ) { oddy = current_oddy; }

   // condition

   current_condition = condition_comp( xxi, xet, xze );
   if ( current_condition > condition )
   {
      condition = current_condition;
   }

   condition /= 3.0;
   
}

double HexMetric::condition_comp( VerdeVector &xxi, VerdeVector &xet,
                                  VerdeVector &xze )
{
  double det = xxi % ( xet * xze );

  if ( det <= VERDE_DBL_MIN ) { return 1.e+06; }

  static VerdeVector vec1,vec2,vec3;
  
  vec1 = xxi * xet;
  vec2 = xet * xze;
  vec3 = xze * xxi;
  
  double term1 = xxi % xxi + xet % xet + xze % xze;
  // double term2 = ( xxi * xet ) % ( xxi * xet ) + ( xet * xze ) %
  //               ( xet * xze ) + ( xze * xxi ) % ( xze * xxi );
  double term2 =  vec1 % vec1 + vec2 % vec2 + vec3 % vec3;

  return sqrt( term1 * term2 ) / det;
}



void HexMetric::print_summary()
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
    for (int i = 5; i < NUM_HEX_METRICS ; i++)
    {
      PRINT_INFO("% 18s ", metricNames[i]); // NO newline
      summary[i].print(); // Adds newline.
    }   
  }
}

int HexMetric::number_failed_hexes()
{
   int number = 0;
   
   for (int i = 0; i < NUM_HEX_METRICS; i++)
   {
      number += failedHexes[i].size();
   }
   return number;
}

   
int HexMetric::print_failed_hexes(VerdeBoolean verbose)
{
  int num_failed = number_failed_hexes();
  
  PRINT_INFO("   Number of Failed Hex Metrics: %d\n", num_failed);
  
  // check for verbose output

  if (num_failed)
  {
    
    for (int i = 0; i < NUM_HEX_METRICS; i++)
    {
      int num_hexes = failedHexes[i].size();
      if (num_hexes)
      {
        if (verbose)
        {
          // print the first line
          FailedHex *hex = failedHexes[i][0];
          PRINT_INFO("\n Failed Metric      Block    Element     Value "
              "    Min. Valid   Max. Valid \n"
              "----------------   -------   -------   --------- "
              "  ----------   ----------\n");
          PRINT_INFO("%16s   %6d   %7d   %10.7f   %10.7f   %10.7f\n",
              metricNames[i],
              hex->block_id(), hex->original_id(),
              hex->metric_value(),range[i].min, range[i].max);
          // print subsequent lines
                                 
          for (int j = 1; j < num_hexes; j++)
          {
            hex = failedHexes[i][j];
            PRINT_INFO("                   %6d   %7d   %10.7f\n",
                hex->block_id(), hex->original_id(),
                hex->metric_value());
          }
        }
        // tmp_val added to get the same output on all platforms
	// the -0.000 and 0.000 issue here -- matches the format %10.3f
	double tmp_val = range[i].min;
        if(range[i].min < 0.001 && range[i].min > -0.001)
          tmp_val = 0.0;
	
        PRINT_INFO("      Failed %s: %d  "
            "\t(valid range: %10.3f to %10.3f)\n", metricNames[i],
            failedHexes[i].size(),tmp_val, range[i].max);
        
      }      
    }
  }
     return num_failed;
}

void HexMetric::failed_hex_list(int index, std::deque<FailedHex*> &hex_list)
{
  for(unsigned int i=0; i<failedHexes[index].size(); i++)
    hex_list.push_back(failedHexes[index][i]);
}


void HexMetric::reset()
{
  for(int i = 0; i < NUM_HEX_METRICS; i++)
  {
    summary[i].reset();
  }
  
  reset_volume();
}

void HexMetric::reset_all()
{
  reset();
  
  for(int i=0; i< NUM_HEX_METRICS; i++)
  {
    for(unsigned int j=0; j<failedHexes[i].size(); j++)
      delete failedHexes[i][j];
    failedHexes[i].clear();
  }
}

void HexMetric::reset_range()
{
  memcpy( range, range_backup, sizeof range );
}

         
void HexMetric::get_weight( VerdeVector &w1, VerdeVector &w2, 
                            VerdeVector &w3 )
{
  if ( elementShape == 1 ) {
     w1.set(1,0,0);
     w2.set(0,aspectRatio,0);
     w3.set(0,0,aspectRatio2);
   }

  double scale = pow(elementSize/(w1%(w2*w3)), 0.33333333333);

  w1 *= scale;
  w2 *= scale;
  w3 *= scale;
     
}
      

double HexMetric::volume(HexRef &hex_ref)
{

  // calc volume

  #if defined(USE_VERDICT)
    
    double node_pos[8][3];
    int ii;
    for(ii = 0; ii<8; ii++)
    {
      node_pos[ii][0] = hex_ref.node_coordinates(ii).x();
      node_pos[ii][1] = hex_ref.node_coordinates(ii).y();
      node_pos[ii][2] = hex_ref.node_coordinates(ii).z();
    }
    
    return v_hex_volume(8, node_pos);

  #else

    calculate(hex_ref, HEX_VOLUME);

    return metrics[HEX_VOLUME];

  #endif

}

double HexMetric::calculate_shear(VerdeVector position[])
{
  double shear;
  double min_shear = 1.;


/*
  VerdeVector q1, q2, q3;
  VerdeVector w1, w2, w3;

  get_weight(w1,w2,w3);

  double detw = determinant(w1,w2,w3);

  if ( detw < VERDE_DBL_MIN ) { return 0; }

  VerdeVector qw1, qw2, qw3;
  form_q(w1,w2,w3,qw1,qw2,qw3);
  // Note: qw needs to be computed at each corner if "brick" weight used
*/
  
  double det;
  VerdeVector xxi, xet, xze;


  // J(0,0,0):

  xxi = position[2] - position[1];
  xet = position[4] - position[1];
  xze = position[5] - position[1];

  det = determinant(xxi,xet,xze);
  if ( det < VERDE_DBL_MIN ) { return 0; }
  //form_q(xxi,xet,xze,q1,q2,q3);
  //shear = skew_x(q1,q2,q3,qw1,qw2,qw3);
  shear = det / sqrt( (xxi%xxi)*(xet%xet)*(xze%xze) );

  min_shear = VERDE_MIN( shear, min_shear );

  // J(1,0,0):

  xxi = position[2] - position[1];
  xet = position[3] - position[2];
  xze = position[6] - position[2];

  det = determinant(xxi,xet,xze);
  if ( det < VERDE_DBL_MIN ) { return 0; } 
  //form_q(xxi,xet,xze,q1,q2,q3);
  //shear = skew_x(q1,q2,q3,qw1,qw2,qw3);
  shear = det / sqrt( (xxi%xxi)*(xet%xet)*(xze%xze) );
  min_shear = VERDE_MIN( shear, min_shear );

  // J(0,1,0):

  xxi = position[3] - position[4];
  xet = position[4] - position[1];
  xze = position[8] - position[4];

  det = determinant(xxi,xet,xze);
  if ( det < VERDE_DBL_MIN ) { return 0; }
  //form_q(xxi,xet,xze,q1,q2,q3);
  //shear = skew_x(q1,q2,q3,qw1,qw2,qw3);
  shear = det / sqrt( (xxi%xxi)*(xet%xet)*(xze%xze) );
  min_shear = VERDE_MIN( shear, min_shear );

  // J(0,0,1):

  xxi = position[6] - position[5];
  xet = position[8] - position[5];
  xze = position[5] - position[1];

  det = determinant(xxi,xet,xze);
  if ( det < VERDE_DBL_MIN ) { return 0; }
  //form_q(xxi,xet,xze,q1,q2,q3);
  //shear = skew_x(q1,q2,q3,qw1,qw2,qw3);
  shear = det / sqrt( (xxi%xxi)*(xet%xet)*(xze%xze) );
  min_shear = VERDE_MIN( shear, min_shear );

  // J(1,1,0):

  xxi = position[3] - position[4];
  xet = position[3] - position[2];
  xze = position[7] - position[3];

  det = determinant(xxi,xet,xze);
  if ( det < VERDE_DBL_MIN ) { return 0; }
  //form_q(xxi,xet,xze,q1,q2,q3);
  //shear = skew_x(q1,q2,q3,qw1,qw2,qw3);
  shear = det / sqrt( (xxi%xxi)*(xet%xet)*(xze%xze) );
  min_shear = VERDE_MIN( shear, min_shear );

  // J(1,0,1):

  xxi = position[6] - position[5];
  xet = position[7] - position[6];
  xze = position[6] - position[2];

  det = determinant(xxi,xet,xze);
  if ( det < VERDE_DBL_MIN ) { return 0; }
  //form_q(xxi,xet,xze,q1,q2,q3);
  //shear = skew_x(q1,q2,q3,qw1,qw2,qw3);
  shear = det / sqrt( (xxi%xxi)*(xet%xet)*(xze%xze) );
  min_shear = VERDE_MIN( shear, min_shear );

  // J(0,1,1):

  xxi = position[7] - position[8];
  xet = position[8] - position[5];
  xze = position[8] - position[4];
 
  det = determinant(xxi,xet,xze);
  if ( det < VERDE_DBL_MIN ) { return 0; }
  //form_q(xxi,xet,xze,q1,q2,q3);
  //shear = skew_x(q1,q2,q3,qw1,qw2,qw3);
  shear = det / sqrt( (xxi%xxi)*(xet%xet)*(xze%xze) );
  min_shear = VERDE_MIN( shear, min_shear );

  // J(1,1,1):

  xxi = position[7] - position[8];
  xet = position[7] - position[6];
  xze = position[7] - position[3];
 
  det = determinant(xxi,xet,xze);
  if ( det < VERDE_DBL_MIN ) { return 0; }
  //form_q(xxi,xet,xze,q1,q2,q3);
  //shear = skew_x(q1,q2,q3,qw1,qw2,qw3);
  shear = det / sqrt( (xxi%xxi)*(xet%xet)*(xze%xze) );
  min_shear = VERDE_MIN( shear, min_shear );

  return min_shear;
}

double HexMetric::calculate_shape(VerdeVector position[])
{
  double shape, det;
  double min_shape = 1.0;
  double two_thirds = 2.0/3.0;

/*
  VerdeVector t1, t2, t3;
  VerdeVector w1, w2, w3;

  get_weight(w1,w2,w3);

  double detw = determinant(w1,w2,w3);

  if ( detw < VERDE_DBL_MIN ) { return 0; }
*/

  VerdeVector xxi, xet, xze;

  // J(0,0,0):

  xxi = position[2] - position[1];
  xet = position[4] - position[1];
  xze = position[5] - position[1];

  det = xxi % (xet * xze);
  if ( det > VERDE_DBL_MIN )
    shape = 3 * pow( det, two_thirds) /( xxi%xxi + xet%xet + xze%xze );
  else
    return 0;

  if ( shape < min_shape ) { min_shape = shape; }

  // J(1,0,0):

  xxi = position[2] - position[1];
  xet = position[3] - position[2];
  xze = position[6] - position[2];

  det = xxi % (xet * xze);
  if ( det > VERDE_DBL_MIN )
    shape = 3 * pow( det, two_thirds) /( xxi%xxi + xet%xet + xze%xze );
  else
    return 0;
  
  if ( shape < min_shape ) { min_shape = shape; }

  // J(0,1,0):

  xxi = position[3] - position[4];
  xet = position[4] - position[1];
  xze = position[8] - position[4];

  det = xxi % (xet * xze);
  if ( det > VERDE_DBL_MIN )
    shape = 3 * pow( det, two_thirds) /( xxi%xxi + xet%xet + xze%xze );
  else
    return 0;

  if ( shape < min_shape ) { min_shape = shape; }

  // J(0,0,1):

  xxi = position[6] - position[5];
  xet = position[8] - position[5];
  xze = position[5] - position[1];
 
  det = xxi % (xet * xze);
  if ( det > VERDE_DBL_MIN )
    shape = 3 * pow( det, two_thirds) /( xxi%xxi + xet%xet + xze%xze );
  else
    return 0;
  
  if ( shape < min_shape ) { min_shape = shape; }

  // J(1,1,0):

  xxi = position[3] - position[4];
  xet = position[3] - position[2];
  xze = position[7] - position[3];
 
  det = xxi % (xet * xze);
  if ( det > VERDE_DBL_MIN )
    shape = 3 * pow( det, two_thirds) /( xxi%xxi + xet%xet + xze%xze );
  else
    return 0;
  
  if ( shape < min_shape ) { min_shape = shape; }

  // J(1,0,1):

  xxi = position[6] - position[5];
  xet = position[7] - position[6];
  xze = position[6] - position[2];
 
  det = xxi % (xet * xze);
  if ( det > VERDE_DBL_MIN )
    shape = 3 * pow( det, two_thirds) /( xxi%xxi + xet%xet + xze%xze );
  else
    return 0;

  if ( shape < min_shape ) { min_shape = shape; }

  // J(0,1,1):

  xxi = position[7] - position[8];
  xet = position[8] - position[5];
  xze = position[8] - position[4];
 
  det = xxi % (xet * xze);
  if ( det > VERDE_DBL_MIN )
    shape = 3 * pow( det, two_thirds) /( xxi%xxi + xet%xet + xze%xze );
  else
    return 0;

  if ( shape < min_shape ) { min_shape = shape; }

  // J(1,1,1):

  xxi = position[7] - position[8];
  xet = position[7] - position[6];
  xze = position[7] - position[3];
 
  det = xxi % (xet * xze);
  if ( det > VERDE_DBL_MIN )
    shape = 3 * pow( det, two_thirds) /( xxi%xxi + xet%xet + xze%xze );
  else
    return 0;

  if ( shape < min_shape ) { min_shape = shape; }

  return min_shape;

/*

  VerdeVector xxi, xet, xze;

  // J(0,0,0):

  xxi = position[2] - position[1];
  xet = position[4] - position[1];
  xze = position[5] - position[1];

  form_t(xxi,xet,xze,w1,w2,w3,t1,t2,t3);

  condition = condition_comp( t1, t2, t3 );

  if ( condition == 1.e+06 ) {
     shape = 0;
  }
  else {
     shape = 3/condition;
  }

  if ( shape < min_shape ) { min_shape = shape; }

  // J(1,0,0):

  xxi = position[2] - position[1];
  xet = position[3] - position[2];
  xze = position[6] - position[2];
 
  form_t(xxi,xet,xze,w1,w2,w3,t1,t2,t3);
  
  condition = condition_comp(  t1, t2, t3 );

  if ( condition == 1.e+06 ) {
     shape = 0;
  }
  else {
     shape = 3/condition;
  }
  if ( shape < min_shape ) { min_shape = shape; }

  // J(0,1,0):

  xxi = position[3] - position[4];
  xet = position[4] - position[1];
  xze = position[8] - position[4];

  form_t(xxi,xet,xze,w1,w2,w3,t1,t2,t3);

  condition = condition_comp(  t1, t2, t3 );
  if ( condition == 1.e+06 ) {
     shape = 0;
  }
  else {
     shape = 3/condition;
  }
  if ( shape < min_shape ) { min_shape = shape; }

  // J(0,0,1):

  xxi = position[6] - position[5];
  xet = position[8] - position[5];
  xze = position[5] - position[1];
 
  form_t(xxi,xet,xze,w1,w2,w3,t1,t2,t3);

  condition = condition_comp(  t1, t2, t3 );
  if ( condition == 1.e+06 ) {
     shape = 0;
  }
  else {
     shape = 3/condition;
  }
  if ( shape < min_shape ) { min_shape = shape; }

  // J(1,1,0):

  xxi = position[3] - position[4];
  xet = position[3] - position[2];
  xze = position[7] - position[3];
 
  form_t(xxi,xet,xze,w1,w2,w3,t1,t2,t3);

  condition = condition_comp(  t1, t2, t3 );
  if ( condition == 1.e+06 ) {
     shape = 0;
  }
  else {
     shape = 3/condition;
  }
  if ( shape < min_shape ) { min_shape = shape; }

  // J(1,0,1):

  xxi = position[6] - position[5];
  xet = position[7] - position[6];
  xze = position[6] - position[2];
 
  form_t(xxi,xet,xze,w1,w2,w3,t1,t2,t3);

  condition = condition_comp( t1, t2, t3 );
  if ( condition == 1.e+06 ) {
     shape = 0;
  }
  else {
     shape = 3/condition;
  }
  if ( shape < min_shape ) { min_shape = shape; }

  // J(0,1,1):

  xxi = position[7] - position[8];
  xet = position[8] - position[5];
  xze = position[8] - position[4];
 
  form_t(xxi,xet,xze,w1,w2,w3,t1,t2,t3);

  condition = condition_comp(  t1, t2, t3 );
  if ( condition == 1.e+06 ) {
     shape = 0;
  }
  else {
     shape = 3/condition;
  }
  if ( shape < min_shape ) { min_shape = shape; }

  // J(1,1,1):

  xxi = position[7] - position[8];
  xet = position[7] - position[6];
  xze = position[7] - position[3];
 
  form_t(xxi,xet,xze,w1,w2,w3,t1,t2,t3);

  condition = condition_comp( t1, t2, t3 );
  if ( condition == 1.e+06 ) {
     shape = 0;
  }
  else {
     shape = 3/condition;
  }
  if ( shape < min_shape ) { min_shape = shape; }

  return min_shape;
*/

}

double HexMetric::calculate_relative_size(VerdeVector position[])
{

  double size = 0;
  double tau;

  VerdeVector xxi, xet, xze;
  double det, det_sum = 0;

  get_weight(xxi, xet, xze);

  double detw = xxi % (xet * xze);

  if ( detw < VERDE_DBL_MIN )
    return 0;

  // J(0,0,0):

  xxi = position[2] - position[1];
  xet = position[4] - position[1];
  xze = position[5] - position[1];

  det = xxi % (xet * xze);
  det_sum += det;


  // J(1,0,0):

  xxi = position[2] - position[1];
  xet = position[3] - position[2];
  xze = position[6] - position[2];

  det = xxi % (xet * xze);
  det_sum += det;


  // J(0,1,0):

  xxi = position[3] - position[4];
  xet = position[4] - position[1];
  xze = position[8] - position[4];

  det = xxi % (xet * xze);
  det_sum += det;


  // J(0,0,1):

  xxi = position[6] - position[5];
  xet = position[8] - position[5];
  xze = position[5] - position[1];
 
  det = xxi % (xet * xze);
  det_sum += det;


  // J(1,1,0):

  xxi = position[3] - position[4];
  xet = position[3] - position[2];
  xze = position[7] - position[3];
 
  det = xxi % (xet * xze);
  det_sum += det;


  // J(1,0,1):

  xxi = position[6] - position[5];
  xet = position[7] - position[6];
  xze = position[6] - position[2];
 
  det = xxi % (xet * xze);
  det_sum += det;


  // J(0,1,1):

  xxi = position[7] - position[8];
  xet = position[8] - position[5];
  xze = position[8] - position[4];
 
  det = xxi % (xet * xze);
  det_sum += det;


  // J(1,1,1):

  xxi = position[7] - position[8];
  xet = position[7] - position[6];
  xze = position[7] - position[3];
 
  det = xxi % (xet * xze);
  det_sum += det;

  if ( det_sum > VERDE_DBL_MIN )
  {
    tau = det_sum / ( 8*detw);
    size = VERDE_MIN( tau*tau, 1.0/tau/tau);
  }

  return size;

/*
  double size_metric=0;
  double tau, xmu;

  VerdeVector t1, t2, t3;
  VerdeVector w1, w2, w3;

  get_weight(w1,w2,w3);

  double detw = determinant(w1,w2,w3);

  if ( detw < VERDE_DBL_MIN ) { return 0; }

  VerdeVector xxi, xet, xze;

  // J(0,0,0):

  xxi = position[2] - position[1];
  xet = position[4] - position[1];
  xze = position[5] - position[1];

  form_t(xxi,xet,xze,w1,w2,w3,t1,t2,t3);

  tau = determinant(t1,t2,t3);
  if ( tau < VERDE_DBL_MIN ) { 
     xmu=0;
  }
  else {
     xmu = VERDE_MIN( tau, 1/tau );
  }
  size_metric += xmu*xmu;

  // J(1,0,0):

  xxi = position[2] - position[1];
  xet = position[3] - position[2];
  xze = position[6] - position[2];
 
  form_t(xxi,xet,xze,w1,w2,w3,t1,t2,t3);

  tau = determinant(t1,t2,t3);
  if ( tau < VERDE_DBL_MIN ) { 
     xmu=0;
  }
  else {
     xmu = VERDE_MIN( tau, 1/tau );
  }
  size_metric += xmu*xmu;

  // J(0,1,0):

  xxi = position[3] - position[4];
  xet = position[4] - position[1];
  xze = position[8] - position[4];

  form_t(xxi,xet,xze,w1,w2,w3,t1,t2,t3);

  tau = determinant(t1,t2,t3);
  if ( tau < VERDE_DBL_MIN ) { 
     xmu=0;
  }
  else {
     xmu = VERDE_MIN( tau, 1/tau );
  }
  size_metric += xmu*xmu;

  // J(0,0,1):

  xxi = position[6] - position[5];
  xet = position[8] - position[5];
  xze = position[5] - position[1];
 
  form_t(xxi,xet,xze,w1,w2,w3,t1,t2,t3);

  tau = determinant(t1,t2,t3);
  if ( tau < VERDE_DBL_MIN ) { 
     xmu=0;
  }
  else {
     xmu = VERDE_MIN( tau, 1/tau );
  }
  size_metric += xmu*xmu;

  // J(1,1,0):

  xxi = position[3] - position[4];
  xet = position[3] - position[2];
  xze = position[7] - position[3];
 
  form_t(xxi,xet,xze,w1,w2,w3,t1,t2,t3);

  tau = determinant(t1,t2,t3);
  if ( tau < VERDE_DBL_MIN ) { 
     xmu=0;
  }
  else {
     xmu = VERDE_MIN( tau, 1/tau );
  }
  size_metric += xmu*xmu;

  // J(1,0,1):

  xxi = position[6] - position[5];
  xet = position[7] - position[6];
  xze = position[6] - position[2];
 
  form_t(xxi,xet,xze,w1,w2,w3,t1,t2,t3);

  tau = determinant(t1,t2,t3);
  if ( tau < VERDE_DBL_MIN ) { 
     xmu=0;
  }
  else {
     xmu = VERDE_MIN( tau, 1/tau );
  }
  size_metric += xmu*xmu;

  // J(0,1,1):

  xxi = position[7] - position[8];
  xet = position[8] - position[5];
  xze = position[8] - position[4];
 
  form_t(xxi,xet,xze,w1,w2,w3,t1,t2,t3);

  tau = determinant(t1,t2,t3);
  if ( tau < VERDE_DBL_MIN ) { 
     xmu=0;
  }
  else {
     xmu = VERDE_MIN( tau, 1/tau );
  }
  size_metric += xmu*xmu;

  // J(1,1,1):

  xxi = position[7] - position[8];
  xet = position[7] - position[6];
  xze = position[7] - position[3];
 
  form_t(xxi,xet,xze,w1,w2,w3,t1,t2,t3);

  tau = determinant(t1,t2,t3);
  if ( tau < VERDE_DBL_MIN ) { 
     xmu=0;
  }
  else {
     xmu = VERDE_MIN( tau, 1/tau );
  }
  size_metric += xmu*xmu;

  return sqrt( size_metric/8 );
*/
}

