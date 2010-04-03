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


#ifndef GENERICIOREGISTER_H
#define GENERICIOREGISTER_H


#include "Types.h"


class GenericIORegister
{
public:
  u16 read() { return m_data; }
  void write( u16 value ) { m_data = value; update(); }

protected:
  u16 m_data;
  // implement this function to process data bits to useful values
  virtual void update() = 0;
};


#endif // GENERICIOREGISTER_H
