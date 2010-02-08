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


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../gba2/cemugba.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTime>
#include <QTimer>

#include "../gba2/common/cdriver_sound.h"    // for dummy sound output
#include "../gba2/common/cdriver_graphics.h" // for dummy graphics output
#include "../gba2/common/cdriver_input.h"    // for dummy keypad input

#include "paintwidget.h"
//#include "cdebugwindow_graphics.h"
#include "sound_output_qt.h"

#include "settings/framedialog.h"

#include "cappsettings.h"


// set this to 60 to manually throttle the game, but audio sync + video sync are probably better
#define FRAME_RATE 120


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  // it is important that the settings are loaded first
  m_settings = NULL;
  m_settings = new CAppSettings( this );
  Q_ASSERT( m_settings != NULL );

  settingsDialog = NULL;
  settingsDialog = new FrameDialog( *m_settings, this );
  Q_ASSERT( settingsDialog != NULL );
  connect( ui->actionSettings, SIGNAL(triggered()), settingsDialog, SLOT(show()) );
  connect( settingsDialog, SIGNAL(accepted()), this, SLOT(applyNewSettings()) );

  m_emuGBA = NULL;
  m_emuGBA = new CEmuGBA;
  Q_ASSERT( m_emuGBA != NULL );

  m_playing = false;

  m_timer = NULL;
  m_timer = new QTimer( this );
  Q_ASSERT( m_timer != NULL );
  m_timer->setInterval( 5 ); // 1000/60 only produces ~37 fps  -  5 produces ~200 fps
  connect( m_timer, SIGNAL(timeout()), this, SLOT(timer_timeout()) );

  m_renderTarget = NULL;
  m_renderTarget = new PaintWidget( this );
  Q_ASSERT( m_renderTarget != NULL );
  m_renderTarget->enableVSync( m_settings->s_enableVSync );
  setCentralWidget( m_renderTarget );

//    m_debugGraphics = NULL;
//    m_debugGraphics = new CDebugWindow_Graphics( this );
//    Q_ASSERT( m_debugGraphics != NULL );
//    m_emuGBA->setDebugDriverGraphics( m_debugGraphics );
//    m_debugGraphics->show();

  m_soundOutput = NULL;
  m_soundOutput = new sound_output_qt( m_settings->s_soundOutputDevice, this );
  Q_ASSERT( m_soundOutput != NULL );
  m_soundOutput->enableAudioSync( m_settings->s_enableAudioSync );

  m_snd = NULL;
//    m_snd = new CDummyDriver_Sound();
  m_snd = (CDriver_Sound *)m_soundOutput;
  Q_ASSERT( m_snd != NULL );
  m_emuGBA->setDriverSound( m_snd );

  m_gfx = NULL;
//    m_gfx = new CDummyDriver_Graphics();
  m_gfx = (CDriver_Graphics *)m_renderTarget;
  Q_ASSERT( m_gfx != NULL );
  m_emuGBA->setDriverGraphics( m_gfx );

  m_inp = NULL;
//    m_inp = new CDummyDriver_Input();
  m_inp = (CDriver_Input *)m_renderTarget;
  Q_ASSERT( m_inp != NULL );
  m_emuGBA->setDriverInput( m_inp );

  ui->statusBar->showMessage( tr("Welcome to VisualBoyAdvance 2.") );
}

MainWindow::~MainWindow()
{
    on_actionUnload_ROM_triggered();

    if( m_snd != NULL ) {
        delete m_snd;
    }

    if( ( m_renderTarget != m_gfx ) && ( m_gfx != NULL ) ) {
        delete m_gfx;
    }

    if( ( m_renderTarget != m_inp ) && ( m_inp != NULL ) ) {
        delete m_inp;
    }

    delete m_emuGBA;

    delete ui;
}

void MainWindow::on_actionLoad_ROM_triggered()
{
  QString startDir = m_fileName;
  if( !m_settings->s_cartridgeFilesDir.isEmpty() ) {
    startDir = m_settings->s_cartridgeFilesDir;
  }

  QString newFileName = QFileDialog::getOpenFileName( this, tr("Select ROM image to load"), startDir, tr("GBA ROMs (*.gba)") );
  if( newFileName.isEmpty() ) return;
  QFile rom( newFileName );
  qint64 size = rom.size();
  if( size > (32*1024*1024) ) {
    QMessageBox::critical( this, tr("Error"), tr("GBA ROM size must not exceed 32 MB.") );
    return;
  }
  rom.open( QFile::ReadOnly );
  QByteArray data = rom.readAll();
  rom.close();
  Q_ASSERT( size == data.size() );
  bool retVal = m_emuGBA->loadROM( (const u8 *const)data.constData(), (u32)size );
  if( !retVal ) {
    QMessageBox::critical( this, tr("Error"), tr("ROM loading failed.") );
    return;
  }
  m_fileName = newFileName;
  ui->actionPlay_Pause->setEnabled( true );
  if( !m_timer->isActive() ) {
    // start emulation
    ui->actionPlay_Pause->trigger();
  }
}

void MainWindow::on_actionUnload_ROM_triggered()
{
  if( !m_fileName.isEmpty() ) {
    if( m_timer->isActive() ) {
      // stop emulation
      ui->actionPlay_Pause->trigger();
    }
    m_emuGBA->closeROM();
    m_fileName.clear();
    ui->actionPlay_Pause->setEnabled( false );
  }
}


void MainWindow::applyNewSettings() {
  if( m_settings == NULL ) return;

  if( m_soundOutput != NULL ) {
    m_soundOutput->enableAudioSync( m_settings->s_enableAudioSync );
  }

  if( m_renderTarget != NULL ) {
    m_renderTarget->enableVSync( m_settings->s_enableVSync );
  }
}


void MainWindow::timer_timeout()
{
  // TODO: m_timeoutCounter: use local static instead of global ?
  const int nextTimeout = ( 1000 * m_timeoutCounter ) / FRAME_RATE;
  const int timePassed = m_timeCounter.elapsed();
  if( timePassed >= nextTimeout ) {
    // show fps
    if( ( m_timeoutCounter % 60 ) == 0 ) {
      const double avgFps = m_timeoutCounter / ( timePassed / 1000.0f );
      ui->statusBar->showMessage( tr("Timer avg fps: ") + QString::number( avgFps ) );
    }

    m_timeoutCounter++;

    bool retVal = false;
    retVal = m_emuGBA->emulate();

    if( retVal == false ) {
      Q_ASSERT( false ); // debug emulator code
      QMessageBox::critical( this, tr("Error"), tr("Emulator code messed up. Pausing emulation.") );
      if( m_timer->isActive() ) {
        m_timer->stop();
      }
    }
  }
}

void MainWindow::on_actionPlay_Pause_triggered()
{
  if( !m_fileName.isEmpty() ) {
    if( m_timer->isActive() ) {
      ui->actionPlay_Pause->setIcon( QIcon(":/MainWindow/play_button.png") );
      m_timer->stop();
    } else {
      ui->actionPlay_Pause->setIcon( QIcon(":/MainWindow/pause_button.png") );
      m_timeoutCounter = 0;
      m_timeCounter.start();
      m_timer->start();
    }
  } else {
    Q_ASSERT( false ); // should not be able to arrive here
  }
}
