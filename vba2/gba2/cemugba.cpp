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

  m_initialized = false;
  m_soundInitialized = false;

  m_cartridge = NULL;
  m_cartridge = new Cartridge();
  assert( m_cartridge != NULL );

  m_biosChip = NULL;
  m_biosChip = new BiosChip();
  assert( m_biosChip != NULL );

  biosChip = m_biosChip; // set global variable in core

  m_romLocked = false;
}


CEmuGBA::~CEmuGBA()
{
  if( m_biosChip != NULL ) {
    delete m_biosChip;
  }

  if( m_cartridge != NULL ) {
    delete m_cartridge;
  }
}


IChipMemory &CEmuGBA::lockROM() {
  assert( !m_romLocked );
  m_romLocked = true;
  return (IChipMemory &)m_cartridge->getROM();
}


void CEmuGBA::unlockROM() {
  assert( m_romLocked );
  m_romLocked = false;
}

bool CEmuGBA::initialize() {
  const bool allDriversLoaded =
             m_soundDriverLoaded &&
             m_graphicsDriverLoaded &&
             m_inputDriverLoaded;
  if( !allDriversLoaded ) {
    assert( false );
    return false;
  }

  bool result = CPULoadRom( m_cartridge );
  assert( result );

  if( m_soundInitialized ) {
    soundReset();
  } else {
    soundDriver = m_snd; // set global variable in core
    result = soundInit();
    assert( result );
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


CartridgeInfo &CEmuGBA::getCartridgeInfo() {
  assert( m_cartridge != NULL );
  return m_cartridge->getInfo();
}


BackupMedia &CEmuGBA::getBackupMedia() {
  assert( m_cartridge != NULL );
  return *m_cartridge->getSave();
}


BiosChip *CEmuGBA::getBiosChip() {
  return m_biosChip;
}
