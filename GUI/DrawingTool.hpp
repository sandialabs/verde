//------------------------------------------------------------------

// Filename          : DrawingTool.hpp
//
// Purpose           : Contians OpenGL commands for drawing entities
//                     to the screen.  Works with the GLWindow class.
//
// Creator           : Clinton Stimpson
//
// Date              : 7 Aug 2001
//
// Owner             : Clinton Stimpson
// -----------------------------------------------------------------


#ifndef DRAWING_TOOL
#define DRAWING_TOOL

#include <qgl.h>
#include <deque>
#include <vector>
#include <string>
#include <qstring.h>
#include "../VerdeVector.hpp"
#include "GLTextTool.hpp"

class QuadElem;
class TriElem;
class EdgeElem;
class VerdeVector;
class FailedHex;
class FailedTet;
class FailedQuad;
class FailedTri;
class FailedEdge;
class GLWindow;
struct PickedEntity;


// RGB Color
struct VerdeColorVal
{
   const char* name;
   float r;
   float g;
   float b;


   bool operator != (VerdeColorVal & color)
   {
     return (color.r != r || color.g != g || color.b != b);
   }
};

// commands for drawing entities
class DrawingList
{
  public:
    DrawingList();
    DrawingList(bool vis, int v_id, int d_id, int color[3], const char* nam);
    ~DrawingList();

    //! Copy Constructor
    DrawingList(const DrawingList& copy_from)
    {
      // The copy constructor should never be called.
      //assert(1 == 0);
      visible = copy_from.visible;
      verde_entity_id = copy_from.verde_entity_id;
      drawing_list_id = copy_from.drawing_list_id;
      color[0] = copy_from.color[0];
      color[1] = copy_from.color[1];
      color[2] = copy_from.color[2];
      name = copy_from.name;
    }

    bool visible;
    int verde_entity_id;
    int drawing_list_id;
    int color[3];
    std::string name;
};



//! Graphics tools 
class DrawingTool
{

friend class GLWindow;
	
public:
  ~DrawingTool();
  static DrawingTool* instance();
  static const int NUM_FIXED_COLORS;

  //! resets the drawing tool and the enumerated drawing lists
  void reset();

  //! resets drawing list of skin and list of labels of nodes and elements on skin
  void reset_surfaces();
  
  //! resets edge drawing list
  void reset_edges();

  //! resets boundary condition list
  void reset_bcs();

  //! resets normals drawing list
  void reset_normals();

  //! resets failed labels drawing list
  void reset_failed_labels();

  //! resets the boundary
  void reset_boundary(int temp=0);
 
  /*! add a surface mesh to the surface mesh drawing list
  // data taken from SkinTool */
  void add_surface_mesh();

  /*! add model edges to the edges drawing list
  // data taken from EdgeTool */
  void add_model_edges();
 
  //! add surface normals to the model data taken from SkinTool
  void add_surface_normals();

  //! add element to the open drawing list
  void add(QuadElem* elem);
  void add(TriElem* elem);
  void add(EdgeElem* elem);

  void clear_highlighted();
  void highlight(std::deque<FailedHex*> &failed_hexes);
  void highlight(std::deque<FailedTet*> &failed_tets);
  void highlight(std::deque<FailedQuad*> &failed_quads);
  void highlight(std::deque<FailedTri*> &failed_tris);
  void highlight(std::deque<FailedEdge*> &edge_list);
  void highlight(std::deque<QuadElem*> &coincident_quads);
  void highlight(std::deque<TriElem*> &coincident_tris);
  void highlight(std::deque<int> &node_ids);
  void highlight(std::deque<EdgeElem* > &edge_list);

  //! highlight selected items
  //! if which_item is greater than 1, that item in the selected_items 
  //! list will be highlighted
  //! if which_item is 0, then all items will be highlighted
  void highlight_selected(int which_item, 
                          const std::list<PickedEntity>& selected_items);
  //! clears the selected items
  void clear_selected();

  //! sets skin visibility toggle
  void toggle_skin_vis( bool flag );

  //! sets model edges' visibility toggle
  void toggle_model_edges_vis( bool flag );

  //! sets normals' visibility toggle 
  void toggle_normals_vis( bool flag );

  //! sets autofit toggle
  void toggle_autofit( bool flag ) { autofit_toggle = flag; }

  //! get autofit toggle
  bool get_autofit_toggle() { return autofit_toggle; }

  //! sets node label toggle
  void toggle_node_labels( bool flag );

  //! gets node label toggle
  bool get_node_label_toggle(){ return label_node_toggle; }

  //! sets element label toggle
  void toggle_element_labels( bool flag );

  //! gets element label toggle
  bool get_element_label_toggle(){ return label_element_toggle; }

  //! sets failed labels toggle
  void toggle_failed_labels( bool flag );

  //! gets failed labels toggle
  bool get_failed_label_toggle(){ return label_failed_toggle; }
  
  //! sets block label toggle 
  void toggle_block_labels( bool flag );

  //! gets block label toggle 
  bool get_block_label_toggle(){ return label_block_toggle; }

  //! sets nodeset label toggle
  void toggle_nodeset_labels( bool flag );

  //! gets nodeset label toggle
  bool get_nodeset_label_toggle(){ return label_nodeset_toggle; }
  
  //! sets sideset label toggle
  void toggle_sideset_labels( bool flag );
  
  //! gets sideset label toggle
  bool get_sideset_label_toggle(){ return label_sideset_toggle; }

  //! add or remove NodeBC's from drawing lists
  void add_NodeBC(int id);
  void remove_NodeBC(int id);

  //! add or remove SideBC's from drawings lists
  void add_ElementBC(int id);
  void remove_ElementBC(int id);

  //! Bitmap text labeling tool
  GLTextTool textTool;

  //! draws entities to the temp lists
  void draw_hexes ( std::vector<int>& hexes );
  void draw_tets  ( std::vector<int>& tets );
  void draw_knives( std::vector<int>& knives );
  void draw_wedges( std::vector<int>& wedges );
  void draw_pyramids(std::vector<int>& pyramids);
  void draw_quads ( std::vector<int>& quads );
  void draw_tris  ( std::vector<int>& tris );
  void draw_edges ( std::vector<int>& edges );
  void draw_nodes ( std::vector<int>& nodes );
  void clear_temp_lists();

  //! gets bounding box of model
  void get_bounding_box(double bound[3][2], int temp=0);

protected:
  
  //! add element to the open drawing list note: these use vertex arrays
  void add_hex(GLdouble coords[8][3]);
  void add_tet(GLdouble coords[4][3]);
  void add_quad(GLdouble coords[4][3]);
  void add_tri(GLdouble coords[4][3]);
  void add_edge(GLdouble coords[2][3]);

  //! shrink the size of the quad by factor, returns array
  void shrink(QuadElem* quad, double factor, double array[4][3]);

  //! shrink the size of the tri by factor, returns array
  void shrink(TriElem* tri, double factor, double array[3][3]);
  
  //! shrink the size of the edge by factor, returns array
  void shrink(EdgeElem* edge, double factor, double array[2][3]);
  
  //! create label drawing list
  void create_labels(std::deque<FailedHex*>& failed_hexes);
  void create_labels(std::deque<FailedTet*>& failed_tets);
  void create_labels(std::deque<FailedQuad*>& failed_quads);
  void create_labels(std::deque<FailedTri*>& failed_tris);
  void create_labels(std::deque<FailedEdge*>& failed_edges);
  void create_element_labels(std::deque<QuadElem*>& quadlist,
                             std::deque<TriElem*>& trilist,
                             std::deque<EdgeElem*>& edgelist );
  void create_node_labels(std::deque<QuadElem*>& quadlist,
                          std::deque<TriElem*>& trilist,
                          std::deque<EdgeElem*>& edgelist );

private:
 
  //! constructor
  DrawingTool();

  //! singleton pointer
  static DrawingTool* instance_;
	
  static const GLubyte hex_indices[6][4];
  static const GLubyte tet_indices[4][3];
  //static const GLubyte knife_indices[5][4];
  //static const GLubyte wedge_indices[5][4];
  //static const GLubyte pyramid_indices[5][4];
  //static const GLubyte tri_indices[3];
  //static const GLubyte quad_indices[4];

  //! calculates bounding box of active element blocks
  void calc_boundary(VerdeVector vec, int temp = 0 );

  //! flag indicating if bounding box has been used or not
  bool boundary_box_init;

  //! flag indicating if temp bounding box has been used or not
  bool temp_boundary_box_init;
	
  //! contains the x,y,z  min/max boundaries of the mesh
  double boundary[3][2];

  //! contains the x,y,z  min/max boundaries of temporary lists
  double temp_boundary[3][2];

  //! lists that contain the node sets and side sets to draw
  std::deque<DrawingList> DL_NodeBC;
  std::deque<DrawingList> DL_ElementBC;

  //! lists that contain the labels for node sets and side sets to draw
  std::deque<DrawingList> DL_NodeBC_Labels;
  std::deque<DrawingList> DL_ElementBC_Labels;
  DrawingList DL_Block_Labels;

  //! list that contains labels for nodes on skin
  DrawingList DL_Node_Labels;

  //! list that contains labels for elements on skin
  DrawingList DL_Element_Labels;

  //! list that contains the model surfaces and edges
  std::deque<DrawingList> DL_Model;

  //! list that contains misc. utility drawing lists, axis
  std::deque<DrawingList> DL_Util;

  //! list that contains the failed elements to draw
  std::deque<DrawingList> DL_FailedElements;

  //! lists of temporary drawing lists
  std::deque<DrawingList> DL_temp;

  //! lists of temporary drawing lists for labeling
  std::deque<DrawingList> DL_temp_labels;

  //! list of ids and coords for node labels 
  std::deque< std::pair< int, VerdeVector > > temp_node_label_list;

  //! list of ids and coords for element labels 
  std::deque< std::pair< int, VerdeVector > > temp_element_label_list;
 
  //! list that contains the selected items to draw
  DrawingList DL_SelectedItems;

  //! toggle labels
  bool label_node_toggle;
  bool label_element_toggle;
  bool label_failed_toggle;
  bool label_block_toggle;
  bool label_nodeset_toggle;
  bool label_sideset_toggle;

  //! toggle autofit
  bool autofit_toggle;
 
  VerdeColorVal get_color(int index);
};


inline DrawingTool* DrawingTool::instance()
{
  if(!instance_)
  {
    instance_ = new DrawingTool;
  }
  return instance_;
}

class test_Verde_ID_equals
{
  public:
    const int testID;
    test_Verde_ID_equals(int x) : testID(x){}
    bool operator()(DrawingList DL) const
    { return DL.verde_entity_id == testID; }
};

class name_equals
{
  public:
    const char* testID;
    name_equals(const char* name) : testID(name){}
    bool operator()(DrawingList DL) const
    { return (strcmp(DL.name.c_str(), testID) == 0); } 
};


#endif
