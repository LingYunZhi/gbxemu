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


#include <QApplication>
#include <QFileInfo>
#include "mainwindow.h"


int main(int argc, char *argv[])
{
  /* skins
  QStyle *style = NULL;
  style = QApplication::setStyle( "plastique" );
  Q_ASSERT( style != 0 );
  */
  QApplication a(argc, argv);
  QString startRom;
  QStringList argList = a.arguments();
  const int nArguments = argList.size();
  for( int i = 0; i < nArguments; i++ ) {
    // load & run first *.gba file we find in the argument list
    QString filePath = argList.at(i);
    QFileInfo fileInfo( filePath );
    if( !fileInfo.exists() ) continue;
    if( fileInfo.suffix().toLower() == "gba" ) {
      startRom = filePath;
      break;
    }
  }
  MainWindow w( 0, startRom );
  w.show();
  return a.exec();
}
