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

#include "../cappsettings.h"
#include "settingssheet_audiooutput.h"
#include "settingssheet_keyboardinput.h"

#include <QListWidgetItem>


FrameDialog::FrameDialog( CAppSettings &settings, QWidget *parent )
  : QDialog( parent ),
  ui(new Ui::FrameDialog)
{
  ui->setupUi(this);

  // parent will be assigned by the addWidget function
  QListWidgetItem *i;

  SettingsSheet_KeyboardInput *ki = NULL;
  ki = new SettingsSheet_KeyboardInput( settings );
  Q_ASSERT( ki != NULL ); \
  ui->stackedWidget->addWidget( ki ); \
  i = NULL; \
  i = new QListWidgetItem( ki->windowIcon(), ki->windowTitle() ); \
  Q_ASSERT( i != NULL ); \
  ui->listWidget->addItem( i );

  SettingsSheet_AudioOutput *ao = NULL;
  ao = new SettingsSheet_AudioOutput( settings );
  Q_ASSERT( ao != NULL ); \
  ui->stackedWidget->addWidget( ao ); \
  i = NULL; \
  i = new QListWidgetItem( ao->windowIcon(), ao->windowTitle() ); \
  Q_ASSERT( i != NULL ); \
  ui->listWidget->addItem( i );
  connect( this, SIGNAL(accepted()), ao, SLOT(applySettings()) );

  // clicking on item activates corresponding settings sheet
  connect( ui->listWidget, SIGNAL(currentRowChanged(int)), ui->stackedWidget, SLOT(setCurrentIndex(int)) );
}


FrameDialog::~FrameDialog()
{
  delete ui;
}


void FrameDialog::changeEvent(QEvent *e)
{
  QDialog::changeEvent(e);
  switch (e->type()) {
  case QEvent::LanguageChange:
    ui->retranslateUi(this);
    break;
  default:
    break;
  }
}
