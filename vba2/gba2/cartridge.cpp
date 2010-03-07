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


#include "cartridge.h"
#include <assert.h>


Cartridge::Cartridge()
{
  m_info.connectBackupMedia( &m_save );
  m_rom.connectInfo( &m_info );
}


Cartridge::~Cartridge()
{
}


IChipMemory &Cartridge::getROM() {
  return m_rom;
}


CartridgeInfo &Cartridge::getInfo() {
  return m_info;
}


BackupMedia *Cartridge::getSave() {
  return &m_save;
}


bool Cartridge::read8( u32 address, u8 &value ) {
  assert( address & 0x08000000 );

  if( address >= 0x0E000000 ) {
  } else {
    // address < 0x0E000000
    return m_rom.read8( address, value );
  }

  assert( false );
  return false;
}


bool Cartridge::read16( u32 address, u16 &value ) {
  assert( address & 0x08000000 );

  if( address >= 0x0E000000 ) {
  } else {
    // address < 0x0E000000
    return m_rom.read16( address, value );
  }

  assert( false );
  return false;
}


bool Cartridge::read32( u32 address, u32 &value ) {
  assert( address & 0x08000000 );

  if( address >= 0x0E000000 ) {
  } else {
    // address < 0x0E000000
    return m_rom.read32( address, value );
  }

  assert( false );
  return false;
}


bool Cartridge::write8( u32 address, u8 value ) {
  assert( address & 0x08000000 );
  return false;
}


bool Cartridge::write16( u32 address, u16 value ) {
  assert( address & 0x08000000 );
  return false;
}


bool Cartridge::write32( u32 address, u32 value ) {
  assert( address & 0x08000000 );
  return false;
}
