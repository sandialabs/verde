//-------------------------------------------------------------------------
// Filename      : WedgeMetric.cc 
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

#include <algorithm>
#include "stdlib.h"

#include "WedgeMetric.hpp"

#include "VerdeVector.hpp"
#include "VerdeMessage.hpp"
#include "WedgeRef.hpp"
#include "FailedWedge.hpp"

#if defined(USE_VERDICT)
  #include "verdict.h"
#endif



const char* WedgeMetric::metricNames[NUM_WEDGE_METRICS] =
{ "Volume" };

min_max WedgeMetric::range[NUM_WEDGE_METRICS] =
{ {0.0, VERDE_DBL_MAX}      // Volume
};

const min_max WedgeMetric::range_backup[NUM_WEDGE_METRICS] =
{ {0.0, VERDE_DBL_MAX}      // Volume
};

WedgeMetric *WedgeMetric::instance_ = NULL;

WedgeMetric::WedgeMetric()
{
  totalVolume = 0,0;
}


WedgeMetric::~WedgeMetric()
{
  // clean up memory
  reset_all();
}

WedgeMetric *WedgeMetric::instance()
{
   if (!instance_)
      instance_ = new WedgeMetric();
   return instance_;
}

double  WedgeMetric::tet_volume (VerdeVector &node_pos_0,
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

int WedgeMetric::calculate(WedgeRef &wedge_ref)
{
   int malformed_found = VERDE_TRUE;
   
   // Calculate metrics for wedge

#if defined(USE_VERDICT)
    
    double node_pos[6][3];
    int ii;
    for ( ii = 0; ii < 6; ii++ )
    {
      node_pos[ii][0] = wedge_ref.node_coordinates(ii).x();
      node_pos[ii][1] = wedge_ref.node_coordinates(ii).y();
      node_pos[ii][2] = wedge_ref.node_coordinates(ii).z();
    }

    if(doDiagnostic)   
      metrics[WEDGE_VOLUME] = v_wedge_volume(6, node_pos);

      totalVolume += metrics[WEDGE_VOLUME];
#else

   VerdeVector node_pos[6];

   int ii;
   for ( ii = 0; ii < 6; ii++ )
   {
      node_pos[ii] = wedge_ref.node_coordinates(ii);
   }

   // perform volume calc

   metrics[WEDGE_VOLUME] = volume(node_pos);
   

#endif


   // put information in QualitySummary for each metric

   int element_id = wedge_ref.original_id();
 
  if(doDiagnostic)
  { 
    for (int i = 0; i < NUM_WEDGE_METRICS; i++)
    {
      summary[i].add_sample(metric(i),element_id);
      // apply a tolerance to the range of acceptable values
      if (metrics[i] > (range[i].max + metricTolerance) ||
                        metrics[i] < (range[i].min - metricTolerance))
      {
         FailedWedge *bad_elem = new FailedWedge(wedge_ref,metrics[i]);
         failedWedges[i].push_back(bad_elem);
         malformed_found = VERDE_TRUE;
      }
    }
  } 
   
  return malformed_found;
}

double WedgeMetric::volume(VerdeVector node_pos[6])
{
   // for now, divide the wedge into three tets and calc volume   

   double volume = tet_volume(node_pos[0], node_pos[1],
                              node_pos[2], node_pos[3]);

   volume += tet_volume(node_pos[1], node_pos[4],
                        node_pos[5], node_pos[3]);
   
   volume += tet_volume(node_pos[1], node_pos[5],
                        node_pos[2], node_pos[3]);
   
   totalVolume += volume;

   return volume;  

}
 
        
void WedgeMetric::print_summary()
{

   PRINT_INFO("   Function Name     Average    Std Dev    "
              "Minimum   (id)  Maximum  (id)\n"
              " -----------------  ---------  ---------  "
              "--------------  -------------\n");
  if(doDiagnostic)
  {
    PRINT_INFO("\nDIAGNOSTIC METRICS\n"); 
    for (int i = 0; i < NUM_WEDGE_METRICS; i++)
    {
      PRINT_INFO("% 18s ", metricNames[i]); // NO newline
      summary[i].print(); // Adds newline.
    }
  }
}


int WedgeMetric::number_failed_wedges()
{
   int number = 0;
   
   for (int i = 0; i < NUM_WEDGE_METRICS; i++)
   {
      number += failedWedges[i].size();
   }
   return number;
}

  
int WedgeMetric::print_failed_wedges(VerdeBoolean verbose)
{
   int num_failed = number_failed_wedges();
   
   PRINT_INFO("   Number of Failed Wedge Metrics: %d\n", num_failed);

   if (num_failed)
   {

      for (int i = 0; i < NUM_WEDGE_METRICS; i++)
      {
         int num_wedges = failedWedges[i].size();
         if (num_wedges)
         {
            if (verbose)
            {
               // print the first line
               FailedWedge *wedge = failedWedges[i][0];
               PRINT_INFO("\n Failed Metric      Block    Element     Value "
                          "    Min. Valid   Max. Valid \n"
                          "----------------   -------   -------   --------- "
                          "  ----------   ----------\n");
               PRINT_INFO("%16s   %6d   %7d   %10.7f   %10.7f   %10.7f\n",
                          metricNames[i],
                          wedge->block_id(), wedge->original_id(),
                          wedge->metric_value(),range[i].min, range[i].max);
               // print subsequent lines
                       
               for (int j = 1; j < num_wedges; j++)
               {
                  wedge = failedWedges[i][j];
                  PRINT_INFO("                     %6d   %7d   %10.7f\n",
                             wedge->block_id(), wedge->original_id(),
                             wedge->metric_value());
               }
            }
            PRINT_INFO("      Failed %s: %d  "
                       "\t(valid range: %10.3f to %10.3f)\n", metricNames[i],
                       failedWedges[i].size(),range[i].min, range[i].max);
         }
      }
   }
   return num_failed;
}


void WedgeMetric::failed_wedge_list(int index, 
    std::deque<FailedWedge*> &wedge_list)
{
  std::copy(failedWedges[index].begin(), failedWedges[index].end(),
    wedge_list.begin());  
}

void WedgeMetric::reset()
{
   for(int i = 0; i < NUM_WEDGE_METRICS; i++)
   {
      summary[i].reset();
   }
   totalVolume = 0.0;
}

void WedgeMetric::reset_all()
{
  reset();
  for(int i=0; i< NUM_WEDGE_METRICS; i++)
  {
    for(unsigned int j=0; j<failedWedges[i].size(); j++)
      delete failedWedges[i][j];
    failedWedges[i].clear();
  }
}

void WedgeMetric::reset_range()
{
  memcpy( range, range_backup, sizeof range);
}

