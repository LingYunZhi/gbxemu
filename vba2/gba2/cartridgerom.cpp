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


#include "cartridgerom.h"
#include "common/Port.h"
#include <assert.h>
#include <string.h> // memset


CartridgeROM::CartridgeROM()
{
  m_data = NULL;
  m_size = 0;
  m_loaded = false;
  m_locked = false;
  m_connectedInfo = NULL;
}


CartridgeROM::~CartridgeROM() {
  if( m_data != NULL ) {
    delete[] m_data;
  }
}


void CartridgeROM::connectInfo( CartridgeInfo *connectedInfo ) {
  m_connectedInfo = connectedInfo;
}


bool CartridgeROM::isLoaded() {
  return m_loaded;
}


bool CartridgeROM::setSize( u32 size ) {
  if( (size < 192) || m_locked ) {
    assert( false );
    return false;
  }

  if( m_loaded ) unload();
  m_size = size;
  m_data = new u8[m_size];
  if( m_data == NULL ) {
    assert( false );
    return false;
  }

  return true;
}


u32 CartridgeROM::getSize() {
  return m_size;
}


void *CartridgeROM::lockData() {
  assert( !m_locked && (m_size != 0) );
  m_locked = true;
  m_loaded = false;
  memset( m_data, 0x00, m_size );
  return (void *)m_data;
}


bool CartridgeROM::unlockData() {
  if( !m_locked ) {
    assert( false );
    return false;
  }
  m_locked = false;
  m_loaded = true;
  if( m_connectedInfo != NULL ) {
    m_connectedInfo->load( (IChipMemory &)*this );
  }
  return true;
}


const void *CartridgeROM::getData() {
  assert( m_loaded && !m_locked && (m_size != 0) );
  return (void *)m_data;
}


bool CartridgeROM::unload() {
  if( (m_data == NULL) || !m_loaded || m_locked ) {
    assert( false );
    return false;
  }

  delete[] m_data;
  m_data = NULL;
  m_size = 0;
  return true;
}


bool CartridgeROM::read8( u32 address, u8 &value ) {
  assert( m_loaded );
  address &= 0x01FFFFFF;
  if( address >= m_size ) {
    assert( false );
    return false;
  }

  value = m_data[address];
  return true;
}


bool CartridgeROM::read16( u32 address, u16 &value ) {
  assert( m_loaded );
  address &= 0x01FFFFFE;
  if( address >= m_size ) {
//    assert( false );
    return false;
  }

  value = READ16LE( m_data + address );
  return true;
}


bool CartridgeROM::read32( u32 address, u32 &value ) {
  assert( m_loaded );
  address &= 0x01FFFFFC;
  if( address >= m_size ) {
    assert( false );
    return false;
  }

  value = READ32LE( m_data + address );
  return true;
}
