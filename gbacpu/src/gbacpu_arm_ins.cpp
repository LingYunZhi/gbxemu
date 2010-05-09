// This file emulates ARM instructions
#include "gbacpu.h"


// V flag for: LEFT + RIGHT = RESULT
#define SIGNED_OVERFLOW( LEFT, RIGHT, RESULT ) \
    ( \
      ( ((s32)(left) >= 0) && ((s32)(right) >= 0) && ((s32)(result) < 0) ) \
      || \
      ( ((s32)(left) < 0) && ((s32)(right) < 0) && ((s32)(result) >= 0) ) \
    )


// V flag for: LEFT - RIGHT = RESULT
#define SIGNED_UNDERFLOW( LEFT, RIGHT, RESULT ) \
    ( \
      ( ((s32)(left) >= 0) && ((s32)(right) < 0) && ((s32)(result) < 0) ) \
      || \
      ( ((s32)(left) < 0) && ((s32)(right) >= 0) && ((s32)(result) >= 0) ) \
    )


void GbaCpu::aDecodeAndExecute() {
    // TODO: check condition field
    // TODO: decode shifter operand to shifter_operand and shifter_carry_out
    if( cop.op == 0 ) {
        switch( cop.type ) {
        case 0x0: aAND(); break;
        case 0x1: aEOR(); break;
        case 0x2: aSUB(); break;
        case 0x3: aRSB(); break;
        case 0x4: aADD(); break;
        case 0x5: aADC(); break;
        case 0x6: aSBC(); break;
        case 0x7: aRSC(); break;
        case 0x8: aTST(); break;
        case 0x9: aTEQ(); break;
        case 0xA: aCMP(); break;
        case 0xB: aCMN(); break;
        case 0xC: aORR(); break;
        case 0xD: aMOV(); break;
        case 0xE: aBIC(); break;
        case 0xF: aMVN(); break;
        }
    }
}


/*
  The result is written back to reg[Rd] AFTER the flag checks took place
  to ensure that operands are not overwritten (in case Rd == Rn).
*/


// bitwise and
// AND{<cond>}{S}  <Rd>, <Rn>, <shifter_operand>
void GbaCpu::aAND() {
    const u32 left = reg[cop.Rn].uw;
    const u32 right = shifter_operand.uw;
    const u32 result = left & right;

    if( cop.S ) {
        // TODO: handle Rd==15
        cpsr.n = (result >> 31);
        cpsr.z = (result == 0);
        cpsr.c = shifter_carry_out;
    }

    reg[cop.Rd].uw = result;
}


// bitwise exclusive or
// EOR{<cond>}{S}  <Rd>, <Rn>, <shifter_operand>
void GbaCpu::aEOR() {
    const u32 left = reg[cop.Rn].uw;
    const u32 right = shifter_operand.uw;
    const u32 result = left ^ right;

    if( cop.S ) {
        // TODO: handle Rd==15
        cpsr.n = (result >> 31);
        cpsr.z = (result == 0);
        cpsr.c = shifter_carry_out;
    }

    reg[cop.Rd].uw = result;
}


// subtract
// SUB{<cond>}{S}  <Rd>, <Rn>, <shifter_operand>
void GbaCpu::aSUB() {
    const u32 left = reg[cop.Rn].uw;
    const u32 right = shifter_operand.uw;
    const u32 result = left - right;

    if( cop.S ) {
        // TODO: handle Rd==15
        cpsr.n = (result >> 31);
        cpsr.z = (result == 0);
        cpsr.c = !(right > left);
        cpsr.v = SIGNED_UNDERFLOW( left, right, result );
    }

    reg[cop.Rd].uw = result;
}


// reverse subtract
// RSB{<cond>}{S}  <Rd>, <Rn>, <shifter_operand>
void GbaCpu::aRSB() {
    const u32 left = shifter_operand.uw;
    const u32 right = reg[cop.Rn].uw;
    const u32 result = left - right;

    if( cop.S ) {
        // TODO: handle Rd==15
        cpsr.n = (result >> 31);
        cpsr.z = (result == 0);
        cpsr.c = !(right > left);
        cpsr.v = SIGNED_UNDERFLOW( left, right, result );
    }

    reg[cop.Rd].uw = result;
}


// add
// ADD{<cond>}{S}  <Rd>, <Rn>, <shifter_operand>
void GbaCpu::aADD() {
    const u32 left = reg[cop.Rn].uw;
    const u32 right = shifter_operand.uw;
    const u32 result = left + right;

    if( cop.S ) {
        // TODO: handle Rd==15
        cpsr.n = (result >> 31);
        cpsr.z = (result == 0);
        cpsr.c = (right >= -left);
        cpsr.v = SIGNED_OVERFLOW( left, right, result );
    }

    reg[cop.Rd].uw = result;
}


// add with carry
// ADC{<cond>}{S}  <Rd>, <Rn>, <shifter_operand>
void GbaCpu::aADC() {
    const u32 left = reg[cop.Rn].uw;
    const u32 right = shifter_operand.uw;
    const u32 carry = (u32)cpsr.c;
    const u32 temp = left + right;
    const u32 result = temp + carry;

    if( cop.S ) {
        // TODO: handle Rd==15
        cpsr.n = (result >> 31);
        cpsr.z = (result == 0);
        cpsr.c = (right >= -left) || (carry && (temp == 0xFFFFFFFF));
        cpsr.v = SIGNED_OVERFLOW( left, right, temp )
              || SIGNED_OVERFLOW( temp, carry, result );
    }

    reg[cop.Rd].uw = result;
}


// subtract with carry
// SBC{<cond>}{S}  <Rd>, <Rn>, <shifter_operand>
void GbaCpu::aSBC() {
    const u32 left = reg[cop.Rn].uw;
    const u32 right = shifter_operand.uw;
    const u32 borrow = (u32)(cpsr.c == 0);
    const u32 temp = left - right;
    const u32 result = temp - borrow;

    if( cop.S ) {
        // TODO: handle Rd==15
        cpsr.n = (result >> 31);
        cpsr.z = (result == 0);
        cpsr.c = !( (right > left) || (borrow > temp) );
        cpsr.v = SIGNED_UNDERFLOW( left, right, temp )
              || SIGNED_UNDERFLOW( temp, borrow, result );
    }

    reg[cop.Rd].uw = result;
}


// reverse subtract with carry
// RSC{<cond>}{S}  <Rd>, <Rn>, <shifter_operand>
void GbaCpu::aRSC() {
    const u32 left = shifter_operand.uw;
    const u32 right = reg[cop.Rn].uw;
    const u32 borrow = (u32)(cpsr.c == 0);
    const u32 temp = left - right;
    const u32 result = temp - borrow;

    if( cop.S ) {
        // TODO: handle Rd==15
        cpsr.n = (result >> 31);
        cpsr.z = (result == 0);
        cpsr.c = !( (right > left) || (borrow > temp) );
        cpsr.v = SIGNED_UNDERFLOW( left, right, temp )
              || SIGNED_UNDERFLOW( temp, borrow, result );
    }

    reg[cop.Rd].uw = result;
}


void GbaCpu::aTST() {

}


void GbaCpu::aTEQ() {

}


void GbaCpu::aCMP() {

}


void GbaCpu::aCMN() {

}


void GbaCpu::aORR() {

}


void GbaCpu::aMOV() {

}


void GbaCpu::aBIC() {

}


void GbaCpu::aMVN() {

}
