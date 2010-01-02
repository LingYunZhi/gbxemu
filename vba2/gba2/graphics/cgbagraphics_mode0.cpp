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

  if( cnt.colorMode ) { // 256x1 colors

    // Create picture containing all characters (since there is only one
    //   palette, we already know the colors of every tile/block.
    CPicture chars( 8, nChars * 8 ); // 8x8 pixel per char
    COLOR32 *c = chars.picture;
    u32 nPixelsInChars = chars.size;
    while( nPixelsInChars-- ) { *(c++) = bgpal[*(srcChar++)]; }

    // copy chars to BG layer as defined by screen map
    u16 *srcMap = (u16 *)&(vram[cnt.mapOffset]);
    // srcMap is divided in blocks of 32x32 tiles (= 256x256 pixels)
    CPicture &pic = result.BGSC[bg_number][0];
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
            for( s8 charY = 0; charY < 8; charY++ ) {
              for( s8 charX = 0; charX < 8; charX++ ) {
                pic.pixel( x + charX, y + charY ) = chars.pixel( 7 - charX, charOffset + 7 - charY );
              }
            }
          } else { // hFlip && !vFlip
            for( s8 charY = 0; charY < 8; charY++ ) {
              for( s8 charX = 0; charX < 8; charX++ ) {
                pic.pixel( x + charX, y + charY ) = chars.pixel( 7 - charX, charOffset + charY );
              }
            }
          }
        } else { // !hFlip
          if( vFlip ) { // !hFlip && vFlip
            for( s8 charY = 0; charY < 8; charY++ ) {
              for( s8 charX = 0; charX < 8; charX++ ) {
                pic.pixel( x + charX, y + charY ) = chars.pixel( charX, charOffset + 7 - charY );
              }
            }
          } else { // !hFlip && !vFlip
            for( s8 charY = 0; charY < 8; charY++ ) {
              for( s8 charX = 0; charX < 8; charX++ ) {
                pic.pixel( x + charX, y + charY ) = chars.pixel( charX, charOffset + charY );
              }
            }
          }
        }
      }
    }
  } else { // 16x16 colors
  }
}
