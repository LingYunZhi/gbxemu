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


#ifndef CAPPSETTINGS_H
#define CAPPSETTINGS_H


#include <QObject>
#include <QtMultimedia/QAudioDeviceInfo>


class CAppSettings : public QObject
{
  Q_OBJECT

public:
  CAppSettings( QObject *parent = 0 );
  ~CAppSettings();

private:
  void save();
  void load();

public: // settings:
  int     s_soundOutputDevice;
  bool    s_enableAudioSync;
  QString s_cartridgeFilesDir;
  QString s_cartridgeSavesDir;
  bool    s_enableVSync;
  bool    s_enableSmoothStretching;
};


#endif // CAPPSETTINGS_H
