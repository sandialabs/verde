//-------------------------------------------------------------------------
// Filename      : EdgeTool.hpp 
//
// Purpose       : Given a set of bounding polygons, finds the inferred
//                 model edges based on a minimum dihedral angle between
//                 surface polygons
//
// Special Notes : 
//
// Creator       : Ray Meyers
//
// Date          : 02/04/98
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------

#ifndef EDGE_TOOL_HPP
#define EDGE_TOOL_HPP

#include <deque>

#include "VerdeDefines.hpp"

class QuadElem;
class TriElem;
class EdgeElem;
class ElemRef;


enum EdgeCondition { IDENTICAL, COINCIDENT, NO_MATCH };

//! finds the inferred model edges
class EdgeTool
{
public:
   
  //! destructor
  ~EdgeTool();

  /*! processes all current boundary polygons from skintool and finds
  //  all edges whose attached polygons have dihedral > tolerance */
  void find_exterior_edges();

  //! finds coincident edges among the model edge
  void find_coincident_edges();

  //! returns list of edges which bound the boundary polygons
  void boundary_edges (std::deque<EdgeElem*> &edge_list);

  /*! returns list of edges whose boundary polygons have dihedral
  // angle greater than set tolerance */
  void inferred_edges (std::deque<EdgeElem*> &edge_list);

  //! returns all exterior edges, boundary_edges() + inferred_edges
  void exterior_edges (std::deque<EdgeElem*> &edge_list);

  //! returns all non-manifold edges (attached to more than two boundary polygons)
  void non_manifold_edges (std::deque<EdgeElem*> &edge_list);

  //! returns edges in coincidentEdgeList
  void coincident_edges (std::deque<EdgeElem*> &edge_list);

  //! return size of each list
  int number_exterior_edges();
  int number_coincident_edges();
  int number_boundary_edges();
  int number_inferred_edges();
  int number_non_manifold_edges();  
  
  //! returns the singleton pointer
  static EdgeTool *instance();

  //! deletes the singleton pointer
  static void delete_instance();

  //! prints summary information about the current boundary
  void print_summary();

  //! initializes parameters of the EdgeTool
  void initialize(int number_nodes);

  //! resets the EdgeTool by deleting information and resetting values
  void reset();

  //! resets calculated information
  void reset_calculated();

  //! returns the number of non-manifold bar edges in the model
  int number_non_manifold_bar_edges();

  //! returns non-manifold edges resulting from Bar elements
  void non_manifold_bar_edges (std::deque<EdgeElem*> &edge_list);

private:

  //! singleton pointer
  static EdgeTool *instance_;

  /*! cosine of the min angle for defining a model edge
  //  cosine is stored for efficiency in calculations */
  double minDihedralAngleCosine;

  //! constructor
  EdgeTool();
 
  /*! array of exterior quads, tris and edges from which to extract edges, each
  // polygon is stored based upon its nodes, and thus could be in these lists
  // once for each node */
  std::deque<QuadElem*> **quadList;
  std::deque<TriElem*> **triList;
  std::deque<EdgeElem*> **edgeList;
  
  //! the maximum number of nodes for which a given list should store quads
  static int NODES_PER_LIST;

  //! the number of lists allocated in the quadList and triList arrays
  int numberLists;

  //! List of edges which bound the boundary polygons of the model
  std::deque<EdgeElem*> boundaryEdgeList;

  //! edges whose attached polygons have dihedral angle > tolerance
  std::deque<EdgeElem*> inferredEdgeList;

  //! list of edges which have a coincident edge in the edgeList
  std::deque<EdgeElem*> coincidentEdgeList;

  //! list of edges which are non manifold
  std::deque<EdgeElem*> nonManifoldEdgeList;

  //! list of edges from BAR elements which are non manifold
  std::deque<EdgeElem*> nonManifoldBarEdgeList;

  /*! finds the (first) edge in the tool which has the same connectivity
  //  as the given edge */
  EdgeElem* find_match(EdgeElem *edge);
   
  //! adds a new edge to the appropriate quadList
  void add_edge(EdgeElem *new_edge);

  //! adds/removes QuadElem to/from the tool
  void add_quad(QuadElem *quad);
  void remove_quad(QuadElem* quad);

  //! adds/removes tri to/from the tool
  void add_tri(TriElem *tri);
  void remove_tri(TriElem* tri);
   
  //! adds the edges of the Quad to the EdgeTool edgeLists
  void add_quad_edges(QuadElem *quad);
   
  //! adds the edges of the Tri to the EdgeTool edgeLists
  void add_tri_edges(TriElem *tri);
   
  //! initializes the quadLists based on the number of nodes present
  void initialize_quad_list();
  void initialize_tri_list();
  void initialize_edge_list();

  //! return any quad in the list containing the edge (should be only one)
  QuadElem* find_quad_containing_edge(EdgeElem *edge);

  //! return any tri in the list containing the edge (should be only one)
  TriElem* find_tri_containing_edge(EdgeElem *edge);

  //! check for small dihedral angles of entities
  VerdeBoolean small_dihedral_angle(QuadElem *quad, TriElem *tri);
  VerdeBoolean small_dihedral_angle(QuadElem *quad1, QuadElem *quad2);
  VerdeBoolean small_dihedral_angle(TriElem *tri1, TriElem *tri2);

  //! tests the two edges to see if they are coincident in space
  EdgeCondition coincident_edge(EdgeElem *edge0, EdgeElem *edge1);

  //! deletes quadList and triList memory
  void delete_list_memory();  

  //! finds boundary edges (edges connected to a single boundary polygon)
  void find_boundary_edges(std::deque<QuadElem*> &quad_list,
                           std::deque<TriElem*> &tri_list,
                           std::deque<EdgeElem*> &edge_list);

  //! finds edges whose boundary polygons have dihedral angle > tolerancd
  void find_inferred_edges(std::deque<QuadElem*> &quad_list,
                           std::deque<TriElem*> &tri_list);

  
};

inline int EdgeTool::number_exterior_edges()
{
  return inferredEdgeList.size() + boundaryEdgeList.size();
}

inline int EdgeTool::number_boundary_edges()
{
  return  boundaryEdgeList.size();
}

inline int EdgeTool::number_inferred_edges()
{
  return inferredEdgeList.size();
}

inline int EdgeTool::number_non_manifold_edges()
{
  // for version 2.5, we will combine bar edges with others

  return nonManifoldEdgeList.size() + nonManifoldBarEdgeList.size();
}

inline int EdgeTool::number_non_manifold_bar_edges()
{
  return nonManifoldBarEdgeList.size();
}

inline int EdgeTool::number_coincident_edges()
{
  return coincidentEdgeList.size();
}


#endif
