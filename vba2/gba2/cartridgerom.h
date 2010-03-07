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


#ifndef CARTRIDGEROM_H
#define CARTRIDGEROM_H


#include "common/Types.h"
#include "common/ichipmemory.h"
#include "cartridgeinfo.h"


class CartridgeROM : public IChipMemory
{
public:
  CartridgeROM();
  ~CartridgeROM();

  // Optional:
  // Whenever ROM changes, CartridgeInfo::load() will be called.
  // Use NULL pointer to disconnect.
  void connectInfo( CartridgeInfo *connectedInfo );

  bool isLoaded();

  // IChipMemory interface
  virtual bool setSize( u32 size );
  virtual u32  getSize();
  virtual void  *lockData();
  virtual bool unlockData();
  virtual const void *getData();
  virtual bool unload();

  // read access
  // - returns false if bad memory area accessed
  // - forcefully aligns address
  bool read8 ( u32 address,  u8 &value );
  bool read16( u32 address, u16 &value );
  bool read32( u32 address, u32 &value );

private:
  u8 *m_data;
  u32 m_size;
  bool m_loaded;
  bool m_locked;
  CartridgeInfo *m_connectedInfo;
};


#endif // CARTRIDGEROM_H
