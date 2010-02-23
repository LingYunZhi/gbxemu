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


#include "cappsettings.h"
#include <QSettings>
#include <QtCore/QCoreApplication>


CAppSettings::CAppSettings( QObject *parent )
  : QObject( parent )
{
  QCoreApplication::setOrganizationName( "Open Source" );
  QCoreApplication::setApplicationName( "VisualBoyAdvance 2" );
  load();
}


CAppSettings::~CAppSettings() {
  save();
}


void CAppSettings::save() {
  QSettings s;
  s.setValue( "audioOutput/deviceID", s_soundOutputDevice );
  s.setValue( "directories/cartridgeFiles", s_cartridgeFilesDir );
  s.setValue( "directories/cartridgeSaves", s_cartridgeSavesDir );
  s.setValue( "graphicsOutput/enableVSync", s_enableVSync );
  s.setValue( "soundOutput/enableAudioSync", s_enableAudioSync );
}


void CAppSettings::load() {
  QString appDir = qApp->applicationDirPath();

  QSettings s;
  s_soundOutputDevice = s.value( "audioOutput/deviceID", 0 ).toInt();
  s_cartridgeFilesDir = s.value( "directories/cartridgeFiles", appDir ).toString();
  s_cartridgeSavesDir = s.value( "directories/cartridgeSaves", appDir ).toString();
  s_enableVSync = s.value( "graphicsOutput/enableVSync", true ).toBool();
  s_enableAudioSync = s.value( "soundOutput/enableAudioSync", true ).toBool();
}
