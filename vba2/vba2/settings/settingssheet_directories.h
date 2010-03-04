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


#ifndef SETTINGSSHEET_DIRECTORIES_H
#define SETTINGSSHEET_DIRECTORIES_H


#include "settingssheet.h"
#include <QLineEdit>


namespace Ui {
  class SettingsSheet_Directories;
}


class SettingsSheet_Directories : public SettingsSheet {
  Q_OBJECT
  RETRANSLATE_DECL

public:
  SettingsSheet_Directories( CAppSettings &settings, QWidget *parent = 0 );
  ~SettingsSheet_Directories();

private:
  Ui::SettingsSheet_Directories *ui;
  void selectDir( QLineEdit *editControl );
  void selectFile( QLineEdit *editControl );

public slots:
  void applySettings();

private slots:
    void on_pushButtonBIOS_clicked();
    void on_pushButtonCartridgeSaves_clicked();
    void on_pushButtonGameCartridges_clicked();

signals:
    void biosFileChanged( const QString & );
};


#endif // SETTINGSSHEET_DIRECTORIES_H
