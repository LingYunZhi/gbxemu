#include "paintwidget.h"

#include <QPainter>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QImage>
#include <QPoint>

PaintWidget::PaintWidget( QWidget *parent )
    : QWidget( parent )
{
    this->setMinimumSize( srcImgWidth, srcImgHeight );

    m_pixels = NULL;
    m_pixels = new QImage( srcImgWidth, srcImgHeight, QImage::Format_RGB555 );
    Q_ASSERT( m_pixels != NULL );
    m_pixels->fill( 0x0000 ); // initially fill with black

    m_placement = NULL;
    m_placement = new QPoint( 0, 0 );
    Q_ASSERT( m_placement != NULL );
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

void PaintWidget::resizeEvent( QResizeEvent *event )
{
    const int w = event->size().width();
    const int h = event->size().height();
    m_placement->setX( ( w - srcImgWidth ) / 2 );
    m_placement->setY( ( h - srcImgHeight ) / 2 );
}

void PaintWidget::paintEvent( QPaintEvent *event )
{
    QPainter p( this );
    // TODO: draw centered, zoomed, etc.
    p.drawImage( *m_placement, *m_pixels );
}
