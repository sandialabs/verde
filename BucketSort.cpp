//-------------------------------------------------------------------------
// Filename      : BucketSort.cpp 
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

#include <algorithm>

#include "BucketSort.hpp"
#include "QuadElem.hpp"
#include "TriElem.hpp"

const int ELEMENTS_PER_CELL = 8;

//
// Constructor / Destructor ...................................................
//

const double GRID_EXTENSION   =  0.1;

BucketSort::BucketSort(VerdeVector &range_min, VerdeVector &range_max,
                       int number_elements)
{
    // set the bounding range of the reference geometry

   int grid_cells = number_elements / ELEMENTS_PER_CELL + 1;
   
   boundingRangeMinimum = range_min;
   boundingRangeMaximum = range_max;

    // estimate grid cell width

    VerdeVector range_width = boundingRangeMaximum - boundingRangeMinimum;
    double volume_estimate = range_width.x() * range_width.y();
    if (range_width.z() > 1e-06)
    {
       volume_estimate *= range_width.z();
    }
    double grid_cell_width = pow(volume_estimate / (double) grid_cells,
				 1.0 / 3.0);

    VerdeVector cell(1.0, 1.0, 1.0);
    cell *= GRID_EXTENSION * grid_cell_width;
    gridRangeMinimum = boundingRangeMinimum - cell;
    gridRangeMaximum = boundingRangeMaximum + cell;

    // estimate final grid cell width and grid parameters

    range_width = gridRangeMaximum - gridRangeMinimum;
    volume_estimate = range_width.x() * range_width.y() * range_width.z();
    grid_cell_width = pow(volume_estimate / (double) grid_cells, 1.0 / 3.0);

    numberGridCellsX = (int) ceil(range_width.x() / grid_cell_width + 0.5);
    numberGridCellsY = (int) ceil(range_width.y() / grid_cell_width + 0.5);
    numberGridCellsZ = (int) ceil(range_width.z() / grid_cell_width + 0.5);
    numberGridCells  = numberGridCellsX * numberGridCellsY * numberGridCellsZ;

    gridCellWidth.x(range_width.x() / ((double) numberGridCellsX));
    gridCellWidth.y(range_width.y() / ((double) numberGridCellsY));
    gridCellWidth.z(range_width.z() / ((double) numberGridCellsZ));

    gridCellWidthInverse.x(1.0 / gridCellWidth.x());
    gridCellWidthInverse.y(1.0 / gridCellWidth.y());
    gridCellWidthInverse.z(1.0 / gridCellWidth.z());

    // allocate neighborhood list arrays

    quadArray = new std::deque<QuadElem*>* [numberGridCells];
    assert(quadArray != NULL);
    int i;
    for (i = 0; i < numberGridCells; i++)
    {
      quadArray[i] = NULL;
    }

    triArray = new std::deque<TriElem*>* [numberGridCells];
    assert(triArray != NULL);
    for (i = 0; i < numberGridCells; i++)
    {
      triArray[i] = NULL;
    }
}

BucketSort::~BucketSort()
{
    // delete each DLVerdeFaceList and the array of pointers containing them

    int i;
    for (i = 0; i < numberGridCells; i++)
    {
      if (quadArray[i]) delete quadArray[i];
    }

    delete [] quadArray;

    for (i = 0; i < numberGridCells; i++)
    {
      if (triArray[i]) delete triArray[i];
    }

    delete [] triArray;
}


void BucketSort::add_quad(QuadElem* quad)
{
  // quadList is maintained in sorted order to make it faster to search
 
  std::deque<QuadElem*>::iterator found_quad =
    std::find(quadList.begin(), quadList.end(), quad);

  if (found_quad != quadList.end())
  {
    return;
  }
  else
  {
    quadList.push_back(quad);
  }

  // find the cell ranges in i,j k for this quad

   VerdeVector min, max;
   quad->bounding_box(min,max);
   //VerdeVector position;

   int imin,jmin,imax,jmax,kmin,kmax;

   grid_cell_ijk(min,imin,jmin,kmin);
   grid_cell_ijk(max,imax,jmax,kmax);


   // add quad to appropriate cell(s)
   
   for (int i = imin; i <=imax; i++)
   {
      for (int j = jmin; j <= jmax; j++)
      {
         for (int k = kmin; k <= kmax; k++)
         {
            int index = grid_cell_index(i,j,k);
            add_quad_to_cell(quad,index);
         }
      }
   }
}


void BucketSort::remove_quad(QuadElem* quad)
{
  // this quadList is sorted and unique
  std::deque<QuadElem*>::iterator found_quad =
    std::find(quadList.begin(), quadList.end(), quad);

  if(found_quad != quadList.end())
  {
    quadList.erase(found_quad);
  }
  else
  {
    return;
  }
  // find the cell ranges in i,j k for this quad
   
   VerdeVector min, max;
   quad->bounding_box(min,max);
   //VerdeVector position;

   int imin,jmin,imax,jmax,kmin,kmax;

   grid_cell_ijk(min,imin,jmin,kmin);
   grid_cell_ijk(max,imax,jmax,kmax);


   // remove quad from appropriate cell(s)

   for (int i = imin; i <=imax; i++)
   {
      for (int j = jmin; j <= jmax; j++)
      {
         for (int k = kmin; k <= kmax; k++)
         {
            int index = grid_cell_index(i,j,k);
            remove_quad_from_cell(quad,index);
         }
      }
   }
}

void BucketSort::add_tri(TriElem* tri)
{
  std::deque<TriElem*>::iterator found_tri =
    std::find(triList.begin(), triList.end(), tri);
 
  // we found the tri
  if (found_tri != triList.end())
  {
    return;
  }
  else
  {
    triList.push_back(tri);
  }
   
   // find the cell ranges in i,j k for this tri

   VerdeVector min, max;
   tri->bounding_box(min,max);
   //VerdeVector position;

   int imin,jmin,imax,jmax,kmin,kmax;

   grid_cell_ijk(min,imin,jmin,kmin);
   grid_cell_ijk(max,imax,jmax,kmax);


   // add tri to appropriate cell(s)
   
   for (int i = imin; i <=imax; i++)
   {
      for (int j = jmin; j <= jmax; j++)
      {
         for (int k = kmin; k <= kmax; k++)
         {
            int index = grid_cell_index(i,j,k);
            add_tri_to_cell(tri,index);
         }
      }
   }
}


void BucketSort::remove_tri(TriElem* tri)
{
  // triList is unique and sorted
  std::deque<TriElem*>::iterator found_tri =
    std::find(triList.begin(), triList.end(), tri);

   if (found_tri != triList.end())
   {
      triList.erase(found_tri);
   }
   else
   {
      return;
   }

   // find the cell ranges in i,j k for this tri
   
   VerdeVector min, max;
   tri->bounding_box(min,max);
   //VerdeVector position;

   int imin,jmin,imax,jmax,kmin,kmax;

   grid_cell_ijk(min,imin,jmin,kmin);
   grid_cell_ijk(max,imax,jmax,kmax);


   // remove tri from appropriate cell(s)

   for (int i = imin; i <=imax; i++)
   {
      for (int j = jmin; j <= jmax; j++)
      {
         for (int k = kmin; k <= kmax; k++)
         {
            int index = grid_cell_index(i,j,k);
            remove_tri_from_cell(tri,index);
         }
      }
   }
}

void BucketSort::grid_cell_ijk(const VerdeVector &position, int &i, int &j, int &k)
{
  VerdeVector range_vec = position;
  range_vec -= gridRangeMinimum;

  i = (int) (range_vec.x() * gridCellWidthInverse.x());
  j = (int) (range_vec.y() * gridCellWidthInverse.y());
  k = (int) (range_vec.z() * gridCellWidthInverse.z());

  return;
}

int BucketSort::grid_cell_index(int &i, int &j, int &k)
{
  return numberGridCellsX * (numberGridCellsY * k + j) + i;
}

void BucketSort::add_quad_to_cell(QuadElem* quad, int index)
{
  if (quadArray[index])
  {
    std::deque<QuadElem*>::iterator found_quad =
      std::find(quadArray[index]->begin(), 
                quadArray[index]->end(), quad);
    if (found_quad == quadArray[index]->end())
    {
      quadArray[index]->push_back(quad);
    }
  }
  else
  {
    quadArray[index] = new std::deque<QuadElem*>;
    quadArray[index]->push_back(quad);
  }
}

void BucketSort::remove_quad_from_cell(QuadElem* quad, int index)
{
  if(index < 0)
    return;

  if (quadArray[index]) 
  {
    std::deque<QuadElem*>::iterator found_quad =
      std::find(quadArray[index]->begin(), 
                quadArray[index]->end(), quad);
    if (found_quad != quadArray[index]->end())
    {
      quadArray[index]->erase(found_quad);
    }
    if (quadArray[index]->empty()) {
      delete quadArray[index];
      quadArray[index] = 0;
    }
  }
}

void BucketSort::add_tri_to_cell(TriElem* tri, int index)
{
  if (triArray[index])
  {
    std::deque<TriElem*>::iterator found_tri =
      std::find(triArray[index]->begin(),
          triArray[index]->end(), tri);
    if (found_tri == triArray[index]->end())
    {
      triArray[index]->push_back(tri);
    }
  }
  else
  {
    triArray[index] = new std::deque<TriElem*>;
    triArray[index]->push_back(tri);
  }
}

void BucketSort::remove_tri_from_cell(TriElem* tri, int index)
{
  if(index < 0)
    return;
  
  if (triArray[index]) 
  {
    std::deque<TriElem*>::iterator found_tri =
      std::find(triArray[index]->begin(),
          triArray[index]->end(), tri);
    if (found_tri != triArray[index]->end()) 
    {
      triArray[index]->erase(found_tri);
    }
    if (triArray[index]->empty()) 
    {
      delete triArray[index];
      triArray[index] = 0;
    }
  }
}

void BucketSort::get_neighborhood_quads(std::deque<QuadElem*> &quad_list)
{
  // retrieve quads over the current bounding box range

  for (int k = boundingCellMinimumZ; k <= boundingCellMaximumZ; k++)
  {
    int kn = numberGridCellsY * k;
    for (int j = boundingCellMinimumY; j <= boundingCellMaximumY; j++)
    {
      int jn = numberGridCellsX * (kn + j);
      for (int i = boundingCellMinimumX; i <= boundingCellMaximumX; i++)
      {
        int in = jn + i;
        if (quadArray[in])
        {
          std::copy(quadArray[in]->begin(), quadArray[in]->end(),
              std::back_inserter(quad_list));
        }
      }
    }
  }
  std::unique(quad_list.begin(), quad_list.end()); 
}

void BucketSort::get_neighborhood_tris(std::deque<TriElem*> &tri_list)
{
  // retrieve tris over the current bounding box range

  for (int k = boundingCellMinimumZ; k <= boundingCellMaximumZ; k++)
  {
    int kn = numberGridCellsY * k;
    for (int j = boundingCellMinimumY; j <= boundingCellMaximumY; j++)
    {
      int jn = numberGridCellsX * (kn + j);
      for (int i = boundingCellMinimumX; i <= boundingCellMaximumX; i++)
      {
        int in = jn + i;
        if (triArray[in])
        {
          std::copy(triArray[in]->begin(), triArray[in]->end(),
              std::back_inserter(tri_list));
        }
      }
    }
  }
  std::unique(tri_list.begin(), tri_list.end());
}


int BucketSort::number_quads()
{
   return quadList.size();
}

void BucketSort::set_bounds(VerdeVector &min,
                                         VerdeVector &max)
{
    // set the range and cell min and max values for the vector

    boundingRangeMinimum = min;
    boundingRangeMaximum = max;

    cell_from_range();
}


void BucketSort::cell_from_range()
{
    // Evaluate boundingCellMin and Max from the boundingRangeMin and Max
    // parameters

    VerdeVector range_vec = boundingRangeMinimum - gridRangeMinimum;
    boundingCellMinimumX  = (int) (range_vec.x() * gridCellWidthInverse.x());
    boundingCellMinimumY  = (int) (range_vec.y() * gridCellWidthInverse.y());
    boundingCellMinimumZ  = (int) (range_vec.z() * gridCellWidthInverse.z());

    if (boundingCellMinimumX < 0) boundingCellMinimumX = 0;
    if (boundingCellMinimumY < 0) boundingCellMinimumY = 0;
    if (boundingCellMinimumZ < 0) boundingCellMinimumZ = 0;

    range_vec = boundingRangeMaximum - gridRangeMinimum;
    boundingCellMaximumX  = (int) (range_vec.x() * gridCellWidthInverse.x());
    boundingCellMaximumY  = (int) (range_vec.y() * gridCellWidthInverse.y());
    boundingCellMaximumZ  = (int) (range_vec.z() * gridCellWidthInverse.z());

    if (boundingCellMaximumX >= numberGridCellsX)
        boundingCellMaximumX = numberGridCellsX - 1;
    if (boundingCellMaximumY >= numberGridCellsY)
        boundingCellMaximumY = numberGridCellsY - 1;
    if (boundingCellMaximumZ >= numberGridCellsZ)
        boundingCellMaximumZ = numberGridCellsZ - 1;
}


/*
void BucketSort::check(int bound_size, int int_size)
{
   int num_quads = number_quads();
   
   if ( (bound_size + int_size) != num_quads )
   {
      PRINT_INFO("BucketSort Problem: boundary = %d, internal = %d, "
                 "quadSearch = %d\n", bound_size, int_size, num_quads);
   }
}

*/
