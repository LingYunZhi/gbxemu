/*  VisualBoyAdvance 2
    Copyright (C) 2009-2010  VBA development team

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


#ifndef CPICTURE_H
#define CPICTURE_H


#include "../common/Types.h"


/// 32 bit RGBA color as used in modern PC systems
typedef struct {
  u8 b; ///< blue  0-255
  u8 g; ///< green 0-255
  u8 r; ///< red   0-255
  u8 a; ///< alpha 0-255
} COLOR32;


/// A class to manage a picture in COLOR32 format
class CPicture
{
public:
  CPicture( unsigned int width = 0, unsigned int height = 0 );
  ~CPicture();
  void create( unsigned int width, unsigned int height );
  COLOR32 &pixel( unsigned int x, unsigned int y ); ///< get a reference to the pixel at the specified coordinates
  COLOR32 *picture; ///< address of picture data
  unsigned int size; ///< count of pixel that are stored in picture
  unsigned int width; ///< width of picture in pixels
  unsigned int height; ///< height of picture in pixels
};


#endif // CPICTURE_H
