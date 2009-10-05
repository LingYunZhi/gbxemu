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


#ifndef CDRIVER_INPUT_H
#define CDRIVER_INPUT_H

#include "Types.h"

// input driver interface
class CDriver_Input
{
public:
    virtual ~CDriver_Input() { };
    virtual u16 getKeyStates() = 0;

    // getKeyStates() returns a combination of the following flags:
    static const u16 BUTTON__NONE  = 0x0000;
    static const u16 BUTTON_A      = 0x0001;
    static const u16 BUTTON_B      = 0x0002;
    static const u16 BUTTON_SELECT = 0x0004;
    static const u16 BUTTON_START  = 0x0008;
    static const u16 BUTTON_RIGHT  = 0x0010;
    static const u16 BUTTON_LEFT   = 0x0020;
    static const u16 BUTTON_UP     = 0x0040;
    static const u16 BUTTON_DOWN   = 0x0080;
    static const u16 BUTTON_R      = 0x0100;
    static const u16 BUTTON_L      = 0x0200;
    static const u16 BUTTON__ALL   = 0x03FF;
};


// dummy input driver
class CDummyDriver_Input : public CDriver_Input
{
public:
    u16 getKeyStates() { return CDriver_Input::BUTTON__NONE; };
};

#endif // CDRIVER_INPUT_H
