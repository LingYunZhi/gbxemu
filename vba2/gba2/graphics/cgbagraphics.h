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
#include "cpicture.h"


class CGBAGraphics
{
// /----------
public:
  CGBAGraphics();
  ~CGBAGraphics();

  /// copy VRAM
  void setVRAM( const u8 *vram_src );
  /// read & interpret all video registers
  void setIO( const u8 *io );
  /// convert & copy palettes
  void setPAL( const u8 *pal );
  /// prepare image data for render API
  bool process();

// \----------


  /// display control
  struct structDISPCNT {
    u8 bgMode; // 0-5
    bool frameNumber; // frame buffer 0 or 1
    bool oamAccessDuringHBlank;
    bool objCharMapping;
    bool forcedBlank;
    bool displayBG[4];
    bool displayOBJ;
    bool displayWIN0;
    bool displayWIN1;
    bool displayOBJWIN;
  };

  /// background control
  struct structBGCNT {
    u8 priority; ///< (max) 0 1 2 3 (min)
    u16 charOffset; ///< adress of tiles/characters in VRAM
    bool mosaic; ///< enables or disables mosaic effect
    bool colorMode; ///< false: 16x16  true: 256x1
    u16 mapOffset; ///< adress of map/screen data in VRAM
    bool wrapAround; ///< area overflow flag (only BG2&3)
    u8 size; ///< for char mode: 0=256x256  1=512x256  2=256x512  3=512x512
    u32 width; ///< width of BG when combining all its macro-blocks
    u32 height; ///< height of BG when combining all its macro-blocks
    bool isRotScale; ///< false: no rotation/scaling
  };


  typedef struct {
    struct structDISPCNT DISPCNT; /// display control registers
    struct structBGCNT   BGCNT[4]; /// background control registers
    CPicture             BGIMAGE[4]; /// a picture of the whole BG layer
  } RESULT;

  RESULT result; // this struct will be used by the renderer later on


private:
  // all have to be true to be able to work
  bool vramLoaded, ioLoaded, palLoaded;

  u8 *vram; // video memory [96 KiB]
  COLOR32 bgpal[256]; // BG palette (converted to RGBA color)
  COLOR32 objpal[256]; // sprite palette (converted to RGBA color)

  /// convert 15 bit GBA color to 32 bit RGBA color
  /// GBA color format: (MSB) 1x 5b 5g 5r (LSB)
  COLOR32 colorTable[ 0x8000 ];

  u16 BG0HOFS, BG1HOFS, BG2HOFS, BG3HOFS; // horiontal offset (only in character mode)
  u16 BG0VOFS, BG1VOFS, BG2VOFS, BG3VOFS; // vertical offset (only in character mode)

  /// create a picture out of the BG data and character data
  bool buildCharBG( u8 bg_number );  
  bool buildRotScaleBG( u8 bg_number );
  bool buildBitmapBG( u8 mode );
};


#endif // CGBAGRAPHICS_H
