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


#ifndef ICHIPACCESS_H
#define ICHIPACCESS_H


#include "Types.h"


/**
  This class provides a standardized interface for read and write access to a generic chip.

  All functions return false, if the chip could not handle the requested address.
  Override them with specialized code for your class.
  */
class IChipAccess
{
public:
  virtual bool read8  ( u32 address,  u8 &value ) { return false; }
  virtual bool read16 ( u32 address, u16 &value ) { return false; }
  virtual bool read32 ( u32 address, u32 &value ) { return false; }
  virtual bool write8 ( u32 address,  u8  value ) { return false; }
  virtual bool write16( u32 address, u16  value ) { return false; }
  virtual bool write32( u32 address, u32  value ) { return false; }
};


#endif // ICHIPACCESS_H
