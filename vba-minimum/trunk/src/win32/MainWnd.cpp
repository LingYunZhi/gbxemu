#include "stdafx.h"
#include "VBA.h"
#include "MainWnd.h"

#include <winsock.h>
#include <shlwapi.h>

#include "FileDlg.h"
#include "Reg.h"
#include "WinResUtil.h"
#include "Logging.h"

#include "../System.h"
#include "../AutoBuild.h"
#include "../gba/GBA.h"
#include "../gba/Flash.h"
#include "../gba/RTC.h"
#include "../gba/Sound.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// MainWnd

MainWnd::MainWnd()
{
  m_hAccelTable = NULL;
  arrow = LoadCursor(NULL, IDC_ARROW);
}

MainWnd::~MainWnd()
{
}

bool MainWnd::fileExists( LPCTSTR lpFileName )
{
	// check if file exists
	return GetFileAttributes( lpFileName ) != INVALID_FILE_ATTRIBUTES;
}

long MainWnd::fileSize( const char *const fileName )
{
    FILE *f = fopen( fileName, "rb" );
    fseek( f, 0, SEEK_END );
    const long size = ftell( f );
    fclose( f );
    return size;
}


BEGIN_MESSAGE_MAP(MainWnd, CWnd)
  //{{AFX_MSG_MAP(MainWnd)
  ON_WM_CLOSE()
  ON_COMMAND(ID_HELP_ABOUT, OnHelpAbout)
  ON_COMMAND(ID_HELP_FAQ, OnHelpFaq)
  ON_COMMAND(ID_FILE_OPEN_GBA, OnFileOpenGBA)
  ON_WM_INITMENUPOPUP()
  ON_COMMAND(ID_FILE_PAUSE, OnFilePause)
  ON_UPDATE_COMMAND_UI(ID_FILE_PAUSE, OnUpdateFilePause)
  ON_COMMAND(ID_FILE_RESET, OnFileReset)
  ON_UPDATE_COMMAND_UI(ID_FILE_RESET, OnUpdateFileReset)
  ON_UPDATE_COMMAND_UI(ID_FILE_RECENT_FREEZE, OnUpdateFileRecentFreeze)
  ON_COMMAND(ID_FILE_RECENT_RESET, OnFileRecentReset)
  ON_COMMAND(ID_FILE_RECENT_FREEZE, OnFileRecentFreeze)
  ON_COMMAND(ID_FILE_EXIT, OnFileExit)
  ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
  ON_UPDATE_COMMAND_UI(ID_FILE_CLOSE, OnUpdateFileClose)
  ON_COMMAND(ID_FILE_LOAD, OnFileLoad)
  ON_UPDATE_COMMAND_UI(ID_FILE_LOAD, OnUpdateFileLoad)
  ON_COMMAND(ID_FILE_SAVE, OnFileSave)
  ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
  ON_COMMAND(ID_FILE_IMPORT_BATTERYFILE, OnFileImportBatteryfile)
  ON_UPDATE_COMMAND_UI(ID_FILE_IMPORT_BATTERYFILE, OnUpdateFileImportBatteryfile)
  ON_COMMAND(ID_FILE_EXPORT_BATTERYFILE, OnFileExportBatteryfile)
  ON_UPDATE_COMMAND_UI(ID_FILE_EXPORT_BATTERYFILE, OnUpdateFileExportBatteryfile)
  ON_COMMAND(ID_FILE_ROMINFORMATION, OnFileRominformation)
  ON_UPDATE_COMMAND_UI(ID_FILE_ROMINFORMATION, OnUpdateFileRominformation)
  ON_COMMAND(ID_FILE_TOGGLEMENU, OnFileTogglemenu)
  ON_UPDATE_COMMAND_UI(ID_FILE_TOGGLEMENU, OnUpdateFileTogglemenu)
  ON_COMMAND(ID_OPTIONS_VIDEO_VSYNC, OnOptionsVideoVsync)
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIDEO_VSYNC, OnUpdateOptionsVideoVsync)
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIDEO_X1, OnUpdateOptionsVideoX1)
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIDEO_X2, OnUpdateOptionsVideoX2)
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIDEO_X3, OnUpdateOptionsVideoX3)
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIDEO_X4, OnUpdateOptionsVideoX4)
  ON_COMMAND(ID_OPTIONS_VIDEO_FULLSCREEN, OnOptionsVideoFullscreen)
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIDEO_FULLSCREEN, OnUpdateOptionsVideoFullscreen)
  ON_WM_MOVING()
  ON_WM_MOVE()
  ON_WM_SIZING()
  ON_WM_SIZE()
  ON_COMMAND(ID_OPTIONS_VIDEO_FULLSCREENSTRETCHTOFIT, OnOptionsVideoFullscreenstretchtofit)
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIDEO_FULLSCREENSTRETCHTOFIT, OnUpdateOptionsVideoFullscreenstretchtofit)
  ON_COMMAND(ID_OPTIONS_VIDEO_RENDEROPTIONS_D3DBILINEAR, OnOptionsVideoRenderoptionsD3dbilinear)
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIDEO_RENDEROPTIONS_D3DBILINEAR, OnUpdateOptionsVideoRenderoptionsD3dbilinear)

  ON_WM_CONTEXTMENU()
  ON_COMMAND(ID_OPTIONS_EMULATOR_DIRECTORIES, OnOptionsEmulatorDirectories)
  ON_COMMAND(ID_OPTIONS_EMULATOR_DISABLESTATUSMESSAGES, OnOptionsEmulatorDisablestatusmessages)
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_EMULATOR_DISABLESTATUSMESSAGES, OnUpdateOptionsEmulatorDisablestatusmessages)
  ON_COMMAND(ID_OPTIONS_EMULATOR_SYNCHRONIZE, OnOptionsEmulatorSynchronize)
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_EMULATOR_SYNCHRONIZE, OnUpdateOptionsEmulatorSynchronize)
  ON_COMMAND(ID_OPTIONS_EMULATOR_PAUSEWHENINACTIVE, OnOptionsEmulatorPausewheninactive)
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_EMULATOR_PAUSEWHENINACTIVE, OnUpdateOptionsEmulatorPausewheninactive)

  ON_COMMAND(ID_OPTIONS_SOUND_CHANNEL1, OnOptionsSoundChannel1)
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_SOUND_CHANNEL1, OnUpdateOptionsSoundChannel1)
  ON_COMMAND(ID_OPTIONS_SOUND_CHANNEL2, OnOptionsSoundChannel2)
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_SOUND_CHANNEL2, OnUpdateOptionsSoundChannel2)
  ON_COMMAND(ID_OPTIONS_SOUND_CHANNEL3, OnOptionsSoundChannel3)
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_SOUND_CHANNEL3, OnUpdateOptionsSoundChannel3)
  ON_COMMAND(ID_OPTIONS_SOUND_CHANNEL4, OnOptionsSoundChannel4)
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_SOUND_CHANNEL4, OnUpdateOptionsSoundChannel4)
  ON_COMMAND(ID_OPTIONS_SOUND_DIRECTSOUNDA, OnOptionsSoundDirectsounda)
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_SOUND_DIRECTSOUNDA, OnUpdateOptionsSoundDirectsounda)
  ON_COMMAND(ID_OPTIONS_SOUND_DIRECTSOUNDB, OnOptionsSoundDirectsoundb)
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_SOUND_DIRECTSOUNDB, OnUpdateOptionsSoundDirectsoundb)
  ON_COMMAND(ID_OPTIONS_LANGUAGE_SYSTEM, OnOptionsLanguageSystem)
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_LANGUAGE_SYSTEM, OnUpdateOptionsLanguageSystem)
  ON_COMMAND(ID_OPTIONS_LANGUAGE_ENGLISH, OnOptionsLanguageEnglish)
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_LANGUAGE_ENGLISH, OnUpdateOptionsLanguageEnglish)
  ON_COMMAND(ID_OPTIONS_LANGUAGE_OTHER, OnOptionsLanguageOther)
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_LANGUAGE_OTHER, OnUpdateOptionsLanguageOther)
  ON_COMMAND(ID_OPTIONS_JOYPAD_CONFIGURE_1, OnOptionsJoypadConfigure1)
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_JOYPAD_CONFIGURE_1, OnUpdateOptionsJoypadConfigure1)
  ON_COMMAND(ID_OPTIONS_JOYPAD_CONFIGURE_2, OnOptionsJoypadConfigure2)
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_JOYPAD_CONFIGURE_2, OnUpdateOptionsJoypadConfigure2)
  ON_COMMAND(ID_OPTIONS_JOYPAD_CONFIGURE_3, OnOptionsJoypadConfigure3)
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_JOYPAD_CONFIGURE_3, OnUpdateOptionsJoypadConfigure3)
  ON_COMMAND(ID_OPTIONS_JOYPAD_CONFIGURE_4, OnOptionsJoypadConfigure4)
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_JOYPAD_CONFIGURE_4, OnUpdateOptionsJoypadConfigure4)
  ON_COMMAND(ID_OPTIONS_JOYPAD_MOTIONCONFIGURE, OnOptionsJoypadMotionconfigure)
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_JOYPAD_MOTIONCONFIGURE, OnUpdateOptionsJoypadMotionconfigure)
  ON_COMMAND(ID_TOOLS_LOGGING, OnToolsLogging)
  ON_UPDATE_COMMAND_UI(ID_TOOLS_LOGGING, OnUpdateToolsLogging)
  ON_WM_PAINT()
  ON_COMMAND(ID_TOOLS_CUSTOMIZE, OnToolsCustomize)
  ON_UPDATE_COMMAND_UI(ID_TOOLS_CUSTOMIZE, OnUpdateToolsCustomize)
  ON_COMMAND(ID_HELP_BUGREPORT, OnHelpBugreport)
  ON_WM_MOUSEMOVE()
  ON_WM_INITMENU()
  ON_WM_ACTIVATE()
  ON_WM_DROPFILES()
  ON_COMMAND(ID_FILE_SAVEGAME_OLDESTSLOT, OnFileSavegameOldestslot)
  ON_UPDATE_COMMAND_UI(ID_FILE_SAVEGAME_OLDESTSLOT, OnUpdateFileSavegameOldestslot)
  ON_COMMAND(ID_FILE_LOADGAME_MOSTRECENT, OnFileLoadgameMostrecent)
  ON_UPDATE_COMMAND_UI(ID_FILE_LOADGAME_MOSTRECENT, OnUpdateFileLoadgameMostrecent)
	ON_COMMAND(ID_FILE_LOADGAME_AUTOLOADMOSTRECENT, OnFileLoadgameAutoloadmostrecent)
	ON_UPDATE_COMMAND_UI(ID_FILE_LOADGAME_AUTOLOADMOSTRECENT, OnUpdateFileLoadgameAutoloadmostrecent)
	ON_COMMAND(ID_OPTIONS_VIDEO_FULLSCREENMAXSCALE, OnOptionsVideoFullscreenmaxscale)
	ON_COMMAND(ID_HELP_GNUPUBLICLICENSE, OnHelpGnupubliclicense)

	//}}AFX_MSG_MAP
  ON_COMMAND_EX_RANGE(ID_FILE_MRU_FILE1, ID_FILE_MRU_FILE10, OnFileRecentFile)
  ON_COMMAND_EX_RANGE(ID_FILE_LOADGAME_SLOT1, ID_FILE_LOADGAME_SLOT10, OnFileLoadSlot)
  ON_COMMAND_EX_RANGE(ID_FILE_SAVEGAME_SLOT1, ID_FILE_SAVEGAME_SLOT10, OnFileSaveSlot)
  ON_UPDATE_COMMAND_UI_RANGE(ID_FILE_LOADGAME_SLOT1, ID_FILE_LOADGAME_SLOT10, OnUpdateFileLoadGameSlot)
  ON_UPDATE_COMMAND_UI_RANGE(ID_FILE_SAVEGAME_SLOT1, ID_FILE_SAVEGAME_SLOT10, OnUpdateFileSaveGameSlot)
  ON_COMMAND_EX_RANGE(ID_OPTIONS_VIDEO_X1, ID_OPTIONS_VIDEO_X4, OnOptionVideoSize)
  ON_COMMAND_EX_RANGE(ID_OPTIONS_VIDEO_LAYERS_BG0, ID_OPTIONS_VIDEO_LAYERS_OBJWIN, OnVideoLayer)
  ON_UPDATE_COMMAND_UI_RANGE(ID_OPTIONS_VIDEO_LAYERS_BG0, ID_OPTIONS_VIDEO_LAYERS_OBJWIN, OnUpdateVideoLayer)
  ON_COMMAND(ID_OPTIONS_VIDEO_LAYERS_RESET, OnVideoLayerReset)
  ON_COMMAND(ID_SYSTEM_MINIMIZE, OnSystemMinimize)
  ON_COMMAND_EX_RANGE(ID_OPTIONS_EMULATOR_SHOWSPEED_NONE, ID_OPTIONS_EMULATOR_SHOWSPEED_TRANSPARENT, OnOptionsEmulatorShowSpeed)
  ON_UPDATE_COMMAND_UI_RANGE(ID_OPTIONS_EMULATOR_SHOWSPEED_NONE, ID_OPTIONS_EMULATOR_SHOWSPEED_TRANSPARENT, OnUpdateOptionsEmulatorShowSpeed)
  ON_COMMAND_EX_RANGE(ID_OPTIONS_JOYPAD_DEFAULTJOYPAD_1, ID_OPTIONS_JOYPAD_DEFAULTJOYPAD_4, OnOptionsJoypadDefault)
  ON_UPDATE_COMMAND_UI_RANGE(ID_OPTIONS_JOYPAD_DEFAULTJOYPAD_1, ID_OPTIONS_JOYPAD_DEFAULTJOYPAD_4, OnUpdateOptionsJoypadDefault)
  ON_COMMAND_EX_RANGE(ID_OPTIONS_JOYPAD_AUTOFIRE_A, ID_OPTIONS_JOYPAD_AUTOFIRE_R, OnOptionsJoypadAutofire)
  ON_UPDATE_COMMAND_UI_RANGE(ID_OPTIONS_JOYPAD_AUTOFIRE_A, ID_OPTIONS_JOYPAD_AUTOFIRE_R, OnUpdateOptionsJoypadAutofire)
  ON_MESSAGE(WM_SYSCOMMAND, OnMySysCommand)
  ON_COMMAND(ID_RENDERAPI_D3DMOTIONBLUR, &MainWnd::OnRenderapiD3dmotionblur)
  ON_UPDATE_COMMAND_UI(ID_RENDERAPI_D3DMOTIONBLUR, &MainWnd::OnUpdateRenderapiD3dmotionblur)
  ON_WM_NCLBUTTONDOWN()
  ON_WM_WINDOWPOSCHANGING()
  ON_COMMAND(ID_EMULATOR_BIOSFILES, &MainWnd::OnEmulatorBiosfiles)
  ON_WM_NCRBUTTONDOWN()

  ON_COMMAND(ID_OUTPUTAPI_XAUDIO2CONFIG, &MainWnd::OnOutputapiXaudio2config)
  ON_UPDATE_COMMAND_UI(ID_OUTPUTAPI_XAUDIO2CONFIG, &MainWnd::OnUpdateOutputapiXaudio2config)
  ON_WM_ENTERSIZEMOVE()
  ON_COMMAND(ID_AUDIO_CORE_SETTINGS, &MainWnd::OnAudioCoreSettings)
  ON_UPDATE_COMMAND_UI(ID_AUDIO_CORE_SETTINGS, &MainWnd::OnUpdateAudioCoreSettings)
  END_MESSAGE_MAP()


  /////////////////////////////////////////////////////////////////////////////
// MainWnd message handlers

void MainWnd::OnClose()
{
  emulating = false;
  CWnd::OnClose();
}

bool MainWnd::FileRun()
{
  // save battery file before we change the filename...
  if(rom != NULL) {
    writeBatteryFile();
    theApp.emulator.emuCleanUp();
    emulating = false;
#ifdef APU_LOGGER_H
    end_apu_log();
#endif
  }
  IMAGE_TYPE type = IMAGE_GBA;

  if(type == IMAGE_UNKNOWN) {
    //printErrorMessage(IDS_UNSUPPORTED_FILE_TYPE,
                  //"Unsupported file type: %s", theApp.filename);
    return false;
  }
  theApp.cartridgeType = type;
  if(type == IMAGE_GBA) {
      long fs = fileSize( theApp.filename );
      if( fs == 0 ) return false;
      FILE *f = fopen( theApp.filename, "rb" );
      if( f == NULL ) return false;
      u8 *data = new u8[fs];
      fread( data, 1, fs, f );
      fclose( f );
    int size = CPULoadRom( data, fs );
    if(!size)
      return false;

    theApp.romSize = size;

    theApp.emulator = GBASystem;
  }

  if(theApp.soundInitialized) {
    if(theApp.cartridgeType == IMAGE_GBA)
      soundReset();
  } else {
	  soundInit();
    theApp.soundInitialized = true;
  }

#ifdef APU_LOGGER_H
  begin_apu_log("apu_log.txt");
#endif

  skipBios = theApp.skipBiosFile;

  if( theApp.useBiosFileGBA && !theApp.biosFileNameGBA.IsEmpty() ) {
      long fs = fileSize( theApp.biosFileNameGBA.GetString() );
      if( fs == 0 ) return false;
      FILE *f = fopen( theApp.biosFileNameGBA.GetString(), "rb" );
      if( f == NULL ) return false;
      u8 *data = new u8[fs];
      fread( data, 1, fs, f );
      fclose( f );
      CPUInit( true, data, fs );
      delete [] data;
  } else {
      CPUInit();
  }

  CPUReset();

  readBatteryFile();

  theApp.addRecentFile(theApp.filename);

  theApp.updateWindowSize(theApp.videoOption);

  emulating = true;

  if(theApp.autoLoadMostRecent)
    OnFileLoadgameMostrecent();

  theApp.renderedFrames = 0;

  toolsClearLog();

  return true;
}

void MainWnd::OnInitMenuPopup(CMenu* pMenu, UINT nIndex, BOOL bSysMenu)
{
  ASSERT(pMenu != NULL);

  CCmdUI state;
  state.m_pMenu = pMenu;
  ASSERT(state.m_pOther == NULL);
  ASSERT(state.m_pParentMenu == NULL);

  // determine if menu is popup in top-level menu and set m_pOther to
  //  it if so (m_pParentMenu == NULL indicates that it is secondary popup)
  HMENU hParentMenu;
  if (AfxGetThreadState()->m_hTrackingMenu == pMenu->m_hMenu)
    state.m_pParentMenu = pMenu;    // parent == child for tracking popup
  else if ((hParentMenu = ::GetMenu(m_hWnd)) != NULL) {
    CWnd* pParent = GetTopLevelParent();
    // child windows don't have menus -- need to go to the top!
    if (pParent != NULL &&
        (hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL) {
      int nIndexMax = ::GetMenuItemCount(hParentMenu);
      for (int nIndex = 0; nIndex < nIndexMax; nIndex++) {
        if (::GetSubMenu(hParentMenu, nIndex) == pMenu->m_hMenu) {
          // when popup is found, m_pParentMenu is containing menu
          state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
          break;
        }
      }
    }
  }

  state.m_nIndexMax = pMenu->GetMenuItemCount();
  for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
       state.m_nIndex++) {
    state.m_nID = pMenu->GetMenuItemID(state.m_nIndex);
    if (state.m_nID == 0)
      continue; // menu separator or invalid cmd - ignore it

    ASSERT(state.m_pOther == NULL);
    ASSERT(state.m_pMenu != NULL);
    if (state.m_nID == (UINT)-1) {
      // possibly a popup menu, route to first item of that popup
      state.m_pSubMenu = pMenu->GetSubMenu(state.m_nIndex);
      if (state.m_pSubMenu == NULL ||
          (state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
          state.m_nID == (UINT)-1) {
        continue;       // first item of popup can't be routed to
      }
      state.DoUpdate(this, FALSE);    // popups are never auto disabled
    } else {
      // normal menu item
      // Auto enable/disable if frame window has 'm_bAutoMenuEnable'
      //    set and command is _not_ a system command.
      state.m_pSubMenu = NULL;
      state.DoUpdate(this, state.m_nID < 0xF000);
    }

    // adjust for menu deletions and additions
    UINT nCount = pMenu->GetMenuItemCount();
    if (nCount < state.m_nIndexMax) {
      state.m_nIndex -= (state.m_nIndexMax - nCount);
      while (state.m_nIndex < nCount &&
             pMenu->GetMenuItemID(state.m_nIndex) == state.m_nID) {
        state.m_nIndex++;
      }
    }
    state.m_nIndexMax = nCount;
  }
}

void MainWnd::OnMoving(UINT fwSide, LPRECT pRect)
{
	CWnd::OnMoving(fwSide, pRect);

	if( emulating ) {
		soundPause();
	}
}

void MainWnd::OnMove(int x, int y)
{
  CWnd::OnMove(x, y);

  if(!theApp.changingVideoSize) {
    if(this) {
      if(!IsIconic()) {
        RECT r;

        GetWindowRect(&r);
        theApp.windowPositionX = r.left;
        theApp.windowPositionY = r.top;
        theApp.adjustDestRect();
        regSetDwordValue("windowX", theApp.windowPositionX);
        regSetDwordValue("windowY", theApp.windowPositionY);
      }
    }
  }
}

void MainWnd::OnSizing(UINT fwSide, LPRECT pRect)
{	// the OnSizing event only occurs in windowed mode
	CWnd::OnSizing(fwSide, pRect);

	// pause sound to prevent low sound buffers
	if( emulating ) {
		soundPause();
	}

	// maintain minimal window size
	RECT size = { 0, 0, theApp.sizeX, theApp.sizeY };
	AdjustWindowRectEx(
		&size,
		WS_POPUP | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
		FALSE,
		0 );
	MENUBARINFO mbi;
	mbi.cbSize = sizeof(MENUBARINFO);
	GetMenuBarInfo( this->GetSafeHwnd(), OBJID_MENU, 0, &mbi );
	const LONG menuHeight = mbi.rcBar.bottom - mbi.rcBar.top + 1;
	// +1 because of that white line, wherever it comes from
	const LONG width = size.right - size.left;
	const LONG height = size.bottom - size.top + menuHeight;
	if( ( pRect->right - pRect->left ) < width ) {
		pRect->right = pRect->left + width;
	}
	if( ( pRect->bottom - pRect->top ) < height ) {
		pRect->bottom = pRect->top + height;
	}
}

void MainWnd::OnSize(UINT nType, int cx, int cy)
{
  CWnd::OnSize(nType, cx, cy);

  bool redraw = ( ( cx < theApp.surfaceSizeX ) || ( cy < theApp.surfaceSizeY ) );

  if(!theApp.changingVideoSize) {
    if(this) {
      if(!IsIconic()) {
        if(theApp.iconic) {
          if(emulating) {
            soundResume();
            theApp.paused = false;
          }
        }
        if(theApp.videoOption <= VIDEO_4X) {
          theApp.surfaceSizeX = cx;
          theApp.surfaceSizeY = cy;
          theApp.adjustDestRect();
          if(theApp.display)
            theApp.display->resize(theApp.dest.right-theApp.dest.left, theApp.dest.bottom-theApp.dest.top);
		  if( redraw && emulating ) {
			  theApp.painting = true;
			  systemDrawScreen();
			  theApp.painting = false;
			  theApp.renderedFrames--;
		  }
        }
      } else {
        if(emulating) {
          if(!theApp.paused) {
            theApp.paused = true;
            soundPause();
          }
        }
        theApp.iconic = true;
      }
    }
  }
}

CString MainWnd::getDirFromFile(CString& file)
{
  CString temp = file;
  int index = temp.ReverseFind('\\');

  if(index != -1) {
    temp = temp.Left(index);
    if(temp.GetLength() == 2 && temp[1] == ':')
      temp += "\\";
  } else {
    temp = "";
  }
  return temp;
}

bool MainWnd::isDriveRoot(CString& file)
{
  if(file.GetLength() == 3) {
    if(file[1] == ':' && file[2] == '\\')
      return true;
  }
  return false;
}

void MainWnd::writeBatteryFile()
{
  CString buffer;
  CString filename;

  int index = theApp.filename.ReverseFind('\\');

  if(index != -1)
    buffer = theApp.filename.Right(theApp.filename.GetLength()-index-1);
  else
    buffer = theApp.filename;

  CString saveDir = regQueryStringValue("batteryDir", NULL);
  if( saveDir[0] == '.' ) {
	  // handle as relative path
	  char baseDir[MAX_PATH+1];
	  GetModuleFileName( NULL, baseDir, MAX_PATH );
	  baseDir[MAX_PATH] = '\0'; // for security reasons
	  PathRemoveFileSpec( baseDir ); // removes the trailing file name and backslash
	  strcat( baseDir, "\\" );
	  strcat( baseDir, saveDir );
	  saveDir = baseDir;
	}

  if(saveDir.IsEmpty())
    saveDir = getDirFromFile(theApp.filename);

  if(isDriveRoot(saveDir))
    filename.Format("%s%s.sav", saveDir, buffer);
  else
    filename.Format("%s\\%s.sav", saveDir, buffer);

  if(theApp.emulator.emuWriteBattery)
    theApp.emulator.emuWriteBattery((const char *)filename);
}


void MainWnd::readBatteryFile()
{
  CString buffer;
  CString filename;

  int index = theApp.filename.ReverseFind('\\');

  if(index != -1)
    buffer = theApp.filename.Right(theApp.filename.GetLength()-index-1);
  else
    buffer = theApp.filename;

  CString saveDir = regQueryStringValue("batteryDir", NULL);
  if( saveDir[0] == '.' ) {
	  // handle as relative path
	  char baseDir[MAX_PATH+1];
	  GetModuleFileName( NULL, baseDir, MAX_PATH );
	  baseDir[MAX_PATH] = '\0'; // for security reasons
	  PathRemoveFileSpec( baseDir ); // removes the trailing file name and backslash
	  strcat( baseDir, "\\" );
	  strcat( baseDir, saveDir );
	  saveDir = baseDir;
	}

  if(saveDir.IsEmpty())
    saveDir = getDirFromFile(theApp.filename);

  if(isDriveRoot(saveDir))
    filename.Format("%s%s.sav", saveDir, buffer);
  else
    filename.Format("%s\\%s.sav", saveDir, buffer);

  bool res = false;

  if(theApp.emulator.emuReadBattery)
    res = theApp.emulator.emuReadBattery(filename);

  if(res)
    systemScreenMessage(winResLoadString(IDS_LOADED_BATTERY));
}

CString MainWnd::winLoadFilter(UINT id)
{
  CString res = winResLoadString(id);
  res.Replace('_','|');

  return res;
}

bool MainWnd::loadSaveGame(const char *name)
{
  if(theApp.emulator.emuReadState)
    return theApp.emulator.emuReadState(name);
  return false;
}

bool MainWnd::writeSaveGame(const char *name)
{
  if(theApp.emulator.emuWriteState)
    return theApp.emulator.emuWriteState(name);
  return false;
}

void MainWnd::OnContextMenu(CWnd* pWnd, CPoint point)
{
  winMouseOn();
}

void MainWnd::OnSystemMinimize()
{
  ShowWindow(SW_SHOWMINIMIZED);
}


bool MainWnd::fileOpenSelect()
{
	//CString initialDir = regQueryStringValue( _T("romdir"), _T(".") );
	CString filter = winLoadFilter( IDS_FILTER_GBAROM );
	CString title = winResLoadString( IDS_SELECT_ROM );
/*
	if( initialDir[0] == '.' ) {
		// handle as relative path
		char baseDir[MAX_PATH+1];
		GetModuleFileName( NULL, baseDir, MAX_PATH );
		baseDir[MAX_PATH] = '\0'; // for security reasons
		PathRemoveFileSpec( baseDir ); // removes the trailing file name and backslash
		strcat( baseDir, "\\" );
		strcat( baseDir, initialDir );
		initialDir = baseDir;
	}
*/
    CFileDialog dlg( TRUE,
                     NULL,
                     NULL,
                     OFN_FILEMUSTEXIST |
                     OFN_HIDEREADONLY,
                     filter,
                     this );

    if( IDOK == dlg.DoModal() ) {
        theApp.filename = dlg.GetPathName();
        return true;
    }

    return false;
}


void MainWnd::OnPaint()
{
  CPaintDC dc(this); // device context for painting

  if(emulating) {
    theApp.painting = true;
    systemDrawScreen();
    theApp.painting = false;
    theApp.renderedFrames--;
  }
}

BOOL MainWnd::PreTranslateMessage(MSG* pMsg)
{
  if (CWnd::PreTranslateMessage(pMsg))
    return TRUE;

  if(pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST) {
    return theApp.hAccel != NULL &&  ::TranslateAccelerator(m_hWnd, theApp.hAccel, pMsg);
  }

  return FALSE;
}


void MainWnd::winMouseOn()
{
  SetCursor(arrow);
  if(theApp.videoOption > VIDEO_4X) {
    theApp.mouseCounter = 10;
  } else
    theApp.mouseCounter = 0;
}

void MainWnd::OnMouseMove(UINT nFlags, CPoint point)
{
  winMouseOn();

  CWnd::OnMouseMove(nFlags, point);
}

void MainWnd::OnInitMenu(CMenu* pMenu)
{
  CWnd::OnInitMenu(pMenu);

  soundPause();
}

void MainWnd::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
  CWnd::OnActivate(nState, pWndOther, bMinimized);

  bool a = (nState == WA_ACTIVE) || (nState == WA_CLICKACTIVE);

  if(a && theApp.input) {
    theApp.active = a;
    theApp.input->activate();
    if(!theApp.paused && emulating) {
      soundResume();
    }
  } else {
    theApp.wasPaused = true;
    if(theApp.pauseWhenInactive) {
      if(emulating) {
        soundPause();
      }
      theApp.active = a;
    }
  }

  if(theApp.paused && emulating)
  {
    theApp.painting = true;
    systemDrawScreen();
    theApp.painting = false;
    theApp.renderedFrames--;
  }
}

void MainWnd::OnDropFiles(HDROP hDropInfo)
{
  char szFile[1024];

  if(DragQueryFile(hDropInfo,
                   0,
                   szFile,
                   1024)) {
    theApp.filename = szFile;
    if(FileRun()) {
      SetForegroundWindow();
      emulating = TRUE;
    } else {
      emulating = FALSE;
      soundPause();
    }
  }
  DragFinish(hDropInfo);
}

LRESULT MainWnd::OnMySysCommand(WPARAM wParam, LPARAM lParam)
{
  if(emulating && !theApp.paused) {
    if((wParam&0xFFF0) == SC_SCREENSAVE || (wParam&0xFFF0) == SC_MONITORPOWER)
      return 0;
  }
  return Default();
}

void MainWnd::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	// pause sound before process is halted
	if( emulating ) {
		soundPause();
	}

	CWnd::OnNcLButtonDown(nHitTest, point);
}

void MainWnd::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	CWnd::OnWindowPosChanging(lpwndpos);

	// pause sound before changing window position/size
	if( emulating ) {
		soundPause();
	}
}

void MainWnd::OnNcRButtonDown(UINT nHitTest, CPoint point)
{
	// pause sound before process is halted
	if( emulating ) {
		soundPause();
	}

	CWnd::OnNcRButtonDown(nHitTest, point);
}

void MainWnd::OnEnterSizeMove()
{
    // The WM_ENTERSIZEMOVE message is sent one time to a window after it enters the moving or sizing modal loop.
    // Causes the emulator to stop when moving or resizing the window.

	if( emulating ) {
        // pause sound before entering DefWindowProc
		soundPause();
	}

    CWnd::OnEnterSizeMove();
}
