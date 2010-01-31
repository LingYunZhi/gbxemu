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

#ifndef CDRIVER_SOUND_H
#define CDRIVER_SOUND_H

#include "Types.h"

/**
 * Sound driver abstract interface for the core to use to output sound.
 * Subclass this to implement a new sound driver.
 */
class CDriver_Sound
{
public:

	/**
	 * Destructor. Free the resources allocated by the sound driver.
	 */
        virtual ~CDriver_Sound() { };

	/**
	 * Initialize the sound driver.
	 * @param sampleRate In Hertz
	 */
	virtual bool init(long sampleRate) = 0;

	/**
	 * Tell the driver that the sound stream has paused
	 */
	virtual void pause() = 0;

	/**
	 * Reset the sound driver
	 */
	virtual void reset() = 0;

	/**
	 * Tell the driver that the sound stream has resumed
	 */
	virtual void resume() = 0;

	/**
	 * Write length bytes of data from the finalWave buffer to the driver output buffer.
	 */
	virtual void write(u16 * finalWave, int length) = 0;

	virtual void setThrottle(unsigned short throttle) { };
};


// dummy sound driver
class CDummyDriver_Sound : public CDriver_Sound
{
public:
    bool init(long sampleRate) { return true; };
    void pause() { };
    void reset() { };
    void resume() { };
    void write(u16 * finalWave, int length) { };
    void setThrottle(unsigned short throttle) { };
};

#endif // CDRIVER_SOUND_H
