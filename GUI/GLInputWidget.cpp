//----------------------------------------------------------------------
// Filename         : GLInputWidget.cpp
//
// Purpose          : OpenGL window class derived loosely from a QT OpenGL 
//                    class which supports model transformations
//                    
// Creator          : Karl Merkley
//
// Date             : 7 Aug 2001
//
// Owner            : Karl Merkley
//----------------------------------------------------------------------
//


#include <math.h>
#include <stdlib.h>

#include <qcursor.h>

#include "GLInputWidget.hpp"
#include "DrawingTool.hpp"
#include "VerdeGUIApp.hpp"

#include "PickTool.hpp"


/*----------------------------------------------------------------------*/
/*!
** These functions implement a simple trackball-like motion control.
*/
static GLdouble identityMatrix[16] = {
    1.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 1.0
};

// Random min and max factors for near and far clipping planes.
#define MIN_CLIP_FACTOR .1
#define MAX_CLIP_FACTOR 10

GLInputWidget::GLInputWidget( QWidget* parent, const char* name) 
        : QGLWidget( parent, name), 
            x_trans(0), y_trans(0), z_trans(0.0), 
            scale(1.0), angle(0.0), b_rotate(false)
{
   setCursor(Qt::CrossCursor);
   setFocusPolicy(QWidget::StrongFocus); 
   x_rot_center = 0.0;
   y_rot_center = 0.0;
   z_rot_center = 0.0;
   x_centroid = 0.0;
   y_centroid = 0.0;
   z_centroid = 0.0;
   lastPos[0] = lastPos[1] = lastPos[2] = 0.0;
   memcpy(objectXform, identityMatrix, sizeof(identityMatrix));
   near_clip = MIN_CLIP_FACTOR;
   far_clip = MAX_CLIP_FACTOR;
   mMouseDown = false;
   perspectiveFlag = true;
   radius = 1.;
   mQuickTransform = true;
}

GLInputWidget::~GLInputWidget()
{
   delete PickTool::instance();
}



/*!
 * This is the one truly public function in this class.  This
 * applies all the necessary transformations to the model.  The
 * call syntax is of the form
 *
 * \example
 *    glPushMatrix();
 *       transform();
 *       draw_model();
 *    glPopMatrix();
 *
 * The push and pop ensure that no other transformation affect the
 * desired transformation applied via the transform routine.
 *
*/
void GLInputWidget::transform()
{
   // Non-zero scale factors will make pan and auto-fit
   // not work.  Don't implement scale without looking at
   // these other functions
   //glScalef( scale, scale, scale );

   glLoadIdentity();

   if (b_rotate)
   {
      glPushMatrix();
         glLoadIdentity();
         glRotatef(angle, axis[0], axis[1], axis[2]);
         glMultMatrixd((GLdouble *) objectXform);
         glGetDoublev(GL_MODELVIEW_MATRIX, objectXform);
      glPopMatrix();
   }
    
   if(!perspectiveFlag)
   {
     if(scale>0 && scale<=maxScale)
     {
       glScalef(scale,scale,scale);
       setScale(scale); 
     }
     else
     {
       double scale_fact = z_original/z_trans; 
       glScalef(scale_fact, scale_fact, scale_fact);
       setScale(scale_fact); 
     }
   }

   glTranslatef( x_trans, y_trans, z_trans );
   glMultMatrixd((GLdouble*)objectXform);
   glTranslatef( -x_rot_center, -y_rot_center, -z_rot_center);

}

/*! The eyepoint is a function of the model size and
// the z model center.  Use these values to determine
// the near and far clipping planes.  
// \note that the  near clip value must always be 
// greater than (and preferably not to close to) zero.
*/
void GLInputWidget::setModelSize(double r)
{
   radius = r;  
   //! Note that the FACTORs are totally arbitrary.
   //! They will probably need to be tweaked.
   far_clip = fabs(z_rot_center) + MAX_CLIP_FACTOR*radius;
   near_clip = MIN_CLIP_FACTOR * radius;

   //! Make sure that the near and far actually get updated.
   resizeGL(width(), height());
}
void GLInputWidget::setModelCenter(double x, double y, double z)
{
   x_rot_center = x;
   y_rot_center = y;
   z_rot_center = z;
   updateGL();
}

void GLInputWidget::setModelCentroid(double x, double y, double z)
{
   x_centroid = x;
   y_centroid = y;
   z_centroid = z;
   updateGL();
}


/*!
  Set the the scale (this is currently disabled in transform)
*/
void GLInputWidget::setScale( double s )
{
    scale = s;
}

//! Set the X translation
void GLInputWidget::setXTrans( double x )
{
    x_trans = x;
}

//! Set the Y translation
void GLInputWidget::setYTrans( double y )
{
    y_trans = y;
}

//! Set the Z translation
void GLInputWidget::setZTrans( double z )
{

   //if we're in orthog. viewing, calculate how much to scale by
   if(!perspectiveFlag)
   {
     double my_scale = z_original/z;
     setScale(my_scale); 
   }
   
   // if scale is too great (zooming in too much), halt the zoom 
   if(!perspectiveFlag && ( scale >= maxScale || scale <= 0 )) 
   {
     //do nothing
     return;
   }
   else
   {
     //! Don't translate beyond the far clipping plane
     if (z > -far_clip + radius)
     {
       z_trans = z;
     }
     else
       z_trans = -far_clip + radius;

   }
}

//! Set the total translation and update the graphics afterwards
void GLInputWidget::setTranslation( double x, double y, double z )
{
    setXTrans( x_trans + x );
    setYTrans( y_trans + y );
    setZTrans( z_trans + z );
    
    updateGL();
}


/*! See \em{Interactive Computer Graphics with OpenGL} by Edward Angel
   for details on the trackball routine.
*/
void GLInputWidget::trackball(int x, int y, float v[3])
{
    float dist, a;

    //! project x,y onto a hemi-sphere centered within width, height
    v[0] = (2.0F*x - width()) / width();
    v[1] = (height() - 2.0F*y) / height();
    dist = (float) sqrt(v[0]*v[0] + v[1]*v[1]);
    v[2] = (float) cos((M_PI/2.0F) * ((dist < 1.0F) ? dist : 1.0F));
    a = 1.0F / (float) sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    v[0] *= a;
    v[1] *= a;
    v[2] *= a;
}

//! Get the mouse down event
void GLInputWidget::mousePressEvent( QMouseEvent *e )
{
    e->accept();

    // handle picking
    if(e->state() & ControlButton)
    {
       PickTool::instance()->process_pick_event(e);
    }

    // handle mouse rotations, translations, and panning
    else
    {
       start_x = e->x(); 
       start_y = e->y();

       int mouse_button = 0;
	
       if( e->button() == LeftButton )
         mouse_button = verde_gui_app->leftButton(); 
       else if ( e->button() == MidButton )
         mouse_button = verde_gui_app->middleButton(); 
       else if ( e->button() == RightButton )
         mouse_button = verde_gui_app->rightButton();

       switch(mouse_button)
       {
	
         case 0:
         {
           trackball(e->x(), e->y(), lastPos);
	   break;
         }
         case 2:
         {
           //! Get the current transformation matrices
           //! to make the object track the mouse on pan.
           glGetIntegerv(GL_VIEWPORT, viewport);
           glGetDoublev (GL_PROJECTION_MATRIX, projmatrix);

	   break;
         }

       }
       oldPos = e->pos();
       mMouseDown = true;
    }
}

//! Get the mouse up event
void GLInputWidget::mouseReleaseEvent( QMouseEvent *e )
{
    e->accept();
    b_rotate = false;
    oldPos = e->pos();
    mMouseDown = false;
    if(mQuickTransform)
      updateGL();
}

/*! Get the mouse move event.  There is a lot to do to get
 * the pan work correctly in perspective mode.
 */
void GLInputWidget::mouseMoveEvent( QMouseEvent *e )
{
  e->accept();

  /*! if we didn't get a mouse down event to begin with,
  // our mouse move calculations will be wrong
  // lets return. */
  if(mMouseDown == false)
    return;

  int mouse_button = 0;

  if( e->state() == LeftButton )
    mouse_button = verde_gui_app->leftButton(); 
  else if ( e->state() == MidButton )
    mouse_button = verde_gui_app->middleButton(); 
  else if ( e->state() == RightButton )
    mouse_button = verde_gui_app->rightButton();



  switch(mouse_button)
  { 

    //! Handle the rotations 
    case 0:
    {
       float curPos[3], dx, dy, dz;

       b_rotate = true;

       trackball(e->x(), e->y(), curPos);

       dx = curPos[0] - lastPos[0];
       dy = curPos[1] - lastPos[1];
       dz = curPos[2] - lastPos[2];

       if (dx || dy || dz) 
       {
          angle = 90.0F * sqrt(dx*dx + dy*dy + dz*dz);

          axis[0] = lastPos[1]*curPos[2] - lastPos[2]*curPos[1];
          axis[1] = lastPos[2]*curPos[0] - lastPos[0]*curPos[2];
          axis[2] = lastPos[0]*curPos[1] - lastPos[1]*curPos[0];
    
          lastPos[0] = curPos[0];
          lastPos[1] = curPos[1];
          lastPos[2] = curPos[2];

          updateGL();
       }
      break;
    }

    //! Handle zoom
    case 1:
    {
       double dx = e->x() - oldPos.x();
       double dy = e->y() - oldPos.y();
       double rx = dx / width();
       double ry = dy / height();
       
       if(!verde_gui_app->zoomDir() )
       {
         rx *= -1; 
         ry *= -1; 
       }
     
       // TODO: should be * user defined gain from GUI
       
       setTranslation( 0, 0, (ry)*(far_clip-near_clip)/5. ); 

       break;
    }

    //! Handle Translating
    case 2: 
    {
      double delta_x= 0,  delta_y =0; 

      GLdouble mvmatrix[16];
      memcpy(mvmatrix, identityMatrix, sizeof(identityMatrix));
      GLdouble wx,  wy,  wz;         // Origin mapped to screen space
      GLdouble wx1, wy1, wz1;        // returned world x, y, z coords  
      GLdouble last_x, last_y, last_z;  // returned world coords of last step

      mvmatrix[12] = x_trans - x_rot_center;
      mvmatrix[13] = y_trans - y_rot_center;
      mvmatrix[14] = z_trans - z_rot_center;

      mvmatrix[0]  = scale;
      mvmatrix[5]  = scale;
      mvmatrix[10] = scale;

      /*! Transform pixel coordinates to lower left origin
        note viewport[3] is height of window in pixels  */
      GLint real_y = viewport[3] - (GLint) e->y() - 1;

      /*! Assume the model is translated to (or near enough) the origin
      // and get the z-depth value (wz) at that point 
      */
      gluProject (x_rot_center, y_rot_center, z_rot_center, 
        mvmatrix, projmatrix, viewport, &wx, &wy, &wz);

      //! Convert the current screen coordinates to world coordinates
      gluUnProject ((GLdouble) e->x(), (GLdouble) real_y, (GLdouble) wz,
        mvmatrix, projmatrix, viewport, &wx1, &wy1, &wz1);

      //! Same process for the previous point
      GLint real_old_y = viewport[3] - (GLint) oldPos.y() - 1;
      gluUnProject ((GLdouble) oldPos.x(), (GLdouble) real_old_y,  wz,
        mvmatrix, projmatrix, viewport, &last_x, &last_y, &last_z);

      //! Find the delta translation
      delta_x = wx1 - last_x;
      delta_y = wy1 - last_y;

      setTranslation( delta_x, delta_y, 0 );

      break;
    }

  }
    oldPos = e->pos();
}

/*!
// Reset the graphics transformations to a default state
// Certain variables such as model center and near/far clip
// are not reset by this routine. If we reset a new model it 
// must set these values.  If we reset an existing model they
// will already be set and have not been modified.
*/
void GLInputWidget::reset()
{
   x_trans = y_trans = 0.0;
   //
   // TODO: access the FOV from a camera variable.  For now 35 is the FOV
   // that is set in the call to gluPerspective.
   //
   // Calculate the distance from the eye to the model.  Use half the
   // FOV as the angle and the spherical radius as the height. 
   //
   z_trans = -radius/(tan((35./2.)*M_PI/180.));
   //z_trans = (far_clip - near_clip) / 2;
   scale = 1.0;
   maxScale = 200.0;
   angle = 0.0;
   b_rotate = false;
   axis[0] = axis[1] = axis[2] = 0.0;
   lastPos[0] = lastPos[1] = lastPos[2] = 0.0;
   memcpy(objectXform, identityMatrix, sizeof(identityMatrix));
}



#define DEF_keyboardAngleStep  3.0 / 180
#define DEF_stepSize           .2

bool GLInputWidget::event(QEvent* e)
{
   // find the tab/shift+tab keypress event and intercept it
   if(e->type() == QEvent::KeyPress )
   {
      QKeyEvent* ke = (QKeyEvent*)e;
      keyPressEvent( ke );
      return ke->isAccepted();
   }
   else
      return QGLWidget::event(e);
}



void GLInputWidget::keyPressEvent( QKeyEvent *e )
{
   switch( e->key() )
   {
      case Key_Tab:
	 PickTool::instance()->step();
         e->accept();

      default:
         break;

      /*
      // Move forwards.
      case Key_W:
         setTranslation( 0, 0, -DEF_stepSize);
         updateGL();
         break;

      // Move backwards.
      case Key_S:
         setTranslation( 0, 0, DEF_stepSize);
         updateGL();
         break;

      // Strafe left.
      case Key_A:
         setTranslation( -DEF_stepSize, 0, 0);
         updateGL();
         break;

      // Strafe right.
      case Key_D:
         setTranslation( DEF_stepSize, 0, 0);
         updateGL();
         break;

      // Strafe up.
      case Key_R:
         setTranslation( 0, -DEF_stepSize, 0);
         updateGL();
         break;

      // Strafe down.
      case Key_F:
         setTranslation( 0, DEF_stepSize, 0);
         updateGL();
         break;

      // Roll to the left (counterclockwise).
      case Key_Q:
         setRotation(0, 0, -DEF_keyboardAngleStep );
         updateGL();
         break;

      // Roll to the right (clockwise).
      case Key_E:
         setRotation(0, 0, DEF_keyboardAngleStep );
         updateGL();
         break;

      // Orbit left.
      case Key_Left:
      {
         setRotation( 0, -DEF_keyboardAngleStep, 0 );
         updateGL();
         break;
       }

      // Orbit right.
      case Key_Right:
      {
         setRotation( 0, DEF_keyboardAngleStep, 0 );
         updateGL();
         break;
      }

      // Orbit up.
      case Key_Up:
         setRotation( -DEF_keyboardAngleStep, 0, 0 );
         updateGL();
         break;

      // Orbit down.
      case Key_Down:
         setRotation( DEF_keyboardAngleStep, 0, 0 );
         updateGL();
         break;

      case Key_Escape:            // ESC key.
         //exitMouseDrag( 0, 0 );
         //exit( 0 );
         break;

   */
   }
}

/*
void GLInputWidget::keyReleaseEvent( QKeyEvent *e )
{
   switch( e->key() )
   {
      default:
         break;
   }
}
*/

void GLInputWidget::get_near_far_clips(GLdouble& getnear, GLdouble& getfar )
{
	getnear = near_clip;
	getfar = far_clip;
}

              
