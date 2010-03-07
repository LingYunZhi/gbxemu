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


#ifndef ICHIPMEMORY_H
#define ICHIPMEMORY_H


#include "Types.h"


/**
  This class abstracts access to memory data in chips like ROM, RAM, EEPROM, FLASH, SRAM, etc
  */
class IChipMemory
{
public:
  // Some chips need to know their size first before uploading data.
  // Changing the size will unload all data first!
  virtual bool setSize( u32 size ) = 0;
  virtual u32  getSize() = 0;

  // In order to upload data to the chip, use lockData() followed by unlockData().
  virtual void  *lockData() = 0;
  virtual bool unlockData() = 0;

  // If you only need read access to the data, use this function.
  virtual const void *getData() = 0;

  // The usage of this function is optional to save some memory during runtime
  virtual bool unload() = 0;
};


#endif // ICHIPMEMORY_H
