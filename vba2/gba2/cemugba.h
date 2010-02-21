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


#ifndef CEMUGBA_H
#define CEMUGBA_H

#include "common/Types.h"
class CDriver_Sound;
class CDriver_Graphics;
class CDriver_Input;
class BackupMedia;

class CEmuGBA
{
public:
    CEmuGBA();
    ~CEmuGBA();

    // load drivers before loading ROM
    // use CDummyDriver_xxx if no custom driver present
    bool setDriverSound   ( CDriver_Sound    *drv );
    bool setDriverGraphics( CDriver_Graphics *drv );
    bool setDriverInput   ( CDriver_Input    *drv );
    bool setDebugDriverGraphics( CDriver_Graphics *drv );

    bool loadROM( const u8 *const romData, const u32 romSize );
    bool closeROM();

    bool emulate();

    void toggleFastForward( const bool enable );

    BackupMedia *getBackupMedia();


private:
    CDriver_Sound    *m_snd;
    bool m_soundDriverLoaded;

    CDriver_Graphics *m_gfx;
    bool m_graphicsDriverLoaded;

    // a second graphics driver which can be used for debugging purposes
    CDriver_Graphics *m_gfxdbg;
    bool m_graphicsDebugDriverLoaded;

    CDriver_Input    *m_inp;
    bool m_inputDriverLoaded;

    bool m_romLoaded;
    bool m_soundInitialized;

    BackupMedia *m_backupMedia;
};

#endif // CEMUGBA_H
