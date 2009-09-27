#ifndef CEMUGBA_H
#define CEMUGBA_H

#include "common/Types.h"
class CSoundOutputDummy;

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
    CSoundOutputDummy *m_dummySound;
};

#endif // CEMUGBA_H
