#ifndef PAINTWIDGET_H
#define PAINTWIDGET_H

#include <QWidget>
#include "../gba2/common/cdriver_graphics.h"
class QResizeEvent;
class QPaintEvent;
class QImage;
class QPoint;

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
    void resizeEvent( QResizeEvent *event );
    void paintEvent( QPaintEvent *event );

private:
    QImage *m_pixels;
    QRectF *m_placement;
    static const int srcImgWidth = 240;
    static const int srcImgHeight = 160;
    static const int srcImgPixelCount = srcImgWidth * srcImgHeight;
};

#endif // PAINTWIDGET_H
