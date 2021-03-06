#pragma once

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"

#include "AcceleratorManager.h"
#include "Display.h"
#include "Input.h"
#include "../System.h"

/////////////////////////////////////////////////////////////////////////////
// VBA:
// See VBA.cpp for the implementation of this class
//

enum VIDEO_SIZE{
  VIDEO_1X, VIDEO_2X, VIDEO_3X, VIDEO_4X,
  VIDEO_320x240, VIDEO_640x480, VIDEO_800x600, VIDEO_1024x768, VIDEO_1280x1024,
  VIDEO_OTHER
};

enum IMAGE_TYPE {
  IMAGE_UNKNOWN = -1,
  IMAGE_GBA     = 0,
  //IMAGE_GB      = 1
};


class VBA : public CWinApp
{
 public:
  CMenu m_menu;
  HMENU menu;
  HMENU popup;
  int windowPositionX;
  int windowPositionY;

  int fsWidth;
  int fsHeight;
  int fsColorDepth;
  int fsFrequency;
  int fsAdapter;
  bool fsForceChange;

  int sizeX;
  int sizeY;

  int surfaceSizeX;
  int surfaceSizeY;
  int videoOption;
  bool fullScreenStretch;
  bool disableStatusMessage;
  int showSpeed;
  BOOL showSpeedTransparent;
  int showRenderedFrames;
  bool screenMessage;
  CString screenMessageBuffer;
  DWORD screenMessageTime;
  IDisplay *display;
  IMAGE_TYPE cartridgeType;
  bool soundInitialized;

  bool useBiosFileGBA;
  bool skipBiosFile;
  CString biosFileNameGBA;

  bool active;
  bool paused;
  CString recentFiles[10];
  bool recentFreeze;
  bool syncToVideo;
  bool changingVideoSize;

  UINT32 xa2Device;
  UINT32 xa2BufferCount;
  bool xa2Upmixing;

  bool gpuBilinear;
  bool gpuMotionBlur;

  bool iconic;
  bool dinputKeyFocus;
  bool pauseWhenInactive;
  int languageOption;
  CString languageName;
  HMODULE languageModule;
  int renderedFrames;
  Input *input;
  int joypadDefault;
  int autoFire;
  bool autoFireToggle;
  bool painting;
  int sensorX;
  int sensorY;
  int mouseCounter;
  bool wasPaused;
  bool autoLoadMostRecent;
  int maxScale;
  int romSize;
  VIDEO_SIZE lastWindowed;
  VIDEO_SIZE lastFullscreen;

  CAcceleratorManager winAccelMgr;
  HACCEL hAccel;

  RECT rect;
  RECT dest;

  struct EmulatedSystem emulator;

  CString filename;

  CString wndClass;

 public:
  VBA();
  ~VBA();

  void adjustDestRect();
  void updateMenuBar();
  CString winLoadFilter(UINT id);

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(VBA)
 public:
  virtual BOOL InitInstance();
  virtual BOOL OnIdle(LONG lCount);
  //}}AFX_VIRTUAL

  // Implementation

 public:
  void saveSettings();
  bool initInput();
  HMODULE winLoadLanguage(const char *name);
  void winSetLanguageOption(int option, bool force);
  void shutdownDisplay();
  bool preInitialize();
  bool updateRenderMethod0(bool force);
  bool updateRenderMethod(bool force);
  bool initDisplay();
  void updateWindowSize(int value);
  void updateVideoSize(UINT id);
  void loadSettings();
  void addRecentFile(CString file);
};

    extern VBA theApp;
	extern int emulating;
