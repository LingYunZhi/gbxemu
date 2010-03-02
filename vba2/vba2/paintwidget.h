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


#ifndef PAINTWIDGET_H
#define PAINTWIDGET_H


#include <QtOpenGL/QGLWidget>
#include "../gba2/common/cdriver_graphics.h"
#include "../gba2/common/cdriver_input.h"
class QResizeEvent;
class QPaintEvent;
class QImage;
class QPoint;


// PaintWidget is a Qt Widget that offers GBA graphic output capabilities.
class PaintWidget : public QGLWidget, public CDriver_Graphics, public CDriver_Input
{
  Q_OBJECT

  // ### CDriver_Graphics interface:
public:
  virtual bool displayFrame( const void *const data );


  // ### CDriver_Input interface:
  virtual u16 getKeyStates();


  // ### Widget code:
public:
  PaintWidget( QWidget *parent = 0 );
  ~PaintWidget();

protected:
  void resizeEvent( QResizeEvent *event );
  void paintEvent( QPaintEvent *event );
  void keyPressEvent( QKeyEvent *event );
  void keyReleaseEvent( QKeyEvent *event );

private:
  QImage *m_pixels;
  QRectF *m_placement;
  static const int srcImgWidth = 240;
  static const int srcImgHeight = 160;
  static const int srcImgPixelCount = srcImgWidth * srcImgHeight;

  u16 m_keys;
  bool m_smooth;

public slots:
  void enableVSync( bool enable );
  void enableSmoothStretching( bool enable );
};


#endif // PAINTWIDGET_H
