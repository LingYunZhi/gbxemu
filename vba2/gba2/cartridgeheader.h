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


// this class is not required by the emulator core, but a helper class for the GUI


#ifndef CARTRIDGEHEADER_H
#define CARTRIDGEHEADER_H


#include "common/Types.h"


class CartridgeHeader
{
public:
  // romData must be >= 192 bytes!
  CartridgeHeader( const u8 *romData );

  char gameTitle[13]; // 12 + zero byte
  char gameCode[5]; // 4 + zero byte
  char makerCode[3]; // 2 + zero byte
  u8 gameVersion; // 0x00 = first release
  bool debuggingEnabled;
  bool checksumOK;
};


#endif // CARTRIDGEHEADER_H
