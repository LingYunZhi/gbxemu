#ifndef GBACPU_H
#define GBACPU_H

#include "types.h"
#include <assert.h>
#include "imemory.h"

class GbaCpu {
private:
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
    typedef union {
        u32 uw;
        struct {
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
        };
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

    IMemory &mem; // reference to memory interface

    // aXXX methods are ARM versions
    // tXXX methods are THUMB versions

    // ARM instruction format
    typedef union {
        // unsigned word
        u32 uw;

        // data processing
        struct {
            // shifter operand
            unsigned so : 12;
            // instruction
            unsigned Rd : 4; // register
            unsigned Rn : 4; // register
            unsigned S : 1; // 1=update CPSR
            unsigned type : 4; // type of data processing instruction
            unsigned I : 1; // immediate shifter operand
            unsigned op : 2; // opcode category, must be 0 or 1
            // condition
            unsigned cond : 4;
        };
    } aINSTRUCTION;


    aINSTRUCTION cop; // current opcode being processed
    REGISTER shifter_operand; // shifter operand value of current opcode (cop)
    bool shifter_carry_out;


    void aDecodeAndExecute(); // execute appropriate data processing instruction
    bool aConditionMet();
    void aShifterOperand(); // calculate shifter operand
    void aAND(); // logical and
    void aEOR(); // logical xor
    void aSUB(); // subtract
    void aRSB(); // reverse subtract
    void aADD(); // add
    void aADC(); // add with carry
    void aSBC(); // subtract with carry
    void aRSC(); // reverse subtract with carry
    void aTST(); // test
    void aTEQ(); // test equivalence
    void aCMP(); // compare
    void aCMN(); // compare negated
    void aORR(); // logical or
    void aMOV(); // move
    void aBIC(); // bit clear
    void aMVN(); // move not

public:
    GbaCpu( IMemory &memoryInterface )
        : mem( memoryInterface )
    {
        testMyself();
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
        assert( sizeof(aINSTRUCTION) == 4 );
    }

    // execute a single instruction
    void step() {
        if( cpsr.thumb ) { // THUMB mode
        } else { // ARM mode
            // fetch instruction from memory
            mem.load32( &cop.uw, reg[15].uw - 8 );
            aDecodeAndExecute();
        }
    }
};

#endif // GBACPU_H
