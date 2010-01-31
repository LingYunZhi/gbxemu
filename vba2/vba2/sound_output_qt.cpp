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


#include "sound_output_qt.h"


sound_output_qt::sound_output_qt()
{
}


sound_output_qt::~sound_output_qt()
{
}


bool sound_output_qt::init( long sampleRate ) {
  return true;
}


void sound_output_qt::pause() {
}


void sound_output_qt::reset() {
}


void sound_output_qt::resume() {

}


void sound_output_qt::write( u16 * finalWave, int length ) {

}


void sound_output_qt::setThrottle( unsigned short throttle ) {

}
