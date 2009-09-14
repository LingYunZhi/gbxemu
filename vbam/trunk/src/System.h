#pragma once

#include "common/Types.h"

class SoundDriver;

struct EmulatedSystem {
  // main emulation function
  void (*emuMain)(int);
  // reset emulator
  void (*emuReset)();
  // clean up memory
  void (*emuCleanUp)();
  // load battery file
  bool (*emuReadBattery)(const char *);
  // write battery file
  bool (*emuWriteBattery)(const char *);
  // load state
  bool (*emuReadState)(const char *);
  // save state
  bool (*emuWriteState)(const char *);
  // emulator update CPSR (ARM only)
  void (*emuUpdateCPSR)();
  // emulator has debugger
  bool emuHasDebugger;
  // clock ticks to emulate
  int emuCount;
};

void log(const char *,...);
extern int systemVerbose;

void systemDrawScreen();
// updates the joystick data
bool systemReadJoypads();
// return information about the given joystick, -1 for default joystick
u32 systemReadJoypad(int);
u32 systemGetClock();
SoundDriver * systemSoundInit();
void systemScreenMessage(const char *);
void systemUpdateMotionSensor();
int  systemGetSensorX();
int  systemGetSensorY();
void systemShowSpeed(int);
void system10Frames(int);
void systemFrame();

extern int systemSpeed;

extern int systemSaveUpdateCounter;
#define SYSTEM_SAVE_UPDATED 30
#define SYSTEM_SAVE_NOT_UPDATED 0
