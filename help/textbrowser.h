#ifndef _TEXTBROWSER_
#define _TEXTBROWSER_

#include <qtextbrowser.h>

class TextBrowser : public QTextBrowser
{
  Q_OBJECT

public:
  TextBrowser( QWidget* parent=0, const char* name=0 ) :
    QTextBrowser(parent, name){}
  ~TextBrowser(){}

public slots:
   virtual void setSource(const QString& name);
};
#endif


