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


#include "cartridgeheader.h"


inline void copyMemory( const u8 *from, u8 *to, u8 length ) {
  while( length-- ) {
    *to++ = *from++;
  }
}


CartridgeHeader::CartridgeHeader( const u8 *romData )
{
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
}
