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
public:
  CGBAGraphics();
  ~CGBAGraphics();

  void setVRAM( const u8 *vram_src );
  void setIO( const u8 *io );
  void setPAL( const u8 *pal );
  bool process(); // prepare image data for render API


  /// convert 15 bit GBA color to 32 bit RGBA color
  static void gba2rgba( COLOR32 *dest, u16 src );

  /// display control
  struct structDISPCNT {
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


  typedef struct structRESULT {
    struct structDISPCNT  DISPCNT; // display control registers
    struct structBGCNT    BGCNT[4]; // background control registers
    /// BG screen ( = division of BG )
    /// contains resulting bitmap of BG screens
    /// each BG may consist of up to 4 macro-blocks of 256x256 pixels
    /// for 512x512:  sc0=top-left  sc1=top-right  sc2=bottom-left  sc3=bottom-right
    CPicture              BGSC[4][4]; ///< BGSC[BG#][block#]
  } RESULT;

  RESULT result; // this struct will be used by the renderer later on


private:
  // all have to be true to be able to work
  bool vramLoaded, ioLoaded, palLoaded;

  u8 *vram; // video memory [96 KiB]
  COLOR32 bgpal[256]; // BG palette (converted to RGBA color)
  COLOR32 objpal[256]; // sprite palette (converted to RGBA color)

  u16 BG0HOFS, BG1HOFS, BG2HOFS, BG3HOFS; // horiontal offset (only in character mode)
  u16 BG0VOFS, BG1VOFS, BG2VOFS, BG3VOFS; // vertical offset (only in character mode)

  void buildCharBG( struct structBGCNT *cnt, CPicture &pic );
  bool process_mode0();
};


#endif // CGBAGRAPHICS_H
