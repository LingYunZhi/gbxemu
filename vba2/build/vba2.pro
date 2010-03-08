# -------------------------------------------------
# Project created by QtCreator 2009-09-15T13:35:51
# -------------------------------------------------
TARGET = vba2
TEMPLATE = app
QT += opengl \
    multimedia
include( vba2_include.pro )
SOURCES += ../vba2/main.cpp \
    ../vba2/mainwindow.cpp \
    ../gba2/apu/Multi_Buffer.cpp \
    ../gba2/apu/Gb_Oscs.cpp \
    ../gba2/apu/Gb_Apu_State.cpp \
    ../gba2/apu/Gb_Apu.cpp \
    ../gba2/apu/Effects_Buffer.cpp \
    ../gba2/apu/Blip_Buffer.cpp \
    ../gba2/gba/Sound.cpp \
    ../gba2/gba/Mode5.cpp \
    ../gba2/gba/Mode4.cpp \
    ../gba2/gba/Mode3.cpp \
    ../gba2/gba/Mode2.cpp \
    ../gba2/gba/Mode1.cpp \
    ../gba2/gba/Mode0.cpp \
    ../gba2/gba/memory.cpp \
    ../gba2/gba/GBAGfx.cpp \
    ../gba2/gba/GBA.cpp \
    ../gba2/gba/bios.cpp \
    ../gba2/cemugba.cpp \
    ../vba2/paintwidget.cpp \
    ../gba2/gba/ARM7TDMI_THUMB.cpp \
    ../gba2/gba/ARM7TDMI_ARM.cpp \
    ../gba2/graphics/cgbagraphics.cpp \
    ../gba2/graphics/cpicture.cpp \
    ../vba2/cdebugwindow_graphics.cpp \
    ../vba2/sound_output_qt.cpp \
    ../vba2/settings/framedialog.cpp \
    ../vba2/settings/settingssheet_audiooutput.cpp \
    ../vba2/settings/settingssheet_keyboardinput.cpp \
    ../vba2/cappsettings.cpp \
    ../vba2/settings/settingssheet_directories.cpp \
    ../gba2/backupmedia.cpp \
    ../gba2/bioschip.cpp \
    ../vba2/settings/settingssheet_graphicsoutput.cpp \
    ../gba2/cartridge.cpp \
    ../gba2/cartridgerom.cpp \
    ../gba2/cartridgeinfo.cpp
HEADERS += ../vba2/mainwindow.h \
    ../gba2/apu/Multi_Buffer.h \
    ../gba2/apu/Gb_Oscs.h \
    ../gba2/apu/Gb_Apu.h \
    ../gba2/apu/Effects_Buffer.h \
    ../gba2/apu/Blip_Buffer.h \
    ../gba2/apu/blargg_source.h \
    ../gba2/apu/blargg_config.h \
    ../gba2/apu/blargg_common.h \
    ../gba2/common/Types.h \
    ../gba2/common/cdriver_sound.h \
    ../gba2/common/Port.h \
    ../gba2/gba/Sound.h \
    ../gba2/gba/memory.h \
    ../gba2/gba/GBAGfx.h \
    ../gba2/gba/GBA.h \
    ../gba2/gba/bios.h \
    ../gba2/System.h \
    ../gba2/cemugba.h \
    ../gba2/common/cdriver_graphics.h \
    ../vba2/paintwidget.h \
    ../gba2/common/cdriver_input.h \
    ../gba2/gba/ARM7TDMI.h \
    ../gba2/graphics/cgbagraphics.h \
    ../gba2/graphics/cpicture.h \
    ../vba2/cdebugwindow_graphics.h \
    ../vba2/sound_output_qt.h \
    ../vba2/settings/framedialog.h \
    ../vba2/settings/settingssheet_audiooutput.h \
    ../vba2/settings/settingssheet_keyboardinput.h \
    ../vba2/cappsettings.h \
    ../vba2/settings/settingssheet_directories.h \
    ../vba2/settings/settingssheet.h \
    ../gba2/backupmedia.h \
    ../gba2/cverbose.h \
    ../gba2/bioschip.h \
    ../vba2/settings/settingssheet_graphicsoutput.h \
    ../gba2/cartridge.h \
    ../gba2/cartridgerom.h \
    ../gba2/common/ichipmemory.h \
    ../gba2/cartridgeinfo.h \
    ../gba2/common/ichipaccess.h
FORMS += ../vba2/mainwindow.ui \
    ../vba2/settings/framedialog.ui \
    ../vba2/settings/settingssheet_audiooutput.ui \
    ../vba2/settings/settingssheet_keyboardinput.ui \
    ../vba2/settings/settingssheet_directories.ui \
    ../vba2/settings/settingssheet_graphicsoutput.ui
RESOURCES += ../vba2/resource/resource.qrc
RC_FILE = ../vba2/resource/windows.rc
