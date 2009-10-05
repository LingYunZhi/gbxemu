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
};


// dummy input driver
class CDummyDriver_Input : public CDriver_Input
{
public:
    u16 getKeyStates() { return 0x0000; };
};

#endif // CDRIVER_INPUT_H
