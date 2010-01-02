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


#include "cpicture.h"
#include <assert.h>


CPicture::CPicture( unsigned int width, unsigned int height )
{
  picture = NULL;
  size = 0;

  if( ( width != 0 ) && ( height != 0 ) ) {
    create( width, height );
  }
}


CPicture::~CPicture()
{
  if( picture != NULL ) {
    delete [] picture;
  }
}


void CPicture::create( unsigned int width, unsigned int height ) {
  const unsigned int newSize = width * height;
  assert( newSize != 0 );
  // no need to reallocate memory if the size is the same
  if( newSize != size ) {
    if( picture != NULL ) {
      delete [] picture;
    }
    picture = new COLOR32[newSize];
    assert( picture != NULL ); // catch out of memory
  }
  size = newSize;
  this->width = width;
  this->height = height;
}


COLOR32 &CPicture::pixel( unsigned int x, unsigned int y ) {
  return picture[ x + ( y * width ) ];
}
