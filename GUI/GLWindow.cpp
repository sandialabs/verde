//----------------------------------------------------------------------
// Filename         : GLWindow.cpp
//
// Purpose          : OpenGL window class derived from a QT OpenGL class
//                    which supports drawing to the screen.
//                    
// Creator          : Clinton Stimpson
//
// Date             : 7 Aug 2001
//
// Owner            : Clinton Stimpson
//----------------------------------------------------------------------


#include <math.h>
#include <list>
#include <algorithm>
#include <qfont.h>
#include "GLWindow.hpp"
#include "GLInputWidget.hpp"
#include "DrawingTool.hpp"
#ifndef WIN32
#include <GL/glx.h> //to supply UN*X font support
#endif

GLWindow* GLWindow::first_instance_ = NULL;

GLWindow::GLWindow( QWidget* parent, const char* name )
	: GLInputWidget(parent, name)
{
  if(!first_instance_) 
    first_instance_ = this;

  mGraphicsMode = WIREFRAME;
  mDrawTempOnly = false;
  mPointSize = 5;
  maxScale = 200.0;
  
}


GLWindow::~GLWindow()
{
     delete DrawingTool::instance(); 
}

void GLWindow::set_background_color(QColor color)
{
   mBackgroundColor = color;
   qglClearColor(color);


   // here we set label white or black depending on background color
   int h, s, v;
   mBackgroundColor.hsv(&h, &s, &v);
   
   if(v>127)
     mLabelColor.setRgb(0, 0, 0);
   else
     mLabelColor.setRgb(255, 255, 255);

}


void GLWindow::initializeGL()
{
  set_background_color( black );
  
  QGLFormat gl_format;
  gl_format.setDoubleBuffer(true);
  gl_format.setDepth(true);
  gl_format.setRgba(true);
  gl_format.setAlpha(false);
  gl_format.setAccum(false);
  gl_format.setStencil(false);
  gl_format.setStereo(false);
  gl_format.setDirectRendering(true);
  gl_format.setOverlay(false);
  gl_format.setOption(DepthBuffer);
  
  QGLFormat::setDefaultFormat(gl_format);
  
  glEnable(GL_DEPTH_TEST);
  
  setup_lighting();

  // application default font
  QFont font;
  DrawingTool::instance()->textTool.build_bitmap_font(this, font);

}

void GLWindow::setup_lighting()
{
  glEnable( GL_LIGHTING );
  glEnable( GL_LIGHT0 );
  GLfloat white_light_amb[] = { 0.5f, 0.5f, 0.5f, 1.0f };
  GLfloat white_light_spec[] = { 0.5f, 0.5f, 0.5f, 1.0f };
  GLfloat white_light_diff[] = { 0.5f, 0.5f, 0.5f, 1.0f };
  
  glLightfv(GL_LIGHT0, GL_AMBIENT, white_light_amb);
  glLightfv(GL_LIGHT0, GL_SPECULAR, white_light_spec);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light_diff);
  
  GLfloat light_position[] = {10.0, 10.0, 20.0, 0.0 };
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

void GLWindow::draw_axis()
{

  int view_port_size;
  view_port_size = ( width() + height() ) / 16;

  //save the current viewport
  glPushAttrib(GL_VIEWPORT_BIT);

  // set a new viewport
  glViewport( 0, 0, view_port_size, view_port_size );
  glPolygonMode(GL_FRONT, GL_FILL);

  // set the projection matrix for this viewport
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho( -1.0, 1.0, -1.0, 1.0, -1.0, 1.0 );
  glMatrixMode(GL_MODELVIEW);

  // save the last model view matrix
  glPushMatrix();

   //start from scratch
   glLoadIdentity();

   //get rotation transformations
   glMultMatrixd((GLdouble *) objectXform);

   // set line width
   glLineWidth(1.);

   //draw the axis
   glBegin(GL_LINES);

   // x-axis
   glColor3f(1.0f, 0.0f, 0.0f);
   glVertex3d(0, 0, 0);
   glVertex3d(0.8, 0, 0);

   // y-axis
   glColor3f(0.0f, 1.0f, 0.0f);
   glVertex3d(0, 0, 0);
   glVertex3d(0, 0.8, 0);

   // z-axis
   glColor3f(0.0f, 0.0f, 1.0f);
   glVertex3d(0, 0, 0);
   glVertex3d(0, 0, 0.8);
 
   glEnd();

   glPushMatrix();
    GLUquadricObj *my_obj = gluNewQuadric();

    //z-cone
    glTranslatef(0,0,0.5);
    glColor3f(0.0f, 0.0f, 1.0f);
    gluCylinder(my_obj, 0.1, 0.0, .3, 6, 2);

    //x-cone
    glTranslatef(0.5,0,-0.5);
    glRotatef(90.0, 0, 1, 0);
    glColor3f(1.0f, 0.0f, 0.0f);
    gluCylinder(my_obj, 0.1, 0.0, .3, 6, 2);
       
    //y-cone
    glTranslatef(0.0,0.5,-0.5);
    glRotatef(-90.0, 1,0,0);
    glColor3f(0.0f, 1.0f, 0.0f);
    gluCylinder(my_obj, 0.1, 0.0, .3, 6, 2);

    gluDeleteQuadric(my_obj);

   glPopMatrix(); 


   // white text
   glColor3ub(mLabelColor.red(), mLabelColor.green(), mLabelColor.blue());


   //glPushAttrib(GL_DEPTH_BUFFER_BIT);
   glDepthRange(0, .8);

   // X-axis
   glRasterPos3d(0.8,0,0);
   DrawingTool::instance()->textTool.printGLf("X");

   // Y-axis
   glRasterPos3d(0,0.8,0);
   DrawingTool::instance()->textTool.printGLf("Y");

   // Z-axis
   glRasterPos3d(0,0,0.8);
   DrawingTool::instance()->textTool.printGLf("Z");

   //glPopAttrib();


   // restore the model view matrix
  glPopMatrix();
  
  // restore the viewport
  glPopAttrib();
  
  // restore the preivous projection matrix
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);


}
void GLWindow::paintGL()
{

  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  glPushMatrix();
  {
     transform();

     if(!mDrawTempOnly)
     {
       // skip everything except model edges if we are doing quick transform
       if( !(mQuickTransform && mMouseDown) )
       {
         glDepthRange(0.0, 0.999997);
         draw_failed_elements(mGraphicsMode);
         
         glDepthRange(0.0, 0.999998);
         draw_boundary_conditions(mGraphicsMode);
       }
       
       glDepthRange(0.0, 0.999999);
       draw_model_edges(mGraphicsMode);
       
       if( !(mQuickTransform && mMouseDown) )
       {	       
         glDepthRange(0.0, 1.0);
         draw_surface_normals(mGraphicsMode);
         
	 draw_surface_mesh(mGraphicsMode);
         
         draw_failed_labels();
         draw_block_labels();
       }
     }
  
     draw_temp_lists(mGraphicsMode);
     
     glClear( GL_DEPTH_BUFFER_BIT );
     draw_selected_entities(mGraphicsMode);

     draw_axis();
  }   
  glPopMatrix();

}

// This function toggles between perspective and orthographic viewing, 
// passing resizeGL the current window width and height
void GLWindow::switch_viewing_mode()
{
  // if we're going into orthographic viewing
  if(!get_perspective())
  {
    if( mDrawTempOnly )
      center_model(); 
        
    else
    {
      // check to make sure that scale won't be too big.
      if(z_trans == 0)
         z_trans = .1;
      
      double my_scale = GLWindow::first_instance()->z_original/z_trans;
      if(my_scale > maxScale || my_scale < 0)
      {
        // set the z back to its original location
        z_trans = z_original;
        reset(); 
      }
      else
      {
        // set scale to new value, so it doesn't 'jump' 
        // when you first switch from persp. to orthog.
        GLWindow::first_instance()->setScale(my_scale);
      }
    }
  }
   
  resizeGL( width(), height() );
}

void GLWindow::resizeGL( int w, int h )
{

  if(get_perspective())
  {

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(35, (GLfloat)w/(GLfloat)h, this->near_clip, this->far_clip);
    glViewport ( 0, 0, w, h );
    glMatrixMode(GL_MODELVIEW);
  }

  else
  {
    double boundary[3][2];
    DrawingTool::instance()->get_bounding_box(boundary);
 
    double height = fabs(boundary[1][0] - boundary[1][1]);
    double width = fabs(boundary[0][0] - boundary[0][1]);
    double depth = fabs(boundary[2][0] - boundary[2][1]);

    // find largest dimension of the bounding box 
    double size = height;
    if(width > height)
      size = width;
    else if( depth > width )
      size = depth;
    else if( depth > height)
      size = depth;

    // map this dimension to viewport width and height 
    double o_width, o_height;
    if(w > h)
    {
      o_height = size;
      o_width = w*size/h;
    }
    else
    {
      o_width = size;
      o_height = h*size/w;
    }

    // set up orthographic viewport
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho( -(GLfloat)o_width, (GLfloat)o_width, -(GLfloat)o_height, (GLfloat)o_height, far_clip*-10., far_clip*10.);
    glViewport ( 0, 0, w, h );
    glMatrixMode(GL_MODELVIEW);
  }
}

void GLWindow::set_graphics_mode(GraphicsMode mode)
{
  mGraphicsMode = mode;
}

void GLWindow::draw_selected_entities(GraphicsMode& /*mode*/)
{
   DrawingList* selected_DL = &(DrawingTool::instance()->DL_SelectedItems);

   // nothing to highlight
   if(!selected_DL->drawing_list_id)
      return;

   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   glLineWidth(2.0);
   glPointSize(mPointSize);
   glColor3f(1.0f, 1.0f, 0.0f);
   glCallList(selected_DL->drawing_list_id);
   
}

void GLWindow::draw_temp_lists(GraphicsMode& /*mode*/)
{
  std::deque<DrawingList>* Failed_DL = &(DrawingTool::instance()->DL_temp);
  std::deque<DrawingList>::iterator DL_iterator;
  
  glDisable(GL_LIGHTING);
  glPointSize(0.01f);
  glLineWidth(0.01f);

  for(DL_iterator = Failed_DL->begin(); DL_iterator != Failed_DL->end(); ++DL_iterator)
  {
    int list_id = (*DL_iterator).drawing_list_id;
    // support wireframe only for now until we can get some other
    // issues sorted out -- setting color in drawinglist etc...
   /* 
    if(mode == HIDDENLINE)
    {
      glShadeModel( GL_FLAT );
      glEnable(GL_POLYGON_OFFSET_FILL);
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      glPolygonOffset(1.0,1.0);
      QColor back_color = get_background_color();
      glColor3ub(back_color.red(), back_color.green(), back_color.blue());
      glCallList(list_id);
      glDisable(GL_POLYGON_OFFSET_FILL); 
    }
    else if(mode == SMOOTHSHADE)
    {
      glShadeModel(GL_FLAT);
      glEnable(GL_POLYGON_OFFSET_FILL);
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      glPolygonOffset(1.0,1.0);               
      glColor3ub(160, 0, 0);
      glCallList(list_id);
      glDisable(GL_POLYGON_OFFSET_FILL);
    }
    */
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(1.0);
    glPointSize(mPointSize);
    glCallList(list_id);
  }

  // Possibly have to draw labels

  int list_id = 0;

  // see if we need to draw labels and if a list doesn't exist 
  if(DrawingTool::instance()->label_node_toggle)
  {
    std::deque<DrawingList>* temp_labels = &(DrawingTool::instance()->DL_temp_labels);
    std::deque< std::pair< int, VerdeVector> > *node_label_list = &(DrawingTool::instance()->temp_node_label_list);   

    //Try to find a drawing list named 'node labels'
    for(DL_iterator = temp_labels->begin(); DL_iterator != temp_labels->end(); ++DL_iterator)
      if(strcmp( (*DL_iterator).name.c_str(), "node labels") == 0 )
        list_id = (*DL_iterator).drawing_list_id;

    // set label color
    glColor3ub(mLabelColor.red(), mLabelColor.green(), mLabelColor.blue());

    // if the list isn't found
    if( list_id == 0 )
    {
      list_id = glGenLists(1);
      int color[3] = {0,0,0};
      DrawingList new_DL(true, 0,  list_id, color, "node labels" );
      temp_labels->push_back(new_DL);
      
      glNewList(list_id, GL_COMPILE);

      std::deque< std::pair< int, VerdeVector > >::iterator iter;
      for(iter = node_label_list->begin(); iter != node_label_list->end() ; iter++ )
      {
        VerdeVector position;
        position = iter->second;

        glPushMatrix();
          //! Note that glRasterPos* uses the world coordinates
          glRasterPos3d(position.x(), position.y(), position.z());
          DrawingTool::instance()->textTool.printGLf ("%d", iter->first );
        glPopMatrix();
      }
      glEndList();
      glCallList(list_id);
    }
    else
      glCallList(list_id);
  }

  //reset list_id
  list_id = 0;

  //Try to find a drawing list named 'element labels'
  if(DrawingTool::instance()->label_element_toggle)
  {
    std::deque<DrawingList>* temp_labels = &(DrawingTool::instance()->DL_temp_labels);
    std::deque< std::pair< int, VerdeVector> > *element_label_list = &(DrawingTool::instance()->temp_element_label_list);   

    //Try to find a drawing list named 'element labels'
    for(DL_iterator = temp_labels->begin(); DL_iterator != temp_labels->end(); ++DL_iterator)
      if(strcmp( (*DL_iterator).name.c_str(), "element labels") == 0 )
        list_id = (*DL_iterator).drawing_list_id;

    // set label color
    glColor3ub(mLabelColor.red(), mLabelColor.green(), mLabelColor.blue());

    // if the list isn't found
    if( list_id == 0 )
    {
      list_id = glGenLists(1);
      int color[3] = {0,0,0};
      DrawingList new_DL(true, 0,  list_id, color, "element labels" );
      temp_labels->push_back(new_DL);
      glNewList(list_id, GL_COMPILE);

      std::deque< std::pair< int, VerdeVector > >::iterator iter;
      for(iter = element_label_list->begin(); iter != element_label_list->end() ; iter++ )
      {
        VerdeVector position;
        position = iter->second;

        glPushMatrix();
          //! Note that glRasterPos* uses the world coordinates
          glRasterPos3d(position.x(), position.y(), position.z());
          DrawingTool::instance()->textTool.printGLf ("%d", iter->first );
        glPopMatrix();
      }
      glEndList();
      glCallList(list_id);
    }
    else
      glCallList(list_id);
  }

}


void GLWindow::draw_failed_labels()
{

  // if labels are on and the list exists display it.
  if (DrawingTool::instance()->get_failed_label_toggle()) //TODO: switch on this
  {
    // find "label failed" drawing list in DL_Model list
    std::deque<DrawingList>* DL_label = &(DrawingTool::instance()->DL_Util);
    std::deque<DrawingList>::iterator it_labels;

    it_labels = std::find_if(DL_label->begin(), 
        DL_label->end(), name_equals("label failed"));

    // set label color
    glColor3ub(mLabelColor.red(), mLabelColor.green(), mLabelColor.blue());

    if(it_labels != DL_label->end())
      glCallList((*it_labels).drawing_list_id);
  }
}

void GLWindow::draw_block_labels()
{

  if( DrawingTool::instance()->get_block_label_toggle() )
  {
      glColor3ub(DrawingTool::instance()->DL_Block_Labels.color[0], 
        DrawingTool::instance()->DL_Block_Labels.color[1], 
        DrawingTool::instance()->DL_Block_Labels.color[2]);
        glColor3ub(mLabelColor.red(), mLabelColor.green(), mLabelColor.blue());
      glCallList( DrawingTool::instance()->DL_Block_Labels.drawing_list_id);
  }
}

void GLWindow::draw_failed_elements(GraphicsMode& mode)
{
  std::deque<DrawingList>* Failed_DL = &(DrawingTool::instance()->DL_FailedElements);
  std::deque<DrawingList>::iterator DL_iterator;
   int list_id = 0;

   for(DL_iterator = Failed_DL->begin(); DL_iterator != Failed_DL->end(); ++DL_iterator)
   {
     if(strcmp( (*DL_iterator).name.c_str(), "failed") == 0 )
     {
       list_id = (*DL_iterator).drawing_list_id;
     }
   }

   if(!list_id) return;
   
   glDisable(GL_LIGHTING);
   if(mode == HIDDENLINE)
   {
      glShadeModel( GL_FLAT );
      glEnable(GL_POLYGON_OFFSET_FILL);
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      glPolygonOffset(1.0,1.0);
      QColor back_color = get_background_color();
      glColor3ub(back_color.red(), back_color.green(), back_color.blue());
      glCallList(list_id);
      glDisable(GL_POLYGON_OFFSET_FILL); 
   }
   else if(mode == SMOOTHSHADE)
   {
      glShadeModel(GL_FLAT);
      glEnable(GL_POLYGON_OFFSET_FILL);
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      glPolygonOffset(1.0,1.0);               
      glColor3ub(160, 0, 0);
      glCallList(list_id);
      glDisable(GL_POLYGON_OFFSET_FILL);
   }

   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   glLineWidth(2.0);
   glPointSize(mPointSize);
   glColor3f(1.0f, 0.0f, 0.0f);
   glCallList(list_id);
}

void GLWindow::draw_boundary_conditions(GraphicsMode& mode)
{
  std::deque<DrawingList>::iterator DL_iterator;
  std::deque<DrawingList>* Node_DL = &(DrawingTool::instance()->DL_NodeBC);

  // nodesets look the same in all modes
  // draw the node BC's
  glDisable(GL_LIGHTING);
  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);
  glLineWidth(1.0);
  glPointSize(mPointSize);
  for(DL_iterator = Node_DL->begin();
      DL_iterator != Node_DL->end();
      ++DL_iterator)
  {
    glColor3ub((*DL_iterator).color[0], 
        (*DL_iterator).color[1], 
        (*DL_iterator).color[2]);
    
    glCallList((*DL_iterator).drawing_list_id);
  }

  if( DrawingTool::instance()->get_nodeset_label_toggle() )
  {   
    // drawing nodeset labels
    std::deque<DrawingList>* Node_DL_Labels = &(DrawingTool::instance()->DL_NodeBC_Labels);

    glColor3ub(mLabelColor.red(), mLabelColor.green(), mLabelColor.blue());

    for(DL_iterator = Node_DL_Labels->begin();
        DL_iterator != Node_DL_Labels->end();
        ++DL_iterator)
    {
      glCallList((*DL_iterator).drawing_list_id);
    }
  }

  if( DrawingTool::instance()->get_sideset_label_toggle() )
  {   
    // drawing sideset labels
    std::deque<DrawingList>* Element_DL_Labels = &(DrawingTool::instance()->DL_ElementBC_Labels);

    glColor3ub(mLabelColor.red(), mLabelColor.green(), mLabelColor.blue());

    for(DL_iterator = Element_DL_Labels->begin();
        DL_iterator != Element_DL_Labels->end();
        ++DL_iterator)
    {
      glCallList((*DL_iterator).drawing_list_id);
    }
  }

  // draw the Element BC's
  std::deque<DrawingList>* Element_DL = &(DrawingTool::instance()->DL_ElementBC);

  if(mode == SMOOTHSHADE)
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  else
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  
  for(DL_iterator = Element_DL->begin();
      DL_iterator != Element_DL->end();
      ++DL_iterator)
  {
    glColor3ub((*DL_iterator).color[0],
        (*DL_iterator).color[1],
	(*DL_iterator).color[2]);

    glCallList((*DL_iterator).drawing_list_id);
  }
  glDisable(GL_CULL_FACE);

}

void GLWindow::draw_model_edges(GraphicsMode&)
{
  std::deque<DrawingList>* Edge_DL = &(DrawingTool::instance()->DL_Model);
   // find "edge" drawing list in DL_Model list
   int list_id = 0;
   std::deque<DrawingList>::iterator DL_iterator;
   for(DL_iterator = Edge_DL->begin(); DL_iterator != Edge_DL->end(); ++DL_iterator)
   {
      if(strcmp( (*DL_iterator).name.c_str(), "edge") == 0 )
      {
         list_id = (*DL_iterator).drawing_list_id;
	 // return if invisible, unless we are doing quick transform
         if( !(*DL_iterator).visible && !(mQuickTransform && mMouseDown))
            return;
      }
   }

   if(!list_id)
      return;

   glDisable(GL_LIGHTING);
   glLineWidth(2.0);
   glColor3f(0.3f, 0.3f, 1.0f);
   glCallList(list_id);
}

void GLWindow::draw_surface_normals(GraphicsMode&)
{

  std::deque<DrawingList>* Normal_DL = &(DrawingTool::instance()->DL_Model);
   // find "normal" drawing list in DL_Model list
   int list_id = 0;
   std::deque<DrawingList>::iterator DL_iterator;
   for(DL_iterator = Normal_DL->begin(); DL_iterator != Normal_DL->end() && list_id==0; ++DL_iterator)
   {  
      if(strcmp( (*DL_iterator).name.c_str(), "normal") == 0 )
      {
         list_id = (*DL_iterator).drawing_list_id;
         if( !(*DL_iterator).visible )
            return;
      }
   }

   if(!list_id)
      return;
  
   glDisable(GL_LIGHTING);
   //glColor3f(0.1f, 0.8f, 1.0f);
   glLineWidth(1.0);
   glCallList(list_id);
}


void GLWindow::draw_surface_mesh(GraphicsMode& mode)
{

  std::deque<DrawingList>* Surface_DL = &(DrawingTool::instance()->DL_Model);
   // find "normal" drawing list in DL_Model list
   int list_id = 0;
   std::deque<DrawingList>::iterator DL_iterator;
   for(DL_iterator = Surface_DL->begin(); DL_iterator != Surface_DL->end() && list_id==0; ++DL_iterator)
   {  
      if(strcmp( (*DL_iterator).name.c_str(), "surface") == 0 )
      {
         list_id = (*DL_iterator).drawing_list_id;
         if( !(*DL_iterator).visible )
            return;
      }
   }

  if(!list_id)
     return;

  // wireframe mode
  if(mode == WIREFRAME)
  {
    glLineWidth(1.0);
    glCallList(list_id);
  }
  else if(mode == SMOOTHSHADE)
  {
    GLfloat material_specular[] = { 0.4f, 0.4f, 0.4f, 0.5f };
    GLfloat material_shininess[] = { 1.0 };
    //GLfloat material_emission[] = {0.0, 0.0, 0.0, 0.0};
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glShadeModel(GL_FLAT);
    
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0,1.0);		
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glLineWidth(1.0);

    glPushAttrib(GL_LIGHTING_BIT);    
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, material_shininess);		
    glCallList(list_id);

    glPopAttrib();
    
    glDisable(GL_POLYGON_OFFSET_FILL);
    
    glDisable(GL_LIGHTING);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glCallList(list_id);
    
  }
  
  else if(mode == HIDDENLINE)
  {
    glEnable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    glShadeModel(GL_FLAT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0,1.0);
    glLineWidth(1.0);
        
    GLfloat invisible_polygon[4];
    QColor back_color = get_background_color();
    invisible_polygon[0] = ((float)back_color.red())/255.;    
    invisible_polygon[1] = ((float)back_color.green())/255.;
    invisible_polygon[2] = ((float)back_color.blue())/255.;
    invisible_polygon[3] = 1.0f;

    glPushAttrib(GL_LIGHTING_BIT);  // save current lighting
    glMaterialfv(GL_FRONT, GL_EMISSION, invisible_polygon);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, invisible_polygon);
    glMaterialfv(GL_FRONT, GL_SPECULAR, invisible_polygon);
    
    glCallList(list_id);
    glPopAttrib();  // undo the material properties

    glDisable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_LIGHTING);
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(1.0);
    glCallList(list_id);
    
  }

  // Call drawing list containing node labels
  if(DrawingTool::instance()->label_node_toggle)
  {
    glColor3ub(mLabelColor.red(), mLabelColor.green(), mLabelColor.blue());
    glCallList( DrawingTool::instance()-> DL_Node_Labels.drawing_list_id );
  }

  // Call drawing list containing element labels
  if(DrawingTool::instance()->label_element_toggle)
  {
    glColor3ub(mLabelColor.red(), mLabelColor.green(), mLabelColor.blue());
    glCallList( DrawingTool::instance()-> DL_Element_Labels.drawing_list_id );
  }
  
}


void GLWindow::center_model()
{
  double boundary[3][2];
  
  if( mDrawTempOnly && DrawingTool::instance()->get_autofit_toggle() )
    DrawingTool::instance()->get_bounding_box(boundary, 1);
  else 
    DrawingTool::instance()->get_bounding_box(boundary);
 
  float xCenter = (boundary[0][0] + boundary[0][1]) / 2.0;
  float yCenter = (boundary[1][0] + boundary[1][1]) / 2.0;
  float zCenter = (boundary[2][1] + boundary[2][0]) / 2.0;
  //-------------------Just inserted this check which sets the bounding 
// box to 1,1,1 of it was 0,0,0------//
  if( xCenter==0 && yCenter==0 && zCenter==0)
  {
    xCenter = 1;
    yCenter = 1;
    zCenter = 1;
    boundary[0][0] = 1;
    boundary[1][0] = 1;
    boundary[2][0] = 1;
    boundary[0][1] = 0;
    boundary[1][1] = 0;
    boundary[2][1] = 0;
  }

  setModelCenter( xCenter, yCenter, zCenter);
  setModelCentroid( xCenter, yCenter, zCenter);

  double height = fabs(boundary[1][0] - boundary[1][1])/2.0;
  double width = fabs(boundary[0][0] - boundary[0][1])/2.0;
  double depth = fabs(boundary[2][0] - boundary[2][1])/2.0;
  
  double center_radius = sqrt( height*height + width*width + depth*depth);
  setModelSize( center_radius );
  
  z_trans = -center_radius/(tan((35./2.)*M_PI/180.));
  z_trans *= 1.1;

  // set z_original so model gets scaled 
  if( mDrawTempOnly && !get_perspective() )
  {
    DrawingTool::instance()->get_bounding_box(boundary);
    height = fabs(boundary[1][0] - boundary[1][1])/2.0;
    width = fabs(boundary[0][0] - boundary[0][1])/2.0;
    depth = fabs(boundary[2][0] - boundary[2][1])/2.0;
    center_radius = sqrt( height*height + width*width + depth*depth);

    z_original = -center_radius/(tan((35./2.)*M_PI/180.));
  }
  else
    z_original = z_trans;

  setXTrans(0);
  setYTrans(0);
  setZTrans(z_trans);

}

void GLWindow::resize_radius()
{

  double boundary[3][2];
  DrawingTool::instance()->get_bounding_box(boundary);
  
  float xCenter = (boundary[0][0] + boundary[0][1])/2.0;
  float yCenter = (boundary[1][0] + boundary[1][1])/2.0;
  float zCenter = (boundary[2][0] + boundary[2][1])/2.0;
 
  // if centroids are not different, get out
  if( x_centroid == xCenter && y_centroid == yCenter && z_centroid == zCenter )
    return; 

  // update centroid 
  setModelCentroid( xCenter, yCenter, zCenter );

  double width  = fabs(boundary[0][0] - boundary[0][1])/2.0;
  double height = fabs(boundary[1][0] - boundary[1][1])/2.0;
  double depth  = fabs(boundary[2][0] - boundary[2][1])/2.0;
  
  double new_radius = sqrt( width*width + height*height + depth*depth );

  // take difference between center of rotation and centroid of model 
  VerdeVector rad_diff(x_rot_center - xCenter, 
                       y_rot_center - yCenter,
                       z_rot_center - zCenter);

  new_radius += rad_diff.length();
  setModelSize( new_radius );
}

void GLWindow::size_clip_planes()
{
  double boundary[3][2];

  DrawingTool::instance()->get_bounding_box(boundary);

  double height = fabs(boundary[1][0] - boundary[1][1])/2.0;
  double width = fabs(boundary[0][0] - boundary[0][1])/2.0;
  double depth = fabs(boundary[2][0] - boundary[2][1])/2.0;

  double center_radius = sqrt( height*height + width*width + depth*depth);
  setModelSize( center_radius );

}




