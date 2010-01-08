#ifndef CDEBUGWINDOW_GRAPHICS_H
#define CDEBUGWINDOW_GRAPHICS_H

#include <QDialog>
#include "../gba2/common/cdriver_graphics.h"
class QImage;
class QPaintEvent;
class QMouseEvent;
class QMenu;


class CDebugWindow_Graphics : public QDialog, public CDriver_Graphics
{
  Q_OBJECT

public:
  CDebugWindow_Graphics( QWidget *parent = 0 );
  ~CDebugWindow_Graphics();

  // CDriver_Graphics interface
  virtual bool renderFrame( CGBAGraphics::RESULT &data );

protected:
  virtual void paintEvent( QPaintEvent *event );
  virtual void mousePressEvent ( QMouseEvent *event );

private:
  static const quint8 nSurfaces = 4;
  quint8 currentSurface;
  QImage *surface[nSurfaces];
  QMenu *menu;

private slots:
  void showBG0();
  void showBG1();
  void showBG2();
  void showBG3();
};


#endif // CDEBUGWINDOW_GRAPHICS_H
