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


#include "cemugba.h"

#include "gba/GBA.h"
#include "gba/Sound.h"
#include "common/cdriver_sound.h"
#include "common/cdriver_graphics.h"
#include <assert.h>

CEmuGBA::CEmuGBA()
{
    m_romLoaded = false;
    m_soundInitialized = false;

    m_snd = NULL;
    m_snd = new CDummyDriver_Sound();
    assert( m_snd != NULL );

    m_gfx = NULL;
    m_gfx = new CDummyDriver_Graphics();
    assert( m_gfx != NULL );
}

CEmuGBA::~CEmuGBA()
{
    if( m_snd != NULL ) {
        delete m_snd;
    }

    if( m_gfx != NULL ) {
        delete m_gfx;
    }
}

bool CEmuGBA::loadROM( const u8 *const romData, const u32 romSize )
{
    CPULoadRom( romData, romSize );
    if( m_soundInitialized ) {
        soundReset();
    } else {
        soundDriver = m_snd;
        soundInit();
        m_soundInitialized = true;
    }
    CPUInit();
    CPUReset();
    m_romLoaded = true;
    return true;
}

bool CEmuGBA::closeROM()
{
    if( m_romLoaded ) {
        CPUCleanUp();
        m_romLoaded = false;
        return true;
    }

    return false;
}

bool CEmuGBA::emulate()
{
    if( !m_romLoaded ) return false;

    CPULoop( cyclesPerFrame );
    return true;
}
