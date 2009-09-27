#ifndef SYSTEM_H
#define SYSTEM_H

#include "common/Types.h"

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

 // TODO: implement
inline void log(const char *,...) {}

 // TODO: implement all
inline void systemDrawScreen() {}
// updates the joystick data
inline bool systemReadJoypads() {return false;}
// return information about the given joystick, -1 for default joystick
inline u32 systemReadJoypad(int) {return 0;}
inline u32 systemGetClock() {return 0;}
inline void systemScreenMessage(const char *) {}
inline void systemUpdateMotionSensor() {}
inline int  systemGetSensorX() {return 0;}
inline int  systemGetSensorY() {return 0;}
inline void systemShowSpeed(int) {}
inline void system10Frames(int) {}
inline void systemFrame() {}

#endif // SYSTEM_H
