#include "stdafx.h"
#include "MainWnd.h"

#include "AccelEditor.h"
#include "AVIWrite.h"
#include "Disassemble.h"
#include "FileDlg.h"
#include "IOViewer.h"
#include "MapView.h"
#include "MemoryViewerDlg.h"
#include "OamView.h"
#include "PaletteView.h"
#include "Reg.h"
#include "TileView.h"
#include "WavWriter.h"
#include "WinResUtil.h"
#include "Logging.h"

#include "../gba/GBA.h"
#include "../gba/Globals.h"
#include "../gba/Sound.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


void MainWnd::OnToolsDisassemble()
{
  if(theApp.cartridgeType == IMAGE_GBA) {
    Disassemble *dlg = new Disassemble();
    dlg->Create(IDD_DISASSEMBLE, this);
    dlg->ShowWindow(SW_SHOW);
  }
}

void MainWnd::OnUpdateToolsDisassemble(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(theApp.videoOption <= VIDEO_4X);
}

void MainWnd::OnToolsLogging()
{
  toolsLogging();
}

void MainWnd::OnUpdateToolsLogging(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(theApp.videoOption <= VIDEO_4X);
}

void MainWnd::OnToolsIoviewer()
{
  IOViewer *dlg = new IOViewer;
  dlg->Create(IDD_IO_VIEWER,this);
  dlg->ShowWindow(SW_SHOW);
}

void MainWnd::OnUpdateToolsIoviewer(CCmdUI* pCmdUI)
{
  pCmdUI->Enable( (theApp.videoOption <= VIDEO_4X) && (theApp.cartridgeType == IMAGE_GBA));
}

void MainWnd::OnToolsMapview()
{
  if(theApp.cartridgeType == IMAGE_GBA) {
    MapView *dlg = new MapView;
    dlg->Create(IDD_MAP_VIEW, this);
    dlg->ShowWindow(SW_SHOW);
  }
}

void MainWnd::OnUpdateToolsMapview(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(theApp.videoOption <= VIDEO_4X);
}

void MainWnd::OnToolsMemoryviewer()
{
  if(theApp.cartridgeType == IMAGE_GBA) {
    MemoryViewerDlg *dlg = new MemoryViewerDlg;
    dlg->Create(IDD_MEM_VIEWER, this);
    dlg->ShowWindow(SW_SHOW);
  }
}

void MainWnd::OnUpdateToolsMemoryviewer(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(theApp.videoOption <= VIDEO_4X);
}

void MainWnd::OnToolsOamviewer()
{
  if(theApp.cartridgeType == IMAGE_GBA) {
    OamView *dlg = new OamView;
    dlg->Create(IDD_OAM_VIEW, this);
    dlg->ShowWindow(SW_SHOW);
  }
}

void MainWnd::OnUpdateToolsOamviewer(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(theApp.videoOption <= VIDEO_4X);
}

void MainWnd::OnToolsPaletteview()
{
  if(theApp.cartridgeType == IMAGE_GBA) {
    PaletteView *dlg = new PaletteView;
    dlg->Create(IDD_PALETTE_VIEW, this);
    dlg->ShowWindow(SW_SHOW);
  }
}

void MainWnd::OnUpdateToolsPaletteview(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(theApp.videoOption <= VIDEO_4X);
}

void MainWnd::OnToolsTileviewer()
{
  if(theApp.cartridgeType == IMAGE_GBA) {
    TileView *dlg = new TileView;
    dlg->Create(IDD_TILE_VIEWER, this);
    dlg->ShowWindow(SW_SHOW);
  }
}

void MainWnd::OnUpdateToolsTileviewer(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(theApp.videoOption <= VIDEO_4X);
}


void MainWnd::OnOptionsSoundStartrecording()
{
  CString captureBuffer;

  CString capdir = regQueryStringValue("soundRecordDir", NULL);

  if(capdir.IsEmpty())
    capdir = getDirFromFile(theApp.filename);

  CString filter = theApp.winLoadFilter(IDS_FILTER_WAV);
  CString title = winResLoadString(IDS_SELECT_WAV_NAME);

  LPCTSTR exts[] = { ".WAV" };

  FileDlg dlg(this, "", filter, 1, "WAV", exts, capdir, title, true);

  if(dlg.DoModal() == IDCANCEL) {
    return;
  }

  captureBuffer = theApp.soundRecordName =  dlg.GetPathName();
  theApp.soundRecording = true;

  if(dlg.m_ofn.nFileOffset > 0) {
    captureBuffer = captureBuffer.Left(dlg.m_ofn.nFileOffset);
  }

  int len = captureBuffer.GetLength();

  if(len > 3 && captureBuffer[len-1] == '\\')
    captureBuffer = captureBuffer.Left(len-1);
  regSetStringValue("soundRecordDir", captureBuffer);
}

void MainWnd::OnUpdateOptionsSoundStartrecording(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(!theApp.soundRecording);
}

void MainWnd::OnOptionsSoundStoprecording()
{
  if(theApp.soundRecorder) {
    delete theApp.soundRecorder;
    theApp.soundRecorder = NULL;
  }
  theApp.soundRecording = false;
}

void MainWnd::OnUpdateOptionsSoundStoprecording(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(theApp.soundRecording);
}


void MainWnd::OnToolsRecordStartavirecording()
{
	CString captureBuffer;
	CString capdir = regQueryStringValue( "aviRecordDir", NULL );

	if( capdir.IsEmpty() ) {
		capdir = getDirFromFile( theApp.filename );
	}

	CString filter = theApp.winLoadFilter( IDS_FILTER_AVI );
	CString title = winResLoadString( IDS_SELECT_AVI_NAME );

	LPCTSTR exts[] = { ".AVI" };

	FileDlg dlg( this, "", filter, 1, "AVI", exts, capdir, title, true );

	if( dlg.DoModal() == IDCANCEL ) {
		return;
	}

	captureBuffer = theApp.soundRecordName =  dlg.GetPathName();
	theApp.aviRecordName = captureBuffer;
	theApp.aviRecording = true;

	if( dlg.m_ofn.nFileOffset > 0 ) {
		captureBuffer = captureBuffer.Left( dlg.m_ofn.nFileOffset );
	}

	int len = captureBuffer.GetLength();

	if( ( len > 3 ) && captureBuffer[ len - 1 ] == '\\' ) {
		captureBuffer = captureBuffer.Left( len - 1 );
	}

	regSetStringValue( "aviRecordDir", captureBuffer );


	// create AVI file
	bool ret;

	if( theApp.aviRecorder ) {
		delete theApp.aviRecorder;
		theApp.aviRecorder = NULL;
	}
	theApp.aviRecorder = new AVIWrite();

	// create AVI file
	ret = theApp.aviRecorder->CreateAVIFile( theApp.aviRecordName );
	if( !ret ) {
		systemMessage( IDS_AVI_CANNOT_CREATE_AVI, "Cannot create AVI file." );
		delete theApp.aviRecorder;
		theApp.aviRecorder = NULL;
		theApp.aviRecording = false;
		return;
	}

	// add video stream
	ret = theApp.aviRecorder->CreateVideoStream(
		theApp.sizeX,
		theApp.sizeY,
		( systemColorDepth == 32 ) ? 24 : 16,
		60,
		this->GetSafeHwnd()
		);
	if( !ret ) {
		systemMessage( IDS_AVI_CANNOT_CREATE_VIDEO, "Cannot create video stream in AVI file. Make sure the selected codec supports input in RGB24 color space!" );
		delete theApp.aviRecorder;
		theApp.aviRecorder = NULL;
		theApp.aviRecording = false;
		return;
	}

	// add audio stream
	ret = theApp.aviRecorder->CreateAudioStream(
		2,
		soundGetSampleRate(),
		16,
		this->GetSafeHwnd()
		);
	if( !ret ) {
		systemMessage( IDS_AVI_CANNOT_CREATE_AUDIO, "Cannot create audio stream in AVI file." );
		delete theApp.aviRecorder;
		theApp.aviRecorder = NULL;
		theApp.aviRecording = false;
		return;
	}
}


void MainWnd::OnUpdateToolsRecordStartavirecording(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( !theApp.aviRecording && emulating );
}


void MainWnd::OnToolsRecordStopavirecording()
{
	if( theApp.aviRecorder ) {
		delete theApp.aviRecorder;
		theApp.aviRecorder = NULL;
	}
	theApp.aviRecording = false;
}


void MainWnd::OnUpdateToolsRecordStopavirecording(CCmdUI* pCmdUI)
{
  pCmdUI->Enable( theApp.aviRecording );
}

void MainWnd::OnToolsRecordStartmovierecording()
{
  CString captureBuffer;
  CString capdir = regQueryStringValue("movieRecordDir", "");

  if(capdir.IsEmpty())
    capdir = getDirFromFile(theApp.filename);

  CString filter = theApp.winLoadFilter(IDS_FILTER_VMV);
  CString title = winResLoadString(IDS_SELECT_MOVIE_NAME);

  LPCTSTR exts[] = { ".VMV" };

  FileDlg dlg(this, "", filter, 1, "VMV", exts, capdir, title, true);

  if(dlg.DoModal() == IDCANCEL) {
    return;
  }

  CString movieName = dlg.GetPathName();
  captureBuffer = movieName;

  if(dlg.m_ofn.nFileOffset > 0) {
    captureBuffer = captureBuffer.Left(dlg.m_ofn.nFileOffset);
  }

  int len = captureBuffer.GetLength();

  if(len > 3 && captureBuffer[len-1] == '\\')
    captureBuffer = captureBuffer.Left(len-1);

  regSetStringValue("movieRecordDir", captureBuffer);

  theApp.movieFile = fopen(movieName, "wb");

  if(!theApp.movieFile) {
    systemMessage(IDS_CANNOT_OPEN_FILE, "Cannot open file %s",
                  (const char *)movieName);
    return;
  }

  int version = 1;

  fwrite(&version, 1, sizeof(int), theApp.movieFile);

  movieName = movieName.Left(movieName.GetLength()-3) + "VM0";

  if(writeSaveGame(movieName)) {
    theApp.movieFrame = 0;
    theApp.movieLastJoypad = 0;
    theApp.movieRecording = true;
    theApp.moviePlaying = false;
  } else {
    systemMessage(IDS_CANNOT_OPEN_FILE, "Cannot open file %s",
                  (const char *)movieName);
  }
}

void MainWnd::OnUpdateToolsRecordStartmovierecording(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(!theApp.movieRecording);
}

void MainWnd::OnToolsRecordStopmovierecording()
{
  if(theApp.movieRecording) {
    if(theApp.movieFile != NULL) {
      // record the last joypad change so that the correct time can be
      // recorded
      fwrite(&theApp.movieFrame, 1, sizeof(int), theApp.movieFile);
      fwrite(&theApp.movieLastJoypad, 1, sizeof(u32), theApp.movieFile);
      fclose(theApp.movieFile);
      theApp.movieFile = NULL;
    }
    theApp.movieRecording = false;
    theApp.moviePlaying = false;
    theApp.movieLastJoypad = 0;
  }
}

void MainWnd::OnUpdateToolsRecordStopmovierecording(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(theApp.movieRecording);
}

void MainWnd::OnToolsPlayStartmovieplaying()
{
  static bool moviePlayMessage = false;

  if(!moviePlayMessage) {
    moviePlayMessage = true;
    CString msg = winResLoadString(IDS_MOVIE_PLAY);
    CString title = winResLoadString(IDS_CONFIRM_ACTION);
    if(MessageBox(msg,
                  title,
                  MB_OKCANCEL) == IDCANCEL)
      return;
  }

  CString captureBuffer;
  CString capdir = regQueryStringValue("movieRecordDir", "");

  if(capdir.IsEmpty())
    capdir = getDirFromFile(theApp.filename);

  CString filter = theApp.winLoadFilter(IDS_FILTER_VMV);
  CString title = winResLoadString(IDS_SELECT_MOVIE_NAME);

  LPCTSTR exts[] = { ".VMV" };

  FileDlg dlg(this, "", filter, 1, "VMV", exts, capdir, title, false);

  if(dlg.DoModal() == IDCANCEL) {
    return;
  }

  CString movieName = dlg.GetPathName();
  captureBuffer = movieName;

  theApp.movieFile = fopen(movieName, "rb");

  if(!theApp.movieFile) {
    systemMessage(IDS_CANNOT_OPEN_FILE, "Cannot open file %s",
                  (const char *)movieName);
    return;
  }
  int version = 0;
  fread(&version, 1, sizeof(int), theApp.movieFile);
  if(version != 1) {
    systemMessage(IDS_UNSUPPORTED_MOVIE_VERSION,
                  "Unsupported movie version %d.",
                  version);
    fclose(theApp.movieFile);
    theApp.movieFile = NULL;
    return;
  }
  movieName = movieName.Left(movieName.GetLength()-3)+"VM0";
  if(loadSaveGame(movieName)) {
    theApp.moviePlaying = true;
    theApp.movieFrame = 0;
    theApp.moviePlayFrame = 0;
    theApp.movieLastJoypad = 0;
    theApp.movieReadNext();
  } else {
    systemMessage(IDS_CANNOT_OPEN_FILE, "Cannot open file %s",
                  (const char *)movieName);
  }
}

void MainWnd::OnUpdateToolsPlayStartmovieplaying(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(!theApp.moviePlaying);
}

void MainWnd::OnToolsPlayStopmovieplaying()
{
  if(theApp.moviePlaying) {
    if(theApp.movieFile != NULL) {
      fclose(theApp.movieFile);
      theApp.movieFile = NULL;
    }
    theApp.moviePlaying = false;
    theApp.movieLastJoypad = 0;
  }
}

void MainWnd::OnUpdateToolsPlayStopmovieplaying(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(theApp.moviePlaying);
}

void MainWnd::OnToolsCustomize()
{
  AccelEditor dlg;

  if(dlg.DoModal()) {
    theApp.winAccelMgr = dlg.mgr;
    theApp.winAccelMgr.UpdateWndTable();
    theApp.winAccelMgr.Write();
    theApp.winAccelMgr.UpdateMenu(theApp.menu);
  }
}

void MainWnd::OnUpdateToolsCustomize(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(theApp.videoOption != VIDEO_320x240);
}
