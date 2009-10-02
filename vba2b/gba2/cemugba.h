#ifndef CEMUGBA_H
#define CEMUGBA_H

#include "common/Types.h"
class CDriver_Sound;
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
    CDriver_Sound    *m_snd;
    CDriver_Graphics *m_gfx;
};

#endif // CEMUGBA_H
