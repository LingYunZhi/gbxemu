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


#ifndef PRESSKEYDIALOG_H
#define PRESSKEYDIALOG_H


#include <QDialog>
#include <QLabel>
#include <QKeyEvent>
#include <QString>


class PressKeyDialog : public QDialog
{
  Q_OBJECT

public:
  PressKeyDialog( QString message, QWidget *parent = 0 )
    : QDialog( parent )
  {
    this->setWindowTitle( tr("Assign Key") );

    QLabel *label = new QLabel( message, this );
    label->setAlignment( Qt::AlignCenter );
    label->setWordWrap( true );
  }

protected:
  void keyPressEvent( QKeyEvent *e ) {
    done( e->key() );
  }
};


#endif // PRESSKEYDIALOG_H
