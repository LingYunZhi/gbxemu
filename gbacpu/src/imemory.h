#ifndef IMEMORY_H
#define IMEMORY_H

#include "types.h"

// memory interface
// connects CPU to memory subsystem
// all methods return false if the address was not available
class IMemory {
public:

    virtual bool store32( u32 data, u32 address ) = 0;
    virtual bool store16( u16 data, u32 address ) = 0;
    virtual bool store8( u8 data, u32 address ) = 0;

    virtual bool load32( u32 *data, u32 address ) = 0;
    virtual bool load16( u16 *data, u32 address ) = 0;
    virtual bool load8( u8 *data, u32 address ) = 0;
};

#endif // IMEMORY_H
