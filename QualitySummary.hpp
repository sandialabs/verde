//-------------------------------------------------------------------------
// Filename      : QualitySummary.hpp 
//
// Purpose       : holds statistical data for any number of samples of
//                 a single metric.
//
// Special Notes : 
//
// Creator       : 
//
// Date          : 10/14/98
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------
#ifndef QUALITY_SUMMARY
#define QUALITY_SUMMARY

class MeshEntity;

//! holds statistical data for any number of samples of a single metric
class QualitySummary {

public:
   //! Constructor
   QualitySummary();
  
   //! adds a sample to the summary
   void add_sample(const double sample, int element_id);
 
   //! resets the values in the summary  
   void reset();

   //! returns mean value
   double mean() const;

   //! returns standard deviation value
   double std_dev() const;

   //! returns minimum value
   double min_value() const;

   //! returns maximum value
   double max_value() const;

   //! returns the number of samples
   int number_of_samples() const;

   //! returns id of minimum element
   int min_id() const;

   //! returns id of maximum element 
   int max_id() const;

   //! prints the summary
   void print() const;

private:
   double meanValue;
   double meanSumSquares;
   double minValue;
   double maxValue;
   int    numSamples;
   int   minID;
   int   maxID;
};

inline double QualitySummary::mean() const {return meanValue;}
inline double QualitySummary::min_value() const {return minValue;}
inline double QualitySummary::max_value() const {return maxValue;}
inline int    QualitySummary::number_of_samples() const {return numSamples;}

#endif
