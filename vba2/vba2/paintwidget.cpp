/*  VisualBoyAdvance 2
    Copyright (C) 2009-2010  VBA development team

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "paintwidget.h"

#include <QPainter>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QImage>
#include <QPoint>
#include "cappsettings.h"


PaintWidget::PaintWidget( CAppSettings &settings, QWidget *parent )
  : QGLWidget( parent ), m_settings( settings )
{
  setMinimumSize( srcImgWidth, srcImgHeight );
  setFocusPolicy( Qt::StrongFocus );

  m_pixels = NULL;
  m_pixels = new QImage( srcImgWidth, srcImgHeight, QImage::Format_RGB555 );
  Q_ASSERT( m_pixels != NULL );
  m_pixels->fill( 0x0000 ); // initially fill with black

  m_placement = NULL;
  m_placement = new QRectF( 0, 0, 1, 1 );
  Q_ASSERT( m_placement != NULL );

  m_buttonStates = CDriver_Input::BUTTON__NONE;
  m_smooth = false;
}


PaintWidget::~PaintWidget()
{
  if( m_placement != NULL ) {
    delete m_placement;
  }

  if( m_pixels != NULL ) {
    delete m_pixels;
  }
}


void PaintWidget::enableVSync( bool enable ) {
  QGLFormat f = format();
  Q_ASSERT( ( f.swapInterval() == 0 ) || ( f.swapInterval() == 1 ) );
  const bool vsync_enabled = f.swapInterval() != 0;
  if( vsync_enabled != enable ) {
    f.setSwapInterval( enable ? 1 : 0 );
    setFormat( f );
  }
}


void PaintWidget::enableSmoothStretching( bool enable ) {
  m_smooth = enable;
}


bool PaintWidget::displayFrame( const void *const data )
{
    // we have to swap red and blue bits because the GBA and Qt handle 16 bit colors differently
    // TODO: use quint64 for parallel data processing
    const quint16 *source = (const quint16*)data;
    quint16 *dest = (quint16*)m_pixels->bits();
    quint16 r, g, b;
    for( int counter = 0; counter < srcImgPixelCount; counter++ ) {
        const quint16 color = *(source++);
        r = ( color & 0x1f ) << 10;
        g = color & 0x3e0;
        b = ( color & 0x7c00 ) >> 10;
        *(dest++) = r | g | b;
    }
    this->repaint(); // TODO: replace with update to make use of VSync
    return true;
}


u16 PaintWidget::getKeyStates()
{
    return m_buttonStates;
}


void PaintWidget::resizeEvent( QResizeEvent *event )
{
    const int widgetWidth  = event->size().width();
    const int widgetHeight = event->size().height();
    const int scaleX = widgetWidth / srcImgWidth;
    const int scaleY = widgetHeight / srcImgHeight;
    const int scaleMin = ( scaleX < scaleY ) ? scaleX : scaleY;
    const int scaledWidth  = scaleMin * srcImgWidth;
    const int scaledHeight = scaleMin * srcImgHeight;
    const int left = ( widgetWidth  - scaledWidth  ) / 2;
    const int top  = ( widgetHeight - scaledHeight ) / 2;

    m_placement->setTopLeft( QPointF( left, top ) );
    m_placement->setSize( QSizeF( scaledWidth, scaledHeight ) );
}


void PaintWidget::paintEvent( QPaintEvent *event )
{
    QPainter p( this );
    p.setRenderHint( QPainter::SmoothPixmapTransform, m_smooth );
    p.drawImage( *m_placement, *m_pixels );
}


void PaintWidget::keyPressEvent( QKeyEvent *event )
{
  const int pressedKey = event->key();
  bool found = false;
  const int nKeys = m_settings.s_keyAssignments.size();
  for( int i = 0; i < nKeys; i++ ) {
    const KeyAssignment *a = m_settings.s_keyAssignments.at(i);
    if( pressedKey == a->getKeyCode() ) {
      m_buttonStates |= a->getGBAButtonCode();
      found = true;
      break;
    }
  }

  if( !found ) {
    QWidget::keyPressEvent( event );
  }
}


void PaintWidget::keyReleaseEvent( QKeyEvent *event )
{
  const int releasedKey = event->key();
  bool found = false;
  const int nKeys = m_settings.s_keyAssignments.size();
  for( int i = 0; i < nKeys; i++ ) {
    const KeyAssignment *a = m_settings.s_keyAssignments.at(i);
    if( releasedKey == a->getKeyCode() ) {
      m_buttonStates ^= a->getGBAButtonCode();
      found = true;
      break;
    }
  }

  if( !found ) {
    QWidget::keyReleaseEvent( event );
  }
}
