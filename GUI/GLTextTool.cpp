//-------------------------------------------------------------------
// Filename          : GLTextTool.cpp
//
// Purpose           : Contains OpenGL commands labeling entities.
//
// Creator           : Karl Merkley
//
// Date              : 15 Jan 2002
//
// Owner             : Karl Merkley
//-------------------------------------------------------------------

/*! \file 
 * Basic usage:
 *      -# build_bitmap_font(Qfont("Times", 12) - builds the drawing lists based
 *      on a given font.  This has some OS dependencies and may need to be called
 *      after the window has been created.
 *      -# glRasterPos*() - uses the world coordinates to set the location where
 *      the label will be drawn.
 *      -# printGLf() - draws the string using a printf style argument. Note that
 *      the current total string length may not exceed 255 characters.
 */      

#include <iterator>

#include <qgl.h>

#ifndef WIN32
 #include <GL/glx.h>
#endif

#include "GLTextTool.hpp"
#include "../VerdeVector.hpp"


//! Draw integer labels at the specified positions
void GLTextTool::draw_labels(std::list<LabelData>& labels)
{
  std::list<LabelData>::iterator iter;

  for (iter = labels.begin(); iter != labels.end(); ++iter)
  {
    //! retrieve the world position of the points
    double x = (*iter).position.x();
    double y = (*iter).position.y();
    double z = (*iter).position.z();

    glPushMatrix();
      //! Note that glRasterPos* uses the world coordinates
      glRasterPos3d(x,y,z);
      printGLf("%d", (*iter).label);
    glPopMatrix();
  }
}

//! Build a bitmap font for either X11 or WIN32
GLvoid GLTextTool::build_bitmap_font(QWidget* graphics_window, QFont& font)
{
 // build 96 display lists out of our font starting at char 32 

  mCurrentFont = font;

  //! Initalize the drawing lists once and reuse them if the font changes
  if (!font_base)
    font_base = glGenLists(96);      /* storage for 96 characters */
  
  if(!font_base)
    return;


#ifndef WIN32

  // prevent compiler warning
  graphics_window = graphics_window;
  glXUseXFont( mCurrentFont.handle(), 32, 96, font_base);

#else
  
  HDC hDC = GetDC(graphics_window->winId());
  if(hDC == NULL)
    return;

  SelectObject(hDC, mCurrentFont.handle());
  wglUseFontBitmaps(hDC, 32, 96, font_base);

#endif

}


//! Called by the GLWindow destructor to clean up the bitmap font lists
GLvoid GLTextTool::delete_font()
{
  if(!font_base)
    return;

  glDeleteLists(font_base, 96);
}

/*! printf style method for displaying openGL text.  The
 *  drawing position must be specified with glRasterPos*() 
 *  before drawing.
 */
GLvoid GLTextTool::printGLf(const char *fmt, ...)
{
  if(!font_base)
    return;

  va_list ap;     /* our argument pointer */
  char text[256];

  if (fmt == NULL)    /* if there is no string to draw do nothing */
    return;


  va_start(ap, fmt);  /* make ap point to first unnamed arg */
    //TODO: Is this portable?? use vsprintf if not
#ifndef WIN32
    vsnprintf(text, 256, fmt, ap);
#else
	_vsnprintf(text, 256, fmt, ap);
#endif
  va_end(ap);

  glPushAttrib(GL_LIST_BIT);
    glListBase(font_base - 32);
    glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);
  glPopAttrib();
}


