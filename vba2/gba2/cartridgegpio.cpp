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


#include "cartridgegpio.h"
#include <assert.h>

/*
#define TRACE_GPIO
#ifdef TRACE_GPIO
#include <stdio.h>
#endif
*/

CartridgeGPIO::CartridgeGPIO()
{
  m_writeOnly = true;
  m_dataDirection = 0;
  m_connectedDevice = NULL;
}


void CartridgeGPIO::connectDevice( IGPIO *device ) {
  m_connectedDevice = device;
}


bool CartridgeGPIO::read16( u32 address, u16 &value ) {
  if( m_writeOnly ) return false;

  switch( address )
  {
  default:
    break;
  case 0x080000C4:
    if( m_connectedDevice == NULL ) break;
    assert( m_dataDirection != 0xF ); // at least one bit should be marked to be received
    value = (u16)m_connectedDevice->receive4( m_dataDirection );
    return true;
  case 0x080000C6:
    value = (u16)m_dataDirection;
    return true;
  case 0x080000C8:
    value = m_writeOnly ? 0 : 1;
    return true;
  }

  return false;
}


bool CartridgeGPIO::write16( u32 address, u16 value ) {
  switch( address )
  {
  default:
    break;
  case 0x080000C4: // data port
    if( m_connectedDevice == NULL ) break;
    assert( m_dataDirection != 0 ); // at least one bit should be marked to be sent
    m_connectedDevice->send4( (u8)(value & 0x000F), m_dataDirection );
    return true;
  case 0x080000C6: // data port direction
    m_dataDirection = value & 0x000F;
    return true;
  case 0x080000C8: // I/O port control
    m_writeOnly = !(value & 1);
    return true;
  }

  return false;
}
