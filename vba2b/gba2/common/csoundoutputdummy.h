#ifndef CSOUNDOUTPUTDUMMY_H
#define CSOUNDOUTPUTDUMMY_H

#include "SoundDriver.h"

class CSoundOutputDummy : public SoundDriver
{
public:
    CSoundOutputDummy() { };
    ~CSoundOutputDummy() { };
    bool init(long sampleRate) { return true; };
    void pause() { };
    void reset() { };
    void resume() { };
    void write(u16 * finalWave, int length) { };
    void setThrottle(unsigned short throttle) { };
};

#endif // CSOUNDOUTPUTDUMMY_H
