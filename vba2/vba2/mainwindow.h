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


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTime>
class CEmuGBA; // saves compile time compared to including complete header file
class QTimer;
class CDriver_Sound;
class CDriver_Graphics;
class CDriver_Input;
class PaintWidget;
class sound_output_qt;
//class CDebugWindow_Graphics;
class FrameDialog;
class CAppSettings;

namespace Ui
{
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget *parent = 0, QString file = QString());
  ~MainWindow();

private:
  Ui::MainWindow *ui;
  CEmuGBA *m_emuGBA;
  QString m_fileName; // ROM file path, or empty when no ROM loaded
  QString m_saveFile;
  bool m_paused; // true: emulation paused  false: emulation running
  int m_timeoutCounter;
  QTime m_timeCounter;

  // m_timer times out around 3 times the GBA frame rate in order to allow exacter frame display time calculation
  QTimer *m_timer;
  CDriver_Graphics *m_gfx;
  CDriver_Input    *m_inp;
  PaintWidget      *m_renderTarget;
  //  CDebugWindow_Graphics *m_debugGraphics;
  sound_output_qt  *m_soundOutput;

  FrameDialog *settingsDialog;

  CAppSettings *m_settings;

protected:
  void dragEnterEvent( QDragEnterEvent *event );
  void dropEvent( QDropEvent *event );

public slots:
  // load & run game, also load backup media if possible
  bool loadGame( QString romFile );
  // load BIOS from file
  bool loadBIOS( QString biosFile );

private slots:
  void on_actionAbout_triggered();
  void pauseEmulation();
  void resumeEmulation();
  // backup save game data to file
  bool saveBackupMedia();
  // load save game data from file (if it exists)
  bool loadBackupMedia();
  // add file path to recent files list
  void addToRecentFiles( QString file );
  // manage recent files in menu bar
  void manageRecentFiles();
  // slot to manage click on recent file item
  void openRecentFile();

  // initialize or reinitialize sound output
  void resetSound();
  void timer_timeout();
  void on_actionPlay_Pause_triggered();
  void on_actionUnload_ROM_triggered();
  void on_actionLoad_ROM_triggered();
};

#endif // MAINWINDOW_H
