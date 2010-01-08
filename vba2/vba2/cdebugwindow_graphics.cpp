#include "cdebugwindow_graphics.h"

#include <QPainter>
#include <QMenu>
#include <QMouseEvent>


CDebugWindow_Graphics::CDebugWindow_Graphics( QWidget *parent )
  : QDialog( parent )
{
  resize( 256, 256 );
  move( parent->frameGeometry().topRight() );

  for( quint8 s = 0; s < nSurfaces; s++ ) {
    surface[s] = NULL;
    surface[s] = new QImage();
  }

  showBG0();

  menu = new QMenu( this );
  menu->addAction( tr("BG0"), this, SLOT( showBG0() ) );
  menu->addAction( tr("BG1"), this, SLOT( showBG1() ) );
  menu->addAction( tr("BG2"), this, SLOT( showBG2() ) );
  menu->addAction( tr("BG3"), this, SLOT( showBG3() ) );
}


CDebugWindow_Graphics::~CDebugWindow_Graphics()
{
  for( quint8 s = 0; s < nSurfaces; s++ ) {
    delete surface[s];
    surface[s] = NULL;
  }
}


bool CDebugWindow_Graphics::renderFrame( CGBAGraphics::RESULT &data ) {
  if( data.DISPCNT.displayBG[currentSurface] ) {
      delete surface[currentSurface];
      resize( data.BGIMAGE[currentSurface].width,
              data.BGIMAGE[currentSurface].height );
      surface[currentSurface] =
          new QImage( (uchar*)data.BGIMAGE[currentSurface].picture,
                      data.BGIMAGE[currentSurface].width,
                      data.BGIMAGE[currentSurface].height,
                      QImage::Format_ARGB32 );
  }
  repaint();
  return true;
}


void CDebugWindow_Graphics::paintEvent( QPaintEvent *event ) {
  QPainter p( this );
  p.drawImage( QPoint(0,0), *surface[currentSurface] );
}


void CDebugWindow_Graphics::mousePressEvent ( QMouseEvent *event ) {
  if( event->button() == Qt::RightButton ) {
    menu->popup( event->globalPos() );
  }
}


void CDebugWindow_Graphics::showBG0() {
  currentSurface = 0;
  setWindowTitle( tr("BG0") );
}


void CDebugWindow_Graphics::showBG1() {
  currentSurface = 1;
  setWindowTitle( tr("BG1") );
}


void CDebugWindow_Graphics::showBG2() {
  currentSurface = 2;
  setWindowTitle( tr("BG2") );
}


void CDebugWindow_Graphics::showBG3() {
  currentSurface = 3;
  setWindowTitle( tr("BG3") );
}
