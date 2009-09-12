# -------------------------------------------------
# Project created by QtCreator 2009-09-12T21:30:06
# -------------------------------------------------
QT -= core \
    gui
TARGET = gba
TEMPLATE = lib
CONFIG += staticlib
SOURCES += cemugba.cpp \
    cpu/Sram.cpp \
    cpu/Sound.cpp \
    cpu/RTC.cpp \
    cpu/Mode5.cpp \
    cpu/Mode4.cpp \
    cpu/Mode3.cpp \
    cpu/Mode2.cpp \
    cpu/Mode1.cpp \
    cpu/Mode0.cpp \
    cpu/Globals.cpp \
    cpu/GBA-thumb.cpp \
    cpu/GBAGfx.cpp \
    cpu/GBA-arm.cpp \
    cpu/GBA.cpp \
    cpu/Flash.cpp \
    cpu/elf.cpp \
    cpu/EEprom.cpp \
    cpu/CheatSearch.cpp \
    cpu/Cheats.cpp \
    cpu/bios.cpp \
    cpu/armdis.cpp \
    cpu/agbprint.cpp \
    apu/Multi_Buffer.cpp \
    apu/Gb_Oscs.cpp \
    apu/Gb_Apu_State.cpp \
    apu/Gb_Apu.cpp \
    apu/Effects_Buffer.cpp \
    apu/Blip_Buffer.cpp \
    Util.cpp
HEADERS += cemugba.h \
    cpu/Sram.h \
    cpu/Sound.h \
    cpu/RTC.h \
    cpu/Globals.h \
    cpu/GBAinline.h \
    cpu/GBAGfx.h \
    cpu/GBAcpu.h \
    cpu/GBA.h \
    cpu/Flash.h \
    cpu/elf.h \
    cpu/EEprom.h \
    cpu/CheatSearch.h \
    cpu/Cheats.h \
    cpu/bios.h \
    cpu/armdis.h \
    cpu/agbprint.h \
    apu/Multi_Buffer.h \
    apu/Gb_Oscs.h \
    apu/Gb_Apu.h \
    apu/Effects_Buffer.h \
    apu/Blip_Buffer.h \
    apu/blargg_source.h \
    apu/blargg_config.h \
    apu/blargg_common.h \
    Util.h
