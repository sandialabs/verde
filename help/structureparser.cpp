/****************************************************************************
** $Id: structureparser.cpp,v 1.2.2.1 2004-08-17 17:21:44 rakerr Exp $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "structureparser.h"
#include "helpdialogimpl.h"

#ifdef CANT_USE_STD_IO
#include <iostream.h>
#else
#include <iostream>
#endif
#include <qstring.h>

 
StructureParser::StructureParser(QListView* list)
{
  list_view = list; 
  current_depth = 0;
  current_item = 0;
}
 
StructureParser::~StructureParser()
{
  list_view = 0; 
  current_depth = 0;
  current_item = 0;
}

bool StructureParser::startDocument()
{
    return TRUE;
}

bool StructureParser::startElement( const QString&, const QString&, 
                                    const QString& qName, 
                                    const QXmlAttributes& attr )
{
  if (qName == "title")
  {
    current_item = new HelpNavigationContentsItem( list_view, 0 );
    current_item->setOpen(true);
    parent = current_item;
    previous_item = current_item;
    if (attr.length() != 1)
    {
      std::cout << "Error parsing title of table of contents" << std::endl;
      return TRUE;
    }
    else
      current_item->setLink(attr.value(0));
  }
  else if (qName == "menu")
  {
    parent = current_item;
    current_depth++;
  }
  else if (qName == "a")
  {
    current_item = new HelpNavigationContentsItem( parent, previous_item );
    previous_item = current_item;
    if (attr.length() != 1)
    {
      std::cout << "Error parsing table of contents" << std::endl;
      return TRUE;
    }
    else
      current_item->setLink(attr.value(0));
  }
  return TRUE;
}

bool StructureParser::characters( const QString& string)
{
  QString tmp = string.simplifyWhiteSpace();
  if (current_item != 0 && tmp.length() > 1)
  {
    current_item->setText( 0, tmp );
  }
  return TRUE;
}

bool StructureParser::endElement( const QString&, const QString&, 
                                  const QString& qName)
{

  if (qName == "menu")
  {
    previous_item = parent;
    parent = (HelpNavigationContentsItem*) parent->parent();
    current_depth--;
  }
  return TRUE;
}
