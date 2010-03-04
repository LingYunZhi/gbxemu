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


#include "cemugba.h"
#include "backupmedia.h"
#include "bioschip.h"

#include "gba/GBA.h"
#include "gba/Sound.h"
#include <assert.h>
#include <string.h> // memcpy


CEmuGBA::CEmuGBA()
{
  m_snd = NULL;
  m_soundDriverLoaded = false;

  m_gfx = NULL;
  m_graphicsDriverLoaded = false;

  m_gfxdbg = NULL;
  m_graphicsDebugDriverLoaded = false;

  m_inp = NULL;
  m_inputDriverLoaded = false;

  m_gameROM = NULL;
  m_gameROMSize = 0;
  m_initialized = false;
  m_soundInitialized = false;

  m_backupMedia = NULL;

  m_biosChip = NULL;
  m_biosChip = new BiosChip();
  assert( m_biosChip != NULL );

  biosChip = m_biosChip; // set global variable in core
}


CEmuGBA::~CEmuGBA()
{
  if( m_biosChip != NULL ) {
    delete m_biosChip;
  }

  if( m_backupMedia != NULL ) {
    delete m_backupMedia;
  }

  if( m_gameROM != NULL ) {
    delete[] m_gameROM;
  }
}


bool CEmuGBA::initialize( const u8 *const romData, const u32 romSize )
{
  const bool allDriversLoaded =
             m_soundDriverLoaded &&
             m_graphicsDriverLoaded &&
             m_inputDriverLoaded;
  if( !allDriversLoaded ) return false;

  if( romSize < 192 ) return false;

  // allocate new ROM memory if old and new ROM size are not equal
  if( romSize != m_gameROMSize ) {
    // free old ROM memory
    if( m_gameROM != NULL ) {
      delete[] m_gameROM;
      m_gameROM = NULL;
    }
    m_gameROM = new u8[romSize];
    assert( m_gameROM != NULL );
    m_gameROMSize = romSize;
  }

  // copy over rom data
  assert( m_gameROMSize == romSize );
  memcpy( m_gameROM, romData, m_gameROMSize );

  // initialize backup media
  if( m_backupMedia != NULL ) {
    delete m_backupMedia;
    m_backupMedia = NULL;
  }
  m_backupMedia = new BackupMedia( (u32*)m_gameROM, m_gameROMSize );
  backupMedia = m_backupMedia; // set global variable in core

  CPULoadRom( m_gameROM, m_gameROMSize );

  if( m_soundInitialized ) {
    soundReset();
  } else {
    soundDriver = m_snd; // set global variable in core
    soundInit();
    m_soundInitialized = true;
  }
  CPUInit();
  CPUReset();
  m_initialized = true;
  return true;
}


bool CEmuGBA::shutDown()
{
  if( m_initialized ) {
    CPUCleanUp();
    m_initialized = false;
    return true;
  }

  return false;
}


bool CEmuGBA::emulate()
{
  if( !m_initialized ) return false;
  CPULoop( cyclesPerFrame );
  return true;
}


bool CEmuGBA::setDriverSound( CDriver_Sound *drv )
{
  if( drv == NULL ) return false;
  m_snd = drv;
  m_soundDriverLoaded = true;
  if( m_soundInitialized ) {
    soundDriver = m_snd;
    soundInit();
  }
  return true;
}


bool CEmuGBA::setDriverGraphics( CDriver_Graphics *drv )
{
  if( drv == NULL ) return false;
  m_gfx = drv;
  graphicsDriver = m_gfx; // set global variable in core
  m_graphicsDriverLoaded = true;
  return true;
}


bool CEmuGBA::setDebugDriverGraphics( CDriver_Graphics *drv )
{
  m_gfxdbg = drv;
  graphicsDebugDriver = m_gfxdbg; // set global variable in core
  m_graphicsDebugDriverLoaded = true;
  return true;
}


bool CEmuGBA::setDriverInput( CDriver_Input *drv )
{
  if( drv == NULL ) return false;
  m_inp = drv;
  inputDriver = m_inp; // set global variable in core
  m_inputDriverLoaded = true;
  return true;
}


BackupMedia *CEmuGBA::getBackupMedia() {
  return m_backupMedia;
}


BiosChip *CEmuGBA::getBiosChip() {
  return m_biosChip;
}
