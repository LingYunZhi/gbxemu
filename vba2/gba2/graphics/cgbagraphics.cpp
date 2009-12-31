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
#include "../common/Port.h"


#define SAFE_DELETE_ARRAY( p ) \
  if( (p) != NULL ) { \
    delete [] (p); \
    (p) = NULL; \
  }


CGBAGraphics::CGBAGraphics() {
  vram = NULL;
  vram = new u8[0x4000];

  bg0chars = NULL;
  bg1chars = NULL;
  bg2chars = NULL;
  bg3chars = NULL;

  bg0bmp = NULL;
  bg1bmp = NULL;
  bg2bmp = NULL;
  bg3bmp = NULL;

  vramLoaded = false;
  ioLoaded = false;
  palLoaded = false;
}


CGBAGraphics::~CGBAGraphics() {
  SAFE_DELETE_ARRAY( bg0chars );
  SAFE_DELETE_ARRAY( bg1chars );
  SAFE_DELETE_ARRAY( bg2chars );
  SAFE_DELETE_ARRAY( bg3chars );

  SAFE_DELETE_ARRAY( bg0bmp );
  SAFE_DELETE_ARRAY( bg1bmp );
  SAFE_DELETE_ARRAY( bg2bmp );
  SAFE_DELETE_ARRAY( bg3bmp );

  SAFE_DELETE_ARRAY( vram );
}


void CGBAGraphics::gba2rgba( RGBACOLOR *dest, u16 src ) {
  dest->r = ( src & 0x001F ) << 3; // extend from 5 to 8 bit
  dest->g = ( src & 0x03E0 ) >> 2;
  dest->b = ( src & 0x7C00 ) >> 7;
  dest->a = 0xFF; // TODO: check which alpha value is transparent
  // TODO: use translation that offers pure white (0x00FFFFFF) for 0x7FFF GBA color
}


void CGBAGraphics::setVRAM( const u8 *vram_src ) {
  memcpy( vram, vram_src, 0x4000 );
  vramLoaded = true;
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
    // BG0 = character based
    reg = io[0x08];
    BG0CNT.priority = reg & 3;
    BG0CNT.tileOffset = ( (u16)reg & (BIT2|BIT3) ) << 12;
    BG0CNT.mosaic = reg & BIT6;
    BG0CNT.colorMode = reg & BIT7;
    reg = io[0x09];
    BG0CNT.mapOffset = ( (u16)reg & (BIT0|BIT1|BIT2|BIT3|BIT4) ) << 11;
    BG0CNT.width = ( reg & BIT6 ) ? 512 : 256;
    BG0CNT.height = ( reg & BIT7 ) ? 512 : 256;
    BG0CNT.isRotScale = false;

    // BG1 = character based
    reg = io[0x0A];
    BG1CNT.priority = reg & 3;
    BG1CNT.tileOffset = ( (u16)reg & (BIT2|BIT3) ) << 12;
    BG1CNT.mosaic = reg & BIT6;
    BG1CNT.colorMode = reg & BIT7;
    reg = io[0x0B];
    BG1CNT.mapOffset = ( (u16)reg & (BIT0|BIT1|BIT2|BIT3|BIT4) ) << 11;
    BG1CNT.width = ( reg & BIT6 ) ? 512 : 256;
    BG1CNT.height = ( reg & BIT7 ) ? 512 : 256;
    BG1CNT.isRotScale = false;
  }

  // BG2
  // TODO: leave out some flags for bitmap mode
  BG2CNT.isRotScale = ( m != 0 );
  reg = io[0x0C];
  BG2CNT.priority = reg & 3;
  BG2CNT.tileOffset = ( (u16)reg & (BIT2|BIT3) ) << 12;
  BG2CNT.mosaic = reg & BIT6;
  BG2CNT.colorMode = reg & BIT7;
  reg = io[0x0D];
  BG2CNT.mapOffset = ( (u16)reg & (BIT0|BIT1|BIT2|BIT3|BIT4) ) << 11;
  BG2CNT.wrapAround = reg & BIT5;
  switch( m ) {
  case 0: // BG2 = character based
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

  // BG3
  if( ( m == 0 ) || ( m == 2 ) ) {
    BG3CNT.isRotScale = ( m != 0 );
    reg = io[0x0E];
    BG3CNT.priority = reg & 3;
    BG3CNT.tileOffset = ( (u16)reg & (BIT2|BIT3) ) << 12;
    BG3CNT.mosaic = reg & BIT6;
    BG3CNT.colorMode = reg & BIT7;
    reg = io[0x0F];
    BG3CNT.mapOffset = ( (u16)reg & (BIT0|BIT1|BIT2|BIT3|BIT4) ) << 11;
    BG3CNT.wrapAround = reg & BIT5;
    if( m == 0 ) {
      BG3CNT.width = ( reg & BIT6 ) ? 512 : 256;
      BG3CNT.height = ( reg & BIT7 ) ? 512 : 256;
    } else {
      switch( reg & (BIT6|BIT7) ) {
      case 0: BG3CNT.width = BG3CNT.height =  128; break;
      case 1: BG3CNT.width = BG3CNT.height =  256; break;
      case 2: BG3CNT.width = BG3CNT.height =  512; break;
      case 3: BG3CNT.width = BG3CNT.height = 1024; break;
      }
    }
  }


  BG0HOFS = READ16LE( io + 0x10 ) & 0x01FF; // 9 bit
  BG0VOFS = READ16LE( io + 0x12 ) & 0x01FF;
  BG1HOFS = READ16LE( io + 0x14 ) & 0x01FF;
  BG1VOFS = READ16LE( io + 0x16 ) & 0x01FF;
  BG2HOFS = READ16LE( io + 0x18 ) & 0x01FF;
  BG2VOFS = READ16LE( io + 0x1A ) & 0x01FF;
  BG3HOFS = READ16LE( io + 0x1C ) & 0x01FF;
  BG3VOFS = READ16LE( io + 0x1E ) & 0x01FF;

  ioLoaded = true;
}


void CGBAGraphics::setPAL( const u8 *pal ) {
  // convert & copy palettes
  // GBA color format: (MSB) 1x 5b 5g 5r (LSB)
  for( u16 i = 0; i <= 255; i++ ) {
    // counter i can not be 8 bit because overflow would destroy for loop condition
    gba2rgba( &(bgpal[i]), READ16LE(pal) );
    pal += 2;
  }
  for( u16 i = 0; i <= 255; i++ ) {
    gba2rgba( &(objpal[i]), READ16LE(pal) );
    pal += 2;
  }
  palLoaded = true;
}


bool CGBAGraphics::render() {
  // prepare graphic surfaces and send them to graphics renderer to
  // combine & render them (possibly with hw acceleration)

  if( !( vramLoaded && ioLoaded && palLoaded ) ) {
    // we are missing some ingredients
    assert( false );
    return false;
  }


  switch( DISPCNT.bgMode ) {
  case 0:
    if( DISPCNT.displayBG0 ) {
      SAFE_DELETE_ARRAY( bg0bmp );
      bg0bmp = new RGBACOLOR[BG0CNT.width * BG0CNT.height];
    }
    break;
  }

  return true;
}
