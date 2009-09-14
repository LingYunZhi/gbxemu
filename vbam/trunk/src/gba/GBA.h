#pragma once

// compile logging functionality
#define GBA_LOGGING

#include "../System.h"
#include <stdio.h> // for NULL

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


bool CPUWriteBatteryFile(const char *);
bool CPUReadBatteryFile(const char *);
bool CPUExportEepromFile(const char *);
bool CPUImportEepromFile(const char *);
bool CPUWriteBMPFile(const char *);
void CPUCleanUp();
void CPUUpdateRender();
void CPUUpdateRenderBuffers(bool);
bool CPUReadMemState(char *, int);
bool CPUReadState(const char *);
bool CPUWriteMemState(char *, int);
bool CPUWriteState(const char *);
int CPULoadRom(const u8 *const data, const int size);
void CPUUpdateRegister(u32, u16);
void applyTimer ();
void CPUInit(const bool useBiosFile = false, const u8 *const data = NULL, const int size = 0);
void CPUReset();
void CPULoop(int);
void CPUCheckDMA(int,int);
bool CPUIsGBAImage(const char *);
bool CPUIsZipFile(const char *);

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

#include "Globals.h"
#include "EEprom.h"
#include "Flash.h"


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

void printErrorMessage( ERR_CODE err );
