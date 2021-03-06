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


#ifndef GENERICMEMORY_H
#define GENERICMEMORY_H


#include "common/Types.h"
#include "common/ichipmemory.h"


class GenericMemory : public IChipMemory
{
public:
  GenericMemory();
  ~GenericMemory();

  // IChipMemory interface
public:
  bool setSize( u32 size );
  u32  getSize();
  void *lockData();
  bool unlockData();
  const void *getData();
  bool unload();

private:
  u32 m_size;
  u8 *m_data;
  bool m_locked;

  void newMem();
  void deleteMem();
};


#endif // GENERICMEMORY_H
