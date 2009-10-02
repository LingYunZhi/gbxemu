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


#ifndef SOUND_H
#define SOUND_H

// Sound emulation setup/options and GBA sound emulation

#include "../System.h"
#include <stdio.h>
class CDriver_Sound;

//// Setup/options (these affect GBA and GB sound)

extern CDriver_Sound *soundDriver;

// Initializes sound and returns true if successful. Sets sound quality to
// current value in soundQuality global.
bool soundInit();

// sets the Sound throttle
void soundSetThrottle(unsigned short throttle);

// Manages sound volume, where 1.0 is normal
void soundSetVolume( float );
float soundGetVolume();

// Manages muting bitmask. The bits control the following channels:
// 0x001 Pulse 1
// 0x002 Pulse 2
// 0x004 Wave
// 0x008 Noise
// 0x100 PCM 1
// 0x200 PCM 2
void soundSetEnable( int mask );
int  soundGetEnable();

// Pauses/resumes system sound output
void soundPause();
void soundResume();
extern bool soundPaused; // current paused state

// Cleans up sound. Afterwards, soundInit() can be called again.
void soundShutdown();

//// GBA sound options

long soundGetSampleRate();
void soundSetSampleRate(long sampleRate);

// Sound settings
extern bool soundInterpolation; // 1 if PCM should have low-pass filtering
extern float soundFiltering;    // 0.0 = none, 1.0 = max


//// GBA sound emulation

// GBA sound registers
#define SGCNT0_H 0x82
#define FIFOA_L 0xa0
#define FIFOA_H 0xa2
#define FIFOB_L 0xa4
#define FIFOB_H 0xa6

// Resets emulated sound hardware
void soundReset();

// Emulates write to sound hardware
void soundEvent( u32 addr, u8  data );
void soundEvent( u32 addr, u16 data ); // TODO: error-prone to overload like this

// Notifies emulator that a timer has overflowed
void soundTimerOverflow( int which );

// Notifies emulator that PCM rate may have changed
void interp_rate();

// Notifies emulator that SOUND_CLOCK_TICKS clocks have passed
void psoundTickfn();
extern int SOUND_CLOCK_TICKS;   // Number of 16.8 MHz clocks between calls to soundTick()
extern int soundTicks;          // Number of 16.8 MHz clocks until soundTick() will be called

// Saves/loads emulator state
void soundSaveGame( FILE *out );
void soundReadGame( FILE *in, int version );

class Multi_Buffer;

void flush_samples(Multi_Buffer * buffer);

#endif // SOUND_H
