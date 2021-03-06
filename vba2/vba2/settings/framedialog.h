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


#ifndef FRAMEDIALOG_H
#define FRAMEDIALOG_H


#include <QDialog>
#include "settingssheet.h"
#include "settingssheet_keyboardinput.h"
#include "settingssheet_graphicsoutput.h"
#include "settingssheet_audiooutput.h"
#include "settingssheet_directories.h"


namespace Ui {
  class FrameDialog;
}


class FrameDialog : public QDialog {
  Q_OBJECT
  RETRANSLATE_DECL

public:
  FrameDialog( CAppSettings &settings, QWidget *parent = 0 );
  ~FrameDialog();
  SettingsSheet_KeyboardInput  *sh_keyboardInput;
  SettingsSheet_GraphicsOutput *sh_graphicsOutput;
  SettingsSheet_AudioOutput    *sh_audioOutput;
  SettingsSheet_Directories    *sh_directories;


private:
  Ui::FrameDialog *ui;
};


#endif // FRAMEDIALOG_H
