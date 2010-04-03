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


#include "genericmemory.h"
#include <assert.h>


GenericMemory::GenericMemory()
{
  m_size = 0;
  m_data = NULL;
  m_locked = false;
}


GenericMemory::~GenericMemory()
{
  deleteMem();
}


bool GenericMemory::setSize( u32 size ) {
  if( size == 0 ) return false;

  m_size = size;
  newMem();
  return true;
}


u32 GenericMemory::getSize() {
  return m_size;
}


void *GenericMemory::lockData() {
  assert( m_locked == false );

  m_locked = true;
  return (void *)m_data;
}


bool GenericMemory::unlockData() {
  if( m_locked == false ) return false;

  m_locked = false;
  return true;
}


const void *GenericMemory::getData() {
  return (const void *)m_data;
}


bool GenericMemory::unload() {
  deleteMem();
  return true;
}


void GenericMemory::newMem() {
  if( m_size == 0 ) return;
  deleteMem();

  m_data = new u8[ m_size ];
  assert( m_data != NULL );
}


void GenericMemory::deleteMem() {
  if( m_data != NULL ) {
    delete[] m_data;
    m_data = NULL;
    m_size = 0;
  }
}
