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


#ifndef CARTRIDGEGPIO_H
#define CARTRIDGEGPIO_H


#include "common/Types.h"
#include "common/ichipaccess.h"
#include "common/igpio.h"


/**
  This class represents the 4 bit general purpose input/output bus contained in some ROM chips.

  Special devices like real-time clocks, light/movement sensors or rumble modules can be attached to it.
  */
class CartridgeGPIO : public IChipAccess
{
public:
  CartridgeGPIO();
  void connectDevice( IGPIO *device );
  bool read16 ( u32 address, u16 &value );
  bool write16( u32 address, u16  value );

private:
  bool m_writeOnly;
  u8   m_dataDirection; // 4 lowest bits control data port bits direction: 0=receive, 1=send
  IGPIO *m_connectedDevice;
};


#endif // CARTRIDGEGPIO_H
