/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Designer.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

static const char * book_xpm[]={
    "22 22 4 1",
    "# c #000000",
    "a c #808080",
    "b c #ffffff",
    ". c None",
    "......................",
    "......................",
    "......................",
    "......................",
    ".....##...............",
    "....#ab#....###.......",
    "....#abb#.##bb#.......",
    "....#abbb#abbb###.....",
    "....#abbb#bbbb#a#.....",
    "....#abbb#abbb#a#.....",
    "....#abbb#bbbb#a#.....",
    "....#abbb#abbb#a#.....",
    "....#abbb#bbbb#a#.....",
    ".....#abb#abb##a#.....",
    "......#ab#b##bba#.....",
    ".......#a##aaaaa#.....",
    ".......##a#######.....",
    "........##............",
    "......................",
    "......................",
    "......................",
    "......................"};


#include "helpdialogimpl.h"
#include "topicchooserimpl.h"
#include "mainwindow.h"
#include "structureparser.h"

#include <stdlib.h>

#include <qprogressbar.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qapplication.h>
#include <qprogressbar.h>
#include <qlabel.h>
#include <qframe.h>
#include <qtabwidget.h>
#include <qurl.h>
#include <qheader.h>
#include <qtimer.h>
#include <qlineedit.h>
#include <qfileinfo.h>
#include <qdir.h>
#include <qaccel.h>
#include <qregexp.h>
#include <qpixmap.h>

#include "textbrowser.h"

static QPixmap *bookPixmap = 0;

class SortableString : public QString
{
public:
    SortableString() {}
    SortableString( const QString& other )
	: QString( other ), key( other )
    {
	// "Chapter 1" becomes "chapter01"; "Chapter 12" becomes
	// "chapter102", which is wrong but good enough
	key.replace( QRegExp("(?=\\b[0-9]\\b)"), "0" );
	key.replace( QRegExp("\\W"), " " );
	key = key.stripWhiteSpace().lower();

	// use original string as second sort criterion
	key += QChar( ' ' ) + other;
    }
    QString key;
};

struct Entry
{
    QString link;
    QString title;
    int depth;
#if defined(Q_FULL_TEMPLATE_INSTANTIATION)
    bool operator==( const Entry& ) const { return FALSE; }
#endif
};

bool operator<=( const SortableString &s1, const SortableString &s2 )
{ return s1.key <= s2.key; }
bool operator<( const SortableString &s1, const SortableString &s2 )
{ return s1.key < s2.key; }
bool operator>( const SortableString &s1, const SortableString &s2 )
{ return s1.key > s2.key; }

HelpNavigationListItem::HelpNavigationListItem( QListBox *ls, const QString &txt )
    : QListBoxText( ls, txt )
{
}

void HelpNavigationListItem::addLink( const QString &link )
{
    int hash = link.find( '#' );
    if ( hash == -1 ) {
	linkList << link;
	return;
    }

    QString preHash = link.left( hash );
    if ( linkList.grep( preHash, FALSE ).count() > 0 )
	return;
    linkList << link;
}

HelpNavigationContentsItem::HelpNavigationContentsItem( QListView *v, QListViewItem *after )
    : QListViewItem( v, after )
{
}

HelpNavigationContentsItem::HelpNavigationContentsItem( QListViewItem *v, QListViewItem *after )
    : QListViewItem( v, after )
{
}

void HelpNavigationContentsItem::setLink( const QString &lnk )
{
    theLink = lnk;
}

QString HelpNavigationContentsItem::link() const
{
    return theLink;
}





HelpDialog::HelpDialog( QWidget *parent, MainWindow *h, TextBrowser *v )
    : HelpDialogBase( parent, 0, FALSE ), help( h ), viewer( v ), lwClosed( FALSE )
{
    bookPixmap = new QPixmap( book_xpm );
    documentationPath = QString( getenv( "VERDE_DIR" ) ) + "/doc";

    indexDone = FALSE;
    contentsDone = FALSE;
    contentsInserted = FALSE;
    bookmarksInserted = FALSE;
    editIndex->installEventFilter( this );
    listBookmarks->header()->hide();
    listBookmarks->header()->setStretchEnabled( TRUE );
    listContents->header()->hide();
    listContents->header()->setStretchEnabled( TRUE );
    framePrepare->hide();
    setupTitleMap();
    connect( qApp, SIGNAL(lastWindowClosed()), SLOT(lastWinClosed()) );
}

void HelpDialog::lastWinClosed()
{
    lwClosed = TRUE;
}

void HelpDialog::loadIndexFile()
{
    if ( indexDone )
	return;

    setCursor( waitCursor );
    indexDone = TRUE;
    framePrepare->show();
    qApp->processEvents();

    QString indexFile = documentationPath + "/verde.idx";

    QProgressBar *bar = progressPrepare;
    bar->setTotalSteps( QFileInfo( indexFile ).size()  );
    bar->setProgress( 0 );


    HelpNavigationListItem *lastItem = 0;

    //### if constructed on stack, it will crash on WindowsNT
    QValueList<SortableString>* lst = new QValueList<SortableString>;
    bool buildDb = TRUE;
    QFile f( indexFile );
    if ( QFile::exists( QDir::homeDirPath() + "/.vindexdb" ) ) {
	QFile indexin( QDir::homeDirPath() + "/.vindexdb" );
	if ( !indexin.open( IO_ReadOnly ) )
	    goto build_db;

	QDataStream ds( &indexin );
	QDateTime dt;
	uint size;
	ds >> dt;
	ds >> size;
	if ( size != f.size()  || dt != QFileInfo( f ).lastModified() )
	    goto build_db;

	ds >> *lst;
	indexin.close();
	bar->setProgress( bar->totalSteps() );
	qApp->processEvents();
	buildDb = FALSE;
    }

 build_db:
    if ( buildDb ) {
	if ( f.open( IO_ReadOnly ) ) {
	    QTextStream ts( &f );
	    while ( !ts.atEnd() && !lastWindowClosed() ) {
		qApp->processEvents();
		if ( lastWindowClosed() )
		    break;
		QString l = ts.readLine();
		if ( l.find( "::" ) != -1 ) {
		    int i = l.find( "\"" ) + 1;
		    l.remove( i, l.find( "::" ) + 2 - i );
		}
		lst->append( l );
		if ( bar )
		    bar->setProgress( bar->progress() + l.length() );
	    }
	    if ( lastWindowClosed() ) {
		delete lst;
		return;
	    }
	}
	if ( !lst->isEmpty() ) {
	    qHeapSort( *lst );

	    QFile indexout( QDir::homeDirPath() + "/.vindexdb" );
	    if ( indexout.open( IO_WriteOnly ) ) {
		QDataStream s( &indexout );
		s << QFileInfo( f ).lastModified();
		s << f.size();
		s << *lst;
	    }
	    indexout.close();
	} else {
	    indexDone = FALSE;
	}
    }

    QValueList<SortableString>::Iterator it = lst->begin();
    for ( ; it != lst->end(); ++it ) {
	QString s( *it );
	if ( s.find( "::" ) != -1 )
	    continue;
	if ( s[1] == '~' )
	    continue;
	if ( s.find( "http://" ) != -1 ||
	     s.find( "ftp://" ) != -1 ||
	     s.find( "mailto:" ) != -1 )
	    continue;
	int from = s.find( "\"" );
	if ( from == -1 )
	    continue;
	int to = s.findRev( "\"" );
	if ( to == -1 )
	    continue;
	QString link = s.mid( to + 2 );
	s = s.mid( from + 1, to - from - 1 );

	if ( s.isEmpty() )
	    continue;
	s = s.replace( QRegExp( "\\\\" ), "" );
	if ( !lastItem || lastItem->text() != s )
	    lastItem = new HelpNavigationListItem( listIndex, s );
	lastItem->addLink( link );
    }

    delete lst;
    f.close();

    framePrepare->hide();
    setCursor( arrowCursor );
}

void HelpDialog::setupTitleMap()
{
    if ( contentsDone )
	return;
    contentsDone = TRUE;
    QString titleFile  = documentationPath + "/titleindex";
    QFile f2( titleFile );
    bool buildDb = TRUE;
    if ( QFile::exists( QDir::homeDirPath() + "/.titlemapdb" ) ) {
	QFile titlein( QDir::homeDirPath() + "/.titlemapdb" );
	if ( !titlein.open( IO_ReadOnly ) )
	    goto build_db2;

	QDataStream ds( &titlein );
	QDateTime dt;
	uint size;
	ds >> dt;
	ds >> size;
	if ( size != f2.size() || dt != QFileInfo( f2 ).lastModified() )
	    goto build_db2;
	ds >> titleMap;
	titlein.close();
	qApp->processEvents();
	buildDb = FALSE;
    }

 build_db2:

    if ( buildDb ) {
	if ( !f2.open( IO_ReadOnly ) )
	    return;
	QTextStream ts2( &f2 );
	while ( !ts2.atEnd() ) {
	    QString s = ts2.readLine();
	    int pipe = s.find( '|' );
	    if ( pipe == -1 )
		continue;
	    QString title = s.left( pipe - 1 );
	    QString link = s.mid( pipe + 1 );
	    link = link.simplifyWhiteSpace();
	    titleMap[ link ] = title.stripWhiteSpace();
	}

	QFile titleout( QDir::homeDirPath() + "/.titlemapdb" );
	if ( titleout.open( IO_WriteOnly ) ) {
	    QDataStream s( &titleout );
	    s << QFileInfo( f2 ).lastModified();
	    s << f2.size();
	    s << titleMap;
	}
	titleout.close();
    }
}

void HelpDialog::currentTabChanged( const QString &s )
{
    if ( s.contains( tr( "Index" ) ) ) {
	if ( !indexDone )
	    QTimer::singleShot( 100, this, SLOT( loadIndexFile() ) );
    } else if ( s.contains( tr( "Bookmarks" ) ) ) {
	if ( !bookmarksInserted )
	    insertBookmarks();
    } else if ( s.contains( tr( "Con&tents" ) ) ) {
	if ( !contentsInserted )
	    insertContents();
    }
}

void HelpDialog::currentIndexChanged( QListBoxItem * )
{
}

void HelpDialog::showTopic()
{
    if ( tabWidget->tabLabel( tabWidget->currentPage() ).contains( tr( "Index" ) ) )
	showIndexTopic();
    else if ( tabWidget->tabLabel( tabWidget->currentPage() ).contains( tr( "Bookmarks" ) ) )
	showBookmarkTopic();
    else if ( tabWidget->tabLabel( tabWidget->currentPage() ).contains( tr( "Con&tents" ) ) )
	showContentsTopic();
}

void HelpDialog::showIndexTopic()
{
    QListBoxItem *i = listIndex->item( listIndex->currentItem() );
    if ( !i )
	return;

    editIndex->blockSignals( TRUE );
    editIndex->setText( i->text() );
    editIndex->blockSignals( FALSE );

    HelpNavigationListItem *item = (HelpNavigationListItem*)i;

    QStringList links = item->links();
    if ( links.count() == 1 ) {
	emit showLink( links.first(), item->text() );
    } else {
	QStringList::Iterator it = links.begin();
	QStringList linkList;
	QStringList linkNames;
	for ( ; it != links.end(); ++it ) {
	    linkList << *it;
	    linkNames << titleOfLink( *it );
	}
	QString link = TopicChooser::getLink( this, linkNames, linkList, i->text() );
	if ( !link.isEmpty() )
	    emit showLink( link, i->text() );
    }
}

void HelpDialog::searchInIndex( const QString &s )
{
    QListBoxItem *i = listIndex->firstItem();
    QString sl = s.lower();
    while ( i ) {
	QString t = i->text();
	if ( t.length() >= sl.length() &&
	     i->text().left(s.length()).lower() == sl ) {
	    listIndex->setCurrentItem( i );
	    break;
	}
	i = i->next();
    }
}

QString HelpDialog::titleOfLink( const QString &link )
{
    QUrl u( link );
    QString s = titleMap[ u.fileName() ];
    if ( s.isEmpty() )
	return link;
    return s;
}

bool HelpDialog::eventFilter( QObject * o, QEvent * e )
{
    if ( !o || !e )
	return TRUE;

    if ( o == editIndex && e->type() == QEvent::KeyPress ) {
	QKeyEvent *ke = (QKeyEvent*)e;
	if ( ke->key() == Key_Up ) {
	    int i = listIndex->currentItem();
	    if ( --i >= 0 ) {
		listIndex->setCurrentItem( i );
		editIndex->blockSignals( TRUE );
		editIndex->setText( listIndex->currentText() );
		editIndex->blockSignals( FALSE );
	    }
	    return TRUE;
	} else if ( ke->key() == Key_Down ) {
	    int i = listIndex->currentItem();
	    if ( ++i < int(listIndex->count()) ) {
		listIndex->setCurrentItem( i );
		editIndex->blockSignals( TRUE );
		editIndex->setText( listIndex->currentText() );
		editIndex->blockSignals( FALSE );
	    }
	    return TRUE;
	} else if ( ke->key() == Key_Next || ke->key() == Key_Prior ) {
	    QApplication::sendEvent( listIndex, e);
	    editIndex->blockSignals( TRUE );
	    editIndex->setText( listIndex->currentText() );
	    editIndex->blockSignals( FALSE );
	}
    }

    return QWidget::eventFilter( o, e );
}

void HelpDialog::addBookmark()
{
    if ( !bookmarksInserted )
	insertBookmarks();
    QString link = QUrl( viewer->context(), viewer->source() ).path();
    QString title = titleOfLink( link );
    HelpNavigationContentsItem *i = new HelpNavigationContentsItem( listBookmarks, 0 );
    i->setText( 0, title );
    i->setLink( QUrl( link ).fileName() );
    saveBookmarks();
    help->updateBookmarkMenu();
}

void HelpDialog::removeBookmark()
{
    if ( !listBookmarks->currentItem() )
	return;

    delete listBookmarks->currentItem();
    saveBookmarks();
    help->updateBookmarkMenu();
}

void HelpDialog::insertBookmarks()
{
    if ( bookmarksInserted )
	return;
    bookmarksInserted = TRUE;
    QFile f( QDir::homeDirPath() + "/.verde_bookmarks" );
    if ( !f.open( IO_ReadOnly ) )
	return;
    QTextStream ts( &f );
    while ( !ts.atEnd() ) {
	HelpNavigationContentsItem *i = new HelpNavigationContentsItem( listBookmarks, 0 );
	i->setText( 0, ts.readLine() );
	i->setLink( ts.readLine() );
    }
    help->updateBookmarkMenu();
}

void HelpDialog::currentBookmarkChanged( QListViewItem * )
{
}

void HelpDialog::showBookmarkTopic()
{
    if ( !listBookmarks->currentItem() )
	return;

    HelpNavigationContentsItem *i = (HelpNavigationContentsItem*)listBookmarks->currentItem();
    emit showLink( i->link(), i->text( 0 ) );

}

void HelpDialog::saveBookmarks()
{
    QFile f( QDir::homeDirPath() + "/.verde_bookmarks" );
    if ( !f.open( IO_WriteOnly ) )
	return;
    QTextStream ts( &f );
    QListViewItemIterator it( listBookmarks );
    for ( ; it.current(); ++it ) {
	HelpNavigationContentsItem *i = (HelpNavigationContentsItem*)it.current();
	ts << i->text( 0 ) << endl;
	ts << i->link() << endl;
    }
    f.close();
}

void HelpDialog::insertContents()
{
    if ( contentsInserted )
	return;
    contentsInserted = TRUE;
    if ( !contentsDone )
	setupTitleMap();

    listContents->setSorting( -1 );

    // Parse the modified Table of Contents and build a contents document
    QString tocName = documentationPath + "/verde_toc.xml";
    QFile xmlFile(tocName);
    QXmlInputSource source(&xmlFile);
    QXmlSimpleReader reader;
    StructureParser handler( listContents );
    reader.setContentHandler( &handler) ;
    reader.parse( source );
}

void HelpDialog::currentContentsChanged( QListViewItem * )
{
}

void HelpDialog::showContentsTopic()
{
    HelpNavigationContentsItem *i = (HelpNavigationContentsItem*)listContents->currentItem();
    emit showLink( i->link(), i->text( 0 ) );
}

void HelpDialog::toggleContents()
{
    if ( !isVisible() || tabWidget->currentPageIndex() != 0 ) {
	tabWidget->setCurrentPage( 0 );
	parentWidget()->show();
    }
    else
	parentWidget()->hide();
}

void HelpDialog::toggleIndex()
{
    if ( !isVisible() || tabWidget->currentPageIndex() != 1 || !editIndex->hasFocus() ) {
	tabWidget->setCurrentPage( 1 );
	parentWidget()->show();
	editIndex->setFocus();
    }
    else
	parentWidget()->hide();
}

void HelpDialog::toggleBookmarks()
{
    if ( !isVisible() || tabWidget->currentPageIndex() != 2 ) {
	tabWidget->setCurrentPage( 2 );
	parentWidget()->show();
    }
    else
	parentWidget()->hide();
}
