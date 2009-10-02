#ifndef CEMUGBA_H
#define CEMUGBA_H

#include "common/Types.h"
class SoundDriver;
class CDriver_Graphics;

class CEmuGBA
{
public:
    CEmuGBA();
    ~CEmuGBA();

    bool loadROM( const u8 *const romData, const u32 romSize );
    bool closeROM();

    bool emulate();

private:
    bool m_romLoaded;
    bool m_soundInitialized;
    SoundDriver *m_dummySound;
    CDriver_Graphics *m_gfx;
};

#endif // CEMUGBA_H
