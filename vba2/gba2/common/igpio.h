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


#ifndef IGPIO_H
#define IGPIO_H


#include "Types.h"


/**
  This class provides a standardized interface for sending and receiving 4 data bits through the cartridge general purpose input/output bus.
  */
class IGPIO {
public:
  /**
    send4 must only send bits if corresponding bit in mask4 is 1
    */
  virtual void send4( u8 data4, u8 mask4 ) = 0;

  /**
    receive4 must only return bits if corresponding bit in mask4 is 0
    */
  virtual u8 receive4( u8 mask4 ) = 0;
};


#endif // IGPIO_H
