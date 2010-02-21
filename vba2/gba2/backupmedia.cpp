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


#include "backupmedia.h"

#include <assert.h>
#include <string.h>


BackupMedia::BackupMedia( u32 *romData, u32 romSize )
{
  m_type = findOutType( romData, romSize );
  m_data = NULL;
  m_size = 0;
  writeOccured = false;

  switch( m_type ) {
  case SRAM:
    m_size = 0x8000; // 32 KiB
    m_data = new u8[m_size];
    assert( m_data != NULL );
    // TODO: is 0x00 the correct initial value?
    memset( m_data, 0x00, m_size );
    break;
  }
}


BackupMedia::~BackupMedia() {
  if( m_data != NULL ) {
    delete[] m_data;
  }
}


BackupMedia::BACKUPMEDIATYPE BackupMedia::getType() {
  return m_type;
}


u8 BackupMedia::read( u32 address ) {
  assert( m_type == SRAM );
  assert( address & 0xE000000 );
  assert( (address & 0x0FFFFFF) < 0x8000 );
  return m_data[address & 0x7FFF];
}


void BackupMedia::write( u8 data, u32 address ) {
  assert( m_type == SRAM );
  assert( address & 0xE000000 );
  assert( (address & 0x0FFFFFF) < 0x8000 );
  m_data[address & 0x7FFF] = data;
  writeOccured = true;
}


u32 BackupMedia::getSize() {
  return m_size;
}


u8 *BackupMedia::getData() {
  if( m_size == 0 ) return NULL;
  return m_data;
}


// used by findOutType()
u32 stringToValue( const char s[5] ) {
  // maximum string length: 4 characters + zero byte
  u64 result = 0;
  for( int i = 0; i < 4; i++ ) {
    if( s[i] == 0 ) break;
    result |= ( s[i] << (i*8) );
  }
  return result;
}


BackupMedia::BACKUPMEDIATYPE BackupMedia::findOutType( u32 *romData, u32 romSize )
{
  assert( romData != NULL );

  /* possible strings are:
     - "EEPROM_V"
     - "SRAM_V"
     - "SRAM_F_V"
     - "FLASH_V"
     - "FLASH512_V"
     - "FLASH1M_V"
  */

  // first part
  const u32 _EEPR = stringToValue( "EEPR" );
  const u32 _SRAM = stringToValue( "SRAM" );
  const u32 _FLAS = stringToValue( "FLAS" );

  // following parts
  const u32 _OM_V = stringToValue( "OM_V" );
  const u32 __Vxx = stringToValue(   "_V" );
  const u32 __F_V = stringToValue( "_F_V" );
  const u32 _H_Vx = stringToValue(  "H_V" );
  const u32 _H512 = stringToValue( "H512" );
  const u32 _H1M_ = stringToValue( "H1M_" );

  const u32 endAdress = ( romSize / 4 ) - 2;
  bool saveTypeFound = false;
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
    }
    if( saveTypeFound ) break; // finished
  }

  return result;
}
