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


PaintWidget::PaintWidget( QWidget *parent )
  : QGLWidget( parent )
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

  m_keys = CDriver_Input::BUTTON__NONE;
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
    return m_keys;
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
    switch( event->key() )
    {
    case Qt::Key_S:
        m_keys |= CDriver_Input::BUTTON_A;
        break;
    case Qt::Key_A:
        m_keys |= CDriver_Input::BUTTON_B;
        break;
    case Qt::Key_Y:
        m_keys |= CDriver_Input::BUTTON_SELECT;
        break;
    case Qt::Key_X:
        m_keys |= CDriver_Input::BUTTON_START;
        break;
    case Qt::Key_Right:
        m_keys |= CDriver_Input::BUTTON_RIGHT;
        break;
    case Qt::Key_Left:
        m_keys |= CDriver_Input::BUTTON_LEFT;
        break;
    case Qt::Key_Up:
        m_keys |= CDriver_Input::BUTTON_UP;
        break;
    case Qt::Key_Down:
        m_keys |= CDriver_Input::BUTTON_DOWN;
        break;
    case Qt::Key_W:
        m_keys |= CDriver_Input::BUTTON_R;
        break;
    case Qt::Key_Q:
        m_keys |= CDriver_Input::BUTTON_L;
        break;
    default:
        QWidget::keyPressEvent( event );
    }
}

void PaintWidget::keyReleaseEvent( QKeyEvent *event )
{
    switch( event->key() )
    {
    case Qt::Key_S:
        m_keys ^= CDriver_Input::BUTTON_A;
        break;
    case Qt::Key_A:
        m_keys ^= CDriver_Input::BUTTON_B;
        break;
    case Qt::Key_Y:
        m_keys ^= CDriver_Input::BUTTON_SELECT;
        break;
    case Qt::Key_X:
        m_keys ^= CDriver_Input::BUTTON_START;
        break;
    case Qt::Key_Right:
        m_keys ^= CDriver_Input::BUTTON_RIGHT;
        break;
    case Qt::Key_Left:
        m_keys ^= CDriver_Input::BUTTON_LEFT;
        break;
    case Qt::Key_Up:
        m_keys ^= CDriver_Input::BUTTON_UP;
        break;
    case Qt::Key_Down:
        m_keys ^= CDriver_Input::BUTTON_DOWN;
        break;
    case Qt::Key_W:
        m_keys ^= CDriver_Input::BUTTON_R;
        break;
    case Qt::Key_Q:
        m_keys ^= CDriver_Input::BUTTON_L;
        break;
    default:
        QWidget::keyReleaseEvent( event );
    }
}
