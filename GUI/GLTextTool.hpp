//-------------------------------------------------------------------
// Filename          : GLTextTool.hpp
//
// Purpose           : Contains OpenGL commands labeling entities.
//
// Creator           : Karl Merkley
//
// Date              : 15 Jan 2002
//
// Owner             : Karl Merkley
//-------------------------------------------------------------------

#ifndef GL_TEXT_TOOL
#define GL_TEXT_TOOL

#include <list>
#include <qfont.h>

#include <stdarg.h>

#include "../VerdeVector.hpp"

/*! Sample struct that can be used to store label information
 * Currently not used by Verde */
struct LabelData
{
  int         label;
  VerdeVector position;
};

//! Tool for displaying bitmap text data in an OpenGL window
class GLTextTool
{
  public:
    //! constructor 
   GLTextTool() : font_base(0), mCurrentFont(QFont()) {}

   //! clean up the drawing lists and unmark the initialed flag
   GLvoid delete_font();

   /*! Create the bitmap font drawing list based on font.  This
    * function reuses the drawing lists if the font changes */
   GLvoid build_bitmap_font(QWidget* graphics_window, QFont& font );
   
   //! variable arg function for drawing strings to the GL window
   //
   GLvoid printGLf(const char *fmt, ...);

   //! Not currently used, but potentially useful function for drawing labels
   void draw_labels(std::list<LabelData>& labels);

   //! returns the current font
   QFont current_font(){ return mCurrentFont; }

  private:
   GLuint font_base;     //!< base display list of our font set 
   QFont mCurrentFont;
};

#endif
