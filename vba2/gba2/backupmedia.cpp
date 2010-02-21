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

//#define DEBUG_EEPROM
#ifdef DEBUG_EEPROM
#include <stdio.h>
#endif


BackupMedia::BackupMedia( u32 *romData, u32 romSize )
{
  m_type = findOutType( romData, romSize );
  m_data = NULL;
  m_size = 0;
  writeOccured = false;
  m_eepromWriteOK = true; // TODO: what is its default state?
  m_eepromBitsRead = 68; // 68 means can not read more

  switch( m_type ) {
  case SRAM:
    // SRAM is always 32 KiB
    m_size = SIZE_SRAM;
    m_data = new u8[SIZE_SRAM];
    assert( m_data != NULL );
    memset( m_data, 0xFF, SIZE_SRAM );
    break;
  case EEPROM:
    // EEPROM is either 512 Bytes or 8 KiB
    // size variable will grow if more than 512 Bytes are accessed by the game
    m_size = SIZE_EEPROM_SMALL;
    m_data = new u8[SIZE_EEPROM_LARGE];
    assert( m_data != NULL );
    memset( m_data, 0xFF, SIZE_EEPROM_LARGE );
    m_state = IDLE;
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


u8 BackupMedia::read8( u32 address ) {
  assert( m_type == SRAM );
  assert( address & 0x0E000000 );
  assert( (address & 0x00FFFFFF) < 0x8000 );
  return m_data[address & 0x7FFF];
}


void BackupMedia::write8( u8 data, u32 address ) {
  assert( m_type == SRAM );
  assert( address & 0x0E000000 );
  assert( (address & 0x00FFFFFF) < 0x8000 );
  m_data[address & 0x7FFF] = data;
  writeOccured = true;
}


u16 BackupMedia::read16( u32 address ) {
#ifdef DEBUG_EEPROM
  fprintf( stderr, "EEPROM read from %X\n", address );
#endif
  assert( m_type == EEPROM );
  assert( address & 0x0D000000 );

  if( m_state == READING ) {
    // read out:
    // - garbage (4 bits)
    // - data (64 bits, MSB first)

    assert( m_eepromBitsRead < 68 );

    if( m_eepromBitsRead < 4 ) {
      m_eepromBitsRead++;
      return 0; // garbage
    } else {
      const u8 data_bit_index = m_eepromBitsRead - 4;
      const u8 data_byte_index = data_bit_index >> 3;
      const u8 mask = 1 << ( 7 - (data_bit_index & 7) );
      const u16 current_eeprom_address = m_eepromAddress + data_byte_index;
      assert( current_eeprom_address < m_size );
      m_eepromBitsRead++;
      if( m_eepromBitsRead == 68 ) {
        m_state = IDLE;
      }
      return (m_data[current_eeprom_address] & mask) ? 1 : 0;
    }
  } else if ( (m_state == IDLE) && (address == 0x0D000000) ) {
    // confirm there were no write errors
    return m_eepromWriteOK;
  } else {
    // error: read occured, but no address was specified before
    assert( false );
  }
}


void BackupMedia::write16( u16 data, u32 address ) {
#ifdef DEBUG_EEPROM
  fprintf( stderr, "EEPROM write %X to %X\n", data, address );
#endif
  assert( m_type == EEPROM );
  assert( address & 0x0D000000 );

  const bool bit = data & 1;
  static u8 bit_count; // number of bits already received
  static u16 buffer;

  switch( m_state ) {
  case IDLE:
    // receive a '1' bit to awake from idle state
    assert( bit == 1 );
    bit_count = 0;
    buffer = 0;
    m_state = SETTING_ADDRESS;
    break;
  case SETTING_ADDRESS:
    assert( bit_count <= 7 );
    // receive in order:
    // - requested mode (1=read 0=write)
    // - address (6 bit, MSB first), references an 8 byte block of EEPROM storage
    if( bit_count <= 7 ) {
      buffer <<= 1;
      buffer |= bit;
    }
    if( bit_count == 7 ) {
      // receiving last address bit
      m_state = (buffer & 0x40) ? READING : WRITING;
      m_eepromAddress = (buffer & 0x3F) << 3;
      buffer = 0; // we need this to be 0 in case WRITING
    }
    break;
  case READING:
    // after SETTING_ADDRESS, a single end bit is expected to be received, not more!
    assert( bit_count == 8 );
    m_eepromBitsRead = 0;
    break;
  case WRITING:
    // receive in order:
    // - data to be written to m_eepromAddress (64 bit, MSB first)
    // - a single end bit
    assert( bit_count >= 8 );
    if( bit_count < (8+64) ) {
      m_eepromWriteOK = false; // not done yet
      buffer <<= 1;
      buffer |= bit;
      if( ((bit_count+1) % 8) == 0 ) {
        assert( m_eepromAddress < m_size );
        m_data[ m_eepromAddress ] = buffer;
        m_eepromAddress++;
        writeOccured = true; // signal there was a change to the save data
        buffer = 0;
      }
    } else {
      // ignore end bit
      assert( bit_count == (8+64) );
      m_eepromWriteOK = true; // done writing
      m_state = IDLE; // reset state to beginning
    }
    break;
  }
  bit_count++;
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
