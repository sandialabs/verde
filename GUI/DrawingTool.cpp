//-------------------------------------------------------------------
// Filename          : DrawingTool.cpp
//
// Purpose           : Contians OpenGL commands for drawing entities
//                     to the screen.  Behaves like a database for 
//                     GLWindow.
//
// Creator           : Clinton Stimpson
//
// Date              : 7 Aug 2001
//
// Owner             : Clinton Stimpson
//-------------------------------------------------------------------

#include <math.h>
#include <qgl.h>
#include <qfont.h>
#include <iterator>
#include <algorithm>
#include <vector>

#include "DrawingTool.hpp"
#include "PickTool.hpp"
#include "../QuadElem.hpp"
#include "../TriElem.hpp"
#include "../EdgeElem.hpp"
#include "../VerdeVector.hpp"
#include "../SkinTool.hpp"
#include "../EdgeTool.hpp"
#include "../VerdeVector.hpp"
#include "../FailedHex.hpp"
#include "../FailedTet.hpp"
#include "../FailedQuad.hpp"
#include "../FailedTri.hpp"
#include "../FailedEdge.hpp"
#include "../HexMetric.hpp"
#include "../NodeRef.hpp"
#include "../Mesh.hpp"
#include "../NodeBC.hpp"
#include "../ElementBC.hpp"
#include "../ElementBlock.hpp"
#include "../VerdeApp.hpp"


#ifdef NT
  #define M_PI 3.14159265358979323846
#endif

const int DrawingTool::NUM_FIXED_COLORS = 83;

DrawingList::DrawingList(bool vis, int v_id, int d_id, int col[3], const char* nam)
{
  visible = vis;
  verde_entity_id = v_id;
  drawing_list_id = d_id;
  for(int i=0; i<3; i++)
    color[i] = col[i];
  if(nam!=NULL)
    name = nam;
}

DrawingList::DrawingList()
  : visible(false), verde_entity_id(0), drawing_list_id(0), name("")
{
}

DrawingList::~DrawingList()
{
}

static const VerdeColorVal fixed_colors[DrawingTool::NUM_FIXED_COLORS] =
{
#define COLOR_TABLE(NAME,R,G,B) {NAME,R,G,B},
#include "FixedColorDefinitions.h"
};


VerdeColorVal DrawingTool::get_color(int index)
{ 
  return fixed_colors[ index%(NUM_FIXED_COLORS-1) ];  
}


DrawingTool* DrawingTool::instance_ = NULL;

DrawingTool::DrawingTool()
{
  reset_boundary();
  label_failed_toggle = false;
  label_block_toggle = false;
  label_nodeset_toggle = false;
  label_sideset_toggle = false;
  label_element_toggle = false;
  label_node_toggle = false;
  autofit_toggle = false;

}

DrawingTool::~DrawingTool()
{                         
  textTool.delete_font();
}

void DrawingTool::reset()
{
  reset_surfaces();
  reset_edges();
  clear_highlighted();
  reset_normals();
  reset_boundary();
  reset_failed_labels();
  clear_selected();
  clear_temp_lists();

  if(!verde_app->should_add_mesh())
    reset_bcs();
    
  //textTool.delete_font();
}

void DrawingTool::reset_surfaces()
{


   // find "surface" drawing list in DL_Model list
   int list_id = 0;
   std::deque<DrawingList>::iterator DL_iterator;
   for(DL_iterator = DL_Model.begin(); DL_iterator != DL_Model.end() && list_id==0; ++DL_iterator)
   {
      if(strcmp( (*DL_iterator).name.c_str(), "surface") == 0 )
      {
         list_id = (*DL_iterator).drawing_list_id;
         glDeleteLists(list_id, 1);
         DL_Model.erase(DL_iterator);
      }
   }

   if(!list_id) 
      return;
    
   glDeleteLists(list_id, 1);

   // get rid of label drawing lists
   glDeleteLists(DL_Node_Labels.drawing_list_id, 1);
   glDeleteLists(DL_Element_Labels.drawing_list_id, 1);
}

void DrawingTool::reset_edges()
{

   // find "edge" drawing list in DL_Model list
   int list_id = 0;
   std::deque<DrawingList>::iterator DL_iterator;
   for(DL_iterator = DL_Model.begin(); DL_iterator != DL_Model.end() && list_id==0; ++DL_iterator)
   {
      if(strcmp( (*DL_iterator).name.c_str(), "edge") == 0 )
      {
         list_id = (*DL_iterator).drawing_list_id;
         glDeleteLists(list_id, 1);
         DL_Model.erase(DL_iterator);
      }
   }

   if(!list_id) 
      return;
 
   glDeleteLists(list_id, 1);

}

void DrawingTool::reset_bcs()
{
   std::deque<DrawingList>::iterator DL_iterator;
   while(DL_NodeBC.size())
   {
      DL_iterator = DL_NodeBC.begin();
      glDeleteLists((*DL_iterator).drawing_list_id, 1);
      DL_NodeBC.erase(DL_iterator);
   }

   while(DL_ElementBC.size())
   {
      DL_iterator = DL_ElementBC.begin();
      glDeleteLists((*DL_iterator).drawing_list_id, 1);
      DL_ElementBC.erase(DL_iterator);
   }

   while(DL_ElementBC_Labels.size())
   {
      DL_iterator = DL_ElementBC_Labels.begin();
      glDeleteLists((*DL_iterator).drawing_list_id, 1);
      DL_ElementBC_Labels.erase(DL_iterator);
   }

   while(DL_NodeBC_Labels.size())
   {
      DL_iterator = DL_NodeBC_Labels.begin();
      glDeleteLists((*DL_iterator).drawing_list_id, 1);
      DL_NodeBC_Labels.erase(DL_iterator);
   }
}

void DrawingTool::reset_normals()
{

   // find "normal" drawing list in DL_Model list
   int list_id = 0;
   std::deque<DrawingList>::iterator DL_iterator;
   for(DL_iterator = DL_Model.begin(); DL_iterator != DL_Model.end() && list_id==0; ++DL_iterator)
   {
      if(strcmp( (*DL_iterator).name.c_str(), "normal") == 0 )
      {
         list_id = (*DL_iterator).drawing_list_id;
         glDeleteLists(list_id, 1);
         DL_Model.erase(DL_iterator);
      }
   }
}

//! remove the DrawingList and delete the GL drawing list
void DrawingTool::reset_failed_labels()
{
  int list_id = 0;

  std::deque<DrawingList>::iterator it_labels;
  // find the name in DrawingList that matches "label failed"
  it_labels = std::find_if(DL_Util.begin(), 
      DL_Util.end(), name_equals("label failed"));

  // if we found it, grab the drawing list id
  if(it_labels != DL_Util.end())
  {
      list_id = (*it_labels).drawing_list_id;
      glDeleteLists(list_id, 1);
      DL_Util.erase(it_labels);
  }
}

const GLubyte DrawingTool::hex_indices[6][4] =
{
  {0, 1, 2 ,3},
  {4, 7, 6, 5},
  {0, 3, 7, 4},
  {3, 2, 6, 7},
  {4, 5, 1, 0},
  {5, 6, 2, 1}
}; 

const GLubyte DrawingTool::tet_indices[4][3] =
{
  {0, 2, 1},
  {0, 1, 3},
  {3, 2, 0},
  {1, 2, 3}
}; 

/*
const GLubyte DrawingTool::tri_indices[3] =
{
  0, 1, 2
};
*/
/*
const GLubyte DrawingTool::quad_indices[4] =
{
  0, 1, 2, 3
};
*/
/*
const GLubyte DrawingTool::knife_indices[5][4] =
{

};

const GLubyte DrawingTool::wedge_indices[5][4] =
{


};

const GLubyte DrawingTool::pyramid_indices[5][4] =
{


};
*/


// add hex to the currently open drawing list
void DrawingTool::add_hex(GLdouble coords[8][3])
{
  // assumes that glEnableClientState(GL_VERTEX_ARRAY) was already called
   glVertexPointer(3, GL_DOUBLE, 0, coords);
   glDrawElements(GL_QUADS, 24, GL_UNSIGNED_BYTE, hex_indices);
}

// add tet to the currently open drawing list
void DrawingTool::add_tet(GLdouble coords[4][3] )
{
  // assumes that glEnableClientState(GL_VERTEX_ARRAY) was already called
  glVertexPointer(3, GL_DOUBLE, 0, coords);
  glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_BYTE, tet_indices);
}

// add quad to the currently open drawing list
void DrawingTool::add_quad(GLdouble coords[4][3] )
{
  glBegin(GL_QUADS);
  glVertex3dv(coords[0]);
  glVertex3dv(coords[1]);
  glVertex3dv(coords[2]);
  glVertex3dv(coords[3]);
  glEnd();
}

// add tri to the currently open drawing list
void DrawingTool::add_tri(GLdouble coords[3][3] )
{
  glBegin(GL_TRIANGLES);
  glVertex3dv(coords[0]);
  glVertex3dv(coords[1]);
  glVertex3dv(coords[2]);
  glEnd();
}

// add edge to the currently open drawing list
void DrawingTool::add_edge(GLdouble coords[2][3] )
{
  glBegin(GL_LINES);
  glVertex3dv(coords[0]);
  glVertex3dv(coords[1]);
  glEnd();
}

// adds the surface mesh to the surface mesh drawing list
void DrawingTool::add_surface_mesh()
{

  if((ElementBlock::number_active_blocks() == 0 ||
     ElementBlock::was_just_zero() )  &&
     ElementBC::number_active_sidesets() == 0 &&
     NodeBC::number_active_nodesets() == 0 )
  {
    reset_boundary();
  } 
  else if( autofit_toggle && ElementBlock::number_active_blocks() > 0 )
  {
    reset_boundary();
  }

  // lists for the surface elements
  std::deque<QuadElem*> quadlist;
  std::deque<TriElem*> trilist;
  std::deque<EdgeElem*> edgelist;
  QuadElem* quad;
  TriElem* tri;
  EdgeElem* edge;

  // get exterior quads and tris
  SkinTool::exterior_quads(quadlist);
  SkinTool::exterior_tris(trilist);
  SkinTool::exterior_edges(edgelist);

  // find "surface" drawing list in DL_Model list
   int list_id = 0;
   std::deque<DrawingList>::iterator DL_iterator;
   for(DL_iterator = DL_Model.begin(); DL_iterator != DL_Model.end() && list_id == 0; ++DL_iterator)
   { 
     if(strcmp( (*DL_iterator).name.c_str(), "surface") == 0 )
     {
       list_id = (*DL_iterator).drawing_list_id;
     }
   }

   if(!list_id)
   {
     int color[3] = {0,0,0};
     list_id = glGenLists(1);
     DrawingList new_DL(true, 0,  list_id, color,  "surface" );
     DL_Model.push_front(new_DL);
   }

  // put each element into this list
  glNewList(list_id, GL_COMPILE);

  Mesh* mesh = verde_app->mesh();
  int block_id;
  VerdeColorVal col;
  int curr_block = -1;
  //int num_sw = 0;   // how many times we have to change color

  // case where you have only 1 quad 
  if(quadlist.size() == 1)
  {
     quad = *(quadlist.begin());
     calc_boundary(quad->coordinates(0));
     calc_boundary(quad->coordinates(1));
     calc_boundary(quad->coordinates(2));
     calc_boundary(quad->coordinates(3));
  }
  for(std::deque<QuadElem*>::iterator qiter=quadlist.begin(); qiter != quadlist.end(); ++qiter )
  {
    quad = *qiter;
    block_id = mesh->find_block_for_element(quad->owner_id())->block_id();
    if( curr_block != block_id)
    {
       col = get_color(block_id);
       curr_block = block_id;
       glColor3f(col.r, col.g, col.b);
       GLfloat em[4] = {col.r, col.g, col.b, 1. };
       glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, em);
       //num_sw++;
    }
    add(quad);
  }

  // case where you have only 1 tri
  if(trilist.size() == 1)
  {
     tri = *(trilist.begin());
     calc_boundary(tri->coordinates(0));
     calc_boundary(tri->coordinates(1));
     calc_boundary(tri->coordinates(2));
  }
  for(std::deque<TriElem*>::iterator titer=trilist.begin(); titer != trilist.end(); ++titer )
  {
    tri = *titer;
    block_id = mesh->find_block_for_element(tri->owner_id())->block_id();
    if( curr_block != block_id )
    {
       col = get_color(block_id);
       curr_block = block_id;
       glColor3f(col.r, col.g, col.b);
       GLfloat em[4] = {col.r, col.g, col.b, 1. };
       glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, em);
       //num_sw++;
    }
    add(tri);
  }
  // case where you have only 1 edge 
  if(edgelist.size() == 1)
  {
     edge = *(edgelist.begin());
     calc_boundary(edge->coordinates(0));
     calc_boundary(edge->coordinates(1));
  }
  for(std::deque<EdgeElem*>::iterator eiter=edgelist.begin(); eiter != edgelist.end(); ++eiter)
  {
    edge = *eiter;
    block_id = mesh->find_block_for_element(edge->owner_id())->block_id();
    if( curr_block != block_id )
    {
       col = get_color(block_id);
       curr_block = block_id;
       glColor3f(col.r, col.g, col.b);
       //num_sw++;
    }
    add(edge);
  }
     
  glEndList();

  // create drawing lists for element block labels
  std::deque<ElementBlock*> blocks;
  mesh->element_blocks(blocks);
    
  // first time through, get an id to build a new list 
  if( DL_Block_Labels.drawing_list_id == 0 ) 
  {
    list_id = glGenLists(1);
    DL_Block_Labels.drawing_list_id = list_id;
    DL_Block_Labels.color[0] = 255; 
    DL_Block_Labels.color[1] = 255; 
    DL_Block_Labels.color[2] = 255; 
  }
  else
    list_id = DL_Block_Labels.drawing_list_id;

  glNewList(list_id, GL_COMPILE);

  for(std::deque<ElementBlock*>::iterator biter=blocks.begin(); biter != blocks.end(); ++biter)
  {
    ElementBlock* block = *biter;

    if( !block->is_active() )
      continue;

    int id = block->block_id();

    VerdeVector centroid = block->centroid();    
       
    //! Note that glRasterPos* uses the world coordinates
    glRasterPos3d(centroid.x(), centroid.y(), centroid.z());
    textTool.printGLf("%d", id);
     
    VerdeVector max = block->bounding_box()->get_max();
    VerdeVector min = block->bounding_box()->get_min();

    calc_boundary( max );   
    calc_boundary( min );   
  }
  glEndList();

  //create node and element drawing lists
  create_node_labels(quadlist, trilist, edgelist);
  create_element_labels(quadlist,trilist,edgelist);

}


void DrawingTool::add_model_edges()
{


  // find "edge" drawing list in DL_Model list
   int list_id = 0;
   std::deque<DrawingList>::iterator DL_iterator;
   for(DL_iterator = DL_Model.begin(); DL_iterator != DL_Model.end() && list_id == 0; ++DL_iterator)
   { 
      if(strcmp( (*DL_iterator).name.c_str(), "edge") == 0 )
      {
         list_id = (*DL_iterator).drawing_list_id;
      }
   }
   
   if(!list_id)
   {
      int color[3] = {0,0,0};
      list_id = glGenLists(1);
      DrawingList new_DL(true, 0,  list_id, color,  "edge" );
      DL_Model.push_front(new_DL);
   }

   std::deque<EdgeElem*> edgelist;

  EdgeTool::instance()->exterior_edges(edgelist);
  
  glNewList(list_id, GL_COMPILE);

  for(std::deque<EdgeElem*>::iterator iter = edgelist.begin(); iter != edgelist.end(); ++iter )
    add(*iter);

  glEndList();

}


void DrawingTool::add(QuadElem* elem)
{
  
  VerdeVector node;
  VerdeVector normal = elem->normal();
  glBegin(GL_POLYGON);
  glNormal3d(normal.x(), normal.y(), normal.z());
  
  for(int i=0; i<4; i++)
  {
    node = elem->coordinates(i);
    glVertex3d(node.x(), node.y(), node.z());
  }
  glEnd();
  
}

void DrawingTool::add(TriElem* elem)
{
  VerdeVector node;
  VerdeVector normal = elem->normal();
  
  glBegin(GL_POLYGON);
  
  glNormal3d(normal.x(), normal.y(), normal.z());
  
  for(int i=0; i<3; i++)
  {
    node = elem->coordinates(i);
    glVertex3d(node.x(), node.y(), node.z());
  }
  glEnd();
  

}


void DrawingTool::add(EdgeElem* elem)
{
  VerdeVector node1 = elem->coordinates(0);
  VerdeVector node2 = elem->coordinates(1);
  
  glBegin(GL_LINES);
    glVertex3d(node1.x(), node1.y(), node1.z());
    glVertex3d(node2.x(), node2.y(), node2.z());
  glEnd();

}

void DrawingTool::calc_boundary(VerdeVector vec, int temp)
{
  double val[3] = {vec.x(), vec.y(), vec.z() };
 
  if(temp)
  {
    if(temp_boundary_box_init == true)
    {
     if(val[0] < temp_boundary[0][0])
        temp_boundary[0][0] = val[0];
      if(val[0] > temp_boundary[0][1])
        temp_boundary[0][1] = val[0];
    
      if(val[1] < temp_boundary[1][0])
        temp_boundary[1][0] = val[1];
      if(val[1] > temp_boundary[1][1])
        temp_boundary[1][1] = val[1];
    
      if(val[2] < temp_boundary[2][0])
        temp_boundary[2][0] = val[2];
      if(val[2] > temp_boundary[2][1])
        temp_boundary[2][1] = val[2];
    }
    else
    {
      temp_boundary_box_init = true;
      temp_boundary[0][0] = val[0];
      temp_boundary[0][1] = val[0];
      temp_boundary[1][0] = val[1];
      temp_boundary[1][1] = val[1];
      temp_boundary[2][0] = val[2];
      temp_boundary[2][1] = val[2];
    }
  }
  else
  { 
    if(boundary_box_init == true)
    {
      if(val[0] < boundary[0][0])
        boundary[0][0] = val[0];
      if(val[0] > boundary[0][1])
        boundary[0][1] = val[0];
    
      if(val[1] < boundary[1][0])
        boundary[1][0] = val[1];
      if(val[1] > boundary[1][1])
        boundary[1][1] = val[1];
    
      if(val[2] < boundary[2][0])
        boundary[2][0] = val[2];
      if(val[2] > boundary[2][1])
        boundary[2][1] = val[2];
    }
    else
    {
      boundary_box_init = true;
      boundary[0][0] = val[0];
      boundary[0][1] = val[0];
      boundary[1][0] = val[1];
      boundary[1][1] = val[1];
      boundary[2][0] = val[2];
      boundary[2][1] = val[2];
    }
  }
}

void DrawingTool::reset_boundary(int temp)
{

  if(temp)
  {
    for(int j=0; j<3; j++)
      for(int k=0; k<2;k++)
      {
        temp_boundary[j][k] = 0;
      }
    temp_boundary_box_init = false;
  } 
  else
  {
    for(int j=0; j<3; j++)
      for(int k=0; k<2;k++)
      {
        boundary[j][k] = 0;
      }
    boundary_box_init = false;
  }
}



void DrawingTool::clear_highlighted()
{

  PickTool::instance()->reset();
   
  // find "surface" drawing list in DL_Model list
  int list_id = 0;
  std::deque<DrawingList>::iterator DL_iterator;
    for(DL_iterator = DL_FailedElements.begin(); DL_iterator != DL_FailedElements.end(); ++DL_iterator)
      if(strcmp( (*DL_iterator).name.c_str(), "failed") == 0 )
         list_id = (*DL_iterator).drawing_list_id;

   if(list_id) 
   {
     // just empty the drawing list 
     glNewList(list_id, GL_COMPILE);
     glEndList();
   }

  // if labels are off, get out
  if(!label_failed_toggle) 
    return;

  list_id = 0;
  std::deque<DrawingList>::iterator it_labels;
  // find the name in DrawingList that matches "label failed"
  it_labels = std::find_if(DL_Util.begin(), 
      DL_Util.end(), name_equals("label failed"));
  // if we found it, grab the drawing list id
  if(it_labels != DL_Util.end())
  {
      list_id = (*it_labels).drawing_list_id;
  }

  if(!list_id) 
    return;
 
  // just empty the drawing list
  glNewList(list_id, GL_COMPILE);
  glEndList();
}


void DrawingTool::create_node_labels(std::deque<QuadElem*>& quadlist,
                                     std::deque<TriElem*>& trilist,
                                     std::deque<EdgeElem*>& edgelist)
{
  int list_id;

  // Create node labels first
  if( DL_Node_Labels.drawing_list_id == 0 )
  {
    list_id = glGenLists(1);
    DL_Node_Labels.drawing_list_id = list_id;
    DL_Node_Labels.color[0] = 255; 
    DL_Node_Labels.color[1] = 255; 
    DL_Node_Labels.color[2] = 255; 
  }
  else
    list_id = DL_Node_Labels.drawing_list_id;

  std::vector<int> node_labels;
 
  for(std::deque<QuadElem*>::iterator q_iter = quadlist.begin(); q_iter != quadlist.end(); ++q_iter)
  {
    for(int i=0; i<4; i++)
    {
      int node_id = (*q_iter)->node_id(i);
      node_labels.push_back( node_id );
    }
  }

  for(std::deque<TriElem*>::iterator t_iter = trilist.begin(); t_iter != trilist.end(); ++t_iter )
  {
    for(int i=0; i<3; i++)
    {
      int node_id = (*t_iter)->node_id(i);
      node_labels.push_back( node_id );
    }

  }

  for(std::deque<EdgeElem*>::iterator e_iter = edgelist.begin(); e_iter != edgelist.end(); ++e_iter )
  {
    for(int i=0; i<2; i++)
    {
      int node_id = (*e_iter)->node_id(i);
      node_labels.push_back( node_id );
    }
  }

  // shrink the list down to just what we need
  std::sort(node_labels.begin(), node_labels.end());
  std::unique(node_labels.begin(), node_labels.end());

  // now make the list
  glNewList(list_id, GL_COMPILE);

  const std::vector<int>::const_iterator iter_end = node_labels.end();
  for(std::vector<int>::const_iterator iter = node_labels.begin(); iter != iter_end; ++iter)
  {
    NodeRef node(*iter);
    VerdeVector coords = node.coordinates();
    glRasterPos3d(coords.x(), coords.y(), coords.z());
    textTool.printGLf ("%d", *iter );
  }
  glEndList();

}




void DrawingTool::create_element_labels(std::deque<QuadElem*>& quadlist,
                                        std::deque<TriElem*>& trilist,
                                        std::deque<EdgeElem*>& edgelist)
{
  int list_id;

  // Create element labels

  // first time through, get an id to build a new list 
  if( DL_Element_Labels.drawing_list_id == 0 ) 
  {
    list_id = glGenLists(1);
    DL_Element_Labels.drawing_list_id = list_id;
    DL_Element_Labels.color[0] = 255; 
    DL_Element_Labels.color[1] = 255; 
    DL_Element_Labels.color[2] = 255; 
  }
  else
    list_id = DL_Element_Labels.drawing_list_id;


  QuadElem* quad;
  TriElem* tri;
  EdgeElem* edge;

  glNewList(list_id, GL_COMPILE);
  
  //Create drawing list of node labels
  for(std::deque<QuadElem*>::iterator q_iter = quadlist.begin(); q_iter != quadlist.end(); ++q_iter )
  {
    quad = *q_iter;
    
    //determine centroid of element
    VerdeVector centroid;
    for(int i=0; i<4; i++)
    {
      VerdeVector temp_vec = quad->coordinates(i);
      centroid += temp_vec;
    }    
    centroid /= 4.0;
    
    // get the quad or owner's id
    int id = quad->owner_id();
    
    //! Note that glRasterPos* uses the world coordinates
    glRasterPos3d(centroid.x(), centroid.y(), centroid.z());
    textTool.printGLf ("%d", id );

  }

  for(std::deque<TriElem*>::iterator t_iter = trilist.begin(); t_iter != trilist.end(); ++t_iter )
  {
    tri = *t_iter;

    //determine centroid of element
    VerdeVector centroid;
    for(int i=0; i<3; i++)
    {
      VerdeVector temp_vec = tri->coordinates(i);
      centroid += temp_vec;
    }    
    centroid /= 3.0;
  
    // get the quad or owner's id
    int id = tri->owner_id();       

    //! Note that glRasterPos* uses the world coordinates
    glRasterPos3d(centroid.x(), centroid.y(), centroid.z());
    textTool.printGLf ("%d", id );
  }

  for(std::deque<EdgeElem*>::iterator e_iter = edgelist.begin(); e_iter != edgelist.end(); ++e_iter )
  {
    edge = *e_iter;

    //determine centroid of element
    VerdeVector centroid;
    for(int i=0; i<2; i++)
    {
      VerdeVector temp_vec = edge->coordinates(i);
      centroid += temp_vec;
    }    
    centroid /= 2.0;
  
    // get the quad or owner's id
    int id = edge->owner_id();       

     //! Note that glRasterPos* uses the world coordinates
    glRasterPos3d(centroid.x(), centroid.y(), centroid.z());
    textTool.printGLf ("%d", id );

  }

  glEndList();

}

void DrawingTool::create_labels(std::deque<FailedHex*>& failed_hexes)
{
  // find "label" drawing list in DL_FailedElements list
  // if the list exists, reuse it else create a new one
  
  int list_id = 0;

  std::deque<DrawingList>::iterator it_labels;
  // find the name in DrawingList that matches "failed"
  it_labels = std::find_if(DL_Util.begin(), 
      DL_Util.end(), name_equals("label failed"));

  // if we found it, grab the drawing list id
  if(it_labels != DL_Util.end())
  {
      list_id = (*it_labels).drawing_list_id;
  }
  else // create a new drawing list
  {
    int color[3] = {1,1,1};
    list_id = glGenLists(1);
    DrawingList new_DL(true, 0,  list_id, color,  "label failed" );
    DL_Util.push_front(new_DL);
  }

  glNewList(list_id, GL_COMPILE);

  for(unsigned int i=0; i<failed_hexes.size(); i++ )
  {
    FailedHex* hex = failed_hexes[i];

    VerdeVector coord1, coord2, position;
  
    coord1 = verde_app->mesh()->coordinates(hex->node_id(0));
    coord2 = verde_app->mesh()->coordinates(hex->node_id(6));

    // Compute the mid point
    position = (coord1 + coord2)/2.0;
    //! Note that glRasterPos* uses the world coordinates
    glRasterPos3d(position.x(), position.y(), position.z());
    textTool.printGLf ("%d", hex->index() );
  }
  glEndList();
}

void DrawingTool::create_labels(std::deque<FailedTet*>& failed_tets)
{
  // find "label" drawing list in DL_Util list
  // if the list exists, reuse it else create a new one
  
  int list_id = 0;

  std::deque<DrawingList>::iterator it_labels;
  // find the name in DrawingList that matches "failed"
  it_labels = std::find_if(DL_Util.begin(), 
      DL_Util.end(), name_equals("label failed"));

  // if we found it, grab the drawing list id
  if(it_labels != DL_Util.end())
  {
      list_id = (*it_labels).drawing_list_id;
  }
  else // create a new drawing list
  {
    int color[3] = {1,1,1};
    list_id = glGenLists(1);
    DrawingList new_DL(true, 0,  list_id, color,  "label failed" );
    DL_Util.push_front(new_DL);
  }

  glNewList(list_id, GL_COMPILE);

  for(unsigned int i=0; i<failed_tets.size(); i++ )
  {
    FailedTet* tet = failed_tets[i];

    VerdeVector coord0, coord1, coord2, coord3, position;
  
    // Grab the opposite corners of the hex
    coord0 = verde_app->mesh()->coordinates(tet->node_id(0));
    coord1 = verde_app->mesh()->coordinates(tet->node_id(1));
    coord2 = verde_app->mesh()->coordinates(tet->node_id(2));
    coord3 = verde_app->mesh()->coordinates(tet->node_id(3));

    // Compute the mid point
    position = (coord0 + coord1 + coord2 + coord3)/4.0;

      //! Note that glRasterPos* uses the world coordinates
      glRasterPos3d(position.x(), position.y(), position.z());
      textTool.printGLf("%d", tet->index());
  }
  glEndList();
}

void DrawingTool::create_labels(std::deque<FailedQuad*>& failed_quads)
{
  // find "label" drawing list in DL_Util list
  // if the list exists, reuse it else create a new one
  
  int list_id = 0;

  std::deque<DrawingList>::iterator it_labels;
  // find the name in DrawingList that matches "failed"
  it_labels = std::find_if(DL_Util.begin(), 
      DL_Util.end(), name_equals("label failed"));

  // if we found it, grab the drawing list id
  if(it_labels != DL_Util.end())
  {
      list_id = (*it_labels).drawing_list_id;
  }
  else // create a new drawing list
  {
    int color[3] = {1,1,1};
    list_id = glGenLists(1);
    DrawingList new_DL(true, 0,  list_id, color,  "label failed" );
    DL_Util.push_front(new_DL);
  }

  glNewList(list_id, GL_COMPILE);

  for(unsigned int i=0; i<failed_quads.size(); i++ )
  {
    FailedQuad* quad = failed_quads[i];

    VerdeVector coord0, coord1, position;
  
    // Grab the opposite corners of the hex
    coord0 = verde_app->mesh()->coordinates(quad->node_id(0));
    coord1 = verde_app->mesh()->coordinates(quad->node_id(2));

    // Compute the mid point
    position = (coord0 + coord1)/2.0;
    //! Note that glRasterPos* uses the world coordinates
    glRasterPos3d(position.x(), position.y(), position.z());
    textTool.printGLf("%d", quad->index());

  }
  glEndList();
}


void DrawingTool::create_labels(std::deque<FailedTri*>& failed_tris)
{
  // find "label" drawing list in DL_Util list
  // if the list exists, reuse it else create a new one
  
  int list_id = 0;

  std::deque<DrawingList>::iterator it_labels;
  // find the name in DrawingList that matches "failed"
  it_labels = std::find_if(DL_Util.begin(), 
      DL_Util.end(), name_equals("label failed"));

  // if we found it, grab the drawing list id
  if(it_labels != DL_Util.end())
  {
      list_id = (*it_labels).drawing_list_id;
  }
  else // create a new drawing list
  {
    int color[3] = {1,1,1};
    list_id = glGenLists(1);
    DrawingList new_DL(true, 0,  list_id, color,  "label failed" );
    DL_Util.push_front(new_DL);
  }

  glNewList(list_id, GL_COMPILE);

  for(unsigned int i=0; i<failed_tris.size(); i++ )
  {
    FailedTri* tri = failed_tris[i];

    VerdeVector coord0, coord1, coord2, position;
  
    // Grab the nodal coordinates
    coord0 = verde_app->mesh()->coordinates(tri->node_id(0));
    coord1 = verde_app->mesh()->coordinates(tri->node_id(1));
    coord2 = verde_app->mesh()->coordinates(tri->node_id(2));

    // Compute the mid point
    position = (coord0 + coord1 + coord2)/3.0;

      //! Note that glRasterPos* uses the world coordinates
      glRasterPos3d(position.x(), position.y(), position.z());
      textTool.printGLf("%d", tri->index());
  }
  glEndList();
}

void DrawingTool::create_labels(std::deque<FailedEdge*>& failed_edge)
{
  // find "label" drawing list in DL_Util list
  // if the list exists, reuse it else create a new one
  
  int list_id = 0;

  std::deque<DrawingList>::iterator it_labels;
  // find the name in DrawingList that matches "failed"
  it_labels = std::find_if(DL_Util.begin(), 
      DL_Util.end(), name_equals("label failed"));

  // if we found it, grab the drawing list id
  if(it_labels != DL_Util.end())
  {
      list_id = (*it_labels).drawing_list_id;
  }
  else // create a new drawing list
  {
    int color[3] = {1,1,1};
    list_id = glGenLists(1);
    DrawingList new_DL(true, 0,  list_id, color,  "label failed" );
    DL_Util.push_front(new_DL);
  }

  glNewList(list_id, GL_COMPILE);

  for(unsigned int i=0; i<failed_edge.size(); i++ )
  {
    FailedEdge* edge = failed_edge[i];

    VerdeVector coord0, coord1, position;
  
    // Grab the nodal coordinates
    coord0 = verde_app->mesh()->coordinates(edge->node_id(0));
    coord1 = verde_app->mesh()->coordinates(edge->node_id(1));

    // Compute the mid point
    position = (coord0 + coord1)/2.0;

      //! Note that glRasterPos* uses the world coordinates
      glRasterPos3d(position.x(), position.y(), position.z());
      textTool.printGLf("%d", edge->index());
  }
  glEndList();
}

void DrawingTool::highlight(std::deque<FailedHex*>& failed_hexes)
{
  VerdeVector tmp_coord;
  double coords[8][3];
  unsigned int i,j;
  FailedHex* hex;

  // find "failed" drawing list in DL_FailedElements list
  int list_id = 0;
  std::deque<DrawingList>::iterator DL_iterator;

  for(DL_iterator = DL_FailedElements.begin(); 
      DL_iterator != DL_FailedElements.end(); 
      ++DL_iterator)
  {
      if(strcmp( (*DL_iterator).name.c_str(), "failed") == 0 )
      {
         list_id = (*DL_iterator).drawing_list_id;
      } 
  }

  if(!list_id)
  {
    int color[3] = {0,0,0};
    list_id = glGenLists(1);
    DrawingList new_DL(true, 0, list_id, color,  "failed" );
    DL_FailedElements.push_front(new_DL);
  }

  glNewList(list_id, GL_COMPILE);
   
  glEnableClientState(GL_VERTEX_ARRAY);

  //enable filtering of hex types
  glPushName(PickTool::P_HEX);
  glPushName(0);
    
  for(i=0; i<failed_hexes.size(); i++ )
  {
    hex = failed_hexes[i];
    for(j=0; j<8; j++)
    {
      tmp_coord = verde_app->mesh()->coordinates(hex->node_id(j));
      coords[j][0] = tmp_coord.x();
      coords[j][1] = tmp_coord.y();
      coords[j][2] = tmp_coord.z();
    }
  
    // set the hex id for picking
    glLoadName( hex->index() );

    add_hex(coords);
  }

  glDisableClientState(GL_VERTEX_ARRAY);

  //disable filter for hex types
  glPopName();
  glPopName();
    
  glEndList();

  //! Create the label list everytime, but don't always display it!
  create_labels( failed_hexes );
}

void DrawingTool::highlight(std::deque<FailedTet*>& failed_tets)
{
  VerdeVector tmp_coord;
  double coords[4][3];
  unsigned int i,j;
  FailedTet* tet;
  
  // find "failed" drawing list in DL_FailedElements list
  int list_id = 0;
  std::deque<DrawingList>::iterator DL_iterator;
  for(DL_iterator = DL_FailedElements.begin(); DL_iterator != DL_FailedElements.end(); ++DL_iterator)
    if(strcmp( (*DL_iterator).name.c_str(), "failed") == 0 )
      list_id = (*DL_iterator).drawing_list_id;
  
  if(!list_id)   {
    int color[3] = {0,0,0};
    list_id = glGenLists(1);
    DrawingList new_DL(true, 0,  list_id, color,  "failed" );
    DL_FailedElements.push_front(new_DL);
  }
  
  glNewList(list_id, GL_COMPILE);
  
    // enable filtering for tet types
    glPushName(PickTool::P_TET);
    glPushName(0);
        
    glEnableClientState(GL_VERTEX_ARRAY);
    
    for(i=0; i<failed_tets.size(); i++ )
    {
      tet = failed_tets[i];
      for(j=0; j<4; j++)
      {
        tmp_coord = verde_app->mesh()->coordinates(tet->node_id(j));
        coords[j][0] = tmp_coord.x();
        coords[j][1] = tmp_coord.y();
        coords[j][2] = tmp_coord.z();
      }
      
      // set the tet id for picking
      glLoadName( tet->index() );

      // add the tet to the drawing list
      add_tet(coords);
    }
    glDisableClientState(GL_VERTEX_ARRAY);
    // disable filter for tet types
    glPopName();
    glPopName();
  glEndList();

  //! Create the label list everytime, but don't always display it!
  create_labels( failed_tets );
}

void DrawingTool::highlight(std::deque<FailedQuad*>& failed_quads)
{
    VerdeVector tmp_coord;
    double coords[4][3];
    unsigned int i,j;
    FailedQuad* quad;

  // find "failed" drawing list in DL_FailedElements list
  int list_id = 0;
  std::deque<DrawingList>::iterator DL_iterator;
    for(DL_iterator = DL_FailedElements.begin(); DL_iterator != DL_FailedElements.end(); ++DL_iterator)
      if(strcmp( (*DL_iterator).name.c_str(), "failed") == 0 )
         list_id = (*DL_iterator).drawing_list_id;
   
   if(!list_id)
   {
      int color[3] = {0,0,0};
      list_id = glGenLists(1);
      DrawingList new_DL(true, 0,  list_id, color,  "failed" );
      DL_FailedElements.push_front(new_DL);
   }

   glNewList(list_id, GL_COMPILE);

   // enable filter for quad types
   glPushName(PickTool::P_QUAD);
   glPushName(0);
    
    
   for(i=0; i<failed_quads.size(); i++ )
   {
    quad = failed_quads[i];
    for(j=0; j<4; j++)
    {
      tmp_coord = verde_app->mesh()->coordinates(quad->node_id(j));
      coords[j][0] = tmp_coord.x();
      coords[j][1] = tmp_coord.y();
      coords[j][2] = tmp_coord.z();
    }

    // set the quad id for picking
    glLoadName( quad->index() );

    // add the quad to the drawing list 
    add_quad(coords);
   }
   // disable filter for quad types
   glPopName();
   glPopName();
   glEndList();

  //! Create the label list everytime, but don't always display it!
  create_labels( failed_quads );
}

void DrawingTool::highlight(std::deque<FailedTri*>& failed_tris)
{
   VerdeVector tmp_coord;
   double coords[3][3];
   unsigned int i,j;
   FailedTri* tri;
   
   // find "failed" drawing list in DL_FailedElements list
   int list_id = 0;
   std::deque<DrawingList>::iterator DL_iterator;
   for(DL_iterator = DL_FailedElements.begin(); DL_iterator != DL_FailedElements.end(); ++DL_iterator)
      if(strcmp( (*DL_iterator).name.c_str(), "failed") == 0 )
         list_id = (*DL_iterator).drawing_list_id;
   
   if(!list_id)
   {
      int color[3] = {0,0,0};
      list_id = glGenLists(1);
      DrawingList new_DL(true, 0,  list_id, color,  "failed" );
      DL_FailedElements.push_front(new_DL);
   }
   
   glNewList(list_id, GL_COMPILE);
   
      // enable filter for tris
      glPushName(PickTool::P_TRI);
      glPushName(0);
      
      for(i=0; i< failed_tris.size(); i++ )
      {
         tri = failed_tris[i];
         for(j=0; j<3; j++)
         {
            tmp_coord = verde_app->mesh()->coordinates(tri->node_id(j));
            tmp_coord.get_xyz(coords[j]);
         }
         
         // set the tri id for picking
         glLoadName( tri->index() );
         
         // add the tri to the drawing list
         add_tri(coords);
      }
      

      // disable filter for tris
      glPopName();
      glPopName();
   
   glEndList();

  //! Create the label list everytime, but don't always display it!
  create_labels( failed_tris );
}

void DrawingTool::highlight(std::deque<FailedEdge*>& edge_list)
{
   // find "failed" drawing list in DL_FailedElements list
   int list_id = 0;
   std::deque<DrawingList>::iterator DL_iterator;
   for(DL_iterator = DL_FailedElements.begin(); DL_iterator != DL_FailedElements.end(); ++DL_iterator)
      if(strcmp( (*DL_iterator).name.c_str(), "failed") == 0 )
         list_id = (*DL_iterator).drawing_list_id;
   
   if(!list_id)
   {
      int color[3] = {0,0,0};
      list_id = glGenLists(1);
      DrawingList new_DL(true, 0,  list_id, color,  "failed" );
      DL_FailedElements.push_front(new_DL);
   }
   
   glNewList(list_id, GL_COMPILE);

      //glPushName(PickTool::P_EDGE);
      //glPushName(0);
   
      EdgeElem* curr_edge;
      for(unsigned int i = 0; i < edge_list.size(); i++ )
      {
         curr_edge = edge_list[i];
        // set the edge id for picking
        //glLoadName( curr_edge->index() );
        // add the edge to the drawing list
        add(curr_edge);
      }

      //glPopName();
      //glPopName();  
      
   glEndList();

  //! Create the label list everytime, but don't always display it!
  create_labels( edge_list );
}

void DrawingTool::highlight(std::deque<QuadElem*>& coincident_quads)
{
   VerdeVector tmp_coord;
   double coords[4][3];
   unsigned int i,j;
   QuadElem* quad;
   
   // find "failed" drawing list in DL_FailedElements list
   int list_id = 0;
   std::deque<DrawingList>::iterator DL_iterator;
   for(DL_iterator = DL_FailedElements.begin(); DL_iterator != DL_FailedElements.end(); ++DL_iterator)
      if(strcmp( (*DL_iterator).name.c_str(), "failed") == 0 )
         list_id = (*DL_iterator).drawing_list_id;
   
   if(!list_id)
   {
      int color[3] = {0,0,0};
      list_id = glGenLists(1);
      DrawingList new_DL(true, 0,  list_id, color,  "failed" );
      DL_FailedElements.push_front(new_DL);
   }
   
   glNewList(list_id, GL_COMPILE);
   
      glPushName(PickTool::P_HEX);
      glPushName(0);
      
      for(i=0; i<coincident_quads.size(); i++ )
      {
         quad = coincident_quads[i];
         for(j=0; j<4; j++)
         {
            tmp_coord = verde_app->mesh()->coordinates(quad->node_id(j));
            coords[j][0] = tmp_coord.x();
            coords[j][1] = tmp_coord.y();
            coords[j][2] = tmp_coord.z();
         }
         // set the quad id for picking
         // index doesn't exist yet
         glLoadName( quad->owner_id() );
         // add the quad to the drawing list         
         add_quad(coords);
      }

      glPopName();
      glPopName();

   
   glEndList();
   
}



void DrawingTool::highlight(std::deque<TriElem*>& tri_list)
{
   VerdeVector tmp_coord;
   double coords[3][3];
   unsigned int i,j;
   TriElem* tri;
    
   // find "failed" drawing list in DL_FailedElements list
   int list_id = 0;
   std::deque<DrawingList>::iterator DL_iterator;
   for(DL_iterator = DL_FailedElements.begin(); DL_iterator != DL_FailedElements.end(); ++DL_iterator)
      if(strcmp( (*DL_iterator).name.c_str(), "failed") == 0 )
         list_id = (*DL_iterator).drawing_list_id;
   
   if(!list_id)
   {
      int color[3] = {0,0,0};
      list_id = glGenLists(1);
      DrawingList new_DL(true, 0,  list_id, color,  "failed" );
      DL_FailedElements.push_front(new_DL);
   }
   
   glNewList(list_id, GL_COMPILE);
      
      for(i=0; i < tri_list.size(); i++ )
      {
         tri = tri_list[i];
         for(j=0; j<3; j++)
         {
            tmp_coord = verde_app->mesh()->coordinates(tri->node_id(j));
            coords[j][0] = tmp_coord.x();
            coords[j][1] = tmp_coord.y();
            coords[j][2] = tmp_coord.z();
         }
         // set the tri id for picking
         // index doesn't exit yet
         //glLoadName( tri->index() );
         // add the tri to the drawing list         
         add_tri(coords);
      }
   glEndList();

}

void DrawingTool::highlight( std::deque<int> &node_ids )
{
   VerdeVector tmp_coord;
   double coords[3];
   unsigned int i;
   
   // find "failed" drawing list in DL_FailedElements list
   int list_id = 0;
   std::deque<DrawingList>::iterator DL_iterator;
   for(DL_iterator = DL_FailedElements.begin(); DL_iterator != DL_FailedElements.end(); ++DL_iterator)
      if(strcmp( (*DL_iterator).name.c_str(), "failed") == 0 )
         list_id = (*DL_iterator).drawing_list_id;
   
   if(!list_id)
   {
      int color[3] = {0,0,0};
      list_id = glGenLists(1);
      DrawingList new_DL(true, 0,  list_id, color,  "failed" );
      DL_FailedElements.push_front(new_DL);
   }
   
   glNewList(list_id, GL_COMPILE);
   
      glPushName(PickTool::P_NODE);
      glPushName(0);
      for(i=0; i<node_ids.size(); i++)
      {
         NodeRef node_ref( node_ids[i] );
         tmp_coord = node_ref.coordinates();
         coords[0] = tmp_coord.x();
         coords[1] = tmp_coord.y();
         coords[2] = tmp_coord.z();

         // set the node if for picking
         glLoadName( node_ref.index() );
         glBegin(GL_POINTS);

         // add the node to the drawing list 
         glVertex3d(tmp_coord.x(), tmp_coord.y(), tmp_coord.z() );
         glEnd();
      }

      glPopName();
      glPopName();
   
   glEndList();

}

void DrawingTool::highlight(std::deque<EdgeElem*>& edge_list)
{
   
   // find "failed" drawing list in DL_FailedElements list
   int list_id = 0;
   std::deque<DrawingList>::iterator DL_iterator;
   for(DL_iterator = DL_FailedElements.begin(); DL_iterator != DL_FailedElements.end(); ++DL_iterator)
      if(strcmp( (*DL_iterator).name.c_str(), "failed") == 0 )
         list_id = (*DL_iterator).drawing_list_id;
   
   if(!list_id)
   {
      int color[3] = {0,0,0};
      list_id = glGenLists(1);
      DrawingList new_DL(true, 0,  list_id, color,  "failed" );
      DL_FailedElements.push_front(new_DL);
   }
   
   glNewList(list_id, GL_COMPILE);
   
      glBegin(GL_LINES);
      
      unsigned int i;
      EdgeElem* curr_edge;
      for(i=0; i<edge_list.size(); i++)
      {
         curr_edge = edge_list[i];
         VerdeVector beg = curr_edge->coordinates(0);
         VerdeVector end = curr_edge->coordinates(1);
         // set the edge id for picking
         // index doesn't exist yet
         //glLoadName( curr_edge->index() );
         // add the edge to the drawing list
         glVertex3d(beg.x(), beg.y(), beg.z());
         glVertex3d(end.x(), end.y(), end.z());
      }
      glEnd();
   

   glEndList();
   
}


void DrawingTool::get_bounding_box(double bound[3][2], int temp)
{
  if(temp)
  {
    for(int i=0; i<3; i++)
      for(int j=0; j<2; j++)
        bound[i][j] = temp_boundary[i][j];
  }
  else
  {
    for(int i=0; i<3; i++)
      for(int j=0; j<2; j++)
        bound[i][j] = boundary[i][j];
  }
}


void DrawingTool::add_NodeBC(int id)
{
  NodeBC* node_bc = verde_app->mesh()->get_node_bc(id);
  if(!node_bc) return;

  std::deque<int> node_bc_nodes;
  node_bc->node_id_list(node_bc_nodes);
  if(node_bc_nodes.size() == 0)
    return;
  
  // add the new DL to the list of DL's
  int color[3] = {255, 0, 255};
  int DL = glGenLists(1);
  DrawingList new_list(true, id, DL, color, NULL);
  DL_NodeBC.push_front(new_list);

  unsigned int i;

  glNewList(DL, GL_COMPILE);
    glBegin(GL_POINTS);
    for(i=0; i<node_bc_nodes.size(); i++)
    {
      NodeRef node(node_bc_nodes[i]);
      VerdeVector coords = node.coordinates();
      glVertex3d(coords.x(), coords.y(), coords.z());      
      calc_boundary(node.coordinates());
    }
    glEnd();
  glEndList();


  int list_id = 0;
  // add the new DL to the list of DL's
  color[0] = 255; 
  color[1] = 255; 
  color[2] = 255; 
  list_id = glGenLists(1);
  DrawingList list(true, id, list_id, color, NULL);
  DL_NodeBC_Labels.push_front(list);


  glNewList(list_id, GL_COMPILE);

  for(i=0; i<node_bc_nodes.size(); i++)
  {
    NodeRef node(node_bc_nodes[i]);
    VerdeVector position = node.coordinates();
    
    //! Note that glRasterPos* uses the world coordinates
    glRasterPos3d(position.x(), position.y(), position.z());

    //Do we need to display the exodus id also???
    textTool.printGLf ("%d", id );
  }
  glEndList();

}


void DrawingTool::remove_NodeBC(int id)
{
  // find in the DL_NodeBC a DrawingList that matches the id
  std::deque<DrawingList>::iterator curr_drawlist =
    std::find_if(DL_NodeBC.begin(), DL_NodeBC.end(), test_Verde_ID_equals(id));

  // if we found it, delete the OpenGL drawing list and remove from the list
  if(curr_drawlist != DL_NodeBC.end())
  {
    glDeleteLists((*curr_drawlist).drawing_list_id, 1);
    DL_NodeBC.erase(curr_drawlist);
  }

  // find in the DL_NodeBC_Labels a DrawingList that matches the id
  curr_drawlist = std::find_if(DL_NodeBC_Labels.begin(), 
                               DL_NodeBC_Labels.end(), 
                               test_Verde_ID_equals(id));

  // if we found it, delete the OpenGL drawing list and remove from the list
  if(curr_drawlist != DL_NodeBC_Labels.end())
  {
    glDeleteLists((*curr_drawlist).drawing_list_id, 1);
    DL_NodeBC_Labels.erase(curr_drawlist);
  }

}


void DrawingTool::add_ElementBC(int id)
{
  ElementBC* element_bc = verde_app->mesh()->get_element_bc(id);
  if(!element_bc) return;
  std::deque<QuadElem*> element_bc_quads;
  std::deque<TriElem*> element_bc_tris;
  std::deque<EdgeElem*> element_bc_edges;

  // get the elements to draw
  element_bc->element_lists(element_bc_quads, element_bc_tris,
      element_bc_edges);

  int color[3] = {255, 0, 255};
  int DL = glGenLists(1);
  DrawingList new_list(true, id, DL, color, NULL);
  DL_ElementBC.push_front(new_list);

  QuadElem* curr_quad = NULL;
  TriElem* curr_tri = NULL;
  EdgeElem* curr_edge = NULL;
  double quad_array[4][3];
  double tri_array[3][3];
  double edge_array[2][3];
  
  glNewList(DL, GL_COMPILE);
  {
    unsigned int i;
    for(i=0; i<element_bc_quads.size(); i++)
    {
      curr_quad = element_bc_quads[i];
      shrink(curr_quad, 0.7, quad_array);
      add_quad(quad_array);
      calc_boundary(curr_quad->coordinates(0));
    }
    
    for(i=0; i<element_bc_tris.size(); i++)
    {
      curr_tri = element_bc_tris[i];
      shrink(curr_tri, 0.7, tri_array);
      add_tri(tri_array);
      calc_boundary(curr_tri->coordinates(0));
    }

    for(i=0; i<element_bc_edges.size(); i++)
    {
      curr_edge = element_bc_edges[i];
      shrink(curr_edge, 0.7, edge_array);
      add_edge(edge_array);
      //add(curr_edge);
      calc_boundary(curr_edge->coordinates(0));
    }
  }
  glEndList();

  // find "label sideset" drawing list in DL_Util list
  // if the list exists, reuse it else create a new one
  

  // add the new DL to the list of DL's
  color[0] = 255; 
  color[1] = 255; 
  color[2] = 255; 
  int list_id = glGenLists(1);
 
  DrawingList my_list(true, id, list_id, color, NULL);
  DL_ElementBC_Labels.push_front(my_list);


  glNewList(list_id, GL_COMPILE);
  unsigned int i;

  for(i=0; i<element_bc_quads.size(); i++)
  {
    curr_quad = element_bc_quads[i];

    VerdeVector coord0, coord1, diag, position;
  
    // Grab the opposite corners of the hex
    coord0 = curr_quad->coordinates(0);
    coord1 = curr_quad->coordinates(2);

    // Compute the mid point
    position = (coord0 + coord1)/2.0;
   
    // Push id away a little, so it's not right on the surface 
    diag = coord1 - coord0;
    position += 0.1*(diag.length())*(curr_quad->normal());

      //! Note that glRasterPos* uses the world coordinates
      glRasterPos3d(position.x(), position.y(), position.z());
      textTool.printGLf("%d", id);
  }
   

  for(i=0; i<element_bc_tris.size(); i++)
  {
    curr_tri = element_bc_tris[i];

    VerdeVector coord0, coord1, coord2, side, position;
  
    // Grab the nodal coordinates
    coord0 = curr_tri->coordinates(0);
    coord1 = curr_tri->coordinates(1);
    coord2 = curr_tri->coordinates(2);

    // Compute the mid point
    position = (coord0 + coord1 + coord2)/3.0;
    
    // Push id away a little, so it's not right on the surface 
    side = coord0 - coord1;
    side = 0.1*side.length()*(curr_tri->normal());
    position += side;     
         
      //! Note that glRasterPos* uses the world coordinates
      glRasterPos3d(position.x(), position.y(), position.z());
      textTool.printGLf("%d", id );
  }


  for(i=0; i<element_bc_edges.size(); i++)
  {
    curr_edge = element_bc_edges[i];
      
    // Grad nodal coordinates
    VerdeVector coord0, coord1, position;
    coord0 = curr_edge->coordinates(0); 
    coord1 = curr_edge->coordinates(1); 

    // Compute the mid point
    position = (coord0 + coord1)/2.0;

      //! Note that glRasterPos* uses the world coordinates
      glRasterPos3d(position.x(), position.y(), position.z());
      textTool.printGLf("%d", id );

  }

  glEndList();

}

void DrawingTool::shrink(QuadElem* quad, double factor, double quad_array[4][3])
{
  // get the coordinates of the quad
  VerdeVector coords[4] = {quad->coordinates(0), quad->coordinates(1),
    quad->coordinates(2), quad->coordinates(3)};

  VerdeVector centriod;
  int i;
  for(i=0; i<4; i++)
    centriod += coords[i];
  centriod /= 4.0;

  for(i=0; i<4; i++)
  {
    VerdeVector new_pt = centriod + (coords[i] - centriod)*factor;
    new_pt.get_xyz(quad_array[i]);
  }
}

void DrawingTool::shrink(TriElem* tri, double factor, double tri_array[3][3])
{
  VerdeVector coords[3] = { tri->coordinates(0), tri->coordinates(1),
    tri->coordinates(2)};

  VerdeVector centriod;
  int i;
  for(i=0; i<3; i++)
    centriod += coords[i];
  centriod /= 3.0;

  for(i=0; i<3; i++)
  {
    VerdeVector new_pt = centriod + (coords[i] - centriod)*factor;
    new_pt.get_xyz(tri_array[i]);
  }
}


void DrawingTool::shrink(EdgeElem* edge, double factor, double edge_array[2][3])
{
  VerdeVector coords[2] = { edge->coordinates(0), edge->coordinates(1)};

  VerdeVector centroid = (coords[0] + coords[1]) / 2.0;

  VerdeVector new_pt = centroid + (coords[0] - centroid)*factor;
  new_pt.get_xyz(edge_array[0]);
  new_pt = centroid + (coords[1] - centroid)*factor; 
  new_pt.get_xyz(edge_array[1]);

}

void DrawingTool::remove_ElementBC(int id)
{
  // find in the DL_ElementBC a DrawingList that matches the id
  std::deque<DrawingList>::iterator curr_drawlist =
    std::find_if(DL_ElementBC.begin(), DL_ElementBC.end(), test_Verde_ID_equals(id));

  // if we found it, delete the OpenGL drawing list and remove from the list
  if(curr_drawlist != DL_ElementBC.end())
  {
    glDeleteLists((*curr_drawlist).drawing_list_id, 1);
    DL_ElementBC.erase(curr_drawlist);
  }

  // find in the DL_NodeBC_Labels a DrawingList that matches the id
  curr_drawlist = std::find_if(DL_ElementBC_Labels.begin(), 
                               DL_ElementBC_Labels.end(), 
                               test_Verde_ID_equals(id));

  // if we found it, delete the OpenGL drawing list and remove from the list
  if(curr_drawlist != DL_ElementBC_Labels.end())
  {
    glDeleteLists((*curr_drawlist).drawing_list_id, 1);
    DL_ElementBC_Labels.erase(curr_drawlist);
  }
}


void DrawingTool::add_surface_normals()
{

  // lists for the surface elements
   std::deque<QuadElem*> quadlist;
   std::deque<TriElem*> trilist;
  QuadElem* quad;
  TriElem* tri;


  // get exterior quads and tris
  SkinTool::exterior_quads(quadlist);
  SkinTool::exterior_tris(trilist);

  //Get value for scaling normal
  unsigned int i;
  int adds = 0;
  double scale = 0;
  VerdeVector temp_vec;

  if(quadlist.size())
  {
    if( quadlist.size() > 100 )
    {
      for(i=0; i<quadlist.size(); i+=10 )
      {
        //Get every 10th quad
        quad = quadlist[i];
        temp_vec = quad->coordinates(0) - quad->coordinates(2); 
        scale += temp_vec.length();
        adds++;
      }
    }
    else
    {
      for(i=0; i<quadlist.size(); i++)
      {
        quad = quadlist[i];
        temp_vec = quad->coordinates(0) - quad->coordinates(2); 
        scale += temp_vec.length();
        adds++;
      }
    }
    
    scale /= adds;
  }

  if(trilist.size())
  {
    if( trilist.size() > 100 )
    {
      for(i=0; i<trilist.size(); i+=10 )
      {
        //Get every 10th tri
        tri = trilist[i];
        temp_vec = tri->coordinates(0) - tri->coordinates(2); 
        scale += temp_vec.length();
        adds++;
      }
    }
    else
    {
      for(i=0; i<trilist.size(); i++)
      {
        tri = trilist[i];
        temp_vec = tri->coordinates(0) - tri->coordinates(2); 
        scale += temp_vec.length();
        adds++;
      }
    }
    
    scale /= adds;

  }

  // initialize to the number of normals we will have
  std::vector<VerdeVector> normals((quadlist.size()+trilist.size())*2);
  std::vector<VerdeVector> normal_tips((quadlist.size()+trilist.size())*2);
  
  unsigned int norm_index = 0;

    
    for(i=0; i<quadlist.size(); i++ )
    {
      quad = quadlist[i];
          
      //get quad normal
      VerdeVector normal = quad->normal();
      //normal.normalize();

      //get opposite nodes
      VerdeVector coords0 = quad->coordinates(0);
      VerdeVector coords2 = quad->coordinates(2);
      
      //approximate centroid where coords0 is now centroid
      coords0 += coords2;
      coords0 /= 2;
      
      //scale normal by scale
      normal *= scale;   

      VerdeVector midpoint = coords0 + normal;
      //fill up lists
      normals[norm_index] = coords0;
      normals[norm_index+1] = midpoint;

      normal_tips[norm_index] = midpoint;
      normal_tips[norm_index+1] = (midpoint + 0.2*normal);
      norm_index += 2;
 
    }
  
    for(i=0; i<trilist.size(); i++)
    {
      tri = trilist[i];
          
      //get tri normal
      VerdeVector normal = tri->normal();
      //normal.normalize();

      //get all nodes
      VerdeVector coords0 = tri->coordinates(0);
      VerdeVector coords1 = tri->coordinates(1);
      VerdeVector coords2 = tri->coordinates(2);
      
      //approximate centroid where coords0 is now centroid
      coords0 += coords1;
      coords0 += coords2;
      coords0 /= 3;
      
      //scale normal by scale
      normal *= scale;   
  
      VerdeVector midpoint = coords0 + normal;
      //fill up lists
      normals[norm_index] = coords0;
      normals[norm_index+1] = midpoint;

      normal_tips[norm_index] = midpoint;
      normal_tips[norm_index+1] = midpoint + (0.2*normal);
      norm_index += 2;

    }

   // find "normal" drawing list in DL_Model list
   // if the list exists, set visible flag to true and exit
   int list_id = 0;
   std::deque<DrawingList>::iterator DL_iterator;
   for(DL_iterator = DL_Model.begin(); DL_iterator != DL_Model.end() && list_id==0; ++DL_iterator)
   {      
      if(strcmp( (*DL_iterator).name.c_str(), "normal") == 0 )
      {
         list_id = (*DL_iterator).drawing_list_id;
      }
   }

   if(!list_id)
   {
      int color[3] = {0,0,0};
      list_id = glGenLists(1);
      DrawingList new_DL(true, 0,  list_id, color,  "normal" );
      DL_Model.push_front(new_DL);
   }


    unsigned int j;
  // put each normal into this list
  glNewList(list_id, GL_COMPILE);

    glColor3f(0.1f, 0.8f, 1.0f);
    glBegin(GL_LINES);
    // normals
    for(j=0; j<normals.size(); )
    {
      glVertex3f(normals[j].x(), normals[j].y(), normals[j].z() );
      j++;
      glVertex3f(normals[j].x(), normals[j].y(), normals[j].z() );
      j++;
    }
    glEnd();
  
    // tips
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    for(j=0; j<normal_tips.size(); )
    {
      glVertex3f(normal_tips[j].x(), normal_tips[j].y(), normal_tips[j].z() );
      j++;
      glVertex3f(normal_tips[j].x(), normal_tips[j].y(), normal_tips[j].z() );
      j++;
    }

   glEnd();

  glEndList();
}


void DrawingTool::toggle_normals_vis( bool flag )
{
   // turn normals off
  std::deque<DrawingList>::iterator DL_iterator;
   for(DL_iterator = DL_Model.begin(); DL_iterator != DL_Model.end(); ++DL_iterator)
   {
      if(strcmp( (*DL_iterator).name.c_str(), "normal") == 0 )
         (*DL_iterator).visible = flag; 
   }

   return;
}

void DrawingTool::toggle_node_labels( bool flag )
{ 
  label_node_toggle = flag; 
}

void DrawingTool::toggle_element_labels( bool flag )
{ 
  label_element_toggle = flag; 
}

void DrawingTool::toggle_failed_labels( bool flag )
{ 
  //Need to be able to toggle individual types.
  label_failed_toggle = flag; 
}

void DrawingTool::toggle_block_labels( bool flag )
{ 
  //Need to be able to toggle individual types.
  label_block_toggle = flag; 
}

void DrawingTool::toggle_nodeset_labels( bool flag )
{ 
  //Need to be able to toggle individual types.
  label_nodeset_toggle = flag; 
}

void DrawingTool::toggle_sideset_labels( bool flag )
{ 
  //Need to be able to toggle individual types.
  label_sideset_toggle = flag; 
}

void DrawingTool::toggle_skin_vis( bool flag)
{

   // turn skin off 
  std::deque<DrawingList>::iterator DL_iterator;
   for(DL_iterator = DL_Model.begin(); DL_iterator != DL_Model.end(); ++DL_iterator)
   {
      if(strcmp( (*DL_iterator).name.c_str(), "surface") == 0 )
         (*DL_iterator).visible = flag; 
   }

   return;

}


void DrawingTool::toggle_model_edges_vis( bool flag)
{
   // turn edges off 
  std::deque<DrawingList>::iterator DL_iterator;
   for(DL_iterator = DL_Model.begin(); DL_iterator != DL_Model.end(); ++DL_iterator)
   {
      if(strcmp( (*DL_iterator).name.c_str(), "edge") == 0 )
         (*DL_iterator).visible = flag; 
   }

   return;
}


void DrawingTool::clear_selected()
{
   if(DL_SelectedItems.drawing_list_id > 0)
   {
      glDeleteLists(DL_SelectedItems.drawing_list_id, 1);
      DL_SelectedItems.drawing_list_id = 0;
   }
}


// if which_item = 0 then highlight all entities in the list
// else highlight only id_list[which_item]

void DrawingTool::highlight_selected(int which_item, 
      const std::list<PickedEntity>& entity_list)
{
   if(DL_SelectedItems.drawing_list_id <= 0)
     DL_SelectedItems.drawing_list_id = glGenLists(1);

   //if( entity_list.size() == 0 )
   //   return;
   

   Mesh* mesh = verde_app->mesh();
   ElementBlock* block = 0;

   std::list<PickedEntity>::const_iterator IT_entity_list;

   glNewList(DL_SelectedItems.drawing_list_id, GL_COMPILE);

   int i = 0;
   for(IT_entity_list = entity_list.begin();
         IT_entity_list != entity_list.end();
         ++IT_entity_list )
   {
      i++;
      if(which_item > 0 && i != which_item)
         continue;
      else if( (block = mesh->find_block_for_element((*IT_entity_list).entity_id)) )
      {
         switch ((*IT_entity_list).picktype)
         {
            case PickTool::P_BLOCK:
               break;
               // we are dealing with hexes
            case PickTool::P_HEX:
               {
                  glEnableClientState(GL_VERTEX_ARRAY);
                  // access coordinates for hex and draw
                  HexRef hex(*(block->mesh_container()), (*IT_entity_list).entity_id);
                  GLdouble coords[8][3];
                  int z;
                  for(z=0; z<8; z++)
                  {
                     VerdeVector tmp = mesh->coordinates(hex.node_id(z));
                     tmp.get_xyz(coords[z]);
                  }
                  add_hex(coords);
                  glDisableClientState(GL_VERTEX_ARRAY);
                  break;
               }
            case PickTool::P_TET:
               {
                  glEnableClientState(GL_VERTEX_ARRAY);
                  // access coordinates for tet and draw it
                  TetRef tet(*(block->mesh_container()), (*IT_entity_list).entity_id);
                  GLdouble coords[4][3];
                  int z;
                  for(z=0; z<4; z++)
                  {
                     VerdeVector tmp = mesh->coordinates(tet.node_id(z));
                     tmp.get_xyz(coords[z]);
                  }
                  add_tet(coords);
                  glDisableClientState(GL_VERTEX_ARRAY);
                  break;
               }
            case PickTool::P_KNIFE:
               {
                  
                  break;
               }
            case PickTool::P_WEDGE:
               {
                  
                  break;
               }
            case PickTool::P_PYRAMID:
               {
                  
                  break;
               }
            case PickTool::P_QUAD:
               {
                  QuadRef quad(*(block->mesh_container()), (*IT_entity_list).entity_id);
                  GLdouble coords[4][3];
                  int z;
                  for(z=0; z<4; z++)
                  {
                     VerdeVector tmp = mesh->coordinates(quad.node_id(z));
                     tmp.get_xyz(coords[z]);
                  }
                  add_quad(coords);
                  break;
               }
            case PickTool::P_TRI:
               {
                  TriRef tri(*(block->mesh_container()), (*IT_entity_list).entity_id);
                  GLdouble coords[3][3];
                  int z;
                  for(z=0; z<3; z++)
                  {
                     VerdeVector tmp = mesh->coordinates(tri.node_id(z));
                     tmp.get_xyz(coords[z]);
                  }
                  add_tri(coords);
                  break;
               }
            case PickTool::P_EDGE:
               {
                  EdgeRef edge(*(block->mesh_container()), (*IT_entity_list).entity_id);
                  GLdouble coords[2][3];
                  int z;
                  for(z=0; z<2; z++)
                  {
                     VerdeVector tmp = mesh->coordinates(edge.node_id(z));
                     tmp.get_xyz(coords[z]);
                  }
                  add_edge(coords);
                  break;  
               }
            case PickTool::P_NODE:
               {
                  glBegin(GL_POINTS);
                  VerdeVector tmp = mesh->coordinates((*IT_entity_list).entity_id);
                  glVertex3d(tmp.x(), tmp.y(), tmp.z());
                  glEnd();
                  break;
              }         
            default:
               break;
         }
      }
   }
   glEndList();
}


void DrawingTool::clear_temp_lists()
{
  while(DL_temp.size())
  {
    glDeleteLists(DL_temp.front().drawing_list_id, 1);
    DL_temp.pop_front();
  }

  while(DL_temp_labels.size())
  {
    glDeleteLists(DL_temp_labels.front().drawing_list_id, 1);
    DL_temp_labels.pop_front();
  }

  temp_node_label_list.clear();
  temp_element_label_list.clear();

}

void DrawingTool::draw_hexes ( std::vector<int>& hexes )
{

  ElementBlock* block = 0;
  Mesh* mesh = verde_app->mesh();
  if(!mesh)
    return;

  GLdouble coords[8][3];

  
  int list_id = glGenLists(1);
  int color[3] = {0,0,0};
  DrawingList new_DL(true, 0,  list_id, color, "" );
  DL_temp.push_back(new_DL);

  glNewList(list_id, GL_COMPILE);

  glEnableClientState(GL_VERTEX_ARRAY);
 
  // enable picking 
  glPushName(PickTool::P_HEX);
  glPushName(0);

  VerdeColorVal prev_color;

  std::vector<int>::iterator iter;
  for(iter = hexes.begin(); iter != hexes.end(); iter++)
  {
    block = mesh->find_block_for_element(*iter);
    if(block && strncmp(block->element_type().c_str(), "HEX",3)==0)
    {
      HexRef hex(*block->mesh_container(), *iter);
      // set the id for picking
      glLoadName(*iter);

      VerdeVector temp_vec;
      
      // gather coordinates
      for(int z=0; z<8; z++)
      {
        // get a unique set of nodes of hexes 
        int node_id = hex.node_id( z ); 

        // get the coordinates too
        NodeRef node( node_id );
        VerdeVector node_coords = node.coordinates();
        node_coords.get_xyz(coords[z]);
        temp_vec += node_coords;
 
        temp_node_label_list.push_back( std::pair< int, VerdeVector> (node_id, node_coords ));

        if( autofit_toggle )
          calc_boundary( node_coords, 1 );

      }

      temp_vec /= 8.0;
      int element_id = hex.index();
      temp_element_label_list.push_back( std::pair< int, VerdeVector> (element_id, temp_vec ));

      // set the color
      VerdeColorVal color = get_color(block->block_id());
      if(prev_color != color)
        glColor3f(color.r, color.g, color.b);
      // add the hex
      add_hex(coords);
    }
  }

  std::unique(temp_node_label_list.begin(), temp_node_label_list.end());

  glPopName();
  glPopName();
  
  glDisableClientState(GL_VERTEX_ARRAY);

  glEndList();

}


void DrawingTool::draw_tets  ( std::vector<int>& tets )
{
  ElementBlock* block = 0;
  Mesh* mesh = verde_app->mesh();
  if(!mesh)
    return;

  GLdouble coords[4][3];

  
  int list_id = glGenLists(1);
  int color[3] = {0,0,0};
  DrawingList new_DL(true, 0,  list_id, color, "" );
  DL_temp.push_back(new_DL);

  glNewList(list_id, GL_COMPILE);

  glEnableClientState(GL_VERTEX_ARRAY);
 
  // enable picking 
  glPushName(PickTool::P_TET);
  glPushName(0);

  VerdeColorVal prev_color;

  std::vector<int>::iterator iter;
  for(iter = tets.begin(); iter != tets.end(); iter++)
  {
    block = mesh->find_block_for_element(*iter);
    if(block && strncmp(block->element_type().c_str(),"TET",3)==0)
    {
      TetRef tet(*block->mesh_container(), *iter);
      // set the id for picking
      glLoadName(*iter);
     
      VerdeVector temp_vec;
 
      // gather coordinates
      for(int z=0; z<4; z++)
      {

        // get a unique set of nodes of tets 
        int node_id = tet.node_id( z ); 

        // get the coordinates too
        NodeRef node( node_id );
        VerdeVector node_coords = node.coordinates();
        node_coords.get_xyz(coords[z]);
        temp_vec += node_coords;

        temp_node_label_list.push_back( std::pair< int, VerdeVector> (node_id, node_coords ));

        if( autofit_toggle )
          calc_boundary( node_coords, 1 );

      }

      temp_vec /= 4.0;
      int element_id = tet.index();
      temp_element_label_list.push_back( std::pair< int, VerdeVector> (element_id, temp_vec ));

      // set the color
      VerdeColorVal color = get_color(block->block_id());
      if(prev_color != color)
        glColor3f(color.r, color.g, color.b);
      // add the hex
      add_tet(coords);
    }
  }

  std::unique(temp_node_label_list.begin(), temp_node_label_list.end());

  glPopName();
  glPopName();
  
  glDisableClientState(GL_VERTEX_ARRAY);

  glEndList();

}

void DrawingTool::draw_knives( std::vector<int>& /*knives*/ )
{

}

void DrawingTool::draw_wedges( std::vector<int>& /*wedges*/ )
{

}

void DrawingTool::draw_pyramids(std::vector<int>& /*pyramids*/ )
{

}

void DrawingTool::draw_quads ( std::vector<int>& quads )
{
  ElementBlock* block = 0;
  Mesh* mesh = verde_app->mesh();
  if(!mesh)
    return;

  GLdouble coords[4][3];

  
  int list_id = glGenLists(1);
  int color[3] = {0,0,0};
  DrawingList new_DL(true, 0,  list_id, color, "" );
  DL_temp.push_back(new_DL);

  glNewList(list_id, GL_COMPILE);

 
  // enable picking 
  glPushName(PickTool::P_QUAD);
  glPushName(0);

  VerdeColorVal prev_color;

  std::vector<int>::iterator iter;
  for(iter = quads.begin(); iter != quads.end(); iter++)
  {
    block = mesh->find_block_for_element(*iter);
    if(block && 
        (strncmp(block->element_type().c_str(),"SHELL",5)==0 ||
         strncmp(block->element_type().c_str(),"QUAD",4) ==0))
    {
      QuadRef quad(*block->mesh_container(), *iter);
      // set the id for picking
      glLoadName(*iter);
     
      VerdeVector temp_vec;
 
      // gather coordinates
      for(int z=0; z<4; z++)
      {
        // get a unique set of nodes of quads
        int node_id = quad.node_id( z ); 

        // get the coordinates too
        NodeRef node( node_id );
        VerdeVector node_coords = node.coordinates();
        node_coords.get_xyz(coords[z]);
        temp_vec += node_coords;

        temp_node_label_list.push_back( std::pair< int, VerdeVector> (node_id, node_coords ));

        if( autofit_toggle )
          calc_boundary( node_coords, 1 );

      }

      temp_vec /= 4.0;
      int element_id = quad.index();
      temp_element_label_list.push_back( std::pair< int, VerdeVector> (element_id, temp_vec ));

      // set the color
      VerdeColorVal color = get_color(block->block_id());
      if(prev_color != color)
        glColor3f(color.r, color.g, color.b);
      // add the hex
      add_quad(coords);
    }
  }

  std::unique(temp_node_label_list.begin(), temp_node_label_list.end());

  glPopName();
  glPopName();
  

  glEndList();
}

void DrawingTool::draw_tris  ( std::vector<int>& tris )
{
  ElementBlock* block = 0;
  Mesh* mesh = verde_app->mesh();
  if(!mesh)
    return;

  GLdouble coords[3][3];
  
  int list_id = glGenLists(1);
  int color[3] = {0,0,0};
  DrawingList new_DL(true, 0,  list_id, color, "" );
  DL_temp.push_back(new_DL);

  glNewList(list_id, GL_COMPILE);

 
  // enable picking 
  glPushName(PickTool::P_TRI);
  glPushName(0);

  VerdeColorVal prev_color;

  std::vector<int>::iterator iter;
  for(iter = tris.begin(); iter != tris.end(); iter++)
  {
    block = mesh->find_block_for_element(*iter);
    if(block && strncmp(block->element_type().c_str(),"TRI",3)==0)
    {
      TriRef tri(*block->mesh_container(), *iter);
     
      // set the id for picking
      glLoadName(*iter);

      VerdeVector temp_vec;
      
      // get node ids and coords for node labels 
      // and gather coordinates
      for(int z=0; z<3; z++)
      {
        // get a unique set of nodes of tris
        int node_id = tri.node_id( z ); 

        // get the coordinates too
        NodeRef node( node_id );
        VerdeVector node_coords = node.coordinates();
        node_coords.get_xyz(coords[z]);
        temp_vec += node_coords;

        temp_node_label_list.push_back( std::pair< int, VerdeVector> (node_id, node_coords ));

        if( autofit_toggle )
          calc_boundary( node_coords, 1 );

      }

      temp_vec /= 3.0;
      int element_id = tri.index();
      temp_element_label_list.push_back( std::pair< int, VerdeVector> (element_id, temp_vec ));

      // set the color
      VerdeColorVal color = get_color(block->block_id());
      if(prev_color != color)
        glColor3f(color.r, color.g, color.b);
      // add the hex
      add_tri(coords);
    }
  }

  std::unique(temp_node_label_list.begin(), temp_node_label_list.end());

  glPopName();
  glPopName();
  
  glEndList();

}

void DrawingTool::draw_edges ( std::vector<int>& edges )
{
  ElementBlock* block = 0;
  Mesh* mesh = verde_app->mesh();
  if(!mesh)
    return;

  GLdouble coords[2][3];
  
  int list_id = glGenLists(1);
  int color[3] = {0,0,0};
  DrawingList new_DL(true, 0,  list_id, color, "" );
  DL_temp.push_back(new_DL);

  glNewList(list_id, GL_COMPILE);

  // enable picking 
  glPushName(PickTool::P_EDGE);
  glPushName(0);

  VerdeColorVal prev_color;

  std::vector<int>::iterator iter;
  for(iter = edges.begin(); iter != edges.end(); iter++)
  {
    block = mesh->find_block_for_element(*iter);
    if((block && strncmp(block->element_type().c_str(),"BEA",3)==0)||
       (block && strncmp(block->element_type().c_str(),"BAR",3)==0)||
       (block && strncmp(block->element_type().c_str(),"TRU",3)==0))
    {
      EdgeRef edge(*block->mesh_container(), *iter);
     
      // set the id for picking
      glLoadName(*iter);

      VerdeVector temp_vec;
      
      // get node ids and coords for node labels 
      // and gather coordinates
      for(int z=0; z<2; z++)
      {
        // get a unique set of nodes of tris
        int node_id = edge.node_id( z ); 

        // get the coordinates too
        NodeRef node( node_id );
        VerdeVector node_coords = node.coordinates();
        node_coords.get_xyz(coords[z]);
        temp_vec += node_coords;

        temp_node_label_list.push_back( std::pair< int, VerdeVector> (node_id, node_coords ));

        if( autofit_toggle )
          calc_boundary( node_coords, 1 );

      }

      temp_vec /= 2.0;
      int element_id = edge.index();
      temp_element_label_list.push_back( std::pair< int, VerdeVector> (element_id, temp_vec ));

      // set the color
      VerdeColorVal color = get_color(block->block_id());
      if(prev_color != color)
        glColor3f(color.r, color.g, color.b);
      // add the hex
      add_edge(coords);
    }
  }

  std::unique(temp_node_label_list.begin(), temp_node_label_list.end());

  glPopName();
  glPopName();
  
  glEndList();

}

void DrawingTool::draw_nodes ( std::vector<int>& nodes )
{
  Mesh* mesh = verde_app->mesh();
  if(!mesh)
    return;

  int list_id = glGenLists(1);
  int color[3] = {0,0,0};
  DrawingList new_DL(true, 0,  list_id, color, "" );
  DL_temp.push_back(new_DL);

  glNewList(list_id, GL_COMPILE);

  // set color to green for ALL nodes
  glColor3ub(0,255,0);
 
  // enable picking 
  glPushName(PickTool::P_NODE);
  glPushName(0);

  // iterate through each node
  std::vector<int>::iterator iter;

  // get the nodes' mesh container to see if node is in range
  MeshContainer* container = ElemRef::node_container();

  for(iter = nodes.begin(); iter != nodes.end(); ++iter)
  {
    
    // check to make sure iter is in range
    int node_id = *iter;
    if( node_id < 1 || node_id > container->number_nodes() )
      continue;

    NodeRef node(*iter);

    // get the coordinates
    VerdeVector coords = node.coordinates();
  
    temp_node_label_list.push_back( std::pair< int, VerdeVector> (*iter, coords ));

    if( autofit_toggle )
      calc_boundary( coords, 1 );
 
    // set the id for picking
    glLoadName(*iter);
    
    // draw the node
    glBegin(GL_POINTS);
    glVertex3d(coords.x(), coords.y(), coords.z());

    glEnd();

  }

  glPopName();
  glPopName();
  
  glEndList();


}


