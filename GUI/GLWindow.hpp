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



#ifndef GL_WINDOW
#define GL_WINDOW

#include "GLInputWidget.hpp"

class DrawingTool;
struct LabelData;

enum GraphicsMode { WIREFRAME, SMOOTHSHADE, HIDDENLINE };

//! OpenGL drawing window
class GLWindow : public GLInputWidget
{

friend class PickTool;
	
public:
   GLWindow( QWidget* parent, const char* name );
   ~GLWindow();
   
   //! the only instance of the DrawingTool
   static GLWindow* first_instance() { return first_instance_; };
   
   void set_graphics_mode(GraphicsMode mode);   
   QColor get_background_color() { return mBackgroundColor; }
   void set_background_color(QColor color);
   void switch_viewing_mode();
   
   void center_model();
   void set_draw_temp_only(bool toggle) { mDrawTempOnly = toggle; }
   bool get_draw_temp_only() { return mDrawTempOnly; }
   
   void size_clip_planes();

   void set_point_size( int size ) { mPointSize = size; }
   int get_point_size() { return mPointSize; }

   void resize_radius(); 

protected:
   //! virtual functions that \em must be overloaded
   void initializeGL();
   void resizeGL ( int w, int h );
   void paintGL();

private:

   static GLWindow* first_instance_;
   GraphicsMode mGraphicsMode;
   void setup_lighting();
   
   //! functions called by paintGL()
   void draw_surface_mesh(GraphicsMode& mode);
   void draw_boundary_conditions(GraphicsMode& mode);
   void draw_model_edges(GraphicsMode& mode);
   void draw_surface_normals(GraphicsMode& mode);
   void draw_failed_elements(GraphicsMode& mode);
   void draw_selected_entities(GraphicsMode& mode);
   void draw_failed_labels();
   void draw_block_labels();
   void draw_axis();
   void draw_temp_lists(GraphicsMode& mode);

   QColor mBackgroundColor;
   QColor mLabelColor;
   bool mDrawTempOnly;

   int mPointSize;
   
};


#endif
