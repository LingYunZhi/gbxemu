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


#ifndef SETTINGSSHEET_KEYBOARDINPUT_H
#define SETTINGSSHEET_KEYBOARDINPUT_H


#include "settingssheet.h"


namespace Ui {
  class SettingsSheet_KeyboardInput;
}


class SettingsSheet_KeyboardInput : public SettingsSheet {
  Q_OBJECT
  RETRANSLATE_DECL

public:
  SettingsSheet_KeyboardInput( CAppSettings &settings, QWidget *parent = 0 );
  ~SettingsSheet_KeyboardInput();

private:
  Ui::SettingsSheet_KeyboardInput *ui;
  CAppSettings &m_settings;

public slots:
  void applySettings();

private slots:
  void on_tableWidget_cellDoubleClicked(int row, int column);
  void on_assign_all_clicked();
};


#endif // SETTINGSSHEET_KEYBOARDINPUT_H
