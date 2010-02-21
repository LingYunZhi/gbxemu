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


/**
  This class manages GBA cartridge backup media (EEPROM/SRAM/FLASH).
  */
class BackupMedia
{
public:
  BackupMedia( u32 *romData, u32 romSize );
  ~BackupMedia();

  typedef enum { NONE, SRAM, EEPROM, FLASH64KiB, FLASH128KiB } BACKUPMEDIATYPE;
  static BACKUPMEDIATYPE findOutType( u32 *romData, u32 romSize );
  BACKUPMEDIATYPE getType();

  u8 read( u32 address );
  void write( u8 data, u32 address );

  // use these functions when loading/saving the backup media content from/to a file
  u32 getSize();
  u8 *getData();

  // you don't have to write m_data to a file if no write occured to it
  bool writeOccured;

private:
  BACKUPMEDIATYPE m_type;
  u8 *m_data;
  u32 m_size;
};


#endif // BACKUPMEDIA_H
