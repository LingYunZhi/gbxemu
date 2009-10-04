#include "paintwidget.h"

#include <QPainter>
#include <QImage>
#include <QByteArray>
#include <QTime>

#include <string.h> // for memcpy

PaintWidget::PaintWidget( QWidget *parent )
    : QWidget( parent )
{
    pixels = NULL;
    pixels = new quint8[pixels_size];
    Q_ASSERT( pixels != NULL );
}

PaintWidget::~PaintWidget()
{
    if( pixels != NULL ) {
        delete[] pixels;
    }
}

bool PaintWidget::displayFrame( const void *const data )
{
    // we have to swap red and blue bits because the GBA and Qt handle 16 bit colors differently
    // TODO: use quint64 for parallel data processing
    const quint16 *source = (const quint16*)data;
    quint16 *dest = (quint16*)pixels;
    quint16 r, g, b;
    for( unsigned int counter = 0; counter < srcImgPixelCount; counter++ ) {
        const quint16 color = *(source++);
        r = ( color & 0x1f ) << 10;
        g = color & 0x3e0;
        b = ( color & 0x7c00 ) >> 10;
        *(dest++) = r | g | b;
    }
    this->repaint(); // TODO: replace with update to make use of VSync
    return true;
}

void PaintWidget::paintEvent( QPaintEvent *event )
{
    QPainter p( this );
    // TODO: draw centered, zoomed, etc.
    p.drawImage( QPoint( 10, 20 ), QImage( pixels, srcImgWidth, srcImgHeight, QImage::Format_RGB555 ) );
}
