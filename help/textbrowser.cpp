#include "textbrowser.h"
#include <qevent.h>
#include <qmime.h>
#include <qstring.h>
#include <qtextbrowser.h>
#include <qcursor.h>

void TextBrowser::setSource(const QString& name)
{
  QTextBrowser::setSource(name);

  // Simulate a mouse move event to force a check to see if the
  // cursor is over the link.

  QPoint global_loc = QCursor::pos();
  QPoint widget_loc = this->mapFromGlobal(global_loc);

  QMouseEvent event(QEvent::MouseMove, widget_loc, 0, 0);

  contentsMouseMoveEvent( &event );
}

