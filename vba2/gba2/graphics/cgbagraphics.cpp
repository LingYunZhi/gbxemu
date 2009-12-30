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
#include <assert.h>


CGBAGraphics::CGBAGraphics() {
  vram = NULL;
  vram = new u8[0x4000];
}


CGBAGraphics::~CGBAGraphics() {
  if( vram != NULL ) {
    delete [] vram;
    vram = NULL;
  }
}


void CGBAGraphics::setVRAM( const u8 *vram_src ) {
  memcpy( vram, vram_src, 0x4000 );
}


void CGBAGraphics::setIO( const u8 *io ) {
  // read & interpret all video registers
  u8 reg; // current register (low or high part)

  reg = io[0x00];
  DISPCNT.bgMode = reg & 7;
    if( DISPCNT.bgMode > 5 ) assert( false ); // error
  DISPCNT.frameNumber = reg & BIT4;
  DISPCNT.oamAccessDuringHBlank = reg & BIT5;
  DISPCNT.objCharMapping = reg & BIT6;
  DISPCNT.forcedBlank = reg & BIT7;
  reg = io[0x01];
  DISPCNT.displayBG0 = reg & BIT0;
  DISPCNT.displayBG1 = reg & BIT1;
  DISPCNT.displayBG2 = reg & BIT2;
  DISPCNT.displayBG3 = reg & BIT3;
  DISPCNT.displayOBJ = reg & BIT4;
  DISPCNT.displayWIN0 = reg & BIT5;
  DISPCNT.displayWIN1 = reg & BIT6;
  DISPCNT.displayOBJWIN = reg & BIT7;

  const u8 m = DISPCNT.bgMode; // display mode

  if( m <= 1 ) {
    // BG0
    reg = io[0x08];
    BG0CNT.priority = reg & 3;
    BG0CNT.tileOffset = ( (u16)reg & (BIT2|BIT3) ) << 12;
    BG0CNT.mosaic = reg & BIT6;
    BG0CNT.colorMode = reg & BIT7;
    reg = io[0x09];
    BG0CNT.mapOffset = ( (u16)reg & (BIT0|BIT1|BIT2|BIT3|BIT4) ) << 11;
    BG0CNT.width = ( reg & BIT6 ) ? 512 : 256;
    BG0CNT.height = ( reg & BIT7 ) ? 512 : 256;

    // BG1
    reg = io[0x0A];
    BG1CNT.priority = reg & 3;
    BG1CNT.tileOffset = ( (u16)reg & (BIT2|BIT3) ) << 12;
    BG1CNT.mosaic = reg & BIT6;
    BG1CNT.colorMode = reg & BIT7;
    reg = io[0x0B];
    BG1CNT.mapOffset = ( (u16)reg & (BIT0|BIT1|BIT2|BIT3|BIT4) ) << 11;
    BG1CNT.width = ( reg & BIT6 ) ? 512 : 256;
    BG1CNT.height = ( reg & BIT7 ) ? 512 : 256;
  }

  // BG2
  reg = io[0x0C];
  BG2CNT.priority = reg & 3;
  BG2CNT.tileOffset = ( (u16)reg & (BIT2|BIT3) ) << 12;
  BG2CNT.mosaic = reg & BIT6;
  BG2CNT.colorMode = reg & BIT7;
  reg = io[0x0D];
  BG2CNT.mapOffset = ( (u16)reg & (BIT0|BIT1|BIT2|BIT3|BIT4) ) << 11;
  BG2CNT.wrapAround = reg & BIT5;
  switch( m ) {
  case 0:
    BG2CNT.width = ( reg & BIT6 ) ? 512 : 256;
    BG2CNT.height = ( reg & BIT7 ) ? 512 : 256;
    break;
  case 1: // BG2 = rotation/scaling
  case 2:
    switch( reg & (BIT6|BIT7) ) {
    case 0: BG2CNT.width = BG2CNT.height =  128; break;
    case 1: BG2CNT.width = BG2CNT.height =  256; break;
    case 2: BG2CNT.width = BG2CNT.height =  512; break;
    case 3: BG2CNT.width = BG2CNT.height = 1024; break;
    }
    break;
  case 3: // BG2 = bitmap based
  case 4:
    BG2CNT.width = 240;
    BG2CNT.height = 160;
    break;
  case 5:
    BG2CNT.width = 160;
    BG2CNT.height = 128;
    break;
  }

  if( ( m == 0 ) || ( m == 2 ) ) {
    // BG3 = rotation/scaling
    reg = io[0x0E];
    BG3CNT.priority = reg & 3;
    BG3CNT.tileOffset = ( (u16)reg & (BIT2|BIT3) ) << 12;
    BG3CNT.mosaic = reg & BIT6;
    BG3CNT.colorMode = reg & BIT7;
    reg = io[0x0F];
    BG3CNT.mapOffset = ( (u16)reg & (BIT0|BIT1|BIT2|BIT3|BIT4) ) << 11;
    BG3CNT.wrapAround = reg & BIT5;
    switch( reg & (BIT6|BIT7) ) {
    case 0: BG2CNT.width = BG2CNT.height =  128; break;
    case 1: BG2CNT.width = BG2CNT.height =  256; break;
    case 2: BG2CNT.width = BG2CNT.height =  512; break;
    case 3: BG2CNT.width = BG2CNT.height = 1024; break;
    }
  }
}


void CGBAGraphics::render() {
  // prepare graphic surfaces and send them to graphics renderer to
  // combine & render them (possibly with hw acceleration)

  switch( DISPCNT.bgMode ) {
  case 0:
    break;
  }
}
