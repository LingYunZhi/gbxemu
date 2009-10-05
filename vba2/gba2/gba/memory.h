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


#ifndef MEMORY_H
#define MEMORY_H

#include "../common/Types.h"

#define CPUReadByteQuick(addr)  map[(addr)>>24].address[(addr) & map[(addr)>>24].mask]
#define CPUReadHalfWordQuick(addr)  READ16LE(((u16*)&map[(addr)>>24].address[(addr) & map[(addr)>>24].mask]))
#define CPUReadMemoryQuick(addr)  READ32LE(((u32*)&map[(addr)>>24].address[(addr) & map[(addr)>>24].mask]))

u8 CPUReadByte(u32 address);
u32 CPUReadHalfWord(u32 address);
u16 CPUReadHalfWordSigned(u32 address);
void CPUWriteMemory(u32 address, u32 value);
void CPUWriteHalfWord(u32 address, u16 value);
void CPUWriteByte(u32 address, u8 b);
u32 CPUReadMemory(u32 address);

#endif // MEMORY_H
