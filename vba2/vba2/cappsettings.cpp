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


CAppSettings::CAppSettings( QObject *parent )
  : QObject( parent )
{
  // load settings
  saveLoad( false );
}


CAppSettings::~CAppSettings() {
  // save settings
  saveLoad( true );
}


bool CAppSettings::saveLoad( bool save ) {
  QSettings s( "Open Source", "VisualBoyAdvance 2" );

  // s_soundOutputDevice
  if( save ) {
    s.setValue( "audioOutput/deviceID", s_soundOutputDevice.deviceName() );
  } else {
    const QString str = s.value( "audioOutput/deviceID" ).toString();
    s_soundOutputDevice = QAudioDeviceInfo::defaultOutputDevice();

    const QList<QAudioDeviceInfo> deviceInfo = QAudioDeviceInfo::availableDevices( QAudio::AudioOutput );
    const int nDevices = deviceInfo.size();
    for( int i = 0; i < nDevices; i++ ) {
      const QAudioDeviceInfo &currentInfo = deviceInfo.at(i);
      if( str == currentInfo.deviceName() ) {
        s_soundOutputDevice = currentInfo;
      }
    }
  }

  // directories
  if( save ) {
    s.setValue( "directories/cartridgeFiles", s_cartridgeFilesDir );
    s.setValue( "directories/cartridgeSaves", s_cartridgeSavesDir );
  } else {
    s_cartridgeFilesDir = s.value( "directories/cartridgeFiles" ).toString();
    s_cartridgeSavesDir = s.value( "directories/cartridgeSaves" ).toString();
  }

  return true;
}
