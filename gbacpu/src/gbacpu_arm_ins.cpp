// This file emulates ARM instructions
#include "gbacpu.h"


// V flag for: LEFT + RIGHT = RESULT
#define SIGNED_OVERFLOW( LEFT, RIGHT, RESULT ) \
    ( \
      ( ((s32)(LEFT) >= 0) && ((s32)(RIGHT) >= 0) && ((s32)(RESULT) < 0) ) \
      || \
      ( ((s32)(LEFT) < 0) && ((s32)(RIGHT) < 0) && ((s32)(RESULT) >= 0) ) \
    )


// V flag for: LEFT - RIGHT = RESULT
#define SIGNED_UNDERFLOW( LEFT, RIGHT, RESULT ) \
    ( \
      ( ((s32)(LEFT) >= 0) && ((s32)(RIGHT) < 0) && ((s32)(RESULT) < 0) ) \
      || \
      ( ((s32)(LEFT) < 0) && ((s32)(RIGHT) >= 0) && ((s32)(RESULT) >= 0) ) \
    )

#define ROTATE_RIGHT32( VALUE, ROTATE ) \
    ( ((u32)(VALUE) >> (u32)(ROTATE)) | ((u32)(VALUE) << (32-(u32)(ROTATE))) );


void GbaCpu::aDecodeAndExecute() {
    // TODO: check condition field

    aShifterOperand();

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


void GbaCpu::aShifterOperand() {
    if( cop.I ) {
        // immediate shifter operand
        const u32 immed_8 = cop.uw & 0xFF;
        const u32 rotate_imm = (cop.uw & 0xF00) >> 7;
        shifter_operand.uw = ROTATE_RIGHT32( immed_8, rotate_imm );
        if( rotate_imm == 0 ) {
            shifter_carry_out = cpsr.c;
        } else {
            shifter_carry_out = (shifter_operand.uw >> 31);
        }
    } else {
        // register shifter operand
        const u8 op = (cop.uw >> 4) & (BIT0|BIT1|BIT2);
        const u32 Rm = reg[cop.uw & 0xF].uw;
        switch( op ) {
        case 0: {
                const u32 shift_imm = (cop.uw >> 7) & 0x1F;
                if( shift_imm == 0 ) {
                    // <Rm>
                    shifter_operand.uw = Rm;
                    shifter_carry_out = cpsr.c;
                } else {
                    // <Rm>, LSL #<shift_imm>
                    shifter_operand.uw = Rm << shift_imm;
                    shifter_carry_out = (Rm >> (32 - shift_imm)) & 1;
                }
            }
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        }
    }}


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


// test bits
// TST{<cond>}  <Rn>, <shifter_operand>
void GbaCpu::aTST() {
    const u32 temp = reg[cop.Rn].uw & shifter_operand.uw;

    cpsr.n = (temp >> 31);
    cpsr.z = (temp == 0);
    cpsr.c = shifter_carry_out;
}


// test equivalence
// TEQ{<cond>}  <Rn>, <shifter_operand>
void GbaCpu::aTEQ() {
    const u32 temp = reg[cop.Rn].uw ^ shifter_operand.uw;

    cpsr.n = (temp >> 31);
    cpsr.z = (temp == 0);
    cpsr.c = shifter_carry_out;
}


// compare
// CMP{<cond>}  <Rn>, <shifter_operand>
void GbaCpu::aCMP() {
    const u32 left = reg[cop.Rn].uw;
    const u32 right = shifter_operand.uw;
    const u32 temp = left - right;

    cpsr.n = (temp >> 31);
    cpsr.z = (temp == 0);
    cpsr.c = !(right > left);
    cpsr.v = SIGNED_UNDERFLOW( left, right, temp );
}


// compare negative
// CMN{<cond>}  <Rn>, <shifter_operand>
void GbaCpu::aCMN() {
    const u32 left = reg[cop.Rn].uw;
    const u32 right = shifter_operand.uw;
    const u32 temp = left + right;

    cpsr.n = (temp >> 31);
    cpsr.z = (temp == 0);
    cpsr.c = (right >= -left);
    cpsr.v = SIGNED_OVERFLOW( left, right, temp );
}


// bitwise or
// ORR{<cond>}{S}  <Rd>, <Rn>, <shifter_operand>
void GbaCpu::aORR() {
    const u32 left = reg[cop.Rn].uw;
    const u32 right = shifter_operand.uw;
    const u32 result = left | right;

    if( cop.S ) {
        // TODO: handle Rd==15
        cpsr.n = (result >> 31);
        cpsr.z = (result == 0);
        cpsr.c = shifter_carry_out;
    }

    reg[cop.Rd].uw = result;
}


// move
// MOV{<cond>}{S}  <Rd>, <shifter_operand>
void GbaCpu::aMOV() {
    const u32 result = shifter_operand.uw;

    if( cop.S ) {
        // TODO: handle Rd==15
        cpsr.n = (result >> 31);
        cpsr.z = (result == 0);
        cpsr.c = shifter_carry_out;
    }

    reg[cop.Rd].uw = result;
}


// bit clear
// BIC{<cond>}{S}  <Rd>, <Rn>, <shifter_operand>
void GbaCpu::aBIC() {
    const u32 left = reg[cop.Rn].uw;
    const u32 right = shifter_operand.uw;
    const u32 result = left & (~right);

    if( cop.S ) {
        // TODO: handle Rd==15
        cpsr.n = (result >> 31);
        cpsr.z = (result == 0);
        cpsr.c = shifter_carry_out;
    }

    reg[cop.Rd].uw = result;
}


// move not
// MVN{<cond>}{S}  <Rd>, <shifter_operand>
void GbaCpu::aMVN() {
    const u32 result = ~(shifter_operand.uw);

    if( cop.S ) {
        // TODO: handle Rd==15
        cpsr.n = (result >> 31);
        cpsr.z = (result == 0);
        cpsr.c = shifter_carry_out;
    }

    reg[cop.Rd].uw = result;
}
