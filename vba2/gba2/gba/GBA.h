/*  VisualBoyAdvance 2
    Copyright (C) 2009  VBA development team

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

// compile logging functionality
#define GBA_LOGGING

#include "../System.h"


#ifndef NULL
#define NULL 0
#endif

const u8 SAVE_GAME_VERSION_VBA2 = 20;
const u8 SAVE_GAME_VERSION_CURRENT = SAVE_GAME_VERSION_VBA2;


typedef struct {
  u8 *address;
  u32 mask;
} memoryMap;

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

extern memoryMap map[256];

extern reg_pair reg[45];
extern u8 biosProtected[4];

extern bool N_FLAG;
extern bool Z_FLAG;
extern bool C_FLAG;
extern bool V_FLAG;
extern bool armIrqEnable;
extern bool armState;
extern int armMode;
extern void (*cpuSaveGameFunc)(u32,u8);

// saveDataChanged is true if save data has changed. The new data should be written to a file.
// TODO: reset to false on write
extern bool saveDataChanged;


bool CPUWriteBatteryFile(const char *);
bool CPUReadBatteryFile(const char *);
void CPUCleanUp();
void CPUUpdateRender();
void CPUUpdateRenderBuffers(bool);
bool CPUReadState(const char *);
bool CPUWriteState(const char *);
int CPULoadRom(const u8 *const data, const int size);
void CPUUpdateRegister(u32 address, u16 value);
void CPUInit(const bool useBiosFile = false, const u8 *const data = NULL, const int size = 0);
void CPUReset();
void CPULoop(int);
void CPUCheckDMA(int,int);

extern struct EmulatedSystem GBASystem;

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

#include "EEprom.h"
#include "Flash.h"


// interface to display GBA LCD to user
class CDriver_Graphics;
extern CDriver_Graphics *graphicsDriver;

// interface to receive GBA keypad input from user
class CDriver_Input;
extern CDriver_Input    *inputDriver;

// error codes
enum ERR_CODE {
    ERR_UNSUPPORTED_SGM_VER,
    ERR_CANNOT_LOAD_SGM_WRONG_GAME,
    ERR_SAVE_GAME_NOT_USING_BIOS,
    ERR_SAVE_GAME_USING_BIOS,
    ERR_UNSUPPORTED_SAVE_TYPE,
    ERR_UNSUPPORTED_BIOS_FUNCTION,
    ERR_INVALID_BIOS_FILE_SIZE,
    ERR_ERROR_CREATING_FILE,
    ERR_UNSUPPORTED_ARM_MODE,
    ERR_OUT_OF_MEMORY,
    ERR_UNKNOWN_RTC_COMMAND
};

inline void printErrorMessage( ERR_CODE err ) {} // TODO: implement


// verbose info
extern const u32 VERBOSE_SWI;
extern const u32 VERBOSE_UNALIGNED_MEMORY;
extern const u32 VERBOSE_ILLEGAL_WRITE;
extern const u32 VERBOSE_ILLEGAL_READ;
extern const u32 VERBOSE_DMA0;
extern const u32 VERBOSE_DMA1;
extern const u32 VERBOSE_DMA2;
extern const u32 VERBOSE_DMA3;
extern const u32 VERBOSE_UNDEFINED;
extern const u32 VERBOSE_AGBPRINT;
extern const u32 VERBOSE_SOUNDOUTPUT;

extern u32 systemVerbose;


// constants

// how many cycles one frame takes
extern const u32 cyclesPerFrame;


// shared variables
extern bool cpuSramEnabled;
extern bool cpuFlashEnabled;
extern bool cpuEEPROMEnabled;
extern bool cpuEEPROMSensorEnabled;
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
extern int saveType;
extern bool useBios;
extern bool skipBios;
extern bool syncToAudio;
extern int layerSettings;
extern int layerEnable;

extern u8 *bios;
extern u8 *rom;
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
