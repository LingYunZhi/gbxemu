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
  void exportToFile();
  void showBG0();
  void showBG1();
  void showBG2();
  void showBG3();
};


#endif // CDEBUGWINDOW_GRAPHICS_H
