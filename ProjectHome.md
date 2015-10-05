# VisualBoyAdvance 2 #
A free, open source (GPLv3) GBA emulator.

It runs on Windows, Linux and Mac, because the front-end is completely written with Qt 4.
The code is based on VBA-M, but I already rewrote many parts of the code.

My emulation philosophy is not to create an accurate hardware simulator, but rather an easy-to-use and fast game emulator.

![http://www.gnu.org/graphics/gplv3-127x51.png](http://www.gnu.org/graphics/gplv3-127x51.png)


---


**gbxemu** was originally a minimal version of VisualBoyAdvance with only the GBA core and the SDL interface. It was more like an experiment. I removed it and will not work on it anymore.

Since May 20, 2007, this project contains **dpgmux**, my very own GPL'ed command line multiplexer/demultiplexer for DPG video files, as required by the MoonShell homebrew media application for Nintendo DS. I could not find a specification of the DPG container format, so I took the official encoder's source code (Delphi) as reference. dpgmux is written in Microsoft secure C++ and uses the MediaInfo library for getting information about input streams.

August 21, 2009:
Added **gbaSaveTypeDetect** tool. It scans GBA ROMs by dragging them onto the exe file and creates an output text file with the backup media present in the original cartridge.