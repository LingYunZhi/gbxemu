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


#include "settingssheet_keyboardinput.h"
#include "ui_settingssheet_keyboardinput.h"
#include <QStringList>
#include <QString>
#include "presskeydialog.h"


SettingsSheet_KeyboardInput::SettingsSheet_KeyboardInput( CAppSettings &settings, QWidget *parent )
  : SettingsSheet( settings, parent ), ui(new Ui::SettingsSheet_KeyboardInput), m_settings( settings )
{
  ui->setupUi( this );

  ui->tableWidget->setColumnCount( 1 );
  ui->tableWidget->setRowCount( settings.s_keyAssignments.size() );
  QStringList heading;
  heading << tr("Key");
  ui->tableWidget->setHorizontalHeaderLabels( heading );

  heading.clear();
  for( int i = 0; i < settings.s_keyAssignments.size(); i++ ) {
    heading.append( settings.s_keyAssignments.at( i )->getActionName() );
    const QString keyName = QString::number( settings.s_keyAssignments.at( i )->getKeyCode(), 16 );
    ui->tableWidget->setItem( i, 0, new QTableWidgetItem( keyName ) );
  }
  ui->tableWidget->setVerticalHeaderLabels( heading );
}


SettingsSheet_KeyboardInput::~SettingsSheet_KeyboardInput()
{
  delete ui;
}


void SettingsSheet_KeyboardInput::applySettings() {
}


void SettingsSheet_KeyboardInput::on_assign_all_clicked()
{
  for( int i = 0; i < m_settings.s_keyAssignments.size(); i++ ) {
    on_tableWidget_cellDoubleClicked( i, 0 );
  }
}


void SettingsSheet_KeyboardInput::on_tableWidget_cellDoubleClicked( int row, int column )
{
  const QString &actionName = m_settings.s_keyAssignments.at( row )->getActionName();
  PressKeyDialog dlg( tr("Please press a key for action: ") + actionName , this );
  const int key = dlg.exec();
  if( key != 0 ) {
    ui->tableWidget->item( row, column )->setText( QString::number( key, 16 ) );
    m_settings.s_keyAssignments.at( row )->setKeyCode( key );
  }
}


RETRANSLATE_CODE( SettingsSheet_KeyboardInput )
