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


inline void GbaCpu::aDecodeAndExecute() {
    // avoid subroutine call when instruction is unconditional
    if( cop.cond != 0xE ) {
        if( !aConditionMet() ) {
            return;
        }
    }

    if( cop.op == 0 ) {
        if( (cop.uw & BIT4) && (cop.uw & BIT7) ) {
            // multiply instructions
            switch( cop.type ) {
            case 0x0: aMUL(); break;
            case 0x1: aMLA(); break;
            case 0x4: aUMULL(); break;
            case 0x5: aUMLAL(); break;
            case 0x6: aSMULL(); break;
            case 0x7: aSMLAL(); break;
            }
        } else {
            // ALU instructions
            aShifterOperand();
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
    } else if( (cop.uw & (BIT25|BIT26|BIT27)) == (BIT25|BIT27) ) {
        aB_BL();
    }
}


inline bool GbaCpu::aConditionMet() {
    switch( cop.cond ) {
    case 0x0:
        // EQ - equal
        return cpsr.z == 1;
    case 0x1:
        // NE - not equal
        return cpsr.z == 0;
    case 0x2:
        // CS/HS - carry set/unsigned higher or same
        return cpsr.c == 1;
    case 0x3:
        // CC/LO - carry clear/unsigned lower
        return cpsr.c == 0;
    case 0x4:
        // MI - minus/negative
        return cpsr.n == 1;
    case 0x5:
        // PL - plus/positive or zero
        return cpsr.n == 0;
    case 0x6:
        // VS - overflow
        return cpsr.v == 1;
    case 0x7:
        // VC - no overflow
        return cpsr.v == 0;

    case 0x8:
        // HI - unsigned higher
        return (cpsr.c == 1) && (cpsr.z == 0);
    case 0x9:
        // LS - unsigned lower or same
        return (cpsr.c == 0) && (cpsr.z == 1);
    case 0xA:
        // GE - signed greather than or equal
        return (cpsr.n == cpsr.v);
    case 0xB:
        // LT - signed less than
        return (cpsr.n != cpsr.v);
    case 0xC:
        // GT - signed greater than
        return (cpsr.z == 0) && (cpsr.n == cpsr.v);
    case 0xD:
        // LE - signed less than or equal
        return (cpsr.z == 1) || (cpsr.n != cpsr.v);

    case 0xE:
        // AL - always (unconditional)
        return true;

    default:
        assert( false );
        return false;
    }
}


inline void GbaCpu::aShifterOperand() {
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
            break;
        case 1: {
                // <Rm>, LSL <Rs>
                const u32 Rs = reg[(cop.uw >> 8) & 0xF].uw & 0xFF;
                if( Rs == 0 ) {
                    shifter_operand.uw = Rm;
                    shifter_carry_out = cpsr.c;
                } else {
                    if( Rs < 32 ) {
                        shifter_operand.uw = Rm << Rs;
                        shifter_carry_out = (Rm >> (32 - Rs)) & 1;
                    } else { // Rs >= 32
                        shifter_operand.uw = 0;
                        if( Rs == 32 ) {
                            shifter_carry_out = Rm & 1;
                        } else { // Rs > 32
                            shifter_carry_out = 0;
                        }
                    }
                }
            }
            break;
        case 2: {
                // <Rm>, LSR #<shift_imm>
                const u32 shift_imm = (cop.uw >> 7) & 0x1F;
                if( shift_imm == 0 ) {
                    shifter_operand.uw = 0;
                    shifter_carry_out = Rm >> 31;
                } else {
                    shifter_operand.uw = Rm >> shift_imm;
                    shifter_carry_out = (Rm >> (shift_imm - 1)) & 1;
                }
            }
            break;
        case 3: {
                // <Rm>, LSR <Rs>
                const u32 Rs = reg[(cop.uw >> 8) & 0xF].uw & 0xFF;
                if( Rs == 0 ) {
                    shifter_operand.uw = Rm;
                    shifter_carry_out = cpsr.c;
                } else {
                    if( Rs < 32 ) {
                        shifter_operand.uw = Rm >> Rs;
                        shifter_carry_out = (Rm >> (Rs - 1)) & 1;
                    } else { // Rs >= 32
                        shifter_operand.uw = 0;
                        if( Rs == 32 ) {
                            shifter_carry_out = Rm >> 31;
                        } else { // Rs > 32
                            shifter_carry_out = 0;
                        }
                    }
                }
            }
            break;
        case 4: {
                // <Rm>, ASR #<shift_imm>
                const u32 shift_imm = (cop.uw >> 7) & 0x1F;
                if( shift_imm == 0 ) {
                    if( Rm >> 31 ) {
                        // Rm.MSB set
                        shifter_operand.uw = 0xFFFFFFFF;
                        shifter_carry_out = 1;
                    } else {
                        // Rm.MSB clear
                        shifter_operand.uw = 0;
                        shifter_carry_out = 0;
                    }
                } else {
                    shifter_operand.uw = ((s32)Rm) >> shift_imm;
                    shifter_carry_out = (Rm >> (shift_imm - 1)) & 1;
                }
            }
            break;
        case 5: {
                // <Rm>, ASR <Rs>
                const u32 Rs = reg[(cop.uw >> 8) & 0xF].uw & 0xFF;
                if( Rs == 0 ) {
                    shifter_operand.uw = Rm;
                    shifter_carry_out = cpsr.c;
                } else {
                    if( Rs < 32 ) {
                        shifter_operand.uw = ((s32)Rm) >> Rs;
                        shifter_carry_out = (Rm >> (Rs - 1)) & 1;
                    } else { // Rs >= 32
                        shifter_carry_out = Rm >> 31;
                        if( Rm >> 31 ) {
                            shifter_operand.uw = 0xFFFFFFFF;
                        } else {
                            shifter_operand.uw = 0;
                        }
                    }
                }

            }
            break;
        case 6: {
                const u32 shift_imm = (cop.uw >> 7) & 0x1F;
                if( shift_imm == 0 ) {
                    // <Rm>, RRX
                    shifter_operand.uw = (((u32)cpsr.c) << 31) | (Rm >> 1);
                    shifter_carry_out = Rm & 1;
                } else {
                    // <Rm>, ROR #<shift_imm>
                    shifter_operand.uw = ROTATE_RIGHT32( Rm, shift_imm );
                    shifter_carry_out = (Rm >> (shift_imm - 1)) & 1;
                }

            }
            break;
        case 7: {
                // <Rm>, ROR <Rs>
                const u32 Rs = reg[(cop.uw >> 8) & 0xF].uw & 0xFF;
                if( Rs == 0 ) {
                    shifter_operand.uw = Rm;
                    shifter_carry_out = cpsr.c;
                } else {
                    const u32 Rs5 = Rs & 0x1F;
                    if( Rs5 == 0 ) {
                        shifter_operand.uw = Rm;
                        shifter_carry_out = Rm >> 31;
                    } else {
                        shifter_operand.uw = ROTATE_RIGHT32( Rm, Rs5 );
                        shifter_carry_out = (Rm >> (Rs5 - 1)) & 1;
                    }
                }

            }
            break;
        }
    }
}


/*
  The result is written back to reg[Rd] AFTER the flag checks took place
  to ensure that operands are not overwritten (in case Rd == Rn).
*/


// bitwise and
// AND{<cond>}{S}  <Rd>, <Rn>, <shifter_operand>
inline void GbaCpu::aAND() {
    const u32 left = reg[cop.Rn].uw;
    const u32 right = shifter_operand.uw;
    const u32 result = left & right;

    if( cop.S ) {
        if( cop.Rd == 15 ) {
            copyCurrentSpsrToCpsr();
        } else {
            cpsr.n = (result >> 31);
            cpsr.z = (result == 0);
            cpsr.c = shifter_carry_out;
        }
    }

    reg[cop.Rd].uw = result;
}


// bitwise exclusive or
// EOR{<cond>}{S}  <Rd>, <Rn>, <shifter_operand>
inline void GbaCpu::aEOR() {
    const u32 left = reg[cop.Rn].uw;
    const u32 right = shifter_operand.uw;
    const u32 result = left ^ right;

    if( cop.S ) {
        if( cop.Rd == 15 ) {
            copyCurrentSpsrToCpsr();
        } else {
            cpsr.n = (result >> 31);
            cpsr.z = (result == 0);
            cpsr.c = shifter_carry_out;
        }
    }

    reg[cop.Rd].uw = result;
}


// subtract
// SUB{<cond>}{S}  <Rd>, <Rn>, <shifter_operand>
inline void GbaCpu::aSUB() {
    const u32 left = reg[cop.Rn].uw;
    const u32 right = shifter_operand.uw;
    const u32 result = left - right;

    if( cop.S ) {
        if( cop.Rd == 15 ) {
            copyCurrentSpsrToCpsr();
        } else {
            cpsr.n = (result >> 31);
            cpsr.z = (result == 0);
            cpsr.c = !(right > left);
            cpsr.v = SIGNED_UNDERFLOW( left, right, result );
        }
    }

    reg[cop.Rd].uw = result;
}


// reverse subtract
// RSB{<cond>}{S}  <Rd>, <Rn>, <shifter_operand>
inline void GbaCpu::aRSB() {
    const u32 left = shifter_operand.uw;
    const u32 right = reg[cop.Rn].uw;
    const u32 result = left - right;

    if( cop.S ) {
        if( cop.Rd == 15 ) {
            copyCurrentSpsrToCpsr();
        } else {
            cpsr.n = (result >> 31);
            cpsr.z = (result == 0);
            cpsr.c = !(right > left);
            cpsr.v = SIGNED_UNDERFLOW( left, right, result );
        }
    }

    reg[cop.Rd].uw = result;
}


// add
// ADD{<cond>}{S}  <Rd>, <Rn>, <shifter_operand>
inline void GbaCpu::aADD() {
    const u32 left = reg[cop.Rn].uw;
    const u32 right = shifter_operand.uw;
    const u32 result = left + right;

    if( cop.S ) {
        if( cop.Rd == 15 ) {
            copyCurrentSpsrToCpsr();
        } else {
            cpsr.n = (result >> 31);
            cpsr.z = (result == 0);
            cpsr.c = (right >= -left);
            cpsr.v = SIGNED_OVERFLOW( left, right, result );
        }
    }

    reg[cop.Rd].uw = result;
}


// add with carry
// ADC{<cond>}{S}  <Rd>, <Rn>, <shifter_operand>
inline void GbaCpu::aADC() {
    const u32 left = reg[cop.Rn].uw;
    const u32 right = shifter_operand.uw;
    const u32 carry = (u32)cpsr.c;
    const u32 temp = left + right;
    const u32 result = temp + carry;

    if( cop.S ) {
        if( cop.Rd == 15 ) {
            copyCurrentSpsrToCpsr();
        } else {
            cpsr.n = (result >> 31);
            cpsr.z = (result == 0);
            cpsr.c = (right >= -left) || (carry && (temp == 0xFFFFFFFF));
            cpsr.v = SIGNED_OVERFLOW( left, right, temp )
                  || SIGNED_OVERFLOW( temp, carry, result );
        }
    }

    reg[cop.Rd].uw = result;
}


// subtract with carry
// SBC{<cond>}{S}  <Rd>, <Rn>, <shifter_operand>
inline void GbaCpu::aSBC() {
    const u32 left = reg[cop.Rn].uw;
    const u32 right = shifter_operand.uw;
    const u32 borrow = (u32)(cpsr.c == 0);
    const u32 temp = left - right;
    const u32 result = temp - borrow;

    if( cop.S ) {
        if( cop.Rd == 15 ) {
            copyCurrentSpsrToCpsr();
        } else {
            cpsr.n = (result >> 31);
            cpsr.z = (result == 0);
            cpsr.c = !( (right > left) || (borrow > temp) );
            cpsr.v = SIGNED_UNDERFLOW( left, right, temp )
                  || SIGNED_UNDERFLOW( temp, borrow, result );
        }
    }

    reg[cop.Rd].uw = result;
}


// reverse subtract with carry
// RSC{<cond>}{S}  <Rd>, <Rn>, <shifter_operand>
inline void GbaCpu::aRSC() {
    const u32 left = shifter_operand.uw;
    const u32 right = reg[cop.Rn].uw;
    const u32 borrow = (u32)(cpsr.c == 0);
    const u32 temp = left - right;
    const u32 result = temp - borrow;

    if( cop.S ) {
        if( cop.Rd == 15 ) {
            copyCurrentSpsrToCpsr();
        } else {
            cpsr.n = (result >> 31);
            cpsr.z = (result == 0);
            cpsr.c = !( (right > left) || (borrow > temp) );
            cpsr.v = SIGNED_UNDERFLOW( left, right, temp )
                  || SIGNED_UNDERFLOW( temp, borrow, result );
        }
    }

    reg[cop.Rd].uw = result;
}


// test bits
// TST{<cond>}  <Rn>, <shifter_operand>
inline void GbaCpu::aTST() {
    const u32 temp = reg[cop.Rn].uw & shifter_operand.uw;

    cpsr.n = (temp >> 31);
    cpsr.z = (temp == 0);
    cpsr.c = shifter_carry_out;
}


// test equivalence
// TEQ{<cond>}  <Rn>, <shifter_operand>
inline void GbaCpu::aTEQ() {
    const u32 temp = reg[cop.Rn].uw ^ shifter_operand.uw;

    cpsr.n = (temp >> 31);
    cpsr.z = (temp == 0);
    cpsr.c = shifter_carry_out;
}


// compare
// CMP{<cond>}  <Rn>, <shifter_operand>
inline void GbaCpu::aCMP() {
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
inline void GbaCpu::aCMN() {
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
inline void GbaCpu::aORR() {
    const u32 left = reg[cop.Rn].uw;
    const u32 right = shifter_operand.uw;
    const u32 result = left | right;

    if( cop.S ) {
        if( cop.Rd == 15 ) {
            copyCurrentSpsrToCpsr();
        } else {
            cpsr.n = (result >> 31);
            cpsr.z = (result == 0);
            cpsr.c = shifter_carry_out;
        }
    }

    reg[cop.Rd].uw = result;
}


// move
// MOV{<cond>}{S}  <Rd>, <shifter_operand>
inline void GbaCpu::aMOV() {
    const u32 result = shifter_operand.uw;

    if( cop.S ) {
        if( cop.Rd == 15 ) {
            copyCurrentSpsrToCpsr();
        } else {
            cpsr.n = (result >> 31);
            cpsr.z = (result == 0);
            cpsr.c = shifter_carry_out;
        }
    }

    reg[cop.Rd].uw = result;
}


// bit clear
// BIC{<cond>}{S}  <Rd>, <Rn>, <shifter_operand>
inline void GbaCpu::aBIC() {
    const u32 left = reg[cop.Rn].uw;
    const u32 right = shifter_operand.uw;
    const u32 result = left & (~right);

    if( cop.S ) {
        if( cop.Rd == 15 ) {
            copyCurrentSpsrToCpsr();
        } else {
            cpsr.n = (result >> 31);
            cpsr.z = (result == 0);
            cpsr.c = shifter_carry_out;
        }
    }

    reg[cop.Rd].uw = result;
}


// move not
// MVN{<cond>}{S}  <Rd>, <shifter_operand>
inline void GbaCpu::aMVN() {
    const u32 result = ~(shifter_operand.uw);

    if( cop.S ) {
        if( cop.Rd == 15 ) {
            copyCurrentSpsrToCpsr();
        } else {
            cpsr.n = (result >> 31);
            cpsr.z = (result == 0);
            cpsr.c = shifter_carry_out;
        }
    }

    reg[cop.Rd].uw = result;
}


// multiply
// MUL{<cond>}{S}  <Rd>, <Rm>, <Rs>
inline void GbaCpu::aMUL() {
    const u32 Rm = reg[cop.uw & 0xF].uw;
    const u32 Rs = reg[(cop.uw >> 8) & 0xF].uw;
    const u32 Rd = (cop.uw >> 16) & 0xF;
    const u32 result = Rm * Rs;
    if( cop.S ) {
        cpsr.n = (result >> 31);
        cpsr.z = (result == 0);
    }
    reg[Rd].uw = result;
}


// multiply accumulate
// MLA{<cond>}{S}  <Rd>, <Rm>, <Rs>, <Rn>
inline void GbaCpu::aMLA() {
    const u32 Rm = reg[cop.uw & 0xF].uw;
    const u32 Rs = reg[(cop.uw >> 8) & 0xF].uw;
    const u32 Rn = reg[(cop.uw >> 12) & 0xF].uw;
    const u32 Rd = (cop.uw >> 16) & 0xF;
    const u32 result = (Rm * Rs) + Rn;
    if( cop.S ) {
        cpsr.n = (result >> 31);
        cpsr.z = (result == 0);
    }
    reg[Rd].uw = result;
}


// unsigned multiply long
// UMULL{<cond>}{S}  <RdLo>, <RdHi>, <Rm>, <Rs>
inline void GbaCpu::aUMULL() {
    const u32 Rm = reg[cop.uw & 0xF].uw;
    const u32 Rs = reg[(cop.uw >> 8) & 0xF].uw;
    const u32 RdLo = (cop.uw >> 12) & 0xF;
    const u32 RdHi = (cop.uw >> 16) & 0xF;
    const u64 result = Rm * Rs;
    if( cop.S ) {
        cpsr.n = (result >> 63);
        cpsr.z = (result == 0);
    }
    reg[RdHi].uw = result >> 32;
    reg[RdLo].uw = result & 0xFFFFFFFF;
}


// unsigned multiply accumulate long
// UMLAL{<cond>}{S}  <RdLo>, <RdHi>, <Rm>, <Rs>
inline void GbaCpu::aUMLAL() {
    const u32 Rm = reg[cop.uw & 0xF].uw;
    const u32 Rs = reg[(cop.uw >> 8) & 0xF].uw;
    const u32 RdLo = (cop.uw >> 12) & 0xF;
    const u32 RdHi = (cop.uw >> 16) & 0xF;
    u64 result = (((u64)reg[RdHi].uw) << 32) | ((u64)reg[RdLo].uw);
    result += ((u64)Rm * (u64)Rs);
    if( cop.S ) {
        cpsr.n = (result >> 63);
        cpsr.z = (result == 0);
    }
    reg[RdHi].uw = result >> 32;
    reg[RdLo].uw = result & 0xFFFFFFFF;
}


// signed multiply long
// SMULL{<cond>}{S}  <RdLo>, <RdHi>, <Rm>, <Rs>
inline void GbaCpu::aSMULL() {
    const s32 Rm = reg[cop.uw & 0xF].sw;
    const s32 Rs = reg[(cop.uw >> 8) & 0xF].sw;
    const u32 RdLo = (cop.uw >> 12) & 0xF;
    const u32 RdHi = (cop.uw >> 16) & 0xF;
    const s64 result = Rm * Rs;
    if( cop.S ) {
        cpsr.n = (result < 0);
        cpsr.z = (result == 0);
    }
    reg[RdHi].uw = ((u64)result) >> 32;
    reg[RdLo].uw = result & 0xFFFFFFFF;
}


// signed multiply accumulate long
inline void GbaCpu::aSMLAL() {
    const s32 Rm = reg[cop.uw & 0xF].sw;
    const s32 Rs = reg[(cop.uw >> 8) & 0xF].sw;
    const u32 RdLo = (cop.uw >> 12) & 0xF;
    const u32 RdHi = (cop.uw >> 16) & 0xF;
    s64 result = (s64)((((u64)reg[RdHi].uw) << 32) | ((u64)reg[RdLo].uw));
    result += ((s64)Rm * (s64)Rs);
    if( cop.S ) {
        cpsr.n = (result < 0);
        cpsr.z = (result == 0);
    }
    reg[RdHi].uw = ((u64)result) >> 32;
    reg[RdLo].uw = result & 0xFFFFFFFF;
}


// branch, branch and link
// B{L}{<cond>}  <target_address>
inline void GbaCpu::aB_BL() {
    const bool L = cop.uw & BIT24; // link
    const s32 signed_immed_24 = ((s32)(cop.uw << 8)) >> 6;
    if( L ) {
        reg[14].uw = reg[15].uw - 4;
    }
    reg[15].sw += signed_immed_24;

}


void GbaCpu::copyCurrentSpsrToCpsr() {
    switch( cpsr.mode ) {
    case MODE_FIQ: cpsr.uw = spsr[0].uw; break;
    case MODE_IRQ: cpsr.uw = spsr[1].uw; break;
    case MODE_SVC: cpsr.uw = spsr[2].uw; break;
    case MODE_ABT: cpsr.uw = spsr[3].uw; break;
    case MODE_UND: cpsr.uw = spsr[4].uw; break;
    }
}
