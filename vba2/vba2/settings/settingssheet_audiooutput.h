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


#ifndef SETTINGSSHEET_AUDIOOUTPUT_H
#define SETTINGSSHEET_AUDIOOUTPUT_H

#include <QWidget>
class CAppSettings;


namespace Ui {
  class SettingsSheet_AudioOutput;
}


class SettingsSheet_AudioOutput : public QWidget {
  Q_OBJECT

public:
  SettingsSheet_AudioOutput( CAppSettings &settings, QWidget *parent = 0 );
  ~SettingsSheet_AudioOutput();

protected:
  void changeEvent(QEvent *e);

private:
  Ui::SettingsSheet_AudioOutput *ui;
  CAppSettings &m_settings;

public slots:
  void applySettings();
};


#endif // SETTINGSSHEET_AUDIOOUTPUT_H
