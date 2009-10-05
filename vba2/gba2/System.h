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
inline u32 systemGetClock() {return 0;}
inline void systemScreenMessage(const char *) {}
inline void systemUpdateMotionSensor() {}
inline int  systemGetSensorX() {return 0;}
inline int  systemGetSensorY() {return 0;}
inline void systemShowSpeed(int) {}
inline void system10Frames(int) {}
inline void systemFrame() {}

#endif // SYSTEM_H
