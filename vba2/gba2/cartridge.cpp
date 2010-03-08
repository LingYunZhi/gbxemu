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
  switch( address >> 24 ) {
  case 0x08:
  case 0x09:
  case 0x0A:
  case 0x0B:
  case 0x0C:
  case 0x0D:
    return m_rom.read8( address, value );
  case 0x0E:
    return m_save.read8( address, value );
  }

  assert( false );
  return false;
}


bool Cartridge::write8( u32 address, u8 value ) {
  switch( address >> 24 ) {
  case 0x0E:
    return m_save.write8( address, value );
  }

  assert( false );
  return false;
}


bool Cartridge::read16( u32 address, u16 &value ) {
  switch( address >> 24 ) {
  case 0x0D:
    // EEPROM could be here as well
    // If not, ROM access request is assumed
    // EEPROM can be accessed from 0x0D000000 to 0x0DFFFFFF
    // ROM size is limited to 16 MiB if EEPROM chip is used
    if( m_save.getType() == EEPROM ) {
      return m_save.read16( address, value );
    }
  case 0x08:
  case 0x09:
  case 0x0A:
  case 0x0B:
  case 0x0C:
    return m_rom.read16( address, value );
  }

  assert( false );
  return false;
}


bool Cartridge::write16( u32 address, u16 value ) {
  switch( address >> 24 ) {
  case 0x08:
    // GPIO could be here
    if( m_info.usesRTC ) {
      switch( address ) {
      case 0x080000C4:
      case 0x080000C6:
      case 0x080000C8:
        return 0; // TODO: implement
      }
    }
    break;

  case 0x0D:
    if( m_save.getType() == EEPROM ) {
      return m_save.write16( address, value );
    }
  }

  assert( false );
  return false;
}


bool Cartridge::read32( u32 address, u32 &value ) {
  switch( address >> 24 ) {
  case 0x08:
  case 0x09:
  case 0x0A:
  case 0x0B:
  case 0x0C:
  case 0x0D:
    return m_rom.read32( address, value );
  }

  assert( false );
  return false;
}
