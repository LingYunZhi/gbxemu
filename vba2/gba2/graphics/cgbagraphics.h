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


#ifndef CGBAGRAPHICS_H
#define CGBAGRAPHICS_H


#include "../common/Types.h"


class CGBAGraphics
{
public:
  CGBAGraphics();
  ~CGBAGraphics();

  void setVRAM( const u8 *vram_src );
  void setIO( const u8 *io );
  void setPAL( const u8 *pal );
  void render();

  /// 32 bit RGBA color as used in modern PC systems
  typedef struct structRGBACOLOR {
    u8 r; ///< red   0-255
    u8 g; ///< green 0-255
    u8 b; ///< blue  0-255
    u8 a; ///< alpha 0-255
  } RGBACOLOR;

  /// convert 15 bit GBA color to 32 bit RGBA color
  static void gba2rgba( RGBACOLOR *dest, u16 src );


private:
  u8 *vram; // video memory [16 KiB]

  RGBACOLOR bgpal[256]; // BG palette (RGBA color)
  RGBACOLOR objpal[256]; // sprite palette (RGBA color)

  RGBACOLOR *bg0, *bg1, *bg2, *bg3; // contains resulting bitmap of BG screen

  // video registers
  struct {
    u8 bgMode; // 0-5
    bool frameNumber; // frame buffer 0 or 1
    bool oamAccessDuringHBlank;
    bool objCharMapping;
    bool forcedBlank;
    bool displayBG0;
    bool displayBG1;
    bool displayBG2;
    bool displayBG3;
    bool displayOBJ;
    bool displayWIN0;
    bool displayWIN1;
    bool displayOBJWIN;
  } DISPCNT;

  struct {
    u8 priority; // (max) 0 1 2 3 (min)
    u16 tileOffset; // adress of tiles/characters in VRAM
    bool mosaic;
    bool colorMode; // false: 16x16  true: 256
    u16 mapOffset; // adress of map/screen data in VRAM
    bool wrapAround; // area overflow flag (only BG2/3)
    u16 width;
    u16 height;
  } BG0CNT, BG1CNT, BG2CNT, BG3CNT;

  // horiontal offset (only in character mode)
  u16 BG0HOFS, BG1HOFS, BG2HOFS, BG3HOFS;

  // vertical offset (only in character mode)
  u16 BG0VOFS, BG1VOFS, BG2VOFS, BG3VOFS;
};


#endif // CGBAGRAPHICS_H
