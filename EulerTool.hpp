//-------------------------------------------------------------------------
// Filename      : EulerTool.hpp 
//
// Purpose       : For a given set of elements, euler number
//
// Special Notes : 
//
// Creator       : Ray Meyers
//
// Date          : 12/7/98
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------

#ifndef EULER_TOOL_HPP
#define EULER_TOOL_HPP

#include <deque>

#include "VerdeDefines.hpp"

class EdgeElem;

//class HexRef;
class TetRef;
class PyramidRef;
class WedgeRef;
class QuadElem;
class TriElem;
class QuadRef;
class TriRef;
class ElemRef;


//! Calculate the euler number for a given set of elements.
class EulerTool
{
public:
   
  //! destructor
  ~EulerTool();
   
  //! creates edges for the ElemRef and adds to the tool
  void add_ref(ElemRef &new_elem);

  //! returns number of surface edges calculated
  int number_surface_edges();
   
  //! returns the singleton pointer
  static EulerTool *instance();

  //! prints summary information about the euler information
  void print_summary();

  //! initializes parameters of the EulerTool
  void initialize(int number_nodes);

  //! returns number of boundary edges found by Tool
  int number_boundary_edges();
   
  //! calcuates volume euler for the system
  VerdeBoolean calculate_euler(int number_hexes,
                               int number_tets,
                               int number_pyramids,
                               int number_wedges,
                               int number_knives);

  //! resets the Euler tool and gets rid of allocated data
  void reset();

private:

  //! singleton pointer
  static EulerTool *instance_;

  //! constructor
  EulerTool();

  /*! array of Edge Lists, each list contains edges based upon the
  //  lowest node id in the edge (hash like). */
  std::deque<EdgeElem*> **edgeList;
   
  //! the maximum number of nodes for which a given list should store quads
  static int NODES_PER_LIST;

  //! the number of lists allocated in the edgeList array
  int numberLists;

  //! the number of volume edges found
  int numberEdges;

  /*! finds any edge in the tool which has connectivity
  //  opposite to the given edge */
  VerdeStatus find_match(EdgeElem *edge);
   
  //! adds a new edge to the appropriate edgeList
  void add_edge(EdgeElem *new_edge);

  //! adds the edges of the quad to the tool edgeList
  void add_quad(QuadElem *new_quad);

  //! adds the edges of the tri to the tool edgeList
  void add_tri(TriElem *new_tri);

  //! initializes the edgeLists based on the number of nodes present
  void initialize_edge_list();

  //! counts number of edges found in all edge lists
  int number_edges();

  //! counts number of unmarked edges found in all edge lists
  int number_unmarked_edges();

  //! tries to predict probable topology of input surface(s)
  void predict_2d_topology(int euler_number,
                           int number_boundary_edges);

  //! tries to predict probable topology of input volume(s)
  void predict_3d_topology(int euler_number);
   
};

#endif
