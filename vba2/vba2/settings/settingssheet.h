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


#ifndef SETTINGSSHEET_H
#define SETTINGSSHEET_H


#include <QWidget>
#include "../cappsettings.h"


#define RETRANSLATE_DECL \
protected: \
  void changeEvent( QEvent *e );

#define RETRANSLATE_CODE( CLASS_NAME ) \
  void CLASS_NAME::changeEvent( QEvent *e ) { \
    QWidget::changeEvent( e ); \
    switch( e->type() ) { \
    case QEvent::LanguageChange: \
      ui->retranslateUi( this ); \
      break; \
    default: \
      break; \
    } \
  }



// a base class for settings sheets that are loaded by a QStackedWidget
class SettingsSheet : public QWidget {
  Q_OBJECT

public:
  SettingsSheet( CAppSettings &settings, QWidget *parent = 0 )
    : QWidget( parent ), m_settings( settings ) { }

protected:
  // the sheet shall modify these settings
  CAppSettings &m_settings;

public slots:
  // called when the settings dialog's ok button is pressed
  virtual void applySettings() = 0;
};


#endif // SETTINGSSHEET_H
