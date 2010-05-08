// ARM instructions

#include "gbacpu.h"

void GbaCpu::aDecodeAndExecute() {
    // TODO: check condition field
    // TODO: decode shifter operand to cso and sco
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
        // change CPSR if requested
        if( cop.S ) {
            // TODO: handle Rd==15
            cpsr.n = reg[cop.Rd] >> 31;
            cpsr.z = (reg[cop.Rd] == 0);
            cpsr.c = sco;
            // TODO: is this procedure the same for all data proc insn?
        }
    }
}

// AND{<cond>}{S}  <Rd>, <Rn>, <shifter_operand>
void GbaCpu::aAND() {
    reg[cop.Rd].uw = reg[cop.Rn] & cso;
}

// EOR{<cond>}{S}  <Rd>, <Rn>, <shifter_operand>
void GbaCpu::aEOR() {
    reg[cop.Rd].uw = reg[cop.Rn] ^ cso;
}

void GbaCpu::aSUB() {

}

void GbaCpu::aRSB() {

}

void GbaCpu::aADD() {

}

void GbaCpu::aADC() {

}

void GbaCpu::aSBC() {

}

void GbaCpu::aRSC() {

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
