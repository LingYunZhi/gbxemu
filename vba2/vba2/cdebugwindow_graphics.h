#ifndef CDEBUGWINDOW_GRAPHICS_H
#define CDEBUGWINDOW_GRAPHICS_H

#include <QDialog>
#include "../gba2/common/cdriver_graphics.h"
class QImage;
class QPaintEvent;


class CDebugWindow_Graphics : public QDialog, public CDriver_Graphics
{
public:
  CDebugWindow_Graphics( QWidget *parent = 0 );
  ~CDebugWindow_Graphics();

  // CDriver_Graphics interface
  virtual bool renderFrame( CGBAGraphics::RESULT &data );

protected:
  virtual void paintEvent( QPaintEvent *event );

private:
  static const quint8 nSurfaces = 4;
  QImage *surface[nSurfaces];

};


#endif // CDEBUGWINDOW_GRAPHICS_H
