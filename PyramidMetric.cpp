//-------------------------------------------------------------------------
// Filename      : PyramidMetric.cc 
//
// Purpose       : This class performs metric calculations on Pyramid Elements.
//
// Special Notes : 
//
// Creator       : Ray Meyers
//
// Date          : 02/26/99
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------

#include "stdlib.h"

#include "PyramidMetric.hpp"

#include "VerdeVector.hpp"
#include "VerdeMessage.hpp"
#include "PyramidRef.hpp"
#include "FailedPyramid.hpp"


#if defined(USE_VERDICT)
  #include "verdict.h"
#endif


const char* PyramidMetric::metricNames[NUM_PYRAMID_METRICS] =
{ "Volume" };

min_max PyramidMetric::range[NUM_PYRAMID_METRICS] =
{ {0.0, VERDE_DBL_MAX}      // Scaled Jacobian
};


const min_max PyramidMetric::range_backup[NUM_PYRAMID_METRICS] =
{ {0.0, VERDE_DBL_MAX}      // Scaled Jacobian
};

PyramidMetric *PyramidMetric::instance_ = NULL;

PyramidMetric::PyramidMetric()
{
  totalVolume = 0.0;
}


PyramidMetric::~PyramidMetric()
{
  // clean up memory
  reset_all();
}

PyramidMetric *PyramidMetric::instance()
{
   if (!instance_)
      instance_ = new PyramidMetric();
   return instance_;
}

double  PyramidMetric::tet_volume (VerdeVector &node_pos_0,
                                   VerdeVector &node_pos_1,
                                   VerdeVector &node_pos_2,
                                   VerdeVector &node_pos_3)
{

   // calculate element aspect ratio

   const double one_sixth = 1.0 / 6.0;
   //double one_third = 1.0 / 3.0;


   VerdeVector side0 = node_pos_1 - node_pos_0;
   VerdeVector side2 = node_pos_0 - node_pos_2;
   VerdeVector side3 = node_pos_3 - node_pos_0;
   
   double jacobian_value = side3 % ( side0 * (-side2) );
   double volume = one_sixth * jacobian_value;

   return volume;
}

int PyramidMetric::calculate(PyramidRef &pyramid_ref)
{
   int malformed_found = VERDE_TRUE;
  
#if defined(USE_VERDICT)

   double node_pos[5][3];
   int ii;
   for ( ii = 0; ii < 5; ii++ )
   {
      node_pos[ii][0] = pyramid_ref.node_coordinates(ii).x();
      node_pos[ii][1] = pyramid_ref.node_coordinates(ii).y();
      node_pos[ii][2] = pyramid_ref.node_coordinates(ii).z();
   }

   metrics[PYRAMID_VOLUME] = v_pyramid_volume(5, node_pos); 

   totalVolume += metrics[PYRAMID_VOLUME];

#else
 
   // Calculate metrics for pyramid

   VerdeVector node_pos[5];

   int ii;
   for ( ii = 0; ii < 5; ii++ )
   {
      node_pos[ii] = pyramid_ref.node_coordinates(ii);
   }

   // perform volume calc
  if(doDiagnostic)
   metrics[PYRAMID_VOLUME] = volume(node_pos);

#endif   

   // put information in QualitySummary for each metric
   // create failed elements if needed

   int element_id = pyramid_ref.original_id();

  if(doDiagnostic)
  { 
    for (int i = 0; i < NUM_PYRAMID_METRICS; i++)
    {
      summary[i].add_sample(metric(i),element_id);
      // apply a tolerance to the range of acceptable values
      if (metrics[i] > (range[i].max + metricTolerance) ||
                        metrics[i] < (range[i].min - metricTolerance))
      {
         FailedPyramid *bad_elem = new FailedPyramid(pyramid_ref,metrics[i]);
         failedPyramids[i].push_back(bad_elem);
         malformed_found = VERDE_TRUE;
      }
    }
  }
   
  return malformed_found;

}

double PyramidMetric::volume(VerdeVector node_pos[5])
{
   // for now, divide the pyramid into two tets and calc volume   

   double volume = tet_volume(node_pos[0], node_pos[1],
                              node_pos[3], node_pos[4]);

   volume += tet_volume(node_pos[1], node_pos[2],
                        node_pos[3], node_pos[4]);
   
   totalVolume += volume;

   return volume;  

}
 
        
void PyramidMetric::print_summary()
{

   PRINT_INFO("   Function Name     Average    Std Dev    "
              "Minimum   (id)  Maximum  (id)\n"
              " -----------------  ---------  ---------  "
              "--------------  -------------\n");
  if(doDiagnostic)
  {
    PRINT_INFO("\nDIAGNOSTIC METRICS\n");
     
    for (int i = 0; i < NUM_PYRAMID_METRICS; i++)
    {
      PRINT_INFO("% 18s ", metricNames[i]); // NO newline
      summary[i].print(); // Adds newline.
    }
  }
}


int PyramidMetric::number_failed_pyramids()
{
   int number = 0;
   
   for (int i = 0; i < NUM_PYRAMID_METRICS; i++)
   {
      number += failedPyramids[i].size();
   }
   return number;
}

  
int PyramidMetric::print_failed_pyramids(VerdeBoolean verbose)
{
  int num_failed = number_failed_pyramids();
  
  PRINT_INFO("   Number of Failed Pyramid Metrics: %d\n", num_failed);
  
  if (num_failed)
  {
    for (int i = 0; i < NUM_PYRAMID_METRICS; i++)
    {
      int num_pyramids = failedPyramids[i].size();
      if (num_pyramids)
      {
        if (verbose)
        {
          // print the first line
          FailedPyramid *pyramid = failedPyramids[i][0];
          PRINT_INFO("\n Failed Metric      Block    Element     Value "
              "    Min. Valid   Max. Valid \n"
              "----------------   -------   -------   --------- "
              "  ----------   ----------\n");
          PRINT_INFO("%16s   %6d   %7d   %10.7f   %10.7f   %10.7f\n",
              metricNames[i],
              pyramid->block_id(), pyramid->original_id(),
              pyramid->metric_value(),range[i].min, range[i].max);
          // print subsequent lines
                       
          for (int j = 1; j < num_pyramids; j++)
          {
            pyramid = failedPyramids[i][j];
            PRINT_INFO("                     %6d   %7d   %10.7f\n",
                pyramid->block_id(), pyramid->original_id(),
                pyramid->metric_value());
          }
        }
        PRINT_INFO("      Failed %s: %d  "
            "\t(valid range: %10.3f to %10.3f)\n", metricNames[i],
            failedPyramids[i].size(),range[i].min, range[i].max);
      }
    }
  }
  return num_failed;
}


void PyramidMetric::failed_pyramid_list(int index,
    std::deque<FailedPyramid*> &pyramid_list)
{
  for(unsigned int i=0; i< failedPyramids[index].size(); i++)
    pyramid_list.push_back(failedPyramids[index][i]);
}

void PyramidMetric::reset()
{
   for(int i = 0; i < NUM_PYRAMID_METRICS; i++)
   {
      summary[i].reset();
   }
   totalVolume = 0.0;
}

void PyramidMetric::reset_all()
{
  reset();
  for(int i=0; i< NUM_PYRAMID_METRICS; i++)
  {
    for(unsigned int j=0; j<failedPyramids[i].size(); j++)
      delete failedPyramids[i][j];
    failedPyramids[i].clear();
  }
}

void PyramidMetric::reset_range()
{
  memcpy( range, range_backup, sizeof range );
}

