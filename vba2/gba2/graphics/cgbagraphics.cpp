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

  // initialize color table
  // TODO: use translation that offers pure white (0x00FFFFFF) for 0x7FFF GBA color
  for( u32 color = 0; color < 0x8000; color++ ) {
    COLOR32 &c = colorTable[color];
    c.a = 0xFF; // opaque
    c.r = ( color & 0x001F ) << 3; // extend from 5 to 8 bit
    c.g = ( color & 0x03E0 ) >> 2;
    c.b = ( color & 0x7C00 ) >> 7;
  }


  vramLoaded = false;
  ioLoaded = false;
  palLoaded = false;
}


CGBAGraphics::~CGBAGraphics() {
  SAFE_DELETE_ARRAY( vram );
}


void CGBAGraphics::setVRAM( const u8 *vram_src ) {
  memcpy( vram, vram_src, 0x18000 );
  vramLoaded = true;
}


void CGBAGraphics::setIO( const u8 *io ) {
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
  d->displayBG[0] = reg & BIT0;
  d->displayBG[1] = reg & BIT1;
  d->displayBG[2] = reg & BIT2;
  d->displayBG[3] = reg & BIT3;
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
      case 0: b3->width = b3->height =  128; break; //  16x16  characters
      case 1: b3->width = b3->height =  256; break; //  32x32  characters
      case 2: b3->width = b3->height =  512; break; //  64x64  characters
      case 3: b3->width = b3->height = 1024; break; // 128x128 characters
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
  assert( ioLoaded );
  switch( result.DISPCNT.bgMode ) {
  case 3:
  case 5:
    // mode three and five do not use palettes
    return;
  }

  // counter i can not be 8 bit because overflow would destroy loop condition
  for( u16 i = 0; i <= 255; i++ ) {
    bgpal[i] = colorTable[ READ16LE(pal) ];
    pal += 2;
  }
  for( u16 i = 0; i <= 255; i++ ) {
    objpal[i] = colorTable[ READ16LE(pal) ];
    pal += 2;
  }
  palLoaded = true;
}


bool CGBAGraphics::buildCharBG( u8 bg_number ) {
  const struct structBGCNT &cnt = result.BGCNT[bg_number];
  CPicture &pic = result.BGIMAGE[bg_number];
  pic.create( cnt.width, cnt.height );
  const u16 nChars = 1024; // char BG always offers 1024 different characters
  u8 *srcChar = &vram[ cnt.charOffset ];
  static const u8 nScreensPerSize[4] = { 1, 2, 2, 4 }; // number of screens/macroblocks per BG size setting
  const u8 nScreens = nScreensPerSize[cnt.size]; // number of screens/macroblocks we have to process
  u16 *srcMap = (u16 *)&(vram[cnt.mapOffset]); // where the map data lies

  if( cnt.colorMode ) { // 256x1 colors

    // Create picture containing all characters
    // (since there is only one palette, we already know the colors of every tile/block)
    CPicture chars( 8, nChars * 8 ); // 8x8 pixel per char
    COLOR32 *c = chars.picture;
    u32 nPixelsInChars = chars.size;
    while( nPixelsInChars-- ) { *(c++) = bgpal[*(srcChar++)]; }

    // copy chars to BG layer as defined by screen map
    // srcMap is divided in blocks (=sc) of 32x32 tiles (= 256x256 pixels)
    for( u8 sc = 0; sc < nScreens; sc++ ) {
      const bool isRightScreen = (sc==3) || ((sc==1) && (cnt.size==1));
      const bool isBottomScreen = (sc==2) || ((sc==1) && (cnt.size==2));
      const u16 screenX = isRightScreen ? 256 : 0;
      const u16 screenY = isBottomScreen ? 256 : 0;
      for( u16 y = 0; y < 256; y += 8 ) { // 32 tiles * 8 pixel = 256 width
        for( u16 x = 0; x < 256; x += 8 ) {
          const u16 currentEntry = (*(srcMap++));
          const u16 charNumber = currentEntry & 0x03FF;
          u32 charOffset = charNumber * 8; // character's address
          const bool hFlip = currentEntry & BIT10;
          const bool vFlip = currentEntry & BIT11;
          // copy single character to pic
          // TODO: remove redundant code somehow
          if( hFlip ) {
            if( vFlip ) { // hFlip && vFlip
              for( u8 charY = 0; charY < 8; charY++ ) {
                for( u8 charX = 0; charX < 8; charX++ ) {
                  pic.pixel( screenX + x + charX, screenY + y + charY ) \
                    = chars.pixel( 7 - charX, charOffset + 7 - charY );
                }
              }
            } else { // hFlip && !vFlip
              for( u8 charY = 0; charY < 8; charY++ ) {
                for( u8 charX = 0; charX < 8; charX++ ) {
                  pic.pixel( screenX + x + charX, screenY + y + charY ) \
                    = chars.pixel( 7 - charX, charOffset + charY );
                }
              }
            }
          } else { // !hFlip
            if( vFlip ) { // !hFlip && vFlip
              for( u8 charY = 0; charY < 8; charY++ ) {
                for( u8 charX = 0; charX < 8; charX++ ) {
                  pic.pixel( screenX + x + charX, screenY + y + charY ) \
                    = chars.pixel( charX, charOffset + 7 - charY );
                }
              }
            } else { // !hFlip && !vFlip
              for( u8 charY = 0; charY < 8; charY++ ) {
                for( u8 charX = 0; charX < 8; charX++ ) {
                  pic.pixel( screenX + x + charX, screenY + y + charY ) \
                    = chars.pixel( charX, charOffset + charY );
                }
              }
            }
          }
        }
      }
    }
  } else { // 16x16 colors

    // one byte contains two pixel, so we seperate them for faster operation later
    const u32 numPixels = (u32)nChars * 64;
    u8 chars[numPixels]; // will be filled with optimized pixel data (palette not applied)
    for( u32 i = 0; i < numPixels; i += 2 ) {
      chars[i] = *srcChar & 0x0F;
      chars[i+1] = *(srcChar++) >> 4;
    }

    for( u8 sc = 0; sc < nScreens; sc++ ) {
      const bool isRightScreen = (sc==3) || ((sc==1) && (cnt.size==1));
      const bool isBottomScreen = (sc==2) || ((sc==1) && (cnt.size==2));
      const u16 screenX = isRightScreen ? 256 : 0;
      const u16 screenY = isBottomScreen ? 256 : 0;
      for( u16 y = 0; y < 256; y += 8 ) {
        for( u16 x = 0; x < 256; x += 8 ) {
          // get tile info
          const u16 currentEntry = (*(srcMap++));
          const u16 charNumber = currentEntry & 0x03FF;
          u32 charOffset = charNumber * 64;
          const bool hFlip = currentEntry & BIT10;
          const bool vFlip = currentEntry & BIT11;
          const u8 paletteNumber = ( currentEntry & 0xF000 ) >> 12;
          const u8 paletteOffset = paletteNumber * 16;
          if( hFlip ) {
            if( vFlip ) { // hFlip && vFlip
              charOffset += 63;
              for( u8 charY = 0; charY < 8; charY++ ) {
                for( u8 charX = 0; charX < 8; charX++ ) {
                  const u8 currentPixel = chars[ charOffset - charX - ( charY * 8 ) ];
                  pic.pixel( screenX + x + charX, screenY + y + charY ) \
                      = bgpal[ paletteOffset + currentPixel ];
                }
              }
            } else { // hFlip && !vFlip
              charOffset += 7;
              for( u8 charY = 0; charY < 8; charY++ ) {
                for( u8 charX = 0; charX < 8; charX++ ) {
                  const u8 currentPixel = chars[ charOffset - charX + ( charY * 8 ) ];
                  pic.pixel( screenX + x + charX, screenY + y + charY ) \
                      = bgpal[ paletteOffset + currentPixel ];
                }
              }
            }
          } else { // !hFlip
            if( vFlip ) { // !hFlip && vFlip
              charOffset += 56;
              for( u8 charY = 0; charY < 8; charY++ ) {
                for( u8 charX = 0; charX < 8; charX++ ) {
                  const u8 currentPixel = chars[ charOffset + charX - ( charY * 8 ) ];
                  pic.pixel( screenX + x + charX, screenY + y + charY ) \
                      = bgpal[ paletteOffset + currentPixel ];
                }
              }
            } else { // !hFlip && !vFlip
              for( u8 charY = 0; charY < 8; charY++ ) {
                for( u8 charX = 0; charX < 8; charX++ ) {
                  const u8 currentPixel = chars[ charOffset + charX + ( charY * 8 ) ];
                  pic.pixel( screenX + x + charX, screenY + y + charY ) \
                      = bgpal[ paletteOffset + currentPixel ];
                }
              }
            }
          }
        }
      }
    }

  }
  return true;
}


bool CGBAGraphics::buildRotScaleBG( u8 bg_number ) {
  // allocate picture memory as required
  const u16 screenSize = result.BGCNT[bg_number].width; // = height
  CPicture &pic = result.BGIMAGE[bg_number];
  pic.create( screenSize, screenSize );

  // create picture containing all characters
  // 8x8 pixel per char
  // character data comes as 256 colors x 1 palette format
  // since there is only one palette, we already know the final color of every pixel
  const u16 nChars = 256;
  CPicture chars( 8, nChars * 8 );
  // get VRAM address of character pixels
  u8 *srcChar = &vram[ result.BGCNT[ bg_number ].charOffset ];
  COLOR32 *pixel = chars.picture;
  u32 nPixelsInChars = chars.size;
  while( nPixelsInChars-- )
    *(pixel++) = bgpal[*(srcChar++)];

  // create a picture out of the characters
  // screen data format:
  // one byte per entry
  // all eight bits give the number of the character to use
  u8 *srcMap = &(vram[result.BGCNT[bg_number].mapOffset]);

  // process all entries of screen map
  for( u16 y = 0; y < screenSize; y += 8 ) {
    for( u16 x = 0; x < screenSize; x += 8 ) {
      const u8 charNumber = (*(srcMap++));
      const u16 charOffset = charNumber * 8;
      // copy single character to pic
      // fortunately, rotation/scaling BGs do not support character mirroring
      for( u8 charY = 0; charY < 8; charY++ )
        for( u8 charX = 0; charX < 8; charX++ )
          pic.pixel( x + charX, y + charY ) \
            = chars.pixel( charX, charOffset + charY );
    }
  }
  return true;
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
  case 1:
    return process_mode1();
  case 2:
    return process_mode2();
  }

  return false;
}


bool CGBAGraphics::process_mode0() {
  bool ok = true;

  for( u8 bg = 0; bg < 4; bg++ )
    if( result.DISPCNT.displayBG[bg] )
      ok &= buildCharBG( bg );

  return ok;
}


bool CGBAGraphics::process_mode1() {
  bool ok = true;

  if( result.DISPCNT.displayBG[0] )
    ok &= buildCharBG( 0 );

  if( result.DISPCNT.displayBG[1] )
    ok &= buildCharBG( 1 );

  if( result.DISPCNT.displayBG[2] )
    ok &= buildRotScaleBG( 2 );

  return ok;
}


bool CGBAGraphics::process_mode2() {
  bool ok = true;

  if( result.DISPCNT.displayBG[2] )
    ok &= buildRotScaleBG( 2 );

  if( result.DISPCNT.displayBG[3] )
    ok &= buildRotScaleBG( 3 );

  return ok;
}
