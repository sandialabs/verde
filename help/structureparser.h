/****************************************************************************
** $Id: structureparser.h,v 1.1 2001-11-30 19:03:49 kmerkle Exp $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef STRUCTUREPARSER_H
#define STRUCTUREPARSER_H   

#include <qxml.h>

// forward references
class QString;
class QListView;
class HelpNavigationContentsItem;

class StructureParser : public QXmlDefaultHandler
{
public:

  StructureParser(QListView* list);
  ~StructureParser();

  bool startDocument();
  bool startElement( const QString&, const QString&, const QString& , 
                       const QXmlAttributes& );
  bool characters( const QString &ch);
  bool endElement( const QString&, const QString&, const QString& );

private:
  QListView*                  list_view;
  int                         current_depth;
  HelpNavigationContentsItem* parent;
  HelpNavigationContentsItem* current_item;
  HelpNavigationContentsItem* previous_item;
};                   

#endif 
