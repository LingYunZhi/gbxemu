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


#ifndef KEYASSIGNMENT_H
#define KEYASSIGNMENT_H


#include <QObject>
#include <QString>
#include <QtGlobal>


class KeyAssignment : public QObject
{
  Q_OBJECT

public:
  KeyAssignment( QString actionName, enum Qt::Key assignedKey, quint16 gbaButtonCode, QObject *parent = 0 )
   : QObject( parent )
  {
    Q_ASSERT( !actionName.isEmpty() );
    m_actionName = actionName;
    m_assignedKeyCode = (quint32)assignedKey;
    m_gbaButtonCode = gbaButtonCode;
  }

  const QString &getActionName() const {
    return m_actionName;
  }

  const int getKeyCode() const {
    return m_assignedKeyCode;
  }

  void setKeyCode( int keyCode ) {
    m_assignedKeyCode = keyCode;
  }

  const quint16 getGBAButtonCode() const {
    return m_gbaButtonCode;
  }

private:
  QString m_actionName;
  int m_assignedKeyCode; // Qt::Key
  quint16 m_gbaButtonCode; // GBA's KEYINPUT register
};


#endif // KEYASSIGNMENT_H
