//- Class: GridSearch
//- Description:  The GridSearch class is used to maintain a "bucket sort" of
//-               all mesh nodes used for rapid high performance nearest
//-               neighbor searches.  The object contains a 3-dimensional
//-               array of node lists for each grid cell (i.e. box) containing
//-               nodes that lie within the grid cell.  appropriate calls that
//-               return node, edge, face, and hex lists that lie in the
//-               neighborhood of an input mesh entity are provided.
//- Owner: Jim Hipp
//- Checked by:

#include <math.h>
#include <assert.h>
#include <vector>
#include <deque>
#include "GridSearch.hpp"
#include "VerdeVector.hpp"
#include "NodeRef.hpp"


const double GRID_EXTENSION   =  0.1;



GridSearch::GridSearch( std::vector<NodeRef*> &nodes )
{
  int i;
  //make sure we have nodes
  assert( nodes.size() );

  // find the geometric bounding range mesh
  bounding_range( nodes );

  VerdeVector bound_box_vec = boundingRangeMaximum - boundingRangeMinimum;
  double length = bound_box_vec.length();
  bound_box_vec.normalize();
  VerdeVector offset = (GRID_EXTENSION * length) * bound_box_vec;
  gridRangeMaximum.x(boundingRangeMaximum.x() + 0.1*offset.length());
  gridRangeMaximum.y(boundingRangeMaximum.y() + 0.1*offset.length());
  gridRangeMaximum.z(boundingRangeMaximum.z() + 0.1*offset.length());

  gridRangeMinimum.x(boundingRangeMinimum.x() - 0.1*offset.length());
  gridRangeMinimum.y(boundingRangeMinimum.y() - 0.1*offset.length());
  gridRangeMinimum.z(boundingRangeMinimum.z() - 0.1*offset.length());

  grid_cell_width = (gridRangeMaximum - gridRangeMinimum).length()/ 
              ( .8*pow( nodes.size(), 1.0/3.0 ) );

  // evaluate grid parameters
  VerdeVector cell(1.0, 1.0, 1.0);
  cell *= ( GRID_EXTENSION + 5.0 ) * grid_cell_width;

  VerdeVector range_width = gridRangeMaximum - gridRangeMinimum;

  numberGridCellsX = (int) ceil(range_width.x() / grid_cell_width + 0.5);
  numberGridCellsY = (int) ceil(range_width.y() / grid_cell_width + 0.5);
  numberGridCellsZ = (int) ceil(range_width.z() / grid_cell_width + 0.5);
  numberGridCells  = numberGridCellsX * numberGridCellsY * numberGridCellsZ;

  gridCellWidth.x(range_width.x() / ((double) numberGridCellsX));
  gridCellWidth.y(range_width.y() / ((double) numberGridCellsY));
  gridCellWidth.z(range_width.z() / ((double) numberGridCellsZ));

  //allocate node_to_cell_map
  // node_to_cell_map = new std::deque<int>( nodes.size() );  

  // allocate neighborhood list array
  neighborhoodList = new std::deque<NodeRef*>* [numberGridCells];
  assert(neighborhoodList != NULL);
  for ( i = 0; i < numberGridCells; i++)
  {
    neighborhoodList[i] = NULL;
  }
}

GridSearch::~GridSearch()
{
  if(numberGridCells == 0)
    return;
 
  for(int i=0; i<numberGridCells; i++)
    delete neighborhoodList[i];

  delete [] neighborhoodList;

}

void GridSearch::bounding_range(std::vector<NodeRef*>& node_list)
{

  if ( !node_list.size() )
    return;

  // initialize min and max range values to the first node coordinates
  boundingRangeMinimum = node_list[0]->coordinates();
  boundingRangeMaximum = node_list[0]->coordinates();

  // find the min and max coordinates that completely encloses the
  // node list
  int i;
  for(i=node_list.size(); i--; )
    bounding_range(node_list[i]->coordinates());
}


void GridSearch::bounding_range(const VerdeVector& vec)
{
  if (vec.x() < boundingRangeMinimum.x())
    boundingRangeMinimum.x(vec.x());
  else if (vec.x() > boundingRangeMaximum.x())
    boundingRangeMaximum.x(vec.x());

  if (vec.y() < boundingRangeMinimum.y())
    boundingRangeMinimum.y(vec.y());
  else if (vec.y() > boundingRangeMaximum.y())
    boundingRangeMaximum.y(vec.y());

  if (vec.z() < boundingRangeMinimum.z())
    boundingRangeMinimum.z(vec.z());
  else if (vec.z() > boundingRangeMaximum.z())
    boundingRangeMaximum.z(vec.z());
}

void GridSearch::bounding_range(NodeRef* node)
{
  VerdeVector pos = node->coordinates();	
  double xpos = pos.x();
  double ypos = pos.y();
  double zpos = pos.z();

  if (xpos < boundingRangeMinimum.x())
    boundingRangeMinimum.x(xpos);
  else if (xpos > boundingRangeMaximum.x())
    boundingRangeMaximum.x(xpos);

  if (ypos < boundingRangeMinimum.y())
    boundingRangeMinimum.y(ypos);
  else if (ypos > boundingRangeMaximum.y())
    boundingRangeMaximum.y(ypos);

  if (zpos < boundingRangeMinimum.z())
    boundingRangeMinimum.z(zpos);
  else if (zpos > boundingRangeMaximum.z())
    boundingRangeMaximum.z(zpos);
}

void GridSearch::add_node(std::vector<NodeRef*> &nodes ) //int id 
{
  int j;
  unsigned int i, number_nodes;
  number_nodes = nodes.size();

  for(i=0; i<number_nodes; i++)
  {
    NodeRef *node = nodes[i];
    j = grid_cell_index(node);
    if (j < 0 || j > numberGridCells){
      continue; 
    }
   
    //so node know what cell it is in
    //(*node_to_cell_map)[id] = id;

    //so cell knows what nodes are in it
    //VerdeVector vec = node->coordinates();
    add_node_to_cell(node, j);
  }
}
/*
void GridSearch::add_node(NodeRef* node ) //int id 
{
    int i = grid_cell_index(node);
    if (i < 0 || i > numberGridCells){
      return;
    }
   
    //so node know what cell it is in
    //(*node_to_cell_map)[id] = id;

    //so cell knows what nodes are in it
    VerdeVector vec = node->coordinates();
    add_node_to_cell(node, i);

}
*/
void GridSearch::add_node_to_cell(NodeRef* node, int i)
{
  if (i >= 0 && i < numberGridCells) {
    if (neighborhoodList[i])
    {
      if( std::find(neighborhoodList[i]->begin(),
      		     neighborhoodList[i]->end(), node) == neighborhoodList[i]->end() )
      {
        neighborhoodList[i]->push_back(node);
      }
    }
    else
    {
      neighborhoodList[i] = new std::deque<NodeRef*>;
      neighborhoodList[i]->push_back(node);
    }
  }
}

int GridSearch::grid_cell_index(NodeRef* node)
{
  VerdeVector range_vec = node->coordinates();
  range_vec -= gridRangeMinimum;

  int i = (int) (range_vec.x() / gridCellWidth.x());
  int j = (int) (range_vec.y() / gridCellWidth.y());
  int k = (int) (range_vec.z() / gridCellWidth.z());

  return numberGridCellsX * (numberGridCellsY * k + j) + i;
}

void GridSearch::set_neighborhood_bounds(NodeRef* node)
{
    // initialize min and max range values to the node coordinates


    boundingRangeMinimum = node->coordinates();
    boundingRangeMaximum = node->coordinates();

    // find the cell min and max values

    cell_from_range();
}

void GridSearch::set_neighborhood_bounds(VerdeVector& vec, double size)
{
    // set the range and cell min and max values for the vector

    VerdeVector search_extent_box(size, size, size); 
    boundingRangeMinimum = vec - search_extent_box;
    boundingRangeMaximum = vec + search_extent_box;

    cell_from_range();
}


void GridSearch::cell_from_range()
{
    // Evaluate boundingCellMin and Max from the boundingRangeMin and Max
    // parameters

    VerdeVector range_vec = boundingRangeMinimum - gridRangeMinimum;
    boundingCellMinimumX  = (int) (range_vec.x() / gridCellWidth.x());
    boundingCellMinimumY  = (int) (range_vec.y() / gridCellWidth.y());
    boundingCellMinimumZ  = (int) (range_vec.z() / gridCellWidth.z());

    if (boundingCellMinimumX < 0) boundingCellMinimumX = 0;
    if (boundingCellMinimumY < 0) boundingCellMinimumY = 0;
    if (boundingCellMinimumZ < 0) boundingCellMinimumZ = 0;

    range_vec = boundingRangeMaximum - gridRangeMinimum;
    boundingCellMaximumX  = (int) (range_vec.x() / gridCellWidth.x());
    boundingCellMaximumY  = (int) (range_vec.y() / gridCellWidth.y());
    boundingCellMaximumZ  = (int) (range_vec.z() / gridCellWidth.z());

    if (boundingCellMaximumX >= numberGridCellsX)
        boundingCellMaximumX = numberGridCellsX - 1;
    if (boundingCellMaximumY >= numberGridCellsY)
        boundingCellMaximumY = numberGridCellsY - 1;
    if (boundingCellMaximumZ >= numberGridCellsZ)
        boundingCellMaximumZ = numberGridCellsZ - 1;
}

void GridSearch::get_neighborhood_nodes( std::deque<NodeRef*> &node_list )
{
  // retrieve nodes over the current bounding box range

  node_list.clear();

  for (int k = boundingCellMinimumZ; k <= boundingCellMaximumZ; k++)
  {
    int kn = numberGridCellsY * k;
    for (int j = boundingCellMinimumY; j <= boundingCellMaximumY; j++)
    {
      int jn = numberGridCellsX * (kn + j);
      for (int i = boundingCellMinimumX; i <= boundingCellMaximumX; i++)
      {
        int in = jn + i;
        assert( in >= 0 && in < numberGridCells );
        if (neighborhoodList[in])
        {
           std::copy(neighborhoodList[in]->begin(),
                     neighborhoodList[in]->end(),
                     std::back_insert_iterator< std::deque<NodeRef*> > (node_list));
        }
      }
    }
  }
}
#if 0
void GridSearch::remove_node(CubitNode* node)
{
    // remove node from the appropriate list
    int index = -1;
    ToolData* td_temp = node->get_TD(&TDCellIndex::is_cell_index);
    if (!td_temp)
      return;
    TDCellIndex* td_index = CAST_TO(td_temp, TDCellIndex);
    if( td_index ){
      index = td_index->cell_index();
      node->delete_TD( &TDCellIndex::is_cell_index );
    }
    remove_node_from_cell(node, index);
}



void GridSearch::remove_node_from_cell(CubitNode* node, int i)
{
  if ( i >= 0 && i < numberGridCells) {
    if (neighborhoodList[i]) {
      if (neighborhoodList[i]->move_to(node)) {
        neighborhoodList[i]->extract();
      }
      if (!neighborhoodList[i]->size()) {
        delete neighborhoodList[i];
        neighborhoodList[i] = NULL;
      }
    }
  }
}









#endif


