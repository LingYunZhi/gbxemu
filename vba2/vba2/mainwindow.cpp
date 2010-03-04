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
#include <QDragEnterEvent>
#include <QList>
#include <QUrl>
#include <QAction>

#include "../gba2/common/cdriver_sound.h"    // for dummy sound output
#include "../gba2/common/cdriver_graphics.h" // for dummy graphics output
#include "../gba2/common/cdriver_input.h"    // for dummy keypad input
#include "../gba2/cartridgeheader.h"
#include "../gba2/backupmedia.h"
#include "../gba2/bioschip.h"

#include "paintwidget.h"
//#include "cdebugwindow_graphics.h"
#include "sound_output_qt.h"

#include "settings/framedialog.h"

#include "cappsettings.h"


// set this to 60 to manually throttle the game, but audio sync + video sync are probably better
#define FRAME_RATE 120


MainWindow::MainWindow(QWidget *parent, QString file)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  // it is important that the settings are loaded first
  m_settings = NULL;
  m_settings = new CAppSettings( this );
  Q_ASSERT( m_settings != NULL );

  m_emuGBA = NULL;
  m_emuGBA = new CEmuGBA;
  Q_ASSERT( m_emuGBA != NULL );

  settingsDialog = NULL;
  settingsDialog = new FrameDialog( *m_settings, this );
  Q_ASSERT( settingsDialog != NULL );
  connect( ui->actionSettings, SIGNAL(triggered()), settingsDialog, SLOT(show()) );
  connect( settingsDialog, SIGNAL(accepted()), this, SLOT(resetSound()) );
  connect( settingsDialog->sh_directories, SIGNAL(biosFileChanged(QString)), this, SLOT(loadBIOS(QString)) );

  m_timer = NULL;
  m_timer = new QTimer( this );
  Q_ASSERT( m_timer != NULL );
  m_timer->setInterval( 5 ); // 1000/60 only produces ~37 fps  -  5 produces ~200 fps
  connect( m_timer, SIGNAL(timeout()), this, SLOT(timer_timeout()) );

  m_paused = false;

  m_renderTarget = NULL;
  m_renderTarget = new PaintWidget( this );
  Q_ASSERT( m_renderTarget != NULL );
  m_renderTarget->enableVSync( m_settings->s_enableVSync );
  m_renderTarget->enableSmoothStretching( m_settings->s_enableSmoothStretching );
  setCentralWidget( m_renderTarget );

//    m_debugGraphics = NULL;
//    m_debugGraphics = new CDebugWindow_Graphics( this );
//    Q_ASSERT( m_debugGraphics != NULL );
//    m_emuGBA->setDebugDriverGraphics( m_debugGraphics );
//    m_debugGraphics->show();

  m_soundOutput = NULL;
  resetSound();

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

  loadBIOS( m_settings->s_biosFile );

  ui->statusBar->showMessage( tr("Welcome to VisualBoyAdvance 2.") );

  manageRecentFiles();

  if( !file.isEmpty() ) {
    loadGame( file );
  }

  // create a timer that writes the save game to a file every two minutes (only if save data changed in memory)
  QTimer *saveTimer = NULL;
  saveTimer = new QTimer( this );
  Q_ASSERT( saveTimer != NULL );
  connect( saveTimer, SIGNAL(timeout()), this, SLOT(saveBackupMedia()) );
  saveTimer->start( 2 * 60 * 1000 );
}


MainWindow::~MainWindow()
{
    on_actionUnload_ROM_triggered();

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

  const bool wasPaused = m_paused;

  if( !wasPaused ) {
    // pause emulation while browsing for a file
    pauseEmulation();
  }
  QString newFileName = QFileDialog::getOpenFileName( this, tr("Select ROM image to load"), startDir, tr("GBA ROMs (*.gba)") );
  if( newFileName.isEmpty() ) {
    if( !wasPaused ) {
      resumeEmulation();
    }
    return;
  }

  m_paused = wasPaused;
  loadGame( newFileName );
}


void MainWindow::pauseEmulation() {
  if( m_timer->isActive() ) {
    m_paused = true;
    ui->actionPlay_Pause->setIcon( QIcon(":/MainWindow/play_button.png") );
    ui->actionPlay_Pause->setText( tr("Play") );
    if( m_timer->isActive() ) {
      m_timer->stop();
    }
  }
}


void MainWindow::resumeEmulation() {
  if( !m_timer->isActive() ) {
    m_paused = false;
    ui->actionPlay_Pause->setIcon( QIcon(":/MainWindow/pause_button.png") );
    ui->actionPlay_Pause->setText( tr("Pause") );
    if( !m_timer->isActive() ) {
      m_timeoutCounter = 0;
      m_timeCounter.start();
      m_timer->start();
    }
  }
}


bool MainWindow::saveBackupMedia() {
  // function called by timer but no ROM loaded
  if( m_fileName.isEmpty() ) return false;

  if( m_saveFile.isEmpty() ) {
    Q_ASSERT( false );
    return true;
  }

  BackupMedia *media = m_emuGBA->getBackupMedia();
  if( media == NULL ) return true;

  const u32 size = media->getSize();
  if( size == 0 ) return true;

  // don't save if nothing changed
  if( !media->writeOccured ) return true;

  bool ok = true;

  QFile file( m_saveFile );
  // TODO: create backup before overwriting old save game?
  const u8 *data = media->getData();
  Q_ASSERT( data != NULL );

  if( file.open( QIODevice::WriteOnly ) ) {
    const qint64 written = file.write( (const char *)data, size );
    file.close();
    if( written != (int)size ) {
      QMessageBox::critical( this, tr("Error"), tr("Error writing to file: ") + m_saveFile );
      ok = false;
    }
  } else {
    QMessageBox::critical( this, tr("Error"), tr("Error opening file: ") + m_saveFile );
    ok = false;
  }

  // reset flag because we created a backup just now
  media->writeOccured = false;

  return ok;
}


bool MainWindow::loadBackupMedia() {
  if( m_saveFile.isEmpty() ) {
    Q_ASSERT( false );
    return true;
  }

  BackupMedia *media = m_emuGBA->getBackupMedia();
  if( media == NULL ) return true;

  const u32 size = media->getSize();
  if( size == 0 ) return true;

  bool ok = true;

  QFile file( m_saveFile );
  if( !file.exists() ) return true; // nothing to read from

  // TODO: if(media->writeOccured) create a backup of the currently loaded data before overwriting it

  u8 *data = media->getData();
  Q_ASSERT( data != NULL );

  if( file.open( QIODevice::ReadOnly ) ) {
    const qint64 read = file.read( (char *)data, size );
    file.close();
    if( (read != (int)size) && (media->getType() != media->EEPROM) ) {
      // exception for EEPROM: we can't know the correct size before emulation started
      QMessageBox::critical( this, tr("Error"), tr("Error reading from file: ") + m_saveFile );
      ok = false;
    }
  } else {
    QMessageBox::critical( this, tr("Error"), tr("Error opening file: ") + m_saveFile );
    ok = false;
  }

  // reset flag because we modified it just now
  media->writeOccured = false;

  return ok;
}


bool MainWindow::loadGame( QString romFile ) {
  QFile rom( romFile );
  qint64 size = rom.size();
  if( size > (32*1024*1024) ) {
    QMessageBox::critical( this, tr("Error"), tr("GBA ROM size must not exceed 32 MB.") );
    return false;
  }
  if( size < 192 ) {
    QMessageBox::critical( this, tr("Error"), tr("GBA ROM is too small.") );
    return false;
  }
  rom.open( QFile::ReadOnly );
  QByteArray data = rom.readAll();
  rom.close();
  Q_ASSERT( size == data.size() );
  const u8 *const romData = (const u8 *const)data.constData();

  // build unique save game file name
  CartridgeHeader header( romData );
  QString version;
  if( header.gameVersion > 0 ) {
    // add version only if necessary
    version = " [v" + QString::number( header.gameVersion ) + "]";
  }
  m_saveFile = m_settings->s_cartridgeSavesDir + "/" + header.gameTitle +
               " (" + header.gameCode + ")" + version + ".sav";
  // TODO: add exception for homebrew games with empty gameTitle

  bool retVal = m_emuGBA->initialize( romData, (u32)size );
  if( !retVal ) {
    QMessageBox::critical( this, tr("Error"), tr("ROM loading failed.") );
    return false;
  }
  m_fileName = romFile;

  addToRecentFiles( m_fileName );

  loadBackupMedia();

  if( !m_paused ) {
    resumeEmulation();
  }

  return true;
}


bool MainWindow::loadBIOS( QString biosFile ) {
  // do not change BIOS while emulation is running
  if( !m_fileName.isEmpty() ) return false;

  QFile f( biosFile );
  if( !f.exists() ) {
    Q_ASSERT( false );
    QMessageBox::critical( this, tr("Error"), tr("BIOS file does not exist.") );
    return false;
  }
  BiosChip *chip = m_emuGBA->getBiosChip();
  Q_ASSERT( chip != NULL );
  const qint64 fileSize = f.size();
  const qint64 maxSize = (qint64)chip->SIZE;
  const qint64 readSize = qMin( fileSize, maxSize );
  char *data = (char *)chip->lockData();
  Q_ASSERT( data != NULL );
  f.open( QIODevice::ReadOnly );
  const qint64 bytesRead = f.read( data, readSize );
  f.close();
  if( bytesRead != readSize ) {
    Q_ASSERT( false );
    QMessageBox::critical( this, tr("Error"), tr("Error reading from BIOS file.") );
    return false;
  }
  return true;
}


void MainWindow::addToRecentFiles( QString file ) {
  const int position = m_settings->s_recentFiles.indexOf( file );
  if( position == -1 ) {
    // add file to the front of the recent files list
    m_settings->s_recentFiles.prepend( m_fileName );
    // max sure there are not too many entries
    while( m_settings->s_recentFiles.size() > m_settings->maxRecentFiles ) {
      m_settings->s_recentFiles.removeLast();
    }
  } else {
    // move last accessed item to the top
    m_settings->s_recentFiles.move( position, 0 );
  }

  // update menu
  manageRecentFiles();
}


void MainWindow::manageRecentFiles() {
  // static container to remember all recent file menu actions
  static QList<QAction*> *actions = NULL;
  if( actions == NULL ) {
    actions = new QList<QAction*>;
  }
  Q_ASSERT( actions != NULL );

  // clear recent files from menu
  const int nActions = actions->size();
  for( int i = 0; i < nActions; i++ ) {
    QAction *a = actions->at(i);
    ui->menuFile->removeAction( a );
    disconnect( a, SIGNAL(triggered()), this, SLOT(openRecentFile()) );
  }
  actions->clear();

  // add recent file items
  int c = m_settings->s_recentFiles.size();
  if( c > m_settings->maxRecentFiles ) c = m_settings->maxRecentFiles;
  for( int i = 0; i < c; i++ ) {
    QString filePath = m_settings->s_recentFiles.at(i);
    QFileInfo info( filePath );
    QAction *a = new QAction( info.fileName(), this );
    a->setData( filePath );
    a->setStatusTip( info.path() );
    actions->append( a );
    ui->menuFile->addAction( a );
    connect( a, SIGNAL(triggered()), this, SLOT(openRecentFile()) );
  }
}


void MainWindow::openRecentFile() {
  QAction *a = (QAction *)sender();
  if( a != NULL ) {
    loadGame( a->data().toString() );
  }
}


void MainWindow::on_actionUnload_ROM_triggered()
{
  if( !m_fileName.isEmpty() ) {
    if( m_timer->isActive() ) {
      // stop emulation
      ui->actionPlay_Pause->trigger();
    }

    saveBackupMedia();

    m_emuGBA->shutDown();
    m_fileName.clear();
  }
}


void MainWindow::dragEnterEvent( QDragEnterEvent *event ) {
  if( event->mimeData()->hasUrls() ) {
    event->acceptProposedAction();
  }
}


void MainWindow::dropEvent( QDropEvent *event ) {
  const QMimeData *data = event->mimeData();
  // launch dropped game ROM
  if( data->hasUrls() ) {
    QList<QUrl> files = data->urls();
    const int nFiles = files.size();
    QString file;
    for( int i = 0; i < nFiles; i++ ) {
      file = files.at( i ).toLocalFile();
      if( QFile::exists( file ) ) {
        if( loadGame( file ) ) {
          // if one of the files failed to load, try next one
          break;
        }
      }
    }
  }
}


void MainWindow::resetSound() {
  if( m_settings == NULL ) return;
  if( m_emuGBA == NULL ) return;

  if( m_soundOutput != NULL ) {
    delete m_soundOutput;
    m_soundOutput = NULL;
  }
  m_soundOutput = new sound_output_qt( m_settings->s_soundOutputDevice, this );
  Q_ASSERT( m_soundOutput != NULL );
  m_soundOutput->enableAudioSync( m_settings->s_enableAudioSync );

  m_emuGBA->setDriverSound( (CDriver_Sound *)m_soundOutput );
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
      pauseEmulation();
    }
  }
}


void MainWindow::on_actionPlay_Pause_triggered()
{
  if( m_paused ) {
    resumeEmulation();
  } else {
    pauseEmulation();
  }
}
