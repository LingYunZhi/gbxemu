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
