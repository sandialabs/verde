//----------------------------------------------------------------------
// Filename         : GLWindow.hpp
//
// Purpose          : Class derived from a QT OpenGL class which handles 
//                    all keyboard and mouse inputs and creates 
//                    graphics transformations
//
// Creator          : Karl Merkley
//
// Date             : 7 Aug 2001
//
// Owner            : Karl Merkley
//----------------------------------------------------------------------
//
#ifndef GLCONTROLWIDGET_H
#define GLCONTROLWIDGET_H

#include <list>
#include <qgl.h>

#ifdef WIN32
#define M_PI 3.14159265358979323846
#endif


//! Handles inputs to the graphics window
class GLInputWidget : public QGLWidget
{

public:

    //! constructor
    GLInputWidget( QWidget *parent=0, const char *name=0);

    //! destructor
    virtual ~GLInputWidget();
    
    //! transfroms modelview matrix
    virtual void transform();

    //! resets graphic tranformations to a default state, ie model center, near/far clip planes
    void         reset();

    //! sets the model scale
    void         setScale( double s );

    //! sets X translations
    void         setXTrans( double x );
    //! sets Y translations
    void         setYTrans( double y );
    //! sets Z translations
    void         setZTrans( double z );
    //! sets model center (same as center of rotation)
    void         setModelCenter( double x, double y, double z);
    //! sets model centroid (same as center of rotation)
    void         setModelCentroid( double x, double y, double z);
    //! sets model size (used to fit model to view port)
    void         setModelSize( double r );
    //! sets angle for rotation
    void         setRotationAngle(double a){angle = a;}
   
    //! sets the total translation and update the graphics afterwards 
    virtual void setTranslation( double x, double y, double z );

    //! sets perspective flag
    void         set_perspective( bool value ) { perspectiveFlag = value; }

    //! gets perspective flag
    bool         get_perspective() { return perspectiveFlag; }

    //!  returns the near and far clips
    void         get_near_far_clips(GLdouble& near, GLdouble& far );

    //! get and set the quick transform feature
    void         quick_transform(bool val) { mQuickTransform = val; }
    bool         quick_transform() { return mQuickTransform; }

protected:
    void         mousePressEvent( QMouseEvent *e );
    void         mouseReleaseEvent( QMouseEvent *e );
    void         mouseMoveEvent( QMouseEvent * );
    void         trackball(int x, int y, float v[3]);

//  void         focusOutEvent( QFocusEvent* event);

// The following are optional events that could be defined later.
//  void         mouseDoubleClickEvent( QMouseEvent * );
//  void         wheelEvent( QWheelEvent * );

//  void         showEvent( QShowEvent * );
//  void         hideEvent( QHideEvent * );
    void         keyPressEvent( QKeyEvent *e );
//  void         keyReleaseEvent( QKeyEvent *e );
    bool         event( QEvent* e );

    GLdouble     x_trans, y_trans, z_trans;
    GLdouble     x_rot_center, y_rot_center, z_rot_center;
    GLdouble     x_centroid, y_centroid, z_centroid;
    GLdouble     near_clip, far_clip;
    GLdouble     scale;
    GLdouble     maxScale;
    GLdouble     radius;
//  bool         is_in_circle; 

    //! this is the original value the model is pushed back to
    //! when it is initially displayed (screen is z = 0)
    GLdouble z_original;

    GLint    viewport[4];
    float    angle; 
    float    axis[3];
    GLdouble objectXform[16];
    
    //! mouse is down flag
    bool     mMouseDown;
    bool     mQuickTransform;

private:

    GLdouble projmatrix[16];
    QPoint   oldPos;
    float    lastPos[3];
    int      start_x, start_y;
    bool     b_rotate;


    //! flag to tell if we are in perpsective or orthor. viewing
    bool     perspectiveFlag;
    
};

#endif
