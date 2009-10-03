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

    bool setDriverSound( CDriver_Sound *drv );
    bool setDriverGraphics( CDriver_Graphics *drv );

private:
    bool m_romLoaded;
    bool m_soundInitialized;
    bool m_soundDriverLoaded;
    bool m_graphicsDriverLoaded;
    CDriver_Sound    *m_snd;
    CDriver_Graphics *m_gfx;
};

#endif // CEMUGBA_H
