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


#include "cartridgeinfo.h"
#include <assert.h>
#include "backupmedia.h"


CartridgeInfo::CartridgeInfo()
{
  gameTitle[0] = '\0';
  gameCode[0] = '\0';
  makerCode[0] = '\0';
  gameVersion = 0;
  debuggingEnabled = false;
  checksumOK = false;

  backupType = NONE;
  usesRTC = false;

  m_backupMedia = NULL;
}


inline void copyMemory( const u8 *from, u8 *to, u8 length ) {
  while( length-- ) {
    *to++ = *from++;
  }
}


bool CartridgeInfo::load( IChipMemory &rom ) {
  const u8 *romData = (const u8 *)rom.getData();

  // read cartridge header from ROM
  copyMemory( romData + 0xA0, (u8*)gameTitle, 12 );
  gameTitle[12] = 0;

  copyMemory( romData + 0xAC, (u8*)gameCode, 4 );
  gameCode[4] = 0;

  copyMemory( romData + 0xB0, (u8*)makerCode, 2 );
  makerCode[2] = 0;

  gameVersion = romData[0xBC];

  // handle undefined instructions if bit 2 and 7 are set
  debuggingEnabled = romData[0x9C] & 0x84;

  // calculate checksum
  u8 chk = 0;
  for( u8 i = 0xA0; i <= 0xBC; i++ ) {
    chk -= romData[i];
  }
  chk = ( chk - 0x19 ) & 0xFF;
  checksumOK = ( romData[0xBD] == chk );

  scanChips( (const u32 *)romData, rom.getSize() );

  if( m_backupMedia != NULL ) {
    // tell backup media what chip to emulate
    m_backupMedia->setType( backupType );
  }

  return true;
}


void CartridgeInfo::connectBackupMedia( BackupMedia *backupMedia ) {
  m_backupMedia = backupMedia;
}



u32 stringToValue( const char s[5] ) {
  // maximum string length: 4 characters + zero byte
  u64 result = 0;
  for( int i = 0; i < 4; i++ ) {
    if( s[i] == 0 ) break;
    result |= ( s[i] << (i*8) );
  }
  return result;
}


void CartridgeInfo::scanChips( const u32 *romData, u32 romSize ) {
  assert( (romData != NULL) && (romSize > 192) );

  /* possible strings are:

     backup media:
     - "EEPROM_V"
     - "SRAM_V"
     - "SRAM_F_V"
     - "FLASH_V"
     - "FLASH512_V"
     - "FLASH1M_V"

     real-time clock:
     - "SIIRTC_V"

     It is assumed, that all strings are word-aligned.
  */

  // first part
  const u32 _EEPR = stringToValue( "EEPR" ); // EEPROM
  const u32 _SRAM = stringToValue( "SRAM" ); // SRAM
  const u32 _FLAS = stringToValue( "FLAS" ); // FLASH
  const u32 _SIIR = stringToValue( "SIIR" ); // RTC

  // following parts
  const u32 _OM_V = stringToValue( "OM_V" ); // EEPROM
  const u32 __Vxx = stringToValue(   "_V" ); // SRAM
  const u32 __F_V = stringToValue( "_F_V" ); // SRAM
  const u32 _H_Vx = stringToValue(  "H_V" ); // FLASH  64 KiB
  const u32 _H512 = stringToValue( "H512" ); // FLASH  64 KiB
  const u32 _H1M_ = stringToValue( "H1M_" ); // FLASH 128 KiB
  const u32 _TC_V = stringToValue( "TC_V" ); // RTC

  const u32 endAdress = ( romSize / 4 ) - 2;
  bool saveTypeFound = false;
  bool rtcFound = false;
  BACKUPMEDIATYPE result = NONE;

  for( u32 a/*ddress*/ = 0; a < endAdress; a++ ) {
    const u32 block = romData[a];

    if( block == _EEPR ) { // EEPROM
      if( romData[a+1] == _OM_V ) {
        result = EEPROM;
        saveTypeFound = true;
      }
    } else if( block == _SRAM ) { // SRAM
      const u32 next32 = romData[a+1];
      const u32 next16 = next32 & 0xFFFF;
      if( ( next32 == __F_V ) ||
          ( next16 == __Vxx ) ) {
        result = SRAM;
        saveTypeFound = true;
      }
    } else if( block == _FLAS ) { // FLASH
      const u32 next32 = romData[a+1];
      const u32 next24 = next32 & 0xFFFFFF;
      if( next32 == _H1M_ ) {
        result = FLASH128KiB;
        saveTypeFound = true;
      } else if( ( next32 == _H512 ) ||
                 ( next24 == _H_Vx ) ) {
        result = FLASH64KiB;
        saveTypeFound = true;
      }
    } else if( block == _SIIR ) { // RTC
      const u32 next32 = romData[a+1];
      if( next32 == _TC_V ) {
        rtcFound = true;
      }
    }

    if( saveTypeFound && rtcFound ) break; // finished
  }

  backupType = result;
  usesRTC = rtcFound;
}
