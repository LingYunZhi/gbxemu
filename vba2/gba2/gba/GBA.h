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


#ifndef GBA_H
#define GBA_H


#include "../System.h"

#include "../cartridge.h"
extern Cartridge *cartridge;
extern const u8 *rom;
extern u32 romSize;


#include "../bioschip.h"
extern BiosChip *biosChip;


typedef union {
  struct {
#ifdef WORDS_BIGENDIAN
    u8 B3;
    u8 B2;
    u8 B1;
    u8 B0;
#else
    u8 B0;
    u8 B1;
    u8 B2;
    u8 B3;
#endif
  } B;
  struct {
#ifdef WORDS_BIGENDIAN
    u16 W1;
    u16 W0;
#else
    u16 W0;
    u16 W1;
#endif
  } W;
#ifdef WORDS_BIGENDIAN
  volatile u32 I;
#else
	u32 I;
#endif
} reg_pair;

extern reg_pair reg[45];

extern bool N_FLAG;
extern bool Z_FLAG;
extern bool C_FLAG;
extern bool V_FLAG;
extern bool armIrqEnable;
extern bool armState;
extern int armMode;


void CPUCleanUp();
void CPUUpdateRender();
void CPUUpdateRenderBuffers(bool);
bool CPULoadRom( Cartridge *cart/*ridge*/ );
void CPUUpdateRegister(u32 address, u16 value);
void CPUInit();
void CPUReset();
void CPULoop(int);
void CPUCheckDMA(int,int);

#define R13_IRQ  18
#define R14_IRQ  19
#define SPSR_IRQ 20
#define R13_USR  26
#define R14_USR  27
#define R13_SVC  28
#define R14_SVC  29
#define SPSR_SVC 30
#define R13_ABT  31
#define R14_ABT  32
#define SPSR_ABT 33
#define R13_UND  34
#define R14_UND  35
#define SPSR_UND 36
#define R8_FIQ   37
#define R9_FIQ   38
#define R10_FIQ  39
#define R11_FIQ  40
#define R12_FIQ  41
#define R13_FIQ  42
#define R14_FIQ  43
#define SPSR_FIQ 44

// save game
typedef struct {
  void *address;
  int size;
} variable_desc;


// interface to display GBA LCD to user
class CDriver_Graphics;
extern CDriver_Graphics *graphicsDriver;
extern CDriver_Graphics *graphicsDebugDriver; // a driver to use for debugging

// interface to receive GBA keypad input from user
class CDriver_Input;
extern CDriver_Input    *inputDriver;

// interface for logging warnings
#include "../cverbose.h"
extern CVerbose *verbose;

// how many cycles one frame takes
extern const u32 cyclesPerFrame;


// shared variables
extern u32 cpuDmaCount; // this variable is necessary to be able to autodetect EEPROM size
extern bool eepromSizeDetected; // true if size was detected successfully
extern bool cpuDmaHack;
extern u32 cpuDmaLast;
extern bool timer0On;
extern int timer0Ticks;
extern int cpuTotalTicks;
extern int timer0ClockReload;
extern bool timer1On;
extern int timer1Ticks;
extern int timer1ClockReload;
extern bool timer2On;
extern int timer2Ticks;
extern int timer2ClockReload;
extern bool timer3On;
extern int timer3Ticks;
extern int timer3ClockReload;
extern bool stopState;
extern bool holdState;
extern int holdType;
extern int cpuNextEvent;
extern const u32 objTilesAddress[3];


// globals
extern reg_pair reg[45];
extern bool ioReadable[0x400];
extern bool N_FLAG;
extern bool C_FLAG;
extern bool Z_FLAG;
extern bool V_FLAG;
extern bool armState;
extern bool armIrqEnable;
extern u32 armNextPC;
extern int armMode;
extern int layerSettings;
extern int layerEnable;

extern u8 *internalRAM;
extern u8 *workRAM;
extern u8 *paletteRAM;
extern u8 *vram;
extern u8 *oam;
extern u8 *ioMem;

extern u16 *pix;
extern const unsigned int pix_size;

extern u16 DISPCNT;
extern u16 DISPSTAT;
extern u16 VCOUNT;
extern u16 BG0CNT;
extern u16 BG1CNT;
extern u16 BG2CNT;
extern u16 BG3CNT;
extern u16 BG0HOFS;
extern u16 BG0VOFS;
extern u16 BG1HOFS;
extern u16 BG1VOFS;
extern u16 BG2HOFS;
extern u16 BG2VOFS;
extern u16 BG3HOFS;
extern u16 BG3VOFS;
extern u16 BG2PA;
extern u16 BG2PB;
extern u16 BG2PC;
extern u16 BG2PD;
extern u16 BG2X_L;
extern u16 BG2X_H;
extern u16 BG2Y_L;
extern u16 BG2Y_H;
extern u16 BG3PA;
extern u16 BG3PB;
extern u16 BG3PC;
extern u16 BG3PD;
extern u16 BG3X_L;
extern u16 BG3X_H;
extern u16 BG3Y_L;
extern u16 BG3Y_H;
extern u16 WIN0H;
extern u16 WIN1H;
extern u16 WIN0V;
extern u16 WIN1V;
extern u16 WININ;
extern u16 WINOUT;
extern u16 MOSAIC;
extern u16 BLDMOD;
extern u16 COLEV;
extern u16 COLY;
extern u16 DM0SAD_L;
extern u16 DM0SAD_H;
extern u16 DM0DAD_L;
extern u16 DM0DAD_H;
extern u16 DM0CNT_L;
extern u16 DM0CNT_H;
extern u16 DM1SAD_L;
extern u16 DM1SAD_H;
extern u16 DM1DAD_L;
extern u16 DM1DAD_H;
extern u16 DM1CNT_L;
extern u16 DM1CNT_H;
extern u16 DM2SAD_L;
extern u16 DM2SAD_H;
extern u16 DM2DAD_L;
extern u16 DM2DAD_H;
extern u16 DM2CNT_L;
extern u16 DM2CNT_H;
extern u16 DM3SAD_L;
extern u16 DM3SAD_H;
extern u16 DM3DAD_L;
extern u16 DM3DAD_H;
extern u16 DM3CNT_L;
extern u16 DM3CNT_H;
extern u16 TM0D;
extern u16 TM0CNT;
extern u16 TM1D;
extern u16 TM1CNT;
extern u16 TM2D;
extern u16 TM2CNT;
extern u16 TM3D;
extern u16 TM3CNT;
extern u16 P1;
extern u16 IE;
extern u16 IF;
extern u16 IME;

#endif // GBA_H
