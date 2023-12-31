#include "assistant.h"
#include <qbitmap.h>
#include <qpainter.h>
#include <qsizegrip.h>
#include <qtoolbutton.h>
#include <qpixmap.h>

#include "textbrowser.h"

static const char * close_xpm[] = {
"8 8 2 1",
"# c #000000",
". c None",
"##....##",
".##..##.",
"..####..",
"...##...",
"..####..",
".##..##.",
"##....##",
"........"};

Assistant::Assistant( QWidget *parent )
    : QWidget( parent, 0, WType_Dialog | WStyle_Customize | WStyle_NoBorder )
{
    QPalette pal = palette();
    pal.setBrush( QColorGroup::Base, QColor( 200, 217, 185 ) );
    pal.setBrush( QColorGroup::Background, QColor( 200, 217, 185 ) );
    pal.setBrush( QColorGroup::Button, QColor( 200, 217, 185 ) );
    setPalette( pal );
    sizeGrip = new QSizeGrip( this );
    sizeGrip->adjustSize();
    browser = new TextBrowser( this );
    browser->setFrameStyle( QFrame::NoFrame );
    browser->setText( tr( "Welcome to the <b>Qt Assistant</b>. Qt Assistant will give you quicker access to help and tips while using applications like Qt Designer." ) );
    closeButton = new QToolButton( this );
    closeButton->setPixmap( QPixmap( close_xpm ) );
    closeButton->setFixedSize( 16, 15 );
    closeButton->setAutoRaise( TRUE );
    connect( closeButton, SIGNAL( clicked() ), this, SLOT( hide() ) );
}

void Assistant::paintEvent( QPaintEvent * )
{
    QPainter p( this );
    p.setBrush( NoBrush );
    p.setPen( darkGreen );
    p.drawRect( rect() );
    p.setPen( black );
    p.drawText( 2, 16 - p.fontMetrics().descent(), tr( "Qt Assistant" ) );
    p.setPen( darkGreen );
    p.drawLine( 1, 16, width() - 2, 16 );
}

void Assistant::mousePressEvent( QMouseEvent *e )
{
    offset = e->pos();
}

void Assistant::mouseMoveEvent( QMouseEvent *e )
{
    move( e->globalPos() - offset );
}

void Assistant::resizeEvent( QResizeEvent * )
{
    closeButton->move( width() - closeButton->width() - 1, 1 );
    browser->setGeometry( 1, closeButton->height() + 2, width() - 2,
			  height() - sizeGrip->height() - 3 - closeButton->height() );
    sizeGrip->move( rect().bottomRight() - sizeGrip->rect().bottomRight() - QPoint( 1, 1 ) );
}
