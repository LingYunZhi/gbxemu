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
#include "../gba2/common/cdriver_input.h"


CAppSettings::CAppSettings( QObject *parent )
  : QObject( parent )
{
  loadDefaultKeyAssignments();

  QCoreApplication::setOrganizationName( "Open Source" );
  QCoreApplication::setApplicationName( "VisualBoyAdvance 2" );
  load();
}


CAppSettings::~CAppSettings()
{
  save();
}


void CAppSettings::save()
{
  QSettings s;
  s.setValue( "soundOutput/deviceID", s_soundOutputDevice );
  s.setValue( "soundOutput/enableAudioSync", s_enableAudioSync );
  s.setValue( "directories/biosFile", s_biosFile );
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

  // key assignments
  for( int i = 0; i < s_keyAssignments.size(); i++ ) {
    s.setValue( "keyAssignments/" + s_keyAssignments.at(i)->getActionName(), s_keyAssignments.at(i)->getKeyCode() );
  }
}


void CAppSettings::load()
{
  QString appDir = qApp->applicationDirPath();

  QSettings s;
  s_soundOutputDevice = s.value( "soundOutput/deviceID", 0 ).toInt();
  s_enableAudioSync = s.value( "soundOutput/enableAudioSync", true ).toBool();
  s_biosFile = s.value( "directories/biosFile", "" ).toString();
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

  // key assignments
  for( int i = 0; i < s_keyAssignments.size(); i++ ) {
    bool ok;
    const int key = s.value( "keyAssignments/" + s_keyAssignments.at(i)->getActionName(), 0 ).toInt( &ok );
    if( (key != 0) && ok ) {
      s_keyAssignments.at(i)->setKeyCode( key );
    }
  }
}


void CAppSettings::loadDefaultKeyAssignments()
{
  s_keyAssignments.clear();

  s_keyAssignments.append( new KeyAssignment( "UP", Qt::Key_Up, CDriver_Input::BUTTON_UP, this ) );
  s_keyAssignments.append( new KeyAssignment( "DOWN", Qt::Key_Down, CDriver_Input::BUTTON_DOWN, this ) );
  s_keyAssignments.append( new KeyAssignment( "LEFT", Qt::Key_Left, CDriver_Input::BUTTON_LEFT, this ) );
  s_keyAssignments.append( new KeyAssignment( "RIGHT", Qt::Key_Right, CDriver_Input::BUTTON_RIGHT, this ) );
  s_keyAssignments.append( new KeyAssignment( "B", Qt::Key_A, CDriver_Input::BUTTON_B, this ) );
  s_keyAssignments.append( new KeyAssignment( "A", Qt::Key_S, CDriver_Input::BUTTON_A, this ) );
  s_keyAssignments.append( new KeyAssignment( "L", Qt::Key_Q, CDriver_Input::BUTTON_L, this ) );
  s_keyAssignments.append( new KeyAssignment( "R", Qt::Key_W, CDriver_Input::BUTTON_R, this ) );
  s_keyAssignments.append( new KeyAssignment( "SELECT", Qt::Key_X, CDriver_Input::BUTTON_SELECT, this ) );
  s_keyAssignments.append( new KeyAssignment( "START", Qt::Key_Return, CDriver_Input::BUTTON_START, this ) );
}
