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
#include <string.h> // memset


BackupMedia::BackupMedia() {
  m_type = NONE;
  m_data = NULL;
  m_size = 0;
  writeOccured = false;
}


BackupMedia::~BackupMedia() {
  if( m_data != NULL ) {
    delete[] m_data;
  }
}


void BackupMedia::setType( BACKUPMEDIATYPE type ) {
  m_type = type;

  switch( m_type ) {
  case NONE:
    break;
  case SRAM:
    // SRAM is always 32 KiB
    m_size = SIZE_SRAM;
    m_data = new u8[SIZE_SRAM];
    assert( m_data != NULL );
    memset( m_data, 0xFF, SIZE_SRAM );
    break;
  case EEPROM:
    // EEPROM is either 512 Bytes or 8 KiB
    m_size = 0; // has to be detected first!
    m_eepromAddressBits = 0;
    m_data = new u8[SIZE_EEPROM_LARGE];
    assert( m_data != NULL );
    memset( m_data, 0xFF, SIZE_EEPROM_LARGE );
    m_eepromState = IDLE;
    m_eepromBitsRead = 68; // 68 means can not read more
    break;
  case FLASH64KiB:
    m_size = SIZE_FLASH_SMALL;
    m_data = new u8[SIZE_FLASH_SMALL];
    assert( m_data != NULL );
    memset( m_data, 0xFF, SIZE_FLASH_SMALL );
    m_flashState = F_IDLE;
    m_flashEraseMode = false;
    // (64 KiB) Panasonic MN63F805MNP
    m_mfgID = 0x32;
    m_deviceID = 0x1B;
    break;
  case FLASH128KiB:
    m_size = SIZE_FLASH_LARGE;
    m_data = new u8[SIZE_FLASH_LARGE];
    assert( m_data != NULL );
    memset( m_data, 0xFF, SIZE_FLASH_LARGE );
    m_flashState = F_IDLE;
    m_flashEraseMode = false;
    m_bankSwitched = false;
    // (128 KiB) Sanyo LE26FV10N1TS
    m_mfgID = 0x62;
    m_deviceID = 0x13;
    break;
  }
}


BACKUPMEDIATYPE BackupMedia::getType() {
  return m_type;
}


u8 BackupMedia::read8( u32 address ) {
  assert( address & 0x0E000000 );
  address &= 0xFFFF;

  switch( m_type ) {
  default:
    break;
  case SRAM:
    assert( address < 0x8000 );
    return m_data[address & 0x7FFF];
  case FLASH128KiB:
    if( m_bankSwitched ) {
      address += 0x10000;
    }
    // no break/return here, fall through to FLASH64KiB is intended.
  case FLASH64KiB:
    if( m_flashState == F_IDLE ) {
      // read & return data from flash cells
      return m_data[address];
    }
    // return flash manufacturer/device ID
    if( m_flashState == F_ID_MODE ) {
      if( address == 0 ) {
        return m_mfgID;
      } else if( address == 1 ) {
        return m_deviceID;
      }
    }
    break;
  }

  assert( false );
  return 0;
}


void BackupMedia::write8( u8 data, u32 address ) {
  assert( address & 0x0E000000 );
  address &= 0xFFFF;

  switch( m_type ) {
  default:
    break;
  case SRAM:
    assert( address < 0x8000 );
    m_data[address & 0x7FFF] = data;
    writeOccured = true;
    return;
  case FLASH64KiB:
  case FLASH128KiB:
    switch( m_flashState ) {
    case F_IDLE:
      if( data == 0xF0 ) {
        return; // ignore
      }
      // no break; here
    case F_ID_MODE:
      if( address == 0x5555 ) {
        if( data == 0xAA ) {
          m_flashState = F_RECEIVING_COMMAND1;
          return;
        }
      }
      break;
    case F_RECEIVING_COMMAND1:
      if( (address==0x2AAA) && (data==0x55) ) {
        m_flashState = F_RECEIVING_COMMAND2;
        return;
      }
      break;
    case F_RECEIVING_COMMAND2:
      if( address == 0x5555 ) {
        switch( data ) {
        default: break;
        case 0x10: // erase entire chip
          if( m_flashEraseMode ) {
            memset( m_data, 0xFF, m_size );
            m_flashEraseMode = false;
            m_flashState = F_IDLE;
            writeOccured = true;
            return;
          }
          break;
        case 0x80: // enter erase mode
          m_flashEraseMode = true;
          m_flashState = F_IDLE;
          return;
        case 0x90: // enter ID mode
          m_flashState = F_ID_MODE;
          return;
        case 0xA0: // write single byte mode
          m_flashState = F_WRITE_SINGLE_BYTE;
          return;
        case 0xB0: // enter bank switching mode
          m_flashState = F_BANK_MODE;
          return;
        case 0xF0: // back to idle mode
          m_flashState = F_IDLE;
          return;
        }
      }
      if( m_flashEraseMode &&
          (data == 0x30) &&
          !(address & 0x0FFF) ) {
        // erase specific sector (4 KiB) by writing 0xFF bytes
        if( m_bankSwitched && (m_type==FLASH128KiB) ) {
          address += 0x10000;
        }
        memset( &m_data[address], 0xFF, 0x1000 );
        m_flashEraseMode = false;
        m_flashState = F_IDLE;
        writeOccured = true;
        return;
      }
      break;
    case F_BANK_MODE:
      if( address == 0 ) {
        assert( data <= 1 );
        m_bankSwitched = data & 1;
        m_flashState = F_IDLE;
        return;
      }
      break;
    case F_WRITE_SINGLE_BYTE:
      // write a single byte to address
      if( m_bankSwitched && (m_type==FLASH128KiB) ) {
        address += 0x10000;
      }
      // the address must have been previously erased
      assert( m_data[address] == 0xFF );
      m_data[address] = data;
      m_flashState = F_IDLE;
      writeOccured = true;
      return;
    }
  }

  assert( false );
}


bool BackupMedia::detectEEPROMSize( u32 dmaCount ) {
  assert( m_type == EEPROM );
  assert( m_eepromState == IDLE );

  /*  dmaCount for different EEPROM sizes
      512 Bytes:
      set read address: 9 bits (!)
      read: 68 bits
      write data to address: 73 bits

      8 KiB:
      set read address: 17 bits (!)
      read: 68 bits
      write data to address: 81 bits

      other dmaCount values should NOT occur in the EEPROM address space!
  */

  switch( dmaCount ) {
  case 9:
    m_size = SIZE_EEPROM_SMALL;
    m_eepromAddressBits = ADDRESS_BITS_EEPROM_SMALL;
    return true;
  case 17:
    m_size = SIZE_EEPROM_LARGE;
    m_eepromAddressBits = ADDRESS_BITS_EEPROM_LARGE;
    return true;
  }

  return false;
}


u16 BackupMedia::read16( u32 address ) {
  assert( m_type == EEPROM );
  assert( address & 0x0D000000 );

  switch( m_eepromState ) {
  case READING:
    {
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
          m_eepromState = IDLE;
        }
        return (m_data[current_eeprom_address] & mask) ? 1 : 0;
      }
      break;
    }
  case IDLE:
  case SETTING_ADDRESS:
  case WRITING:
    // acknowledge we are ready
    return 1;
  }

  // error: read occured, but no address was specified before
  assert( false );
  return 0;
}


void BackupMedia::write16( u16 data, u32 address ) {
  assert( m_type == EEPROM );
  assert( address & 0x0D000000 );
  assert( m_size != 0 );

  const bool bit = data & 1;
  static u8 bit_count; // number of bits already received
  static u16 buffer;

  switch( m_eepromState ) {
  case IDLE:
    // receive a '1' bit to awake from idle state
    assert( bit == 1 );
    bit_count = 0;
    buffer = 0;
    m_eepromState = SETTING_ADDRESS;
    break;
  case SETTING_ADDRESS:
    {
      assert( bit_count < m_eepromAddressBits );
      // receive in order:
      // - requested mode (1=read 0=write)
      // - address (6/14 bit, MSB first), references an 8 byte block of EEPROM storage
      if( bit_count < m_eepromAddressBits ) {
        buffer <<= 1;
        buffer |= bit;
      }
      if( bit_count == (m_eepromAddressBits-1) ) {
        // receiving last address bit
        const u16 modeMask = (1 << (m_eepromAddressBits-2));
        m_eepromState = (buffer & modeMask) ? READING : WRITING;
        m_eepromAddress = (buffer & (modeMask - 1)) << 3;
        buffer = 0; // we need this to be 0 in case WRITING
      }
    }
    break;
  case READING:
    // after SETTING_ADDRESS, a single end bit is expected to be received, not more!
    assert( bit_count == m_eepromAddressBits );
    m_eepromBitsRead = 0;
    break;
  case WRITING:
    // receive in order:
    // - data to be written to m_eepromAddress (64 bit, MSB first)
    // - a single end bit
    assert( bit_count >= m_eepromAddressBits );
    if( bit_count < (m_eepromAddressBits+64) ) {
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
      assert( bit_count == (m_eepromAddressBits+64) );
      m_eepromState = IDLE; // reset state to beginning
    }
    break;
  }
  bit_count++;
}


u32 BackupMedia::getSize() {
  if( (m_size == 0) && (m_type == EEPROM) ) {
    // exception: Since we can not know about the size before emulation started, we
    // return the maximum size in case the emu wants to load a save file.
    return SIZE_EEPROM_LARGE;
  }
  return m_size;
}


u8 *BackupMedia::getData() {
  return m_data;
}
