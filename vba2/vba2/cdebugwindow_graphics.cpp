#include "cdebugwindow_graphics.h"

#include <QPainter>


CDebugWindow_Graphics::CDebugWindow_Graphics( QWidget *parent )
  : QDialog( parent )
{
  resize( 256, 256 );
  move( parent->frameGeometry().topRight() );

  for( quint8 s = 0; s < nSurfaces; s++ ) {
    surface[s] = NULL;
    surface[s] = new QImage();
  }
}


CDebugWindow_Graphics::~CDebugWindow_Graphics()
{
  for( quint8 s = 0; s < nSurfaces; s++ ) {
    delete surface[s];
    surface[s] = NULL;
  }
}


bool CDebugWindow_Graphics::renderFrame( CGBAGraphics::RESULT &data ) {
  for( quint8 s = 0; s < 4; s++ ) {
    if( data.DISPCNT.displayBG[s] ) {
      delete surface[s];
      surface[s] = new QImage( (uchar*)data.BGIMAGE[s].picture,
                               data.BGIMAGE[s].width,
                               data.BGIMAGE[s].height, QImage::Format_ARGB32 );
    }
  }
  repaint();
  return true;
}


void CDebugWindow_Graphics::paintEvent( QPaintEvent *event ) {
  QPainter p( this );
//  for( quint8 s = 0; s < nSurfaces; s++ ) {
    p.drawImage( QPoint(0,0), *surface[0] );
//  }
}
