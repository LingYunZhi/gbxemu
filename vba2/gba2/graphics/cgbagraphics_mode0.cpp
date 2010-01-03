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


bool CGBAGraphics::process_mode0() {
  for( u8 bg = 0; bg < 4; bg++ ) {
    if( result.DISPCNT.displayBG[bg] ) {
      switch( result.BGCNT[bg].size ) {
      case 3:
        result.BGSC[bg][3].create( 256, 256 );
        result.BGSC[bg][2].create( 256, 256 );
      case 2:
      case 1:
        result.BGSC[bg][1].create( 256, 256 );
      case 0:
        result.BGSC[bg][0].create( 256, 256 );
      }
      buildCharBG( bg );
    }
  }

  return true;
}


void CGBAGraphics::buildCharBG( u8 bg_number ) {
  const struct structBGCNT &cnt = result.BGCNT[bg_number];
  const u16 nChars = 1024; // TODO: add condition
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
      CPicture &pic = result.BGSC[bg_number][sc];
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
                  pic.pixel( x + charX, y + charY ) = chars.pixel( 7 - charX, charOffset + 7 - charY );
                }
              }
            } else { // hFlip && !vFlip
              for( u8 charY = 0; charY < 8; charY++ ) {
                for( u8 charX = 0; charX < 8; charX++ ) {
                  pic.pixel( x + charX, y + charY ) = chars.pixel( 7 - charX, charOffset + charY );
                }
              }
            }
          } else { // !hFlip
            if( vFlip ) { // !hFlip && vFlip
              for( u8 charY = 0; charY < 8; charY++ ) {
                for( u8 charX = 0; charX < 8; charX++ ) {
                  pic.pixel( x + charX, y + charY ) = chars.pixel( charX, charOffset + 7 - charY );
                }
              }
            } else { // !hFlip && !vFlip
              for( u8 charY = 0; charY < 8; charY++ ) {
                for( u8 charX = 0; charX < 8; charX++ ) {
                  pic.pixel( x + charX, y + charY ) = chars.pixel( charX, charOffset + charY );
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
      CPicture &pic = result.BGSC[bg_number][sc];
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
                  pic.pixel( x + charX, y + charY ) \
                      = bgpal[ paletteOffset + currentPixel ];
                }
              }
            } else { // hFlip && !vFlip
              charOffset += 7;
              for( u8 charY = 0; charY < 8; charY++ ) {
                for( u8 charX = 0; charX < 8; charX++ ) {
                  const u8 currentPixel = chars[ charOffset - charX + ( charY * 8 ) ];
                  pic.pixel( x + charX, y + charY ) \
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
                  pic.pixel( x + charX, y + charY ) \
                      = bgpal[ paletteOffset + currentPixel ];
                }
              }
            } else { // !hFlip && !vFlip
              for( u8 charY = 0; charY < 8; charY++ ) {
                for( u8 charX = 0; charX < 8; charX++ ) {
                  const u8 currentPixel = chars[ charOffset + charX + ( charY * 8 ) ];
                  pic.pixel( x + charX, y + charY ) \
                      = bgpal[ paletteOffset + currentPixel ];
                }
              }
            }
          }
        }
      }
    }
  }
}
