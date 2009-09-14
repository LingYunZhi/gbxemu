#include "stdafx.h"
#include "MainWnd.h"

#include "AccelEditor.h"
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
