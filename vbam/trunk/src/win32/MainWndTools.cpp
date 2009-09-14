#include "stdafx.h"
#include "MainWnd.h"

#include "AccelEditor.h"
#include "FileDlg.h"
#include "MapView.h"
#include "MemoryViewerDlg.h"
#include "OamView.h"
#include "PaletteView.h"
#include "Reg.h"
#include "TileView.h"
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
