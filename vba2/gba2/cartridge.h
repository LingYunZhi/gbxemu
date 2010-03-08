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


#ifndef CARTRIDGE_H
#define CARTRIDGE_H


#include "common/Types.h"
#include "common/ichipmemory.h"
#include "common/ichipaccess.h"
#include "cartridgerom.h"
#include "cartridgeinfo.h"
#include "backupmedia.h"


class Cartridge : public IChipAccess
{
public:
  Cartridge();
  ~Cartridge();

  // return interface to game ROM
  IChipMemory &getROM();
  // return interface to cartridge info
  CartridgeInfo &getInfo();
  // return interface to backup chip (SRAM / EEPROM / FLASH)
  BackupMedia *getSave();


  // access cartridge bus (ROM and additional chips)
  // - address range from 0x08000000 to 0x0FFFFFFF is handled
  // - returns false if address could not be accessed
  bool read8  ( u32 address,  u8 &value );
  bool write8 ( u32 address,  u8  value );
  bool read16 ( u32 address, u16 &value );
  bool write16( u32 address, u16  value );
  bool read32 ( u32 address, u32 &value );

private:
  CartridgeROM  m_rom;
  CartridgeInfo m_info;
  BackupMedia   m_save;
};


#endif // CARTRIDGE_H
