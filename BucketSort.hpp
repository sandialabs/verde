//-------------------------------------------------------------------------
// Filename      : BucketSort.hpp 
//
// Purpose       : BucketSort sorts quads and tris into buckets based on
//                 the spatial coordinates of each element.  The user can
//                 then query for the quads or tris in a given area or near
//                 a given element
//
// Special Notes : 
//
// Creator       : Ray Meyers
//
// Date          : 03/2/99
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------

#ifndef BUCKET_SORT_HPP
#define BUCKET_SORT_HPP

#include <deque>
#include "VerdeVector.hpp"

class QuadElem;
class TriElem;


//! bucket sort
class BucketSort 
{
private:

   //! array of VerdeElement lists for each grid cell
   std::deque<QuadElem*> **quadArray;
   std::deque<TriElem*> ** triArray;

   //! master list of quads/tris which are in the search area
   std::deque<QuadElem*> quadList;
   std::deque<TriElem*> triList;



   //! search grid parameters
   VerdeVector gridRangeMinimum;
   VerdeVector gridRangeMaximum;             
   VerdeVector gridCellWidth;
   VerdeVector gridCellWidthInverse;
   int         numberGridCellsX;
   int         numberGridCellsY;
   int         numberGridCellsZ;
   int         numberGridCells;
   
    // bounding box range and cell parameters
    VerdeVector boundingRangeMinimum;
    VerdeVector boundingRangeMaximum;
    int         boundingCellMinimumX;
    int         boundingCellMinimumY;
    int         boundingCellMinimumZ;
    int         boundingCellMaximumX;
    int         boundingCellMaximumY;
    int         boundingCellMaximumZ;

   

   int grid_cell_index(int &i, int &j, int &k);
   void add_quad_to_cell(QuadElem* quad, int index);
   void remove_quad_from_cell(QuadElem* quad, int index);
   void add_tri_to_cell(TriElem* tri, int index);
   void remove_tri_from_cell(TriElem* tri, int index);
   void grid_cell_ijk(const VerdeVector &position, int &i, int &j, int &k);
   void cell_from_range();
   
   

public:

   //! constructor and destructor
   BucketSort(VerdeVector &range_min, VerdeVector &range_max,
              int number_element);

   ~BucketSort();

   //! adds or removes quads from the neighbor lists
   void add_quad(QuadElem* quad);
   void remove_quad(QuadElem* quad);
   
   //! adds or removes tris from the neighbor lists
   void add_tri(TriElem* tri);
   void remove_tri(TriElem* tri);
   

   void get_neighborhood_quads(std::deque<QuadElem*> &quad_list);

   void get_neighborhood_tris(std::deque<TriElem*> &tri_list);

   
   int number_quads();

   void set_bounds(VerdeVector &min, VerdeVector &max);

};


#endif // BUCKET_SORT_HPP

    
