/*  VisualBoyAdvance 2
    Copyright (C) 2009  VBA development team

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


#include "cgbagraphics.h"


#include <string.h> // for memcpy


CGBAGraphics::CGBAGraphics() {
  vram = NULL;
  vram = new u16[16*1024 / 2]; // 16 KiB VRAM
}


CGBAGraphics::~CGBAGraphics() {
  if( vram != NULL ) {
    delete [] vram;
    vram = NULL;
  }
}


void CGBAGraphics::setVRAM( const u16 *vram_src ) {
  memcpy( vram, vram_src, 16*1024 );
}


void CGBAGraphics::setIO( const u16 *io ) {
  // read & interpret all video registers
  u16 reg; // current register

  reg = io[0x00];
  DISPCNT.bgMode = reg & 7;
  DISPCNT.frameNumber = reg & BIT4;
  DISPCNT.oamAccessDuringHBlank = reg & BIT5;
  DISPCNT.objCharMapping = reg & BIT6;
  DISPCNT.forcedBlank = reg & BIT7;
  DISPCNT.displayBG0 = reg & BIT8;
  DISPCNT.displayBG1 = reg & BIT9;
  DISPCNT.displayBG2 = reg & BIT10;
  DISPCNT.displayBG3 = reg & BIT11;
  DISPCNT.displayOBJ = reg & BIT12;
  DISPCNT.displayWIN0 = reg & BIT13;
  DISPCNT.displayWIN1 = reg & BIT14;
  DISPCNT.displayOBJWIN = reg & BIT15;
}


void CGBAGraphics::render() {
  // prepare graphic surfaces and send them to graphics renderer to
  // combine & render them (possibly with hw acceleration)
}
