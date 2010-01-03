#include "paintwidget.h"

#include <QPainter>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QImage>
#include <QPoint>

#include <string.h> // memcpy
#include <assert.h>

PaintWidget::PaintWidget( QWidget *parent )
    : QWidget( parent )
{
    this->setMinimumSize( srcImgWidth, srcImgHeight );
    this->setFocusPolicy( Qt::StrongFocus );

    m_placement = NULL;
    m_placement = new QRectF( 0, 0, 1, 1 );
    Q_ASSERT( m_placement != NULL );

    m_pixels = NULL;

    m_keys = CDriver_Input::BUTTON__NONE;
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

bool PaintWidget::displayFrame( const void *const data )
{/*
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
   */ return true;
}

bool PaintWidget::renderFrame( CGBAGraphics::RESULT &data ) {
  if( data.DISPCNT.displayBG[3] ) {
    CPicture &pic = data.BGIMAGE[3];
    m_pixels = new QImage( pic.width, pic.height, QImage::Format_ARGB32 );
    const quint32 *source = (const quint32 *)pic.picture;
    quint32 *dest = (quint32 *)m_pixels->bits();
    assert( source != NULL ); // can be removed if every video mode is emulated
    memcpy( dest, source, 4 * pic.width * pic.height );
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
    if( m_pixels != NULL )
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
