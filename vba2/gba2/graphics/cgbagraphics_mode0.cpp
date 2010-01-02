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
  if( result.DISPCNT.displayBG0 ) {
    switch( result.BGCNT[0].size ) {
    case 3:
      result.BGSC[0][3].create( 256, 256 );
      result.BGSC[0][2].create( 256, 256 );
    case 2:
    case 1:
      result.BGSC[0][1].create( 256, 256 );
    case 0:
      result.BGSC[0][0].create( 256, 256 );
    }
    buildCharBG( &result.BGCNT[0], result.BGSC[0][0] );
  }

  return true;
}
