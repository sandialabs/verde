//-------------------------------------------------------------------------
// Filename      : EdgeMetric.cc 
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


#include "EdgeMetric.hpp"
#include "stdlib.h"
//#include "VerdeVector.hpp"
#include "VerdeMessage.hpp"
#include "EdgeRef.hpp"
#include "FailedEdge.hpp"


#if defined(USE_VERDICT)
  #include "verdict.h"
#endif


const char* EdgeMetric::metricNames[NUM_EDGE_METRICS] =
{ "Length" };

min_max EdgeMetric::range[NUM_EDGE_METRICS] =
{ {0.0, VERDE_DBL_MAX}      // Length
};


const min_max EdgeMetric::range_backup[NUM_EDGE_METRICS] =
{ {0.0, VERDE_DBL_MAX}      // Length
};

EdgeMetric *EdgeMetric::instance_ = NULL;

EdgeMetric::EdgeMetric()
{}


EdgeMetric::~EdgeMetric()
{
  // clean up allocated memory
  reset_all();
}

EdgeMetric *EdgeMetric::instance()
{
   if (!instance_)
      instance_ = new EdgeMetric();
   return instance_;
}

int EdgeMetric::calculate(EdgeRef &edge_ref)
{
   int malformed_found = VERDE_TRUE;

#if defined(USE_VERDICT)
   double edge[NUM_EDGE_NODES][3];
   int ii;
   for ( ii = 0; ii < NUM_EDGE_NODES; ii++ )
   {
      edge[ii][0] = edge_ref.node_coordinates(ii).x();
      edge[ii][1] = edge_ref.node_coordinates(ii).y(); 
      edge[ii][2] = edge_ref.node_coordinates(ii).z();
   }

   metrics[EDGE_LENGTH] = v_edge_length(NUM_EDGE_NODES, edge);


#else
   
   // Calculate metrics for edge

  if(doDiagnostic)
  {
    VerdeVector node_pos[NUM_EDGE_NODES];

    int ii;
    for ( ii = 0; ii < NUM_EDGE_NODES; ii++ )
    {
      node_pos[ii] = edge_ref.node_coordinates(ii);
    }

    // perform length calc

    metrics[EDGE_LENGTH] = length(node_pos);
  }
 
#endif

   // put information in QualitySummary for each metric

   int element_id = edge_ref.original_id();

  if(doDiagnostic)
  {
    for (int i = 0; i < NUM_EDGE_METRICS; i++)
    {
      summary[i].add_sample(metric(i),element_id);
      // apply a tolerance to the range of acceptable values
      if (metrics[i] > (range[i].max + metricTolerance) ||
                        metrics[i] < (range[i].min - metricTolerance))
      {
         FailedEdge *bad_elem = new FailedEdge(edge_ref,metrics[i]);
         failedEdges[i].push_back(bad_elem);
         malformed_found = VERDE_TRUE;
      }
    }
  }

  return malformed_found;

}

double EdgeMetric::length(VerdeVector node_pos[2])
{
   // for now, divide the edge into three tets and calc length   

   double length = (node_pos[1] - node_pos[0]) .length();
   
   return length;  

}
 
        
void EdgeMetric::print_summary()
{

   PRINT_INFO("   Function Name     Average    Std Dev    "
              "Minimum   (id)  Maximum  (id)\n"
              " -----------------  ---------  ---------  "
              "--------------  -------------\n");
  
  if(doDiagnostic)
  {
    PRINT_INFO("\nDIAGNOSTIC METRICS\n"); 
    for (int i = 0; i < NUM_EDGE_METRICS; i++)
    {
      PRINT_INFO("% 18s ", metricNames[i]); // NO newline
      summary[i].print(); // Adds newline.
    }
  }
}


int EdgeMetric::number_failed_edges()
{
   int number = 0;
   
   for (int i = 0; i < NUM_EDGE_METRICS; i++)
   {
      number += failedEdges[i].size();
   }
   return number;
}

  
int EdgeMetric::print_failed_edges(VerdeBoolean verbose)
{
  int num_failed = number_failed_edges();
  
  PRINT_INFO("   Number of Failed Edge Metrics: %d\n", num_failed);
  
  if (num_failed)
  {
    
    for (int i = 0; i < NUM_EDGE_METRICS; i++)
    {
      int num_edges = failedEdges[i].size();
      if (num_edges)
      {
        if (verbose)
        {
          // print the first line
          FailedEdge *edge = failedEdges[i][0];
          PRINT_INFO("\n Failed Metric      Block    Element     Value "
              "    Min. Valid   Max. Valid \n"
              "----------------   -------   -------   --------- "
              "  ----------   ----------\n");
          PRINT_INFO("%16s   %6d   %7d   %10.7f   %10.7f   %10.7f\n",
              metricNames[i],
              edge->block_id(), edge->original_id(),
              edge->metric_value(),range[i].min, range[i].max);
          // print subsequent lines
          
          for (int j = 1; j < num_edges; j++)               
          {
            edge = failedEdges[i][j];
            PRINT_INFO("                     %6d   %7d   %10.7f\n",
                edge->block_id(), edge->original_id(),
                edge->metric_value());
          }
        }
        PRINT_INFO("      Failed %s: %d  "
            "\t(valid range: %10.3f to %10.3f)\n", metricNames[i],
            failedEdges[i].size(),range[i].min, range[i].max);
      }
    }
  }
  return num_failed;
}


void EdgeMetric::failed_edge_list(int index,
    std::deque<FailedEdge*> &edge_list)
{
  for(unsigned int i=0; i< failedEdges[index].size(); i++)
    edge_list.push_back(failedEdges[index][i]);
}

void EdgeMetric::reset()
{
   for(int i = 0; i < NUM_EDGE_METRICS; i++)
   {
      summary[i].reset();
   }
   //totalVolume = 0.0;
}

void EdgeMetric::reset_all()
{
  reset();
  for(int i=0; i<NUM_EDGE_METRICS; i++)
  {
    for(unsigned int j=0; j<failedEdges[i].size(); j++)
      delete failedEdges[i][j];
    failedEdges[i].clear();
  }
}

void EdgeMetric::reset_range()
{
  memcpy(range, range_backup, sizeof range);
}

