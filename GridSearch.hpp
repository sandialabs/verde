//- Class: GridSearch
//- Description:  The GridSearch class is used to maintain a "bucket sort" of
//-               all mesh nodes used for rapid high performance nearest
//-               neighbor searches.  The object contains a 3-dimensional
//-               array of node lists for each grid cell (i.e. box) containing
//-               nodes that lie within the grid cell.  appropriate calls that
//-               return node, edge, face, and hex lists that lie in the
//-               neighborhood of an input mesh entity are provided.
//-
//-               GridSearch is a simple neighborhood searcher that stores
//-               CubitNodes in a data base broken up into small cubes about
//-               the meshing region of interest.  GridSearch provides
//-               functionality to return all nodes, edges, faces, or hexes
//-               that lie within the proximity of a user specified
//-               neighborhood.  The neighborhood is a cartesian bounding box
//-               that can be defined by the user in one of many ways.  These
//-               include:
//-
//-               1) a point (defined by a vector or a nodes coordinates) in
//-                  which case the neighborhood is the grid cell that
//-                  contains the point,
//-
//-               2) a bounding box that encloses two points (defined by
//-                  vectors or nodal coordinates),
//-
//-               3) a bounding box that encloses a list of nodes
//-
//-               4) a bounding box that encloses a CubitEdge, CubitFace, or
//-                  CubitHex, and
//-
//-               5) a bounding box that encloses a list of CubitEdges,
//-                  Faces, or Hexes.
//-
//- I haven't added the functionallity to find all nodes enclosed in a
//- MRefFaces bounding box because I haven't had the need, but it could be
//- done easily.  Unfortunately, geometric entities can have rather
//- extensive bounding boxes that may severly reduce the performance gain
//- typically seen when the search neighborhoods are small relative to the
//- global mesh neighborhood.  On the other hand, planar surfaces, or
//- MRefFaces with small bounding boxes will work quite nicely.
//-
//- Owner: Jim Hipp
//- Checked by: 
//- Version: $Id: 

#ifndef GRID_SEARCH_HPP
#define GRID_SEARCH_HPP

#include <vector>
#include <deque>
#include "NodeRef.hpp"
#include "VerdeVector.hpp"

class GridSearch
{
  private:

    //! Array of NodeRef lists for each grid cell    
    std::deque<NodeRef*>** neighborhoodList;

    //! Maps node to which cell it is in
    //std::deque<int>* node_to_cell_map;

    //! Adds nodes to cell neighborhoodList[i]
    void add_node_to_cell(NodeRef* node, int i);

    //! Removes node from cell neighborhoodList[i]
    void remove_node_from_cell(NodeRef* node, int i);

protected:
    
    //! Grid max and min coordinates 
    VerdeVector gridRangeMinimum;
    VerdeVector gridRangeMaximum;
    
    //! 3 double that represent number of grid cells in x,y,z directions. 
    VerdeVector gridCellWidth;

    //! intergers that represent number of grid cells in x,y,z directions.
    int         numberGridCellsX;
    int         numberGridCellsY;
    int         numberGridCellsZ;
   
    //! total number of grid cells
    int         numberGridCells;

    //! Extrema of bounding box of nodes and also used as temp bounding box
    VerdeVector boundingRangeMinimum;
    VerdeVector boundingRangeMaximum;

    //! used for look up of node
    int         boundingCellMinimumX;
    int         boundingCellMinimumY;
    int         boundingCellMinimumZ;
    int         boundingCellMaximumX;
    int         boundingCellMaximumY;
    int         boundingCellMaximumZ;

    //! width of individual grid cells 
    double grid_cell_width;

    //! returns the cell index of the node
    int grid_cell_index(NodeRef* node);

    //! evaluates the integer cell range given the double entity range
    void cell_from_range();

    //! sets boundingRangeMin and Max
    void bounding_range(std::vector<NodeRef*>& node_list);
    void bounding_range(const VerdeVector& vec);
    void bounding_range(NodeRef* node);


  public:

    //! constructor
    GridSearch( std::vector<NodeRef*> &nodes );
    
    //! destructor
    ~GridSearch(); 

    //! add nodes to neighborhoodList 
    void add_node(std::vector<NodeRef*> &nodes ); 

    //! remove nodes from neighborhoodList 
    void remove_node(NodeRef* node);

    //! sets up vicinity (cells) that will be searched
    void set_neighborhood_bounds(VerdeVector& vec, double size);
    void set_neighborhood_bounds(NodeRef* node);

    //! gets nodes in vicinity
    void get_neighborhood_nodes(std::deque<NodeRef*> &node_list);
   
    //! returns with of individual cell 
    double get_grid_cell_width() { return grid_cell_width; }
};


#endif // GRID_SEARCH_HPP
    
