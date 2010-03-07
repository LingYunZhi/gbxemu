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


#ifndef BACKUPMEDIA_H
#define BACKUPMEDIA_H


#include "common/Types.h"
#include "cartridgeinfo.h"


/**
  This class manages GBA cartridge backup media (EEPROM/SRAM/FLASH).
  */
class BackupMedia
{
public:
  BackupMedia();
  ~BackupMedia();

  // Do not forget to use setType before using this class.
  void setType( BACKUPMEDIATYPE type );
  BACKUPMEDIATYPE getType();

  // SRAM
  u8 read8( u32 address );
  void write8( u8 data, u32 address );

  // EEPROM, FLASH
  // call this before any reads/writes from/to EEPROM occured! if it fails, call it again next time
  bool detectEEPROMSize( u32 dmaCount );
  u16 read16( u32 address );
  void write16( u16 data, u32 address );

  // use these functions when loading/saving the backup media content from/to a file
  // TODO: Use IChipMemory interface for this purpose.
  u32 getSize();
  u8 *getData();

  // you don't have to write m_data to a file if no write occured to it
  bool writeOccured;

private:
  BACKUPMEDIATYPE m_type;
  bool m_rtcPresent;
  u8 *m_data;
  u32 m_size;

  // EEPROM
  static const u32 SIZE_EEPROM_SMALL = 0x0200;
  static const u32 SIZE_EEPROM_LARGE = 0x2000;
  static const u32 ADDRESS_BITS_EEPROM_SMALL = 6+2;  // includes the two command bits
  static const u32 ADDRESS_BITS_EEPROM_LARGE = 14+2; //   for faster code later
  typedef enum { IDLE,
                 SETTING_ADDRESS,
                 READING,
                 WRITING } EEPROM_STATE;
  EEPROM_STATE m_eepromState;
  u16 m_eepromAddress;
  u8 m_eepromBitsRead;
  u8 m_eepromAddressBits;

  // SRAM
  static const u32 SIZE_SRAM = 0x8000;
  /** SRAM chip label of Metroid Fusion (Europe) (En,Fr,De,Es,It)
        "256K SRAM" besides chip
        "Hynix 0239A" "HY62WT081ED70C" "KOREA"
    */

  // FLASH
  static const u32 SIZE_FLASH_SMALL = 0x10000;
  static const u32 SIZE_FLASH_LARGE = 0x20000;
  typedef enum { F_IDLE, // = READ mode
                 F_RECEIVING_COMMAND1,
                 F_RECEIVING_COMMAND2,
                 F_ID_MODE,
                 F_BANK_MODE,
                 F_WRITE_SINGLE_BYTE } FLASH_STATE;
  FLASH_STATE m_flashState;
  bool m_flashEraseMode;
  bool m_bankSwitched; // true to access high 64 KiB of a 128 KiB flash chip

  u8 m_mfgID;
  u8 m_deviceID;
  /* known flash chips:
    Manufacturer  chip name     Manufacturer ID Device ID  size/KiB
    Sanyo or SST  LE39FW512     0xBF            0xD4        64
    SST           SST39VF512    0xBF            0xD4        64
    Atmel         AT29LV512     0x1F            0x3D        64
    Macronix      MX29L512      0xC2            0x1C        64
    Panasonic     MN63F805MNP   0x32            0x1B        64
    Macronix      MX29L010      0xC2            0x09       128
    Sanyo         LE26FV10N1TS  0x62            0x13       128
  */
};


#endif // BACKUPMEDIA_H
