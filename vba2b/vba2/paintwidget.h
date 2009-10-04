#ifndef PAINTWIDGET_H
#define PAINTWIDGET_H

#include <QWidget>
#include "../gba2/common/cdriver_graphics.h"
class QByteArray;

// PaintWidget is a Qt Widget that offers GBA graphic output capabilities.
class PaintWidget : public QWidget, public CDriver_Graphics
{
    Q_OBJECT

    // ### CDriver_Graphics interface:
public:
    virtual bool displayFrame( const void *const data );


    // ### Widget code:
public:
    PaintWidget( QWidget *parent = 0 );
    ~PaintWidget();

protected:
    void paintEvent( QPaintEvent *event );

private:
    quint8 *pixels;
    static const unsigned int srcImgWidth = 240;
    static const unsigned int srcImgHeight = 160;
    static const unsigned int srcImgPixelCount = srcImgWidth * srcImgHeight;
    static const unsigned int pixels_size = 2 * srcImgPixelCount;
};

#endif // PAINTWIDGET_H
