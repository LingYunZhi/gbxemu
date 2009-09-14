#include "stdafx.h"
#include "VBA.h"

#include "LangSelect.h"
#include "MainWnd.h"
#include "Reg.h"
#include "resource.h"
#include "WinResUtil.h"
#include "Logging.h"

#include "../System.h"
#include "../gba/GBA.h"
#include "../gba/Globals.h"
#include "../gba/RTC.h"
#include "../gba/Sound.h"
#include "../Util.h"
#include "../common/SoundDriver.h"

#include "../version.h"


extern IDisplay *newDirect3DDisplay();
extern Input *newDirectInput();
extern SoundDriver *newXAudio2_Output();

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int emulating = 0;
int RGB_LOW_BITS_MASK = 0;
int systemSpeed = 0;

int systemVerbose = 0;
int systemSaveUpdateCounter = SYSTEM_SAVE_NOT_UPDATED;
bool soundBufferLow = 0;


/////////////////////////////////////////////////////////////////////////////
// VBA

VBA::VBA()
{
  // COINIT_MULTITHREADED is not supported by SHBrowseForFolder with BIF_USENEWUI
  if( S_OK != CoInitializeEx( NULL, COINIT_APARTMENTTHREADED ) ) {
	  systemMessage( IDS_COM_FAILURE, NULL );
  }

  // ! keep in mind that many of the following values will be really initialized in loadSettings()

  windowPositionX = 0;
  windowPositionY = 0;

  fsAdapter = 0;
  fsWidth = 0;
  fsHeight = 0;
  fsColorDepth = 0;
  fsFrequency = 0;
  fsForceChange = false;
  surfaceSizeX = 0;
  surfaceSizeY = 0;
  sizeX = 0;
  sizeY = 0;
  videoOption = 0;
  fullScreenStretch = false;
  disableStatusMessage = false;
  showSpeed = 0;
  showSpeedTransparent = true;
  showRenderedFrames = 0;
  screenMessage = false;
  screenMessageTime = 0;
  display = NULL;
  menu = NULL;
  popup = NULL;
  cartridgeType = IMAGE_GBA;
  soundInitialized = false;
  useBiosFileGBA = false;
  skipBiosFile = false;
  biosFileNameGBA = _T("");
  active = true;
  paused = false;
  recentFreeze = false;
  syncToVideo = false;
  changingVideoSize = false;

  xa2Device = 0;
  xa2BufferCount = 4;
  xa2Upmixing = false;

  gpuBilinear = false;
  gpuMotionBlur = false;

  iconic = false;
  regEnabled = false;
  pauseWhenInactive = true;
  languageOption = 0;
  languageModule = NULL;
  languageName = "";
  renderedFrames = 0;
  input = NULL;
  joypadDefault = 0;
  autoFire = 0;
  autoFireToggle = false;
  painting = false;
  sensorX = 2047;
  sensorY = 2047;
  mouseCounter = 0;
  wasPaused = false;
  autoLoadMostRecent = false;
  maxScale = 0;
  romSize = 0;
  lastWindowed = VIDEO_3X;
  lastFullscreen = VIDEO_1024x768;

  updateCount = 0;

  systemSaveUpdateCounter = SYSTEM_SAVE_NOT_UPDATED;

  ZeroMemory(&emulator, sizeof(emulator));

  hAccel = NULL;
}

VBA::~VBA()
{
  char winBuffer[2048];

  GetModuleFileName(NULL, winBuffer, 2048);
  char *p = strrchr(winBuffer, '\\');
  if(p)
    *p = 0;

  regInit(winBuffer);

  saveSettings();

  soundPause();
  soundShutdown();

  if(rom != NULL) {
    ((MainWnd *)m_pMainWnd)->writeBatteryFile();
    emulator.emuCleanUp();
  }

  if(input)
    delete input;

  shutdownDisplay();

  CoUninitialize();
}

/////////////////////////////////////////////////////////////////////////////
// The one and only VBA object

VBA theApp;
/////////////////////////////////////////////////////////////////////////////
// VBA initialization

BOOL VBA::InitInstance()
{
  SetRegistryKey(_T("VBA"));

  wndClass = AfxRegisterWndClass(0, LoadCursor(IDC_ARROW), (HBRUSH)GetStockObject(BLACK_BRUSH), LoadIcon(IDI_MAINICON));

  char winBuffer[2048];

  GetModuleFileName(NULL, winBuffer, 2048);
  char *p = strrchr(winBuffer, '\\');
  if(p)
    *p = 0;

  bool force = false;

  if (m_lpCmdLine[0])
  {
    if(__argc > 0) {
      if( 0 == strcmp( __argv[1], "--configpath" ) ) {
        if( __argc > 2 ) {
          strcpy( winBuffer, __argv[2] );
          force = true;
          if( __argc > 3 ) {
            filename = __argv[3];
          }
        }
      } else {
        filename = __argv[1];
      }
    }
  }

  regInit(winBuffer, force);

  loadSettings();

    if(!initDisplay()) {
    if(videoOption >= VIDEO_320x240) {
      regSetDwordValue("video", VIDEO_2X);
    }
    return FALSE;
  }

  
  if(!initInput())
    return FALSE;

  hAccel = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR));

  winAccelMgr.Connect((MainWnd *)m_pMainWnd);

  winAccelMgr.SetRegKey(HKEY_CURRENT_USER, "Software\\Emulators\\VisualBoyAdvance");

  extern void winAccelAddCommands(CAcceleratorManager&);

  winAccelAddCommands(winAccelMgr);

  winAccelMgr.CreateDefaultTable();

  winAccelMgr.Load();

  winAccelMgr.UpdateWndTable();

  winAccelMgr.UpdateMenu(menu);

  if( !filename.IsEmpty() ) {
    if(((MainWnd*)m_pMainWnd)->FileRun())
      emulating = true;
    else
      emulating = false;
  }

  return TRUE;
}

void VBA::adjustDestRect()
{
  POINT point;

  point.x = 0;
  point.y = 0;

  m_pMainWnd->ClientToScreen(&point);
  dest.top = point.y;
  dest.left = point.x;

  point.x = surfaceSizeX;
  point.y = surfaceSizeY;

  m_pMainWnd->ClientToScreen(&point);
  dest.bottom = point.y;
  dest.right = point.x;

  // make sure that dest rect lies in the monitor
  if(videoOption >= VIDEO_320x240) {
    dest.top -= windowPositionY;
    dest.left -= windowPositionX;
    dest.bottom-= windowPositionY;
    dest.right -= windowPositionX;
  }

  if(videoOption > VIDEO_4X) {
	  if(fullScreenStretch) {
		  dest.top = 0;
		  dest.left = 0;
		  dest.right = fsWidth;
		  dest.bottom = fsHeight;
	  } else {
		  int top = (fsHeight - surfaceSizeY) / 2;
		  int left = (fsWidth - surfaceSizeX) / 2;
		  dest.top += top;
		  dest.bottom += top;
		  dest.left += left;
		  dest.right += left;
	  }
  }
}


void VBA::updateMenuBar()
{
  if(menu != NULL) {
    if(m_pMainWnd)
      m_pMainWnd->SetMenu(NULL);
    m_menu.Detach();
    DestroyMenu(menu);
  }

  if(popup != NULL) {
    // force popup recreation if language changed
    DestroyMenu(popup);
    popup = NULL;
  }

  if( ( videoOption >= VIDEO_320x240 ) ) {
	  return;
  }

  m_menu.Attach(winResLoadMenu(MAKEINTRESOURCE(IDR_MENU)));
  menu = (HMENU)m_menu;

  if(m_pMainWnd)
      m_pMainWnd->SetMenu(&m_menu);
}


void log(const char *msg, ...)
{
  CString buffer;
  va_list valist;

  va_start(valist, msg);
  buffer.FormatV(msg, valist);

  toolsLog(buffer);

  va_end(valist);
}

bool systemReadJoypads()
{
  if(theApp.input)
    return theApp.input->readDevices();
  return false;
}

u32 systemReadJoypad(int which)
{
  if(theApp.input)
    return theApp.input->readDevice(which);
  return 0;
}

void systemDrawScreen()
{
  if(theApp.display == NULL)
    return;

  theApp.renderedFrames++;

  if(theApp.updateCount) {
    POSITION pos = theApp.updateList.GetHeadPosition();
    while(pos) {
      IUpdateListener *up = theApp.updateList.GetNext(pos);
      up->update();
    }
  }

  if(!soundBufferLow)
  {
	  theApp.display->render();
  }
  else
	  soundBufferLow = false;

}


u32 systemGetClock()
{
  return GetTickCount();
}

void systemMessage(int number, const char *defaultMsg, ...)
{
  CString buffer;
  va_list valist;
  CString msg = defaultMsg;
  if(number)
    msg = winResLoadString(number);

  va_start(valist, defaultMsg);
  buffer.FormatV(msg, valist);

  AfxGetApp()->m_pMainWnd->MessageBox(buffer, winResLoadString(IDS_ERROR), MB_OK|MB_ICONERROR);

  va_end(valist);
}

void systemShowSpeed(int speed)
{
  systemSpeed = speed;
  theApp.showRenderedFrames = theApp.renderedFrames;
  theApp.renderedFrames = 0;
  if(theApp.videoOption <= VIDEO_4X && theApp.showSpeed) {
    CString buffer;
    if(theApp.showSpeed == 1)
      buffer.Format(VBA_NAME_AND_SUBVERSION "-%3d%%", systemSpeed);
    else
      buffer.Format(VBA_NAME_AND_SUBVERSION "-%3d%%(%d fps)", systemSpeed,
                    theApp.showRenderedFrames);
    
    if( theApp.m_pMainWnd != NULL ) {
        AfxGetApp()->m_pMainWnd->SetWindowText( buffer );
    }
  }
}


void systemFrame()
{
}


void system10Frames(int rate)
{
    if(systemSaveUpdateCounter) {
        if(--systemSaveUpdateCounter <= SYSTEM_SAVE_NOT_UPDATED) {
            ((MainWnd *)theApp.m_pMainWnd)->writeBatteryFile();
            systemSaveUpdateCounter = SYSTEM_SAVE_NOT_UPDATED;
        }
    }

    theApp.wasPaused = false;
}


void systemScreenMessage(const char *msg)
{
  theApp.screenMessage = true;
  theApp.screenMessageTime = GetTickCount();
  theApp.screenMessageBuffer = msg;

  if(theApp.screenMessageBuffer.GetLength() > 40)
    theApp.screenMessageBuffer = theApp.screenMessageBuffer.Left(40);
}

void systemUpdateMotionSensor()
{
  if(theApp.input)
    theApp.input->checkMotionKeys();
}

int systemGetSensorX()
{
  return theApp.sensorX;
}

int systemGetSensorY()
{
  return theApp.sensorY;
}


SoundDriver * systemSoundInit()
{
    SoundDriver * drv = 0;
    soundShutdown();
    drv = newXAudio2_Output();

    return drv;
}


BOOL VBA::OnIdle(LONG lCount)
{
    if( emulating && active && !paused ) {
        emulator.emuMain( emulator.emuCount );
        
        if( mouseCounter ) {
            if( --mouseCounter == 0 ) {
                SetCursor( NULL );
            }
        }
        return TRUE;
    }
    
    return FALSE;
}


void VBA::addRecentFile(CString file)
{
  // Do not change recent list if frozen
  if(recentFreeze)
    return;
  int i = 0;
  for(i = 0; i < 10; i++) {
    if(recentFiles[i].GetLength() == 0)
      break;

    if(recentFiles[i].Compare(file) == 0) {
      if(i == 0)
        return;
      CString p = recentFiles[i];
      for(int j = i; j > 0; j--) {
        recentFiles[j] = recentFiles[j-1];
      }
      recentFiles[0] = p;
      return;
    }
  }
  int num = 0;
  for(i = 0; i < 10; i++) {
    if(recentFiles[i].GetLength() != 0)
      num++;
  }
  if(num == 10) {
    num--;
  }

  for(i = num; i >= 1; i--) {
    recentFiles[i] = recentFiles[i-1];
  }
  recentFiles[0] = file;
}

void VBA::loadSettings()
{
  CString buffer;

  lastFullscreen = (VIDEO_SIZE)regQueryDwordValue("lastFullscreen", VIDEO_1024x768);

  languageOption = regQueryDwordValue("language", 1);
  if(languageOption < 0 || languageOption > 2)
    languageOption = 1;

  buffer = regQueryStringValue("languageName", "");
  if(!buffer.IsEmpty()) {
    languageName = buffer.Left(3);
  } else
    languageName = "";

  winSetLanguageOption(languageOption, true);

  syncToVideo = regQueryDwordValue("syncToVideo", false) ? true : false ;
  syncToAudio = regQueryDwordValue("syncToAudio", 1) ? true : false;
  fullScreenStretch = regQueryDwordValue("stretch", 0) ? true : false;

  videoOption = regQueryDwordValue("video", VIDEO_3X);

  if(videoOption < VIDEO_1X || videoOption > VIDEO_OTHER)
    videoOption = VIDEO_3X;

  fsAdapter = regQueryDwordValue("fsAdapter", 0);
  fsWidth = regQueryDwordValue("fsWidth", 800);
  fsHeight = regQueryDwordValue("fsHeight", 600);
  fsColorDepth = regQueryDwordValue("fsColorDepth", 32);
  fsFrequency = regQueryDwordValue("fsFrequency", 60);

  if(videoOption == VIDEO_OTHER) {
    if(fsWidth < 0 || fsWidth > 4095 || fsHeight < 0 || fsHeight > 4095)
      videoOption = 0;
    if(fsColorDepth != 16 && fsColorDepth != 24 && fsColorDepth != 32)
      videoOption = 0;
  }

  windowPositionX = regQueryDwordValue("windowX", 0);
  if(windowPositionX < 0)
    windowPositionX = 0;
  windowPositionY = regQueryDwordValue("windowY", 0);
  if(windowPositionY < 0)
    windowPositionY = 0;

  useBiosFileGBA = ( regQueryDwordValue("useBiosGBA", 0) == 1 ) ? true : false;

  skipBiosFile = regQueryDwordValue("skipBios", 0) ? true : false;

  buffer = regQueryStringValue("biosFileGBA", "");

  if(!buffer.IsEmpty()) {
    biosFileNameGBA = buffer;
  }

  int res = regQueryDwordValue("soundEnable", 0x30f);
  soundSetEnable(res);

  long soundQuality = regQueryDwordValue("soundQuality", 1);
  soundSetSampleRate(44100 / soundQuality);

  soundSetVolume( (float)(regQueryDwordValue("soundVolume", 100)) / 100.0f );

	soundInterpolation = 1 == regQueryDwordValue( "gbaSoundInterpolation", 1 );
	soundFiltering = (float)regQueryDwordValue( "gbaSoundFiltering", 50 ) / 100.0f;

  gpuBilinear   = ( 1 == regQueryDwordValue("gpuBilinear", 0) )   ? true : false ;
  gpuMotionBlur = ( 1 == regQueryDwordValue("gpuMotionBlur", 0) ) ? true : false;

  disableStatusMessage = regQueryDwordValue("disableStatus", 0) ? true : false;

  showSpeed = regQueryDwordValue("showSpeed", 0);
  if(showSpeed < 0 || showSpeed > 2)
    showSpeed = 0;

  showSpeedTransparent = regQueryDwordValue("showSpeedTransparent", TRUE) ?
    TRUE : FALSE;

  pauseWhenInactive = regQueryDwordValue("pauseWhenInactive", 1) ?
    true : false;

  recentFreeze = regQueryDwordValue("recentFreeze", false) ? true : false;

  switch(videoOption) {
  case VIDEO_320x240:
    fsWidth = 320;
    fsHeight = 240;
    fsColorDepth = 16;
	fsFrequency = 60;
    break;
  case VIDEO_640x480:
    fsWidth = 640;
    fsHeight = 480;
    fsColorDepth = 16;
	fsFrequency = 60;
	break;
  case VIDEO_800x600:
    fsWidth = 800;
    fsHeight = 600;
    fsColorDepth = 16;
	fsFrequency = 60;
	break;
  case VIDEO_1024x768:
    fsWidth = 1024;
    fsHeight = 768;
    fsColorDepth = 16;
	fsFrequency = 60;
	break;
  case VIDEO_1280x1024:
    fsWidth = 1280;
    fsHeight = 1024;
    fsColorDepth = 16;
	fsFrequency = 60;
	break;
  }

  for(int i = 0; i < 10; i++) {
    buffer.Format("recent%d", i);
    char *s = regQueryStringValue(buffer, NULL);
    if(s == NULL)
      break;
    recentFiles[i] = s;
  }

  joypadDefault = regQueryDwordValue("joypadDefault", 0);
  if(joypadDefault < 0 || joypadDefault > 3)
    joypadDefault = 0;

  autoLoadMostRecent = ( 1 == regQueryDwordValue("autoLoadMostRecent", 0) ) ? true : false;

  maxScale = regQueryDwordValue("maxScale", 0);

  xa2Device = regQueryDwordValue( "xa2Device", 0 );
  xa2BufferCount = regQueryDwordValue( "xa2BufferCount", 4 );
  xa2Upmixing = ( 1 == regQueryDwordValue( "xa2Upmixing", 0 ) );
}


void VBA::updateVideoSize(UINT id)
{
  int value = 0;

  switch(id) {
  case ID_OPTIONS_VIDEO_X1:
    value = VIDEO_1X;
    break;
  case ID_OPTIONS_VIDEO_X2:
    value = VIDEO_2X;
    break;
  case ID_OPTIONS_VIDEO_X3:
    value = VIDEO_3X;
    break;
  case ID_OPTIONS_VIDEO_X4:
    value = VIDEO_4X;
    break;
  case ID_OPTIONS_VIDEO_FULLSCREEN320X240:
    value = VIDEO_320x240;
    fsWidth = 320;
    fsHeight = 240;
    fsColorDepth = 16;
    break;
  case ID_OPTIONS_VIDEO_FULLSCREEN640X480:
    value = VIDEO_640x480;
    fsWidth = 640;
    fsHeight = 480;
    fsColorDepth = 16;
    break;
  case ID_OPTIONS_VIDEO_FULLSCREEN800X600:
    value = VIDEO_800x600;
    fsWidth = 800;
    fsHeight = 600;
    fsColorDepth = 16;
    break;
  case ID_OPTIONS_VIDEO_FULLSCREEN1024X768:
    value = VIDEO_1024x768;
    fsWidth = 1024;
    fsHeight = 768;
    fsColorDepth = 32;
    break;
  case ID_OPTIONS_VIDEO_FULLSCREEN1280X1024:
    value = VIDEO_1280x1024;
    fsWidth = 1280;
    fsHeight = 1024;
    fsColorDepth = 32;
    break;
  case ID_OPTIONS_VIDEO_FULLSCREEN:
    value = VIDEO_OTHER;
    break;
  }

  updateWindowSize(value);
}


void VBA::updateWindowSize(int value)
{
  regSetDwordValue("video", value);

  if(value == VIDEO_OTHER) {
    regSetDwordValue("fsWidth", fsWidth);
    regSetDwordValue("fsHeight", fsHeight);
    regSetDwordValue("fsColorDepth", fsColorDepth);
  }

  if(((value >= VIDEO_320x240) &&
      (videoOption != value)) ||
     (videoOption >= VIDEO_320x240 &&
      value <= VIDEO_4X) ||
     fsForceChange) {
    fsForceChange = false;
    changingVideoSize = true;
	if( videoOption <= VIDEO_4X ) {
		lastWindowed = (VIDEO_SIZE)videoOption; // save for when leaving full screen
	} else {
		lastFullscreen = (VIDEO_SIZE)videoOption; // save for when using quick switch to fullscreen
	}
    shutdownDisplay();
    if(input) {
      delete input;
      input = NULL;
    }
    m_pMainWnd->DragAcceptFiles(FALSE);
    CWnd *pWnd = m_pMainWnd;
    m_pMainWnd = NULL;
    pWnd->DestroyWindow();
    delete pWnd;
    videoOption = value;
    if(!initDisplay()) {
      if(videoOption == VIDEO_320x240 ||
         videoOption == VIDEO_640x480 ||
         videoOption == VIDEO_800x600 ||
         videoOption == VIDEO_1024x768 ||
         videoOption == VIDEO_1280x1024 ||
         videoOption == VIDEO_OTHER) {
        regSetDwordValue("video", VIDEO_1X);
      }
      changingVideoSize = false;
      AfxPostQuitMessage(0);
      return;
    }
    if(!initInput()) {
      changingVideoSize = false;
      AfxPostQuitMessage(0);
      return;
    }
    input->checkKeys();


    changingVideoSize = FALSE;
    updateWindowSize(videoOption);
    return;
  }

  sizeX = 240;
  sizeY = 160;

  videoOption = value;

  surfaceSizeX = sizeX;
  surfaceSizeY = sizeY;

  switch(videoOption) {
  case VIDEO_1X:
    surfaceSizeX = sizeX;
    surfaceSizeY = sizeY;
    break;
  case VIDEO_2X:
    surfaceSizeX = sizeX * 2;
    surfaceSizeY = sizeY * 2;
    break;
  case VIDEO_3X:
    surfaceSizeX = sizeX * 3;
    surfaceSizeY = sizeY * 3;
    break;
  case VIDEO_4X:
    surfaceSizeX = sizeX * 4;
    surfaceSizeY = sizeY * 4;
    break;
  case VIDEO_320x240:
  case VIDEO_640x480:
  case VIDEO_800x600:
  case VIDEO_1024x768:
  case VIDEO_1280x1024:
  case VIDEO_OTHER:
    {
      int scaleX = 1;
      int scaleY = 1;
      scaleX = (fsWidth / sizeX);
      scaleY = (fsHeight / sizeY);
      int min = scaleX < scaleY ? scaleX : scaleY;
      if(maxScale)
        min = min > maxScale ? maxScale : min;
      surfaceSizeX = min * sizeX;
      surfaceSizeY = min * sizeY;
      if(fullScreenStretch) {
        surfaceSizeX = fsWidth;
        surfaceSizeY = fsHeight;
      }
    }
    break;
  }

  rect.right = sizeX;
  rect.bottom = sizeY;

  int winSizeX = sizeX;
  int winSizeY = sizeY;

  if(videoOption <= VIDEO_4X) {
    dest.left = 0;
    dest.top = 0;
    dest.right = surfaceSizeX;
    dest.bottom = surfaceSizeY;

    DWORD style = WS_POPUP | WS_VISIBLE;

    style |= WS_OVERLAPPEDWINDOW;

    AdjustWindowRectEx(&dest, style, TRUE, 0); //WS_EX_TOPMOST);

    winSizeX = dest.right-dest.left;
    winSizeY = dest.bottom-dest.top;

      m_pMainWnd->SetWindowPos(0, //HWND_TOPMOST,
                               windowPositionX,
                               windowPositionY,
                               winSizeX,
                               winSizeY,
                               SWP_NOMOVE | SWP_SHOWWINDOW);

	  // content of old seperate 'winCheckMenuBarInfo' function:
      MENUBARINFO info;
      info.cbSize = sizeof(MENUBARINFO);
	  GetMenuBarInfo( theApp.m_pMainWnd->GetSafeHwnd(), OBJID_MENU, 0, &info );
      int menuHeight = GetSystemMetrics(SM_CYMENU); // includes white line
      if((info.rcBar.bottom - info.rcBar.top) > menuHeight) // check for double height menu
	  {
        winSizeY += (info.rcBar.bottom - info.rcBar.top) - menuHeight + 1;
        m_pMainWnd->SetWindowPos(
                                        0, //HWND_TOPMOST,
                                        theApp.windowPositionX,
                                        theApp.windowPositionY,
                                        winSizeX,
                                        winSizeY,
                                        SWP_NOMOVE | SWP_SHOWWINDOW);
      }
  }

  adjustDestRect();

  if(display)
    display->resize(theApp.dest.right-theApp.dest.left, theApp.dest.bottom-theApp.dest.top);

  m_pMainWnd->RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_ALLCHILDREN);
}


bool VBA::initDisplay()
{
  return updateRenderMethod(false);
}


bool VBA::preInitialize()
{
	switch( cartridgeType )
	{
	case IMAGE_GBA:
		sizeX = 240;
		sizeY = 160;
		break;
	}

	switch( videoOption )
	{
	case VIDEO_1X:
		surfaceSizeX = sizeX;
		surfaceSizeY = sizeY;
		break;
	case VIDEO_2X:
		surfaceSizeX = sizeX * 2;
		surfaceSizeY = sizeY * 2;
		break;
	case VIDEO_3X:
		surfaceSizeX = sizeX * 3;
		surfaceSizeY = sizeY * 3;
		break;
	case VIDEO_4X:
		surfaceSizeX = sizeX * 4;
		surfaceSizeY = sizeY * 4;
		break;
	case VIDEO_320x240:
	case VIDEO_640x480:
	case VIDEO_800x600:
	case VIDEO_1024x768:
	case VIDEO_1280x1024:
	case VIDEO_OTHER:
		float scaleX = (float)fsWidth / sizeX;
		float scaleY = (float)fsHeight / sizeY;
		float min = ( scaleX < scaleY ) ? scaleX : scaleY;
		if( fullScreenStretch ) {
			surfaceSizeX = fsWidth;
			surfaceSizeY = fsHeight;
		} else {
			surfaceSizeX = (int)( sizeX * min );
			surfaceSizeY = (int)( sizeY * min );
		}
		break;
	}

	rect.left = 0;
	rect.top = 0;
	rect.right = sizeX;
	rect.bottom = sizeY;

	dest.left = 0;
	dest.top = 0;
	dest.right = surfaceSizeX;
	dest.bottom = surfaceSizeY;


	DWORD style = WS_POPUP | WS_VISIBLE;
	DWORD styleEx = 0;

	if( videoOption <= VIDEO_4X ) {
		style |= WS_OVERLAPPEDWINDOW;
	} else {
		styleEx = 0;
	}

	if( videoOption <= VIDEO_4X ) {
		AdjustWindowRectEx( &dest, style, TRUE, styleEx );
	} else {
		AdjustWindowRectEx( &dest, style, FALSE, styleEx );
	}

	int winSizeX = dest.right-dest.left;
	int winSizeY = dest.bottom-dest.top;

	if( videoOption > VIDEO_4X ) {
		winSizeX = fsWidth;
		winSizeY = fsHeight;
	}

	int x = 0, y = 0;

	if( videoOption <= VIDEO_4X ) {
		x = windowPositionX;
		y = windowPositionY;
	}


	// Create a window
	MainWnd *pWnd = new MainWnd;
	m_pMainWnd = pWnd;

	pWnd->CreateEx(
		styleEx,
		wndClass,
		_T(VBA_NAME_AND_SUBVERSION),
		style,
		x, y,
		winSizeX, winSizeY,
		NULL,
		0
		);

	if( !((HWND)*pWnd) ) {
		// error creating window
		return false;
	}
	pWnd->DragAcceptFiles( TRUE );
	updateMenuBar();
	adjustDestRect();

	return true;
}


bool VBA::updateRenderMethod(bool force)
{
	bool ret = true;

	if( !updateRenderMethod0( force ) ) {
		// fall back to safe configuration
		fsAdapter = 0;
		videoOption = VIDEO_2X;
		ret = updateRenderMethod( true );
	}

	return ret;
}


bool VBA::updateRenderMethod0(bool force)
{
    bool initInput = false;

    if( display != NULL) {
        if( force ) {
            toolsLoggingClose(); // close log dialog
            initInput = true;
            changingVideoSize = true;
            shutdownDisplay();
            if(input) {
                delete input;
                input = NULL;
            }
            CWnd *pWnd = m_pMainWnd;

            m_pMainWnd = NULL;
            pWnd->DragAcceptFiles(FALSE);
            pWnd->DestroyWindow();
            delete pWnd;

            display = NULL;
        }
    } else {
        display = newDirect3DDisplay();
    }

    if( preInitialize() ) {
        if( display->initialize( sizeX, sizeY ) ) {
            if( initInput ) {
                if( !this->initInput() ) {
                    changingVideoSize = false;
                    AfxPostQuitMessage(0);
                    return false;
                }
                input->checkKeys();
                updateMenuBar();
                changingVideoSize = false;
                updateWindowSize(videoOption);

                m_pMainWnd->ShowWindow(SW_SHOW);
                m_pMainWnd->UpdateWindow();
                m_pMainWnd->SetFocus();

                return true;
            } else {
                changingVideoSize = false;
                return true;
            }
        }
    }
    changingVideoSize = false;
    
    return true;
}


void VBA::shutdownDisplay()
{
    if(display != NULL) {
        display->cleanup();
        delete display;
        display = NULL;
    }
}


void VBA::winSetLanguageOption(int option, bool force)
{
  if(((option == languageOption) && option != 2) && !force)
    return;
  switch(option) {
  case 0:
    {
      char lbuffer[10];

      if(GetLocaleInfo(LOCALE_SYSTEM_DEFAULT, LOCALE_SABBREVLANGNAME,
                       lbuffer, 10)) {
        HINSTANCE l = winLoadLanguage(lbuffer);
        if(l == NULL) {
          LCID locIdBase = MAKELCID( MAKELANGID( PRIMARYLANGID( GetSystemDefaultLangID() ), SUBLANG_NEUTRAL ), SORT_DEFAULT );
          if(GetLocaleInfo(locIdBase, LOCALE_SABBREVLANGNAME,
                           lbuffer, 10)) {
            l = winLoadLanguage(lbuffer);
            if(l == NULL) {
              systemMessage(IDS_FAILED_TO_LOAD_LIBRARY,
                            "Failed to load library %s",
                            lbuffer);
              return;
            }
          }
        }
        AfxSetResourceHandle(l);
        if(languageModule != NULL)
#ifdef _AFXDLL
          AfxFreeLibrary( languageModule );
#else
          FreeLibrary( languageModule );
#endif
        languageModule = l;
      } else {
        systemMessage(IDS_FAILED_TO_GET_LOCINFO,
                      "Failed to get locale information");
        return;
      }
    }
    break;
  case 1:
    if(languageModule != NULL)
#ifdef _AFXDLL
      AfxFreeLibrary( languageModule );
#else
      FreeLibrary( languageModule );
#endif
    languageModule = NULL;
    AfxSetResourceHandle(AfxGetInstanceHandle());
    break;
  case 2:
    {
      if(!force) {
        LangSelect dlg;
        if(dlg.DoModal()) {
          HINSTANCE l = winLoadLanguage(languageName);
          if(l == NULL) {
            systemMessage(IDS_FAILED_TO_LOAD_LIBRARY,
                          "Failed to load library %s",
                          languageName);
            return;
          }
          AfxSetResourceHandle(l);
          if(languageModule != NULL)
		  {
#ifdef _AFXDLL
            AfxFreeLibrary( languageModule );
#else
            FreeLibrary( languageModule );
#endif
		  }
          languageModule = l;
        }
      } else {
        if(languageName.IsEmpty())
          return;
        HINSTANCE l = winLoadLanguage(languageName);
        if(l == NULL) {
          systemMessage(IDS_FAILED_TO_LOAD_LIBRARY,
                        "Failed to load library %s",
                        languageName);
          return;
        }
        AfxSetResourceHandle(l);
        if(languageModule != NULL)
          FreeLibrary(languageModule);
        languageModule = l;
      }
    }
    break;
  }
  languageOption = option;
  updateMenuBar();
}

HINSTANCE VBA::winLoadLanguage(const char *name)
{
  CString buffer;

  buffer.Format( _T("vba_%s.dll"), name);

#ifdef _AFXDLL
  HINSTANCE l = AfxLoadLibrary( buffer );
#else
  HMODULE l = LoadLibrary( buffer );
#endif

  if(l == NULL) {
    if(strlen(name) == 3) {
      char buffer2[3];
      buffer2[0] = name[0];
      buffer2[1] = name[1];
      buffer2[2] = 0;
      buffer.Format("vba_%s.dll", buffer2);

#ifdef _AFXDLL
	  return AfxLoadLibrary( buffer );
#else
	  return LoadLibrary( buffer );
#endif
    }
  }
  return l;
}


bool VBA::initInput()
{
  if(input)
    delete input;
  input = newDirectInput();
  if(input->initialize()) {
    input->loadSettings();
    input->checkKeys();
    return true;
  }
  delete input;
  return false;
}

void VBA::winAddUpdateListener(IUpdateListener *l)
{
  updateList.AddTail(l);
  updateCount++;
}

void VBA::winRemoveUpdateListener(IUpdateListener *l)
{
  POSITION pos = updateList.Find(l);
  if(pos) {
    updateList.RemoveAt(pos);
    updateCount--;
    if(updateCount < 0)
      updateCount = 0;
  }
}


CString VBA::winLoadFilter(UINT id)
{
  CString res = winResLoadString(id);
  res.Replace('_','|');

  return res;
}


void VBA::saveSettings()
{
  regSetDwordValue("language", languageOption);

  regSetStringValue("languageName", languageName);

  regSetDwordValue("syncToVideo", syncToVideo);
  regSetDwordValue("syncToAudio", syncToAudio);
  regSetDwordValue("stretch", fullScreenStretch);

  regSetDwordValue("video", videoOption);

  regSetDwordValue("fsAdapter", fsAdapter);
  regSetDwordValue("fsWidth", fsWidth);
  regSetDwordValue("fsHeight", fsHeight);
  regSetDwordValue("fsColorDepth", fsColorDepth);
  regSetDwordValue("fsFrequency", fsFrequency);

  regSetDwordValue("windowX", windowPositionX);
  regSetDwordValue("windowY", windowPositionY);


  regSetDwordValue("useBiosGBA", useBiosFileGBA);

  regSetDwordValue("skipBios", skipBiosFile);

  if(!biosFileNameGBA.IsEmpty())
    regSetStringValue("biosFileGBA", biosFileNameGBA);


  regSetDwordValue("soundEnable", soundGetEnable() & 0x30f);

  regSetDwordValue("soundQuality", 44100 / soundGetSampleRate() );

  regSetDwordValue("soundVolume", (DWORD)(soundGetVolume() * 100.0f));

  regSetDwordValue( "gbaSoundInterpolation", soundInterpolation ? 1 : 0 );
  regSetDwordValue( "gbaSoundFiltering", (DWORD)( soundFiltering * 100.0f ) );

  regSetDwordValue("gpuBilinear", gpuBilinear ? 1 : 0);
  regSetDwordValue("gpuMotionBlur", gpuMotionBlur ? 1 : 0);

  regSetDwordValue("disableStatus", disableStatusMessage);

  regSetDwordValue("showSpeed", showSpeed);

  regSetDwordValue("showSpeedTransparent", showSpeedTransparent);

  regSetDwordValue("recentFreeze", recentFreeze);

  CString buffer;
  for(int i = 0; i < 10; i++) {
    buffer.Format("recent%d", i);
    regSetStringValue(buffer, recentFiles[i]);
  }

  regSetDwordValue("joypadDefault", joypadDefault);
  regSetDwordValue("autoLoadMostRecent", autoLoadMostRecent);
  regSetDwordValue("maxScale", maxScale);
  regSetDwordValue("lastFullscreen", lastFullscreen);
  regSetDwordValue("pauseWhenInactive", pauseWhenInactive);

  regSetDwordValue( "xa2Device", xa2Device );
  regSetDwordValue( "xa2BufferCount", xa2BufferCount );
  regSetDwordValue( "xa2Upmixing", xa2Upmixing ? 1 : 0 );
}
