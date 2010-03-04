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


#include "framedialog.h"
#include "ui_framedialog.h"
#include <QListWidgetItem>


#define ADD_SHEET( SHEET_TYPE, POINTER ) \
{ \
  POINTER = NULL; \
  /* parent will be assigned by the addWidget function */ \
  POINTER = new SHEET_TYPE( settings ); \
  Q_ASSERT( POINTER != NULL ); \
  ui->stackedWidget->addWidget( POINTER ); \
  QListWidgetItem *i; \
  i = NULL; \
  i = new QListWidgetItem( (POINTER)->windowIcon(), (POINTER)->windowTitle() ); \
  Q_ASSERT( i != NULL ); \
  ui->listWidget->addItem( i ); \
  connect( this, SIGNAL(accepted()), POINTER, SLOT(applySettings()) ); \
}


FrameDialog::FrameDialog( CAppSettings &settings, QWidget *parent )
  : QDialog( parent ),
  ui(new Ui::FrameDialog)
{
  ui->setupUi( this );

  ADD_SHEET( SettingsSheet_KeyboardInput, sh_keyboardInput );
  ADD_SHEET( SettingsSheet_GraphicsOutput, sh_graphicsOutput );
  ADD_SHEET( SettingsSheet_AudioOutput, sh_audioOutput );
  ADD_SHEET( SettingsSheet_Directories, sh_directories );

  // clicking on item activates corresponding settings sheet
  connect( ui->listWidget, SIGNAL(currentRowChanged(int)), ui->stackedWidget, SLOT(setCurrentIndex(int)) );
}


FrameDialog::~FrameDialog()
{
  delete ui;
}


RETRANSLATE_CODE( FrameDialog )
