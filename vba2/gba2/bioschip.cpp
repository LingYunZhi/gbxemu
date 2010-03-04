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


#include "bioschip.h"
#include <string.h>
#include <assert.h>
#include "common/Port.h"


BiosChip::BiosChip()
{
  m_locked = false;
  m_loaded = false;
  m_lastRead = 0;
}


void *BiosChip::lockData() {
  memset( m_data, 0x00, SIZE );
  m_locked = true;
  return m_data;
}


void BiosChip::unlockData() {
  if( m_locked ) {
    m_loaded = true;
    m_locked = false;
  }
}


bool BiosChip::isLoaded() {
  return m_loaded;
}


u32 BiosChip::read32( u32 address ) {
  assert( m_loaded && !m_locked && (address<SIZE) && !(address&3) );

  // return new data
  m_lastRead = m_data[address/4];

  return m_lastRead;
}


u32 BiosChip::getLast() {
  assert( m_loaded && !m_locked );
  return m_lastRead;
}
