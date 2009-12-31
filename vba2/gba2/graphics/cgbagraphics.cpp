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
  vram = new u8[0x18000];

  bg0sc0 = NULL; bg0sc1 = NULL; bg0sc2 = NULL; bg0sc3 = NULL;
  bg1sc0 = NULL; bg1sc1 = NULL; bg1sc2 = NULL; bg1sc3 = NULL;
  bg2sc0 = NULL; bg2sc1 = NULL; bg2sc2 = NULL; bg2sc3 = NULL;
  bg3sc0 = NULL; bg3sc1 = NULL; bg3sc2 = NULL; bg3sc3 = NULL;

  vramLoaded = false;
  ioLoaded = false;
  palLoaded = false;
  renderComplete = false;
}


CGBAGraphics::~CGBAGraphics() {
  SAFE_DELETE_ARRAY( bg0sc0 );
  SAFE_DELETE_ARRAY( bg0sc1 );
  SAFE_DELETE_ARRAY( bg0sc2 );
  SAFE_DELETE_ARRAY( bg0sc3 );

  SAFE_DELETE_ARRAY( bg1sc0 );
  SAFE_DELETE_ARRAY( bg1sc1 );
  SAFE_DELETE_ARRAY( bg1sc2 );
  SAFE_DELETE_ARRAY( bg1sc3 );

  SAFE_DELETE_ARRAY( bg2sc0 );
  SAFE_DELETE_ARRAY( bg2sc1 );
  SAFE_DELETE_ARRAY( bg2sc2 );
  SAFE_DELETE_ARRAY( bg2sc3 );

  SAFE_DELETE_ARRAY( bg3sc0 );
  SAFE_DELETE_ARRAY( bg3sc1 );
  SAFE_DELETE_ARRAY( bg3sc2 );
  SAFE_DELETE_ARRAY( bg3sc3 );

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
  memcpy( vram, vram_src, 0x18000 );
  vramLoaded = true;
}


void CGBAGraphics::setIO( const u8 *io ) {
  // read & interpret all video registers
  u16 reg; // current register

  // DISPCNT
  reg = READ16LE(&io[0x00]);
  DISPCNT.bgMode = reg & 7;
    if( DISPCNT.bgMode > 5 ) assert( false ); // error
  DISPCNT.frameNumber = reg & BIT4;
  DISPCNT.oamAccessDuringHBlank = reg & BIT5;
  DISPCNT.objCharMapping = reg & BIT6;
  DISPCNT.forcedBlank = reg & BIT7;
  reg >>= 8;
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
    reg = READ16LE(&io[0x08]);
    BG0CNT.priority = reg & 3;
    BG0CNT.charOffset = ( (u16)reg & (BIT2|BIT3) ) << 12;
    BG0CNT.mosaic = reg & BIT6;
    BG0CNT.colorMode = reg & BIT7;
    reg >>= 8;
    BG0CNT.mapOffset = ( (u16)reg & (BIT0|BIT1|BIT2|BIT3|BIT4) ) << 11;
    BG0CNT.size = ( reg & (BIT6|BIT7) ) >> 6;
    BG0CNT.width = ( reg & BIT6 ) ? 512 : 256;
    BG0CNT.height = ( reg & BIT7 ) ? 512 : 256;
    BG0CNT.isRotScale = false;

    // BG1 = character based
    reg = READ16LE(&io[0x0A]);
    BG1CNT.priority = reg & 3;
    BG1CNT.charOffset = ( (u16)reg & (BIT2|BIT3) ) << 12;
    BG1CNT.mosaic = reg & BIT6;
    BG1CNT.colorMode = reg & BIT7;
    reg >>= 8;
    BG1CNT.mapOffset = ( (u16)reg & (BIT0|BIT1|BIT2|BIT3|BIT4) ) << 11;
    BG1CNT.size = ( reg & (BIT6|BIT7) ) >> 6;
    BG1CNT.width = ( reg & BIT6 ) ? 512 : 256;
    BG1CNT.height = ( reg & BIT7 ) ? 512 : 256;
    BG1CNT.isRotScale = false;
  }

  // BG2
  // TODO: leave out some flags for bitmap mode
  BG2CNT.isRotScale = ( m != 0 );
  reg = READ16LE(&io[0x0C]);
  BG2CNT.priority = reg & 3;
  BG2CNT.charOffset = ( (u16)reg & (BIT2|BIT3) ) << 12;
  BG2CNT.mosaic = reg & BIT6;
  BG2CNT.colorMode = reg & BIT7;
  reg >>= 8;
  BG2CNT.mapOffset = ( (u16)reg & (BIT0|BIT1|BIT2|BIT3|BIT4) ) << 11;
  BG2CNT.wrapAround = reg & BIT5;
  BG2CNT.size = ( reg & (BIT6|BIT7) ) >> 6;
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
    reg = READ16LE(&io[0x0E]);
    BG3CNT.priority = reg & 3;
    BG3CNT.charOffset = ( (u16)reg & (BIT2|BIT3) ) << 12;
    BG3CNT.mosaic = reg & BIT6;
    BG3CNT.colorMode = reg & BIT7;
    reg >>= 8;
    BG3CNT.mapOffset = ( (u16)reg & (BIT0|BIT1|BIT2|BIT3|BIT4) ) << 11;
    BG3CNT.wrapAround = reg & BIT5;
    BG3CNT.size = ( reg & (BIT6|BIT7) ) >> 6;
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


void CGBAGraphics::buildCharBG( struct structBGCNT *cnt, RGBACOLOR *bmp, u32 nChars ) {
  u8 *srcChars = &vram[cnt->charOffset];
  if( cnt->colorMode ) { // 256x1 colors
    // contains all characters/tiles that will be copied to bg*bmp
    RGBACOLOR chars[nChars * 8*8]; // 8x8 pixel per char
    RGBACOLOR *c = chars;
    while( nChars-- ) { *(c++) = bgpal[*srcChars++]; }
    // copy chars to bmp as given by screen map
    u16 *srcMap = (u16 *)&(vram[cnt->mapOffset]);
    // srcMap is divided in blocks of 32x32 tiles (= 256x256 pixels)
    for( u16 y = 0; y < 256; y += 8 ) { // 32 tiles * 8 pixel = 256 width
      for( u16 x = 0; x < 256; x += 8 ) {
        const u32 charNumber = (*(srcMap++)) & 0x03FF;
        const u32 charOffset = charNumber * 8*8;
        // copy char block to bmp
        for( u8 y2 = 0; y2 < 8; y2++ ) {
          // copy a line of 8 pixel from chars to bmp
          memcpy( &(bmp[x + (y*256)]), &(chars[charOffset + (y2*8)]), sizeof(RGBACOLOR) * 8 );
        }
      }
    }
  } else { // 16x16 colors
  }
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
      switch( BG0CNT.size ) {
      case 3:
        SAFE_DELETE_ARRAY( bg0sc3 );
        bg0sc3 = new RGBACOLOR[256*256];
        SAFE_DELETE_ARRAY( bg0sc2 );
        bg0sc2 = new RGBACOLOR[256*256];
      case 2:
      case 1:
        SAFE_DELETE_ARRAY( bg0sc1 );
        bg0sc1 = new RGBACOLOR[256*256];
      case 0:
        SAFE_DELETE_ARRAY( bg0sc0 );
        bg0sc0 = new RGBACOLOR[256*256];
      }
    }
    buildCharBG( &BG0CNT, bg0sc0, 32*32 );
    break;
  }

  renderComplete = true;
  return true;
}


CGBAGraphics::renderResult CGBAGraphics::getRenderResult() {
  renderResult res;
  res.DISPCNT = &DISPCNT;
  res.BGCNT[0] = &BG0CNT;
  res.BGCNT[1] = &BG1CNT;
  res.BGCNT[2] = &BG2CNT;
  res.BGCNT[3] = &BG3CNT;
  res.BGSC[0][0] = bg0sc0;
  res.BGSC[0][1] = bg0sc0;
  res.BGSC[0][2] = bg0sc0;
  res.BGSC[0][3] = bg0sc0;
  res.BGSC[1][0] = bg0sc0;
  res.BGSC[1][1] = bg0sc0;
  res.BGSC[1][2] = bg0sc0;
  res.BGSC[1][3] = bg0sc0;
  res.BGSC[2][0] = bg0sc0;
  res.BGSC[2][1] = bg0sc0;
  res.BGSC[2][2] = bg0sc0;
  res.BGSC[2][3] = bg0sc0;
  res.BGSC[3][0] = bg0sc0;
  res.BGSC[3][1] = bg0sc0;
  res.BGSC[3][2] = bg0sc0;
  res.BGSC[3][3] = bg0sc0;
  return res;
}
