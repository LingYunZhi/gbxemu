#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../gba2/cemugba.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>
#include <assert.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_emuGBA = NULL;
    m_emuGBA = new CEmuGBA;
    Q_ASSERT( m_emuGBA != NULL );

    m_playing = false;

    m_timer = NULL;
    m_timer = new QTimer( this );
    Q_ASSERT( m_timer != NULL );
    m_timer->setInterval( 1000 );
    connect( m_timer, SIGNAL(timeout()), this, SLOT(timer_timeout()) );
}

MainWindow::~MainWindow()
{
    on_actionUnload_ROM_triggered();

    delete m_emuGBA;

    delete ui;
}

void MainWindow::on_actionLoad_ROM_triggered()
{
    QString newFileName = QFileDialog::getOpenFileName(
            this, tr("Select ROM image to load"), m_fileName, tr("GBA ROMs (*.gba)") );
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
    m_emuGBA->loadROM( (const u8 *const)data.constData(), (u32)size );
    m_fileName = newFileName;
    ui->actionPlay_Pause->setEnabled( true );
}

void MainWindow::on_actionUnload_ROM_triggered()
{
    if( !m_fileName.isEmpty() ) {
        m_emuGBA->closeROM();
        m_fileName.clear();
        ui->actionPlay_Pause->setEnabled( false );
    }
}

void MainWindow::timer_timeout()
{
    bool retVal = false;
    retVal = m_emuGBA->emulate();

    if( retVal == false ) {
        QMessageBox::critical( this, tr("Error"), tr("Emulator code messed up. Pausing emulation.") );
        if( m_timer->isActive() ) {
            m_timer->stop();
        }
    }
}

void MainWindow::on_actionPlay_Pause_triggered()
{
    if( !m_fileName.isEmpty() ) {
        if( m_timer->isActive() ) {
            m_timer->stop();
            ui->actionPlay_Pause->setIcon( QIcon(":/MainWindow/play_button.png") );
        } else {
            m_timer->start();
            ui->actionPlay_Pause->setIcon( QIcon(":/MainWindow/pause_button.png") );
        }
    } else {
        Q_ASSERT( false ); // should not be able to arrive here
    }
}
