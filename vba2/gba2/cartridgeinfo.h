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


#ifndef CARTRIDGEINFO_H
#define CARTRIDGEINFO_H


#include "common/IChipMemory.h"
#include "common/Types.h"
class BackupMedia; // avoid recursive include


typedef enum { NONE, SRAM, EEPROM, FLASH64KiB, FLASH128KiB } BACKUPMEDIATYPE;


class CartridgeInfo
{
public:
  CartridgeInfo();

  bool load( IChipMemory &rom );
  void connectBackupMedia( BackupMedia *backupMedia );


  // cartridge header data
  char gameTitle[13]; // 12 + zero byte
  char gameCode[5];   // 4 + zero byte
  char makerCode[3];  // 2 + zero byte
  u8   gameVersion;   // 0x00 = first release
  bool debuggingEnabled;
  bool checksumOK;

  // detected chips used in cartridge
  BACKUPMEDIATYPE backupType;
  bool usesRTC; // true: real-time clock detected

private:
  /**
    This function detects the type of backup chip/RTC used in a commercial (!) cartridge by searching the whole
    game ROM for specific strings that are inserted by Nintendo's build system.

    This does also work for the "Classic NES" series of games, which try to prevent piracy by reading from
    the SRAM area twice at bootup even though they actually use EEPROM later on. Fortunately, these game ROMs
    still only contain the EEPROM string, but not the SRAM string.
    */
  void scanChips( const u32 *romData, u32 romSize );

  BackupMedia *m_backupMedia;
};


#endif // CARTRIDGEINFO_H
