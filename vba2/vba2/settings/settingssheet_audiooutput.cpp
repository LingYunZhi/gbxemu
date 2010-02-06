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


#include "settingssheet_audiooutput.h"
#include "ui_settingssheet_audiooutput.h"

SettingsSheet_AudioOutput::SettingsSheet_AudioOutput(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsSheet_AudioOutput)
{
    ui->setupUi(this);
}

SettingsSheet_AudioOutput::~SettingsSheet_AudioOutput()
{
    delete ui;
}

void SettingsSheet_AudioOutput::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
