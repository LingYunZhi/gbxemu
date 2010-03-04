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


#include "settingssheet_directories.h"
#include "ui_settingssheet_directories.h"


#include <QFileDialog>
#include <QString>
#include <QFile>


SettingsSheet_Directories::SettingsSheet_Directories( CAppSettings &settings, QWidget *parent )
  : SettingsSheet( settings, parent ), ui(new Ui::SettingsSheet_Directories)
{
  ui->setupUi( this );
  ui->lineEditBIOS->setText( m_settings.s_biosFile );
  ui->lineEditGameCartridges->setText( m_settings.s_cartridgeFilesDir );
  ui->lineEditCartridgeSaves->setText( m_settings.s_cartridgeSavesDir );
}


SettingsSheet_Directories::~SettingsSheet_Directories()
{
  delete ui;
}


void SettingsSheet_Directories::applySettings() {
  const QString oldBios = m_settings.s_biosFile;
  const QString newBios = ui->lineEditBIOS->text();
  if( oldBios != newBios ) {
    m_settings.s_biosFile = newBios;
    emit biosFileChanged( newBios );
  }
  m_settings.s_cartridgeFilesDir = ui->lineEditGameCartridges->text();
  m_settings.s_cartridgeSavesDir = ui->lineEditCartridgeSaves->text();
}


RETRANSLATE_CODE( SettingsSheet_Directories )


void SettingsSheet_Directories::selectDir( QLineEdit *editControl ) {
  const QString oldDir = editControl->text();
  const QString dir =
      QFileDialog::getExistingDirectory( this, tr("Select Directory"), oldDir );
  if( !dir.isEmpty() ) {
    editControl->setText( dir );
  }
}


void SettingsSheet_Directories::selectFile( QLineEdit *editControl ) {
  const QString oldFile = editControl->text();
  const QString file =
      QFileDialog::getOpenFileName( this, tr("Select File"), oldFile );
  if( !file.isEmpty() ) {
    editControl->setText( file );
  }
}


void SettingsSheet_Directories::on_pushButtonGameCartridges_clicked()
{
  selectDir( ui->lineEditGameCartridges );
}


void SettingsSheet_Directories::on_pushButtonCartridgeSaves_clicked()
{
  selectDir( ui->lineEditCartridgeSaves );
}


void SettingsSheet_Directories::on_pushButtonBIOS_clicked()
{
  selectFile( ui->lineEditBIOS );
}
