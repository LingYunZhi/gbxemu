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


SettingsSheet_KeyboardInput::SettingsSheet_KeyboardInput( CAppSettings &settings, QWidget *parent )
  : SettingsSheet( settings, parent ), ui(new Ui::SettingsSheet_KeyboardInput)
{
  ui->setupUi(this);
}


SettingsSheet_KeyboardInput::~SettingsSheet_KeyboardInput()
{
  delete ui;
}


void SettingsSheet_KeyboardInput::applySettings() {
}


RETRANSLATE_CODE( SettingsSheet_KeyboardInput )
