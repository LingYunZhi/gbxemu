/*  VisualBoyAdvance 2
    Copyright (C) 2009-2010  VBA development team

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


#include "memory.h"
#include "../common/Port.h"
#include "Sound.h"
#include "GBA.h"
#include <assert.h>

u8 CPUReadByte(u32 address)
{
  switch( address >> 24 ) {
  case 0: {
      if( address < biosChip->SIZE ) {
        u32 value32 = 0;
        if( reg[15].I >> 24 ) { // is this exact enough?
          value32 = biosChip->getLast();
        } else {
          value32 = biosChip->read32( address & 0x3FFC );
        }
        // convert to 8 bit
        const u32 shift = ( address & 3 ) * 8;
        return (u8)( (value32 >> shift) & 0xFF );
      } else goto unreadable;
    }
  case 2:
    return workRAM[address & 0x3FFFF];
  case 3:
    return internalRAM[address & 0x7fff];
  case 4:
    if((address < 0x4000400) && ioReadable[address & 0x3ff])
      return ioMem[address & 0x3ff];
    else goto unreadable;
  case 5:
    return paletteRAM[address & 0x3ff];
  case 6:
    address = (address & 0x1ffff);
    // TODO: is this correct?
    if (((DISPCNT & 7) >2) && ((address & 0x1C000) == 0x18000))
      return 0;
    if ((address & 0x18000) == 0x18000)
      address &= 0x17fff;
    return vram[address];
  case 7:
    return oam[address & 0x3ff];

  case 0x08:
  case 0x09:
  case 0x0A:
  case 0x0B:
  case 0x0C:
  case 0x0D:
  case 0x0E:
  case 0x0F: {
    u8 value;
    if( cartridge->read8( address, value ) ) {
      return value;
    } else goto unreadable;
  }

  default:
  unreadable:
    assert( false );
    if( cpuDmaHack ) {
      return cpuDmaLast & 0xFF;
    } else {
      return 0;
      /*
      if( armState ) {
        return CPUReadByteQuick(reg[15].I+(address & 3)); // verified
      } else {
        return CPUReadByteQuick( reg[15].I + (address & 1) ); // verified
      }
      */
    }
  }
}


u16 CPUReadHalfWord( u32 address )
{
  u16 value;

  switch( address >> 24 ) {
  case 0: {
      if( address < biosChip->SIZE ) {
        u32 value32 = 0;
        if( reg[15].I >> 24 ) { // is this exact enough?
          value32 = biosChip->getLast();
        } else {
          value32 = biosChip->read32( address & 0x3FFC );
        }
        // convert to 16 bit
        if( address & 2 ) {
          value = (u16)((value32>>16) & 0xFFFF);
        } else {
          value = (u16)(value32 & 0xFFFF);
        }
        break;
      } else goto unreadable;
    }
  case 2:
    value = READ16LE(((u16 *)&workRAM[address & 0x3FFFE]));
    break;
  case 3:
    value = READ16LE(((u16 *)&internalRAM[address & 0x7ffe]));
    break;
  case 4:
    if((address < 0x4000400) && ioReadable[address & 0x3fe])
    {
      value =  READ16LE(((u16 *)&ioMem[address & 0x3fe]));
      if (((address & 0x3fe)>0xFF) && ((address & 0x3fe)<0x10E))
      {
        if (((address & 0x3fe) == 0x100) && timer0On)
          value = 0xFFFF - ((timer0Ticks-cpuTotalTicks) >> timer0ClockReload);
        else
          if (((address & 0x3fe) == 0x104) && timer1On && !(TM1CNT & 4))
            value = 0xFFFF - ((timer1Ticks-cpuTotalTicks) >> timer1ClockReload);
          else
            if (((address & 0x3fe) == 0x108) && timer2On && !(TM2CNT & 4))
              value = 0xFFFF - ((timer2Ticks-cpuTotalTicks) >> timer2ClockReload);
            else
              if (((address & 0x3fe) == 0x10C) && timer3On && !(TM3CNT & 4))
                value = 0xFFFF - ((timer3Ticks-cpuTotalTicks) >> timer3ClockReload);
      }
    }
    else goto unreadable;
    break;
  case 5:
    value = READ16LE(((u16 *)&paletteRAM[address & 0x3fe]));
    break;
  case 6:
    address = (address & 0x1fffe);
    if (((DISPCNT & 7) >2) && ((address & 0x1C000) == 0x18000))
    {
      value = 0;
      break;
    }
    if ((address & 0x18000) == 0x18000)
      address &= 0x17fff;
    value = READ16LE(((u16 *)&vram[address]));
    break;
  case 7:
    value = READ16LE(((u16 *)&oam[address & 0x3fe]));
    break;

  case 0x08:
  case 0x09:
  case 0x0A:
  case 0x0B:
  case 0x0C:
  case 0x0D:
  case 0x0E:
  case 0x0F:
    if( cartridge->read16( address, value ) ) {
      break;
    } else {
      goto unreadable;
    }

  default:
  unreadable:
//    assert( false ); // TODO: find out why BIOS wants to read from 0xBFFFFE0
    if( cpuDmaHack ) {
      value = cpuDmaLast & 0xFFFF;
    } else {
      return 0;
      /*
      if( armState ) {
        value = CPUReadHalfWordQuick( reg[15].I + (address & 2) ); // verified
      } else {
        value = CPUReadHalfWordQuick( reg[15].I ); // verified
      }
      */
    }
  }


  // unaligned read
  if( address & 1) {
    value >>= 8; // verified
  }

  return value;
}


void CPUWriteMemory(u32 address, u32 value)
{
  assert( !(address & 0xF0000000) );


#ifdef GBA_LOGGING
  if(address & 3) {
    if(systemVerbose & VERBOSE_UNALIGNED_MEMORY) {
      log("Unaligned word write: %08x to %08x from %08x\n",
        value,
        address,
        armMode ? armNextPC - 4 : armNextPC - 2);
    }
  }
#endif

  switch(address >> 24) {
  case 0x02:
      WRITE32LE(((u32 *)&workRAM[address & 0x3FFFC]), value);
    break;
  case 0x03:
      WRITE32LE(((u32 *)&internalRAM[address & 0x7ffC]), value);
    break;
  case 0x04:
    if(address < 0x4000400) {
      CPUUpdateRegister((address & 0x3FC), value & 0xFFFF);
      CPUUpdateRegister((address & 0x3FC) + 2, (value >> 16));
    } else goto unwritable;
    break;
  case 0x05:
      WRITE32LE(((u32 *)&paletteRAM[address & 0x3FC]), value);
    break;
  case 0x06:
    address = (address & 0x1fffc);
    if (((DISPCNT & 7) >2) && ((address & 0x1C000) == 0x18000))
      return;
    if ((address & 0x18000) == 0x18000)
      address &= 0x17fff;
      WRITE32LE(((u32 *)&vram[address]), value);
    break;
  case 0x07:
      WRITE32LE(((u32 *)&oam[address & 0x3fc]), value);
    break;

  case 0x08:
  case 0x09:
  case 0x0A:
  case 0x0B:
  case 0x0C:
  case 0x0D:
  case 0x0E:
  case 0x0F:
    if( cartridge->write32( address, value ) ) {
      break;
    } else {
      goto unwritable;
    }

  default:
unwritable:
#ifdef GBA_LOGGING
    if(systemVerbose & VERBOSE_ILLEGAL_WRITE) {
      log("Illegal word write: %08x to %08x from %08x\n",
        value,
        address,
        armMode ? armNextPC - 4 : armNextPC - 2);
    }
#endif
    break;
  }
}


void CPUWriteHalfWord(u32 address, u16 value)
{
  assert( !(address & 0xF0000000) );


#ifdef GBA_LOGGING
  if(address & 1) {
    if(systemVerbose & VERBOSE_UNALIGNED_MEMORY) {
      log("Unaligned halfword write: %04x to %08x from %08x\n",
        value,
        address,
        armMode ? armNextPC - 4 : armNextPC - 2);
    }
  }
#endif

  switch(address >> 24) {
  case 2:
      WRITE16LE(((u16 *)&workRAM[address & 0x3FFFE]),value);
    break;
  case 3:
      WRITE16LE(((u16 *)&internalRAM[address & 0x7ffe]), value);
    break;
  case 4:
    if(address < 0x4000400)
      CPUUpdateRegister(address & 0x3fe, value);
    else goto unwritable;
    break;
  case 5:
      WRITE16LE(((u16 *)&paletteRAM[address & 0x3fe]), value);
    break;
  case 6:
    address = (address & 0x1fffe);
    if (((DISPCNT & 7) >2) && ((address & 0x1C000) == 0x18000))
      return;
    if ((address & 0x18000) == 0x18000)
      address &= 0x17fff;
      WRITE16LE(((u16 *)&vram[address]), value);
    break;
  case 7:
      WRITE16LE(((u16 *)&oam[address & 0x3fe]), value);
    break;
    
  case 0x0D:
    // autodetect EEPROM size
    if( !eepromSizeDetected ) {
      eepromSizeDetected = cartridge->getSave()->detectEEPROMSize( cpuDmaCount );
    }
  case 0x08:
  case 0x09:
  case 0x0A:
  case 0x0B:
  case 0x0C:
  case 0x0E:
  case 0x0F:
    if( cartridge->write16( address, value ) ) {
      break;
    } else {
      goto unwritable;
    }
      
  default:
unwritable:
#ifdef GBA_LOGGING
    if(systemVerbose & VERBOSE_ILLEGAL_WRITE) {
      log("Illegal halfword write: %04x to %08x from %08x\n",
        value,
        address,
        armMode ? armNextPC - 4 : armNextPC - 2);
    }
#endif
    break;
  }
}


void CPUWriteByte(u32 address, u8 b)
{
  assert( !(address & 0xF0000000) );

  switch(address >> 24) {
  case 2:
      workRAM[address & 0x3FFFF] = b;
    break;
  case 3:
      internalRAM[address & 0x7fff] = b;
    break;
  case 4:
    if(address < 0x4000400) {
      switch(address & 0x3FF) {
      case 0x60:
      case 0x61:
      case 0x62:
      case 0x63:
      case 0x64:
      case 0x65:
      case 0x68:
      case 0x69:
      case 0x6c:
      case 0x6d:
      case 0x70:
      case 0x71:
      case 0x72:
      case 0x73:
      case 0x74:
      case 0x75:
      case 0x78:
      case 0x79:
      case 0x7c:
      case 0x7d:
      case 0x80:
      case 0x81:
      case 0x84:
      case 0x85:
      case 0x90:
      case 0x91:
      case 0x92:
      case 0x93:
      case 0x94:
      case 0x95:
      case 0x96:
      case 0x97:
      case 0x98:
      case 0x99:
      case 0x9a:
      case 0x9b:
      case 0x9c:
      case 0x9d:
      case 0x9e:
      case 0x9f:
        soundEvent(address&0xFF, b);
        break;
      case 0x301: // HALTCNT, undocumented
        if(b == 0x80)
          stopState = true;
        holdState = 1;
        holdType = -1;
        cpuNextEvent = cpuTotalTicks;
        break;
      default: // every other register
        u32 lowerBits = address & 0x3fe;
        if(address & 1) {
          CPUUpdateRegister(lowerBits, (READ16LE(&ioMem[lowerBits]) & 0x00FF) | (b << 8));
        } else {
          CPUUpdateRegister(lowerBits, (READ16LE(&ioMem[lowerBits]) & 0xFF00) | b);
        }
      }
      break;
    } else goto unwritable;
    break;
  case 5:
    // no need to switch
    *((u16 *)&paletteRAM[address & 0x3FE]) = (b << 8) | b;
    break;
  case 6:
    address = (address & 0x1fffe);
    if (((DISPCNT & 7) >2) && ((address & 0x1C000) == 0x18000))
      return;
    if ((address & 0x18000) == 0x18000)
      address &= 0x17fff;

    // no need to switch
    // byte writes to OBJ VRAM are ignored
    if ((address) < objTilesAddress[((DISPCNT&7)+1)>>2])
    {
        *((u16 *)&vram[address]) = (b << 8) | b;
    }
    break;
  case 7:
    // no need to switch
    // byte writes to OAM are ignored
    //    *((u16 *)&oam[address & 0x3FE]) = (b << 8) | b;
    break;

  case 0x08:
  case 0x09:
  case 0x0A:
  case 0x0B:
  case 0x0C:
  case 0x0D:
  case 0x0E:
  case 0x0F:
    if( cartridge->write8( address, b ) ) {
      break;
    } else {
      goto unwritable;
    }

  default:
unwritable:
#ifdef GBA_LOGGING
    if(systemVerbose & VERBOSE_ILLEGAL_WRITE) {
      log("Illegal byte write: %02x to %08x from %08x\n",
        b,
        address,
        armMode ? armNextPC - 4 : armNextPC -2 );
    }
#endif
    break;
  }
}


u32 CPUReadMemory( u32 address )
{
  u32 value;

  switch( address >> 24 ) {
  case 0:
    if( address < biosChip->SIZE ) {
      if( reg[15].I >> 24 ) { // is this exact enough?
        value = biosChip->getLast();
      } else {
        value = biosChip->read32( address & 0x3FFC );
      }
      break;
    } else goto unreadable;
  case 2:
    value = READ32LE(((u32 *)&workRAM[address & 0x3FFFC]));
    break;
  case 3:
    value = READ32LE(((u32 *)&internalRAM[address & 0x7ffC]));
    break;
  case 4:
    if((address < 0x4000400) && ioReadable[address & 0x3fc]) {
      if(ioReadable[(address & 0x3fc) + 2])
        value = READ32LE(((u32 *)&ioMem[address & 0x3fC]));
      else
        value = READ16LE(((u16 *)&ioMem[address & 0x3fc]));
    } else goto unreadable;
    break;
  case 5:
    value = READ32LE(((u32 *)&paletteRAM[address & 0x3fC]));
    break;
  case 6:
    address = (address & 0x1fffc);
    if (((DISPCNT & 7) >2) && ((address & 0x1C000) == 0x18000))
    {
      value = 0;
      break;
    }
    if ((address & 0x18000) == 0x18000)
      address &= 0x17fff;
    value = READ32LE(((u32 *)&vram[address]));
    break;
  case 7:
    value = READ32LE(((u32 *)&oam[address & 0x3FC]));
    break;

  case 0x08:
  case 0x09:
  case 0x0A:
  case 0x0B:
  case 0x0C:
  case 0x0D:
  case 0x0E:
  case 0x0F:
    if( cartridge->read32( address, value ) ) {
      break;
    } else {
      goto unreadable;
    }

  default:
  unreadable:
    {
      assert( false ); // this should usually not happen
      if( cpuDmaHack ) {
        value = cpuDmaLast;
      } else {
        return 0;
        // return the last prefetched opcode
        /*
        if( armState ) {
          value = CPUReadMemoryQuick( reg[15].I );
        } else {
          // verified
          const u16 lastOp = CPUReadHalfWordQuick( reg[15].I );
          value = (lastOp<<16) | lastOp; // fill high and low 16 bit with lastOp
        }
        */
      }
    }
  }


  // unaligned read
  const u32 offset = address & 3; // not a multiple of four
  if( offset ) {
    const u32 shift = offset << 3; // 1 -> 8   2 -> 16   3 -> 24
    value = (value >> shift) | (value << (32 - shift));
  }

  return value;
}

