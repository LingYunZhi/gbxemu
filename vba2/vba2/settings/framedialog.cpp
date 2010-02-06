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

#include "settingssheet_audiooutput.h"
#include "settingssheet_keyboardinput.h"

#include <QListWidgetItem>


#define ADD_SHEET( type ) \
  w = NULL; \
  w = new type; \
  Q_ASSERT( w != NULL ); \
  ui->stackedWidget->addWidget( w ); \
  i = NULL; \
  i = new QListWidgetItem( w->windowIcon(), w->windowTitle() ); \
  Q_ASSERT( i != NULL ); \
  ui->listWidget->addItem( i );



FrameDialog::FrameDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FrameDialog)
{
  ui->setupUi(this);

  // parent will be assigned by the addWidget function
  QWidget *w;
  QListWidgetItem *i;

  ADD_SHEET( SettingsSheet_KeyboardInput );
  ADD_SHEET( SettingsSheet_AudioOutput );

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
