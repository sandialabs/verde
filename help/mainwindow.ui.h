#include <qtabwidget.h>
#include <qaccel.h>
#include <qdir.h>

QPtrList<MainWindow> *MainWindow::windows = 0;

void MainWindow::init()
{
    if ( !windows )
	windows = new QPtrList<MainWindow>;
    windows->append( this );
    setWFlags( WDestructiveClose );
    browser = new HelpWindow( this, this, "verde_helpwindow" );
    browser->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    QAccel *acc = new QAccel( this );
    acc->connectItem( acc->insertItem( Key_F5 ),
		      browser,
		      SLOT( reload() ) );
    setCentralWidget( browser );

    settings = 0L;

    // Get the path from the environment.  Fail if it is not set.
    QString docPath = QString( getenv("VERDE_DIR") );

    if (docPath == NULL)
    {
      QMessageBox::critical(this,"Verde Help Error", 
          "You must set the environment variable\n VERDE_DIR to run Verde Help",
          QMessageBox::Abort, 
          QMessageBox::NoButton,
          QMessageBox::NoButton);

      exit(0);
    }

    docPath = docPath + "/doc";
    browser->mimeSourceFactory()->addFilePath(docPath);

    connect( actionGoPrev, SIGNAL( activated() ), browser, SLOT( backward() ) );
    connect( actionGoNext, SIGNAL( activated() ), browser, SLOT( forward() ) );
    connect( actionEditCopy, SIGNAL( activated() ), browser, SLOT( copy() ) );
    connect( actionFileExit, SIGNAL( activated() ), qApp, SLOT( closeAllWindows() ) );

    QDockWindow *dw = new QDockWindow;
    helpDock = new HelpDialog( dw, this, browser );
    dw->setResizeEnabled( TRUE );
    dw->setCloseMode( QDockWindow::Always );
    addDockWindow( dw, DockLeft );
    dw->setWidget( helpDock );
    dw->setCaption( "Sidebar" );
    dw->setFixedExtentWidth( 250 );

    connect( helpDock, SIGNAL( showLink( const QString&, const QString & ) ),
	     this, SLOT( showLink( const QString&, const QString & ) ) );

    connect( bookmarkMenu, SIGNAL( activated( int ) ),
	     this, SLOT( showBookmark( int ) ) );

    setupBookmarkMenu();
    connect( browser, SIGNAL( highlighted( const QString & ) ), statusBar(), SLOT( message( const QString & ) ) );
    connect( actionZoomIn, SIGNAL( activated() ), browser, SLOT( zoomIn() ) );
    connect( actionZoomOut, SIGNAL( activated() ), browser, SLOT( zoomOut() ) );

    PopupMenu->insertItem( tr( "Vie&ws" ), createDockWindowMenu() );

    QAccel *a = new QAccel( this, dw );
    a->connectItem( a->insertItem( QAccel::stringToKey( tr("Ctrl+T") ) ), helpDock, SLOT( toggleContents() ) );
    a->connectItem( a->insertItem( QAccel::stringToKey( tr("Ctrl+I") ) ), helpDock, SLOT( toggleIndex() ) );
    a->connectItem( a->insertItem( QAccel::stringToKey( tr("Ctrl+B") ) ), helpDock, SLOT( toggleBookmarks() ) );
    a = new QAccel( dw );
    a->connectItem( a->insertItem( QAccel::stringToKey( tr("Ctrl+T") ) ), helpDock, SLOT( toggleContents() ) );
    a->connectItem( a->insertItem( QAccel::stringToKey( tr("Ctrl+I") ) ), helpDock, SLOT( toggleIndex() ) );
    a->connectItem( a->insertItem( QAccel::stringToKey( tr("Ctrl+B") ) ), helpDock, SLOT( toggleBookmarks() ) );

/*
    // read configuration
    QString keybase("/Qt Assistant/3.0/");
    QSettings config;
    config.insertSearchPath( QSettings::Windows, "/Trolltech" );

    QFont fnt( browser->QWidget::font() );
    fnt.setFamily( config.readEntry( keybase + "Family", fnt.family() ) );
    fnt.setPointSize( config.readNumEntry( keybase + "Size", fnt.pointSize() ) );
    browser->setFont( fnt );
    browser->setLinkUnderline( config.readBoolEntry( keybase + "LinkUnderline", TRUE ) );

    QPalette pal = browser->palette();
    QColor lc( config.readEntry( keybase + "LinkColor", pal.color( QPalette::Active, QColorGroup::Link ).name() ) );
    pal.setColor( QPalette::Active, QColorGroup::Link, lc );
    pal.setColor( QPalette::Inactive, QColorGroup::Link, lc );
    pal.setColor( QPalette::Disabled, QColorGroup::Link, lc );
    browser->setPalette( pal );

    QString family = config.readEntry( keybase + "FixedFamily", browser->styleSheet()->item( "pre" )->fontFamily() );

    QStyleSheet *sh = browser->styleSheet();
    sh->item( "pre" )->setFontFamily( family );
    sh->item( "code" )->setFontFamily( family );
    sh->item( "tt" )->setFontFamily( family );
    browser->setStyleSheet( sh );

    QApplication::sendPostedEvents();
    QString fn = QDir::homeDirPath() + "/.assistanttbrc";
    QFile f( fn );
    if ( f.open( IO_ReadOnly ) ) {
	QTextStream ts( &f );
	ts >> *this;
	f.close();
    }

    helpDock->tabWidget->setCurrentPage( config.readNumEntry( keybase + "SideBarPage", 0 ) );

    if ( !config.readBoolEntry( keybase  + "GeometryMaximized", FALSE ) ) {
	QRect r( pos(), size() );
	r.setX( config.readNumEntry( keybase + "GeometryX", r.x() ) );
	r.setY( config.readNumEntry( keybase + "GeometryY", r.y() ) );
	r.setWidth( config.readNumEntry( keybase + "GeometryWidth", r.width() ) );
	r.setHeight( config.readNumEntry( keybase + "GeometryHeight", r.height() ) );

	QRect desk = QApplication::desktop()->geometry();
	QRect inter = desk.intersect( r );
	resize( r.size() );
	if ( inter.width() * inter.height() > ( r.width() * r.height() / 20 ) ) {
	    move( r.topLeft() );
	}
    }

    QString source = config.readEntry( keybase + "Source", QString::null );
    QString title = config.readEntry( keybase + "Title", source );
    if ( !source.isEmpty() )
	showLink( source, title );
    else
*/
	goHome();
}

void MainWindow::destroy()
{
    windows->removeRef( this );
    if ( windows->isEmpty() ) {
	delete windows;
	windows = 0;
    }
    saveSettings();
}

void MainWindow::about()
{
    static const char *about_text =
    "<p><b>Verde Help is based on the Qt Assistant</b></p>"
    "<p>The Qt documentation browser.</p>"
    "<p>Version 1.0</p>"
    "<p>(C) 2001 Trolltech AS</p>";
    QMessageBox::about( this, tr("Verde Help"), tr( about_text ) );
}

void MainWindow::aboutQt()
{
    QMessageBox::aboutQt( this, tr( "Verde Help" ) );
}

void MainWindow::find()
{
    if ( !findDialog ) {
	findDialog = new FindDialog( this );
	findDialog->setBrowser( browser );
    }
    findDialog->comboFind->setFocus();
    findDialog->comboFind->lineEdit()->setSelection(
        0, findDialog->comboFind->lineEdit()->text().length() );
    findDialog->show();
}

void MainWindow::goHome()
{
    // #### we need a general Qt frontpage with links to Qt Class docu, Designer Manual, Linguist Manual, etc,
    //showLink( "index.html", "Qt Reference Documentation" );
    showLink( "verde.html", "Verde Reference Documentation" );
}

void MainWindow::showLinguistHelp()
{
    showLink( "linguist-manual.html", tr( "Qt Linguist Manual" ) );
}

void MainWindow::print()
{
    QPrinter printer;
    printer.setFullPage(TRUE);
    if ( printer.setup( this ) ) {
	QPaintDeviceMetrics screen( this );
	printer.setResolution( screen.logicalDpiY() );
	QPainter p( &printer );
	QPaintDeviceMetrics metrics(p.device());
	int dpix = metrics.logicalDpiX();
	int dpiy = metrics.logicalDpiY();
	const int margin = 72; // pt
	QRect body( margin*dpix/72, margin*dpiy/72,
		    metrics.width()-margin*dpix/72*2,
		    metrics.height()-margin*dpiy/72*2 );
	QFont font( browser->font() );
	QStringList filePaths = browser->mimeSourceFactory()->filePath();
	QString file;
	QStringList::Iterator it = filePaths.begin();
	for ( ; it != filePaths.end(); ++it ) {
	    file = QUrl( *it, QUrl( browser->source() ).path() ).path();
	    if ( QFile::exists( file ) )
		break;
	    else
		file = QString::null;
	}
	if ( file.isEmpty() )
	    return;
	QFile f( file );
	if ( !f.open( IO_ReadOnly ) )
	    return;
	QTextStream ts( &f );
	QSimpleRichText richText( ts.read(), font, browser->context(), browser->styleSheet(),
				  browser->mimeSourceFactory(), body.height(),
				  Qt::black, FALSE );
	richText.setWidth( &p, body.width() );
	QRect view( body );
	int page = 1;
	do {
	    richText.draw( &p, body.left(), body.top(), view, colorGroup() );
	    view.moveBy( 0, body.height() );
	    p.translate( 0 , -body.height() );
	    p.setFont( font );
	    p.drawText( view.right() - p.fontMetrics().width( QString::number(page) ),
			view.bottom() + p.fontMetrics().ascent() + 5, QString::number(page) );
	    if ( view.top()  >= richText.height() )
		break;
	    printer.newPage();
	    page++;
	} while (TRUE);
    }
}

void MainWindow::updateBookmarkMenu()
{
    for ( MainWindow *mw = windows->first(); mw; mw = windows->next() )
	mw->setupBookmarkMenu();
}

void MainWindow::setupBookmarkMenu()
{
    bookmarkMenu->clear();
    bookmarks.clear();
    bookmarkMenu->insertItem( tr( "&Add Bookmark" ), helpDock, SLOT( addBookmark() ) );

    QFile f( QDir::homeDirPath() + "/.bookmarks" );
    if ( !f.open( IO_ReadOnly ) )
	return;
    QTextStream ts( &f );
    bookmarkMenu->insertSeparator();
    while ( !ts.atEnd() ) {
	QString title = ts.readLine();
	QString link = ts.readLine();
	bookmarks.insert( bookmarkMenu->insertItem( title ), link );
    }
}

void MainWindow::showBookmark( int id )
{
    if ( bookmarks.find( id ) != bookmarks.end() )
	showLink( *bookmarks.find( id ), bookmarkMenu->text( id ) );
}

void MainWindow::showDesignerHelp()
{
    showLink( "designer-manual.html", tr( "Qt Designer Manual" ) );
}

void MainWindow::showLink( const QString & link, const QString & title )
{
    browser->setCaption( title );
    browser->setSource( link );
    browser->setFocus();
}

void MainWindow::showQtHelp()
{
    //showLink( "index.html", tr( "Qt Reference Documentation" ) );
    showLink( "verde.html", "Verde Reference Documentation" );
}

void MainWindow::setFamily( const QString & f )
{
    QFont fnt( browser->QWidget::font() );
    fnt.setFamily( f );
    browser->setFont( fnt );
}

void MainWindow::showSettingsDialog()
{
    if ( !settings )
	settings = new SettingsDialog( this );

    QFontDatabase fonts;
    settings->fontCombo->insertStringList( fonts.families() );
    settings->fontCombo->lineEdit()->setText( browser->QWidget::font().family() );
    settings->fixedfontCombo->insertStringList( fonts.families() );
    settings->fixedfontCombo->lineEdit()->setText( browser->styleSheet()->item( "pre" )->fontFamily() );
    settings->linkUnderlineCB->setChecked( browser->linkUnderline() );
    settings->colorButton->setPaletteBackgroundColor( browser->palette().color( QPalette::Active, QColorGroup::Link ) );

    int ret = settings->exec();

    if ( ret != QDialog::Accepted )
	return;

    QFont fnt( browser->QWidget::font() );
    fnt.setFamily( settings->fontCombo->currentText() );
    browser->setFont( fnt );
    browser->setLinkUnderline( settings->linkUnderlineCB->isChecked() );

    QPalette pal = browser->palette();
    QColor lc = settings->colorButton->paletteBackgroundColor();
    pal.setColor( QPalette::Active, QColorGroup::Link, lc );
    pal.setColor( QPalette::Inactive, QColorGroup::Link, lc );
    pal.setColor( QPalette::Disabled, QColorGroup::Link, lc );
    browser->setPalette( pal );

    QString family = settings->fixedfontCombo->currentText();

    QStyleSheet *sh = browser->styleSheet();
    sh->item( "pre" )->setFontFamily( family );
    sh->item( "code" )->setFontFamily( family );
    sh->item( "tt" )->setFontFamily( family );
    browser->setStyleSheet( sh );
}

void MainWindow::hide()
{
    saveToolbarSettings();
    QMainWindow::hide();
}


MainWindow* MainWindow::newWindow()
{
    saveSettings();
    saveToolbarSettings();
    MainWindow *mw = new MainWindow;
    mw->move( geometry().topLeft() );
    if ( isMaximized() )
	mw->showMaximized();
    else
	mw->show();
    return mw;
}

void MainWindow::saveSettings()
{
   /* QString keybase("/Qt Assistant/3.0/");
    QSettings config;
    config.insertSearchPath( QSettings::Windows, "/Trolltech" );
    config.writeEntry( keybase + "Family",  browser->QWidget::font().family() );
    config.writeEntry( keybase + "Size",  browser->QWidget::font().pointSize() );
    config.writeEntry( keybase + "FixedFamily", browser->styleSheet()->item( "pre" )->fontFamily() );
    config.writeEntry( keybase + "LinkUnderline", browser->linkUnderline() );
    config.writeEntry( keybase + "LinkColor", browser->palette().color( QPalette::Active, QColorGroup::Link ).name() );
    config.writeEntry( keybase + "Source", browser->source() );
    config.writeEntry( keybase + "Title", browser->caption() );
    config.writeEntry( keybase + "SideBarPage", helpDock->tabWidget->currentPageIndex() );
    config.writeEntry( keybase + "GeometryX", x() );
    config.writeEntry( keybase + "GeometryY", y() );
    config.writeEntry( keybase + "GeometryWidth", width() );
    config.writeEntry( keybase + "GeometryHeight", height() );
    config.writeEntry( keybase + "GeometryMaximized", isMaximized() );
    */
}

void MainWindow::saveToolbarSettings()
{
    /*
    QString fn = QDir::homeDirPath() + "/.assistanttbrc";
    QFile f( fn );
    f.open( IO_WriteOnly );
    QTextStream ts( &f );
    ts << *this;
    f.close();
    */
}
