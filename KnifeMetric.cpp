//-------------------------------------------------------------------------
// Filename      : KnifeMetric.cc 
//
// Purpose       : This class performs metric calculations on Knife Elements.
//
// Special Notes : 
//
// Creator       : Ray Meyers
//
// Date          : 03/10/99
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------

#include "stdlib.h"

#include "KnifeMetric.hpp"

#include "VerdeVector.hpp"
#include "VerdeMessage.hpp"
#include "KnifeRef.hpp"
#include "FailedKnife.hpp"

#if defined(USE_VERDICT)
  #include "verdict.h"
#endif


const char* KnifeMetric::metricNames[NUM_KNIFE_METRICS] =
{ "Volume" };

min_max KnifeMetric::range[NUM_KNIFE_METRICS] =
{ {0.0, VERDE_DBL_MAX}      // Scaled Jacobian
};


const min_max KnifeMetric::range_backup[NUM_KNIFE_METRICS] =
{ {0.0, VERDE_DBL_MAX}      // Scaled Jacobian
};


KnifeMetric *KnifeMetric::instance_ = NULL;

KnifeMetric::KnifeMetric()
{
  totalVolume = 0,0;
}


KnifeMetric::~KnifeMetric()
{
  // clean up allocated memory
  reset_all();
}

KnifeMetric *KnifeMetric::instance()
{
   if (!instance_)
      instance_ = new KnifeMetric();
   return instance_;
}

double  KnifeMetric::tet_volume (VerdeVector &node_pos_0,
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

int KnifeMetric::calculate(KnifeRef &knife_ref)
{
   int malformed_found = VERDE_TRUE;

#if defined (USE_VERDICT)

   double node_pos[7][3];
   int ii;
   for ( ii = 0; ii < 7; ii++ )
   {
      node_pos[ii][0] = knife_ref.node_coordinates(ii).x();
      node_pos[ii][1] = knife_ref.node_coordinates(ii).y();
      node_pos[ii][2] = knife_ref.node_coordinates(ii).z();
   }
 
   metrics[KNIFE_VOLUME] = v_knife_volume(7, node_pos);

   totalVolume += metrics[KNIFE_VOLUME];

#else
   
   // Calculate metrics for knife

   VerdeVector node_pos[7];

   int ii;
   for ( ii = 0; ii < 7; ii++ )
   {
      node_pos[ii] = knife_ref.node_coordinates(ii);
   }

   // perform volume calc

  if(doDiagnostic)
    metrics[KNIFE_VOLUME] = volume(node_pos);
    
#endif


   // put information in QualitySummary for each metric

   int element_id = knife_ref.original_id();

  if(doDiagnostic)
  {
    for (int i = 0; i < NUM_KNIFE_METRICS; i++)
    {
      summary[i].add_sample(metric(i),element_id);
      // apply a tolerance to the range of acceptable values
      if (metrics[i] > (range[i].max + metricTolerance) ||
                        metrics[i] < (range[i].min - metricTolerance))
      {
         FailedKnife *bad_elem = new FailedKnife(knife_ref,metrics[i]);
         failedKnives[i].push_back(bad_elem);
         malformed_found = VERDE_TRUE;
      }
    }
  }
   
  return malformed_found;

}

double KnifeMetric::volume(VerdeVector node_pos[7])
{
   // for now, divide the knife into four tets and calc volume   

   double volume = tet_volume(node_pos[0], node_pos[1],
                              node_pos[3], node_pos[4]);

   volume += tet_volume(node_pos[1], node_pos[5],
                        node_pos[3], node_pos[4]);
   
   volume += tet_volume(node_pos[1], node_pos[2],
                        node_pos[3], node_pos[6]);

   volume += tet_volume(node_pos[1], node_pos[3],
                        node_pos[5], node_pos[6]);
   
   totalVolume += volume;

   return volume;  

}
 
        
void KnifeMetric::print_summary()
{

   PRINT_INFO("   Function Name     Average    Std Dev    "
              "Minimum   (id)  Maximum  (id)\n"
              " -----------------  ---------  ---------  "
              "--------------  -------------\n");
  if(doDiagnostic)
  {
    PRINT_INFO("\nDIAGNOSTIC METRICS\n");  
    for (int i = 0; i < NUM_KNIFE_METRICS; i++)
    {
      PRINT_INFO("% 18s ", metricNames[i]); // NO newline
      summary[i].print(); // Adds newline.
    }
  }
}


int KnifeMetric::number_failed_knives()
{
   int number = 0;
   
   for (int i = 0; i < NUM_KNIFE_METRICS; i++)
   {
      number += failedKnives[i].size();
   }
   return number;
}

  
int KnifeMetric::print_failed_knives(VerdeBoolean verbose)
{
  int num_failed = number_failed_knives();
  
  PRINT_INFO("   Number of Failed Knife Metrics: %d\n", num_failed);
  
  if (num_failed)
  {
    
    for (int i = 0; i < NUM_KNIFE_METRICS; i++)
    {
      int num_knives = failedKnives[i].size();
      if (num_knives)
      {
        if (verbose)
        {
          // print the first line
          FailedKnife *knife = failedKnives[i][0];
          PRINT_INFO("\n Failed Metric      Block    Element     Value "
              "    Min. Valid   Max. Valid \n"
              "----------------   -------   -------   --------- "
              "  ----------   ----------\n");
          PRINT_INFO("%16s   %6d   %7d   %10.7f   %10.7f   %10.7f\n",
              metricNames[i],
              knife->block_id(), knife->original_id(),
              knife->metric_value(),range[i].min, range[i].max);
          // print subsequent lines

          for (int j = 1; j < num_knives; j++)
          {
            knife = failedKnives[i][j];
            PRINT_INFO("                     %6d   %7d   %10.7f\n",
                knife->block_id(), knife->original_id(),
                knife->metric_value());
          }
        }
        PRINT_INFO("      Failed %s: %d  "
            "\t(valid range: %10.3f to %10.3f)\n", metricNames[i],
            failedKnives[i].size(),range[i].min, range[i].max);
      }
    }
  }
  return num_failed;
}


void KnifeMetric::failed_knife_list(int index,
    std::deque<FailedKnife*> &knife_list)
{
  for(unsigned int i=0; i<failedKnives[index].size(); i++)
    knife_list.push_back(failedKnives[index][i]);
}

void KnifeMetric::reset()
{
  for(int i = 0; i < NUM_KNIFE_METRICS; i++)
  {
    summary[i].reset();
  }
  totalVolume = 0.0;
}

void KnifeMetric::reset_range()
{
  memcpy(range, range_backup, sizeof range);
}


void KnifeMetric::reset_all()
{
  reset();
  for(int i=0; i< NUM_KNIFE_METRICS; i++)
  {
    for(unsigned int j=0; j<failedKnives[i].size(); j++)
      delete failedKnives[i][j];
    failedKnives[i].clear();
  }
}
