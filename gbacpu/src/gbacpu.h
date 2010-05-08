#ifndef GBACPU_H
#define GBACPU_H

#include "types.h"
#include <assert.h>
#include "imemory.h"

class Gbacpu {
private:
    typedef u32 ADDRESS;
    typedef u32 OPCODE;

    // TODO: only works on little-endian, use WRITE/READLE macros
    typedef union {
        u32 uw;    // unsigned word
        s32 sw;    //   signed word
        u16 uh[2]; // unsigned halfword
        s16 sh[2]; //   signed halfword
        u8  ub[4]; // unsigned byte
        s8  sb[4]; //   signed byte
    } REGISTER;

    // current/saved program status register
    typedef struct {
        // control bits
        unsigned mode : 5; // MODE_xxx
        unsigned thumb : 1; // 0=ARM  1=THUMB
        unsigned fiq : 1; // 1=FIQ disabled
        unsigned irq : 1; // 1=IRQ disabled
        // reserved
        unsigned reserved : 20; // do not change
        // condition code flags
        unsigned v : 1; // overflow flag
        unsigned c : 1; // carry or borrow or extend flag
        unsigned z : 1; // zero flag
        unsigned n : 1; // negative or less than
    } PROGRAM_STATUS_REGISTER;
    static const u8 MODE_USR = 0x10; // user
    static const u8 MODE_FIQ = 0x11; // fast interrupt
    static const u8 MODE_IRQ = 0x12; // interrupt
    static const u8 MODE_SVC = 0x13; // supervisor
    static const u8 MODE_ABT = 0x17; // abort
    static const u8 MODE_UND = 0x1B; // undefined
    static const u8 MODE_SYS = 0x1F; // system

    // will be replaced by the banked registers on mode switch
    REGISTER reg[16]; // 13=SP  14=LR  15=PC
    PROGRAM_STATUS_REGISTER cpsr;
    // TODO: add SPSR for every(?) mode

    IMemory &mem;

public:
    Gbacpu( IMemory &memoryInterface )
        : mem( memoryInterface )
    {
        reset();
    }

    void reset() {
        cpsr.mode = MODE_SVC;
        cpsr.fiq = 1;
        cpsr.irq = 1;
        cpsr.thumb = 0;
        reg[15].uw = 8;
    }

    void testMyself() {
        assert( sizeof(REGISTER) == 4 );
        assert( sizeof(PROGRAM_STATUS_REGISTER) == 4 );
    }

    // execute a single instruction
    void step() {
        // fetch instruction from memory
        OPCODE o;
        mem.load32( (u32*)&o, reg[15].uw - 8 );
    }
};

#endif // GBACPU_H
