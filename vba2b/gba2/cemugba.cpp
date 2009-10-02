#include "cemugba.h"

#include "gba/GBA.h"
#include "gba/Sound.h"
#include "common/csoundoutputdummy.h"
#include "common/cdriver_graphics.h"
#include <assert.h>

CEmuGBA::CEmuGBA()
{
    m_romLoaded = false;
    m_soundInitialized = false;

    m_dummySound = NULL;
    m_dummySound = new CSoundOutputDummy();
    assert( m_dummySound != NULL );

    m_gfx = NULL;
    m_gfx = new CDummyDriver_Graphics();
    assert( m_gfx != NULL );
}

CEmuGBA::~CEmuGBA()
{
    if( m_dummySound != NULL ) {
        delete m_dummySound;
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
        soundDriver = m_dummySound;
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
