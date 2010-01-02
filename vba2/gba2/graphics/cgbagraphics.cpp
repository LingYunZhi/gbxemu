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

  vramLoaded = false;
  ioLoaded = false;
  palLoaded = false;
}


CGBAGraphics::~CGBAGraphics() {
  SAFE_DELETE_ARRAY( vram );
}


void CGBAGraphics::gba2rgba( COLOR32 *dest, u16 src ) {
  dest->r = ( src & 0x001F ) << 3; // extend from 5 to 8 bit
  dest->g = ( src & 0x03E0 ) >> 2;
  dest->b = ( src & 0x7C00 ) >> 7;
  dest->a = 0xFF; // opaque
  // TODO: use translation that offers pure white (0x00FFFFFF) for 0x7FFF GBA color
}


void CGBAGraphics::setVRAM( const u8 *vram_src ) {
  memcpy( vram, vram_src, 0x18000 );
  vramLoaded = true;
}


void CGBAGraphics::setIO( const u8 *io ) {
  // read & interpret all video registers
  u16 reg; // current register
  struct structDISPCNT *d = &result.DISPCNT;
  struct structBGCNT *b0 = &result.BGCNT[0];
  struct structBGCNT *b1 = &result.BGCNT[1];
  struct structBGCNT *b2 = &result.BGCNT[2];
  struct structBGCNT *b3 = &result.BGCNT[3];

  // DISPCNT
  reg = READ16LE(&io[0x00]);
  d->bgMode = reg & 7;
    if( d->bgMode > 5 ) assert( false ); // error
  d->frameNumber = reg & BIT4;
  d->oamAccessDuringHBlank = reg & BIT5;
  d->objCharMapping = reg & BIT6;
  d->forcedBlank = reg & BIT7;
  reg >>= 8;
  d->displayBG0 = reg & BIT0;
  d->displayBG1 = reg & BIT1;
  d->displayBG2 = reg & BIT2;
  d->displayBG3 = reg & BIT3;
  d->displayOBJ = reg & BIT4;
  d->displayWIN0 = reg & BIT5;
  d->displayWIN1 = reg & BIT6;
  d->displayOBJWIN = reg & BIT7;

  const u8 m = d->bgMode; // display mode

  if( m <= 1 ) {
    // BG0 = character based
    reg = READ16LE(&io[0x08]);
    b0->priority = reg & 3;
    b0->charOffset = ( (u16)reg & (BIT2|BIT3) ) << 12;
    b0->mosaic = reg & BIT6;
    b0->colorMode = reg & BIT7;
    reg >>= 8;
    b0->mapOffset = ( (u16)reg & (BIT0|BIT1|BIT2|BIT3|BIT4) ) << 11;
    b0->size = ( reg & (BIT6|BIT7) ) >> 6;
    b0->width = ( reg & BIT6 ) ? 512 : 256;
    b0->height = ( reg & BIT7 ) ? 512 : 256;
    b0->isRotScale = false;

    // BG1 = character based
    reg = READ16LE(&io[0x0A]);
    b1->priority = reg & 3;
    b1->charOffset = ( (u16)reg & (BIT2|BIT3) ) << 12;
    b1->mosaic = reg & BIT6;
    b1->colorMode = reg & BIT7;
    reg >>= 8;
    b1->mapOffset = ( (u16)reg & (BIT0|BIT1|BIT2|BIT3|BIT4) ) << 11;
    b1->size = ( reg & (BIT6|BIT7) ) >> 6;
    b1->width = ( reg & BIT6 ) ? 512 : 256;
    b1->height = ( reg & BIT7 ) ? 512 : 256;
    b1->isRotScale = false;
  }

  // BG2
  // TODO: leave out some flags for bitmap mode
  b2->isRotScale = ( m != 0 );
  reg = READ16LE(&io[0x0C]);
  b2->priority = reg & 3;
  b2->charOffset = ( (u16)reg & (BIT2|BIT3) ) << 12;
  b2->mosaic = reg & BIT6;
  b2->colorMode = reg & BIT7;
  reg >>= 8;
  b2->mapOffset = ( (u16)reg & (BIT0|BIT1|BIT2|BIT3|BIT4) ) << 11;
  b2->wrapAround = reg & BIT5;
  b2->size = ( reg & (BIT6|BIT7) ) >> 6;
  switch( m ) {
  case 0: // BG2 = character based
    b2->width = ( reg & BIT6 ) ? 512 : 256;
    b2->height = ( reg & BIT7 ) ? 512 : 256;
    break;
  case 1: // BG2 = rotation/scaling
  case 2:
    switch( reg & (BIT6|BIT7) ) {
    case 0: b2->width = b2->height =  128; break;
    case 1: b2->width = b2->height =  256; break;
    case 2: b2->width = b2->height =  512; break;
    case 3: b2->width = b2->height = 1024; break;
    }
    break;
  case 3: // BG2 = bitmap based
  case 4:
    b2->width = 240;
    b2->height = 160;
    break;
  case 5:
    b2->width = 160;
    b2->height = 128;
    break;
  }

  // BG3
  if( ( m == 0 ) || ( m == 2 ) ) {
    b3->isRotScale = ( m != 0 );
    reg = READ16LE(&io[0x0E]);
    b3->priority = reg & 3;
    b3->charOffset = ( (u16)reg & (BIT2|BIT3) ) << 12;
    b3->mosaic = reg & BIT6;
    b3->colorMode = reg & BIT7;
    reg >>= 8;
    b3->mapOffset = ( (u16)reg & (BIT0|BIT1|BIT2|BIT3|BIT4) ) << 11;
    b3->wrapAround = reg & BIT5;
    b3->size = ( reg & (BIT6|BIT7) ) >> 6;
    if( m == 0 ) {
      b3->width = ( reg & BIT6 ) ? 512 : 256;
      b3->height = ( reg & BIT7 ) ? 512 : 256;
    } else {
      switch( reg & (BIT6|BIT7) ) {
      case 0: b3->width = b3->height =  128; break;
      case 1: b3->width = b3->height =  256; break;
      case 2: b3->width = b3->height =  512; break;
      case 3: b3->width = b3->height = 1024; break;
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


void CGBAGraphics::buildCharBG( struct structBGCNT *cnt, CPicture &pic ) {
  const u16 nChars = 1024; // TODO: add condition
  u32 nPixelsInChars = nChars * 8*8; // 8x8 pixel per char
  u8 *srcChar = &vram[cnt->charOffset];
  if( cnt->colorMode ) { // 256x1 colors
    // contains all characters/tiles that will be copied to bg*bmp
    COLOR32 chars[nPixelsInChars];
    COLOR32 *c = chars;
    while( nPixelsInChars-- ) { *(c++) = bgpal[*(srcChar++)]; }
    // copy chars to bmp as given by screen map
    u16 *srcMap = (u16 *)&(vram[cnt->mapOffset]);
    // srcMap is divided in blocks of 32x32 tiles (= 256x256 pixels)
    for( u16 y = 0; y < 256; y += 8 ) { // 32 tiles * 8 pixel = 256 width
      for( u16 x = 0; x < 256; x += 8 ) {
        const u16 currentEntry = (*(srcMap++));
        const u16 charNumber = currentEntry & 0x03FF;
        u32 charOffset = charNumber * 8*8; // character's address
        const bool hFlip = currentEntry & BIT10;
        const bool vFlip = currentEntry & BIT11;
        // copy single character to bmp
        // TODO: remove redundant code somehow
        if( hFlip ) {
          if( vFlip ) { // hFlip && vFlip
            charOffset += 63;
            for( s8 charY = 0; charY < 8; charY++ ) {
              for( s8 charX = 0; charX < 8; charX++ ) {
//                bmp[ x + charX + ( ( y + charY ) * 256 ) ]
                pic.pixel( x + charX, y + charY )\
                  = chars[ charOffset - charX - ( charY * 8 ) ];
              }
            }
          } else { // hFlip && !vFlip
            charOffset += 7;
            for( s8 charY = 0; charY < 8; charY++ ) {
              for( s8 charX = 0; charX < 8; charX++ ) {
                pic.pixel( x + charX, y + charY ) \
                  = chars[ charOffset - charX + ( charY * 8 ) ];
              }
            }
          }
        } else { // !hFlip
          if( vFlip ) { // !hFlip && vFlip
            charOffset += 56;
            for( s8 charY = 0; charY < 8; charY++ ) {
              for( s8 charX = 0; charX < 8; charX++ ) {
                pic.pixel( x + charX, y + charY ) \
                  = chars[ charOffset + charX - ( charY * 8 ) ];
              }
            }
          } else { // !hFlip && !vFlip
            for( s8 charY = 0; charY < 8; charY++ ) {
              for( s8 charX = 0; charX < 8; charX++ ) {
                pic.pixel( x + charX, y + charY ) \
                  = chars[ charOffset + charX + ( charY * 8 ) ];
              }
            }
          }
        }
      }
    }
  } else { // 16x16 colors
  }
}


bool CGBAGraphics::process() {
  if( !( vramLoaded && ioLoaded && palLoaded ) ) {
    // we are missing some ingredients
    assert( false );
    return false;
  }

  switch( result.DISPCNT.bgMode ) {
  case 0:
    return process_mode0();
  }

  return false;
}
