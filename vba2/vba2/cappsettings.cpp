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
#include <QFile>


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
  s.setValue( "soundOutput/deviceID", s_soundOutputDevice );
  s.setValue( "soundOutput/enableAudioSync", s_enableAudioSync );
  s.setValue( "directories/cartridgeFiles", s_cartridgeFilesDir );
  s.setValue( "directories/cartridgeSaves", s_cartridgeSavesDir );
  s.setValue( "graphicsOutput/enableVSync", s_enableVSync );
  s.setValue( "graphicsOutput/enableSmoothStretching", s_enableSmoothStretching );

  // recent files
  s.remove( "recentFiles" ); // to prevent dead entries when some files don't exist anymore
  int nFiles = s_recentFiles.size();
  if( nFiles > maxRecentFiles ) nFiles = maxRecentFiles;
  s.setValue( "recentFiles/count", nFiles );
  for( int i = 0; i < nFiles; i++ ) {
    s.setValue( "recentFiles/slot" + QString::number(i), s_recentFiles.at(i) );
  }
}


void CAppSettings::load() {
  QString appDir = qApp->applicationDirPath();

  QSettings s;
  s_soundOutputDevice = s.value( "soundOutput/deviceID", 0 ).toInt();
  s_enableAudioSync = s.value( "soundOutput/enableAudioSync", true ).toBool();
  s_cartridgeFilesDir = s.value( "directories/cartridgeFiles", appDir ).toString();
  s_cartridgeSavesDir = s.value( "directories/cartridgeSaves", appDir ).toString();
  s_enableVSync = s.value( "graphicsOutput/enableVSync", true ).toBool();
  s_enableSmoothStretching = s.value( "graphicsOutput/enableSmoothStretching", false ).toBool();

  // recent files
  int nFiles = s.value( "recentFiles/count", 0 ).toInt();
  if( nFiles > maxRecentFiles ) nFiles = maxRecentFiles;
  QString temp;
  QString keyName;
  for( int i = 0; i < nFiles; i++ ) {
    keyName = "recentFiles/slot" + QString::number(i);
    temp = s.value( keyName, "" ).toString();
    if( temp.isEmpty() ) continue;
    // check if file still exists
    if( !QFile::exists( temp ) ) continue;
    s_recentFiles.append( temp );
  }
}
