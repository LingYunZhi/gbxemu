#pragma once

#include "stdafx.h"
#include "VBA.h"


class MainWnd : public CWnd
{
public:
	MainWnd();
	virtual ~MainWnd();

	HCURSOR arrow;
	HACCEL m_hAccelTable;

	bool FileRun();
	void winMouseOn();
	void screenCapture(int captureNumber);
	bool fileOpenSelect();
	void updateSoundChannels(UINT nID);
	bool fileImportGSACodeFile(CString& fileName);
	bool writeSaveGame(const char *name);
	bool loadSaveGame(const char *name);
	CString winLoadFilter(UINT id);
	void winLoadCheatList(const char *name);
	void winLoadCheatListDefault();
	void readBatteryFile();
	void writeBatteryFile();
	bool isDriveRoot(CString& file);
	CString getDirFromFile(CString& file);
	void winSaveCheatList(const char *name);
	void winSaveCheatListDefault();

private:
	bool fileExists( LPCTSTR lpFileName );


	DECLARE_MESSAGE_MAP()

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg LRESULT OnMySysCommand(WPARAM, LPARAM);
	afx_msg void OnUpdateFileLoadGameSlot(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFileSaveGameSlot(CCmdUI *pCmdUI);
	afx_msg void OnUpdateOptionsJoypadAutofire(CCmdUI *pCmdUI);
	afx_msg BOOL OnOptionsJoypadAutofire(UINT nID);
	afx_msg void OnUpdateOptionsJoypadDefault(CCmdUI *pCmdUI);
	afx_msg BOOL OnOptionsJoypadDefault(UINT nID);
	afx_msg void OnUpdateOptionsFilter(CCmdUI *pCmdUI);
	afx_msg BOOL OnOptionsFilter(UINT nID);
	afx_msg void OnUpdateOptionsPriority(CCmdUI *pCmdUI);
	afx_msg BOOL OnOptionsPriority(UINT nID);
	afx_msg void OnUpdateOptionsEmulatorShowSpeed(CCmdUI *pCmdUI);
	afx_msg BOOL OnOptionsEmulatorShowSpeed(UINT nID);
	afx_msg void OnSystemMinimize();
	afx_msg void OnUpdateVideoLayer(CCmdUI* pCmdUI);
	afx_msg BOOL OnVideoLayer(UINT nID);
	afx_msg void OnVideoLayerReset();
	afx_msg BOOL OnOptionVideoSize(UINT nID);
	afx_msg void OnClose();
	afx_msg void OnHelpAbout();
	afx_msg void OnHelpFaq();
	afx_msg void OnFileOpenGBA();
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnFilePause();
	afx_msg void OnUpdateFilePause(CCmdUI* pCmdUI);
	afx_msg void OnFileReset();
	afx_msg void OnUpdateFileReset(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileRecentFreeze(CCmdUI* pCmdUI);
	afx_msg void OnFileRecentReset();
	afx_msg void OnFileRecentFreeze();
	afx_msg void OnFileExit();
	afx_msg void OnFileClose();
	afx_msg void OnUpdateFileClose(CCmdUI* pCmdUI);
	afx_msg void OnFileLoad();
	afx_msg void OnUpdateFileLoad(CCmdUI* pCmdUI);
	afx_msg void OnFileSave();
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnFileImportBatteryfile();
	afx_msg void OnUpdateFileImportBatteryfile(CCmdUI* pCmdUI);
	afx_msg void OnFileImportGamesharkcodefile();
	afx_msg void OnUpdateFileImportGamesharkcodefile(CCmdUI* pCmdUI);
	afx_msg void OnFileExportBatteryfile();
	afx_msg void OnUpdateFileExportBatteryfile(CCmdUI* pCmdUI);
	afx_msg void OnFileScreencapture();
	afx_msg void OnUpdateFileScreencapture(CCmdUI* pCmdUI);
	afx_msg void OnFileRominformation();
	afx_msg void OnUpdateFileRominformation(CCmdUI* pCmdUI);
	afx_msg void OnFileTogglemenu();
	afx_msg void OnUpdateFileTogglemenu(CCmdUI* pCmdUI);
	afx_msg void OnOptionsVideoVsync();
	afx_msg void OnUpdateOptionsVideoVsync(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOptionsVideoX1(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOptionsVideoX2(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOptionsVideoX3(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOptionsVideoX4(CCmdUI* pCmdUI);
	afx_msg void OnOptionsVideoFullscreen();
	afx_msg void OnUpdateOptionsVideoFullscreen(CCmdUI* pCmdUI);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnOptionsVideoFullscreenstretchtofit();
	afx_msg void OnUpdateOptionsVideoFullscreenstretchtofit(CCmdUI* pCmdUI);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnOptionsEmulatorAssociate();
	afx_msg void OnOptionsEmulatorDirectories();
	afx_msg void OnOptionsEmulatorDisablestatusmessages();
	afx_msg void OnUpdateOptionsEmulatorDisablestatusmessages(CCmdUI* pCmdUI);
	afx_msg void OnOptionsEmulatorSynchronize();
	afx_msg void OnUpdateOptionsEmulatorSynchronize(CCmdUI* pCmdUI);
	afx_msg void OnOptionsEmulatorPausewheninactive();
	afx_msg void OnUpdateOptionsEmulatorPausewheninactive(CCmdUI* pCmdUI);
	afx_msg void OnOptionsEmulatorAgbprint();
	afx_msg void OnUpdateOptionsEmulatorAgbprint(CCmdUI* pCmdUI);
	afx_msg void OnOptionsEmulatorRealtimeclock();
	afx_msg void OnUpdateOptionsEmulatorRealtimeclock(CCmdUI* pCmdUI);
	afx_msg void OnOptionsEmulatorSavetypeAutomatic();
	afx_msg void OnUpdateOptionsEmulatorSavetypeAutomatic(CCmdUI* pCmdUI);
	afx_msg void OnOptionsEmulatorSavetypeEeprom();
	afx_msg void OnUpdateOptionsEmulatorSavetypeEeprom(CCmdUI* pCmdUI);
	afx_msg void OnOptionsEmulatorSavetypeSram();
	afx_msg void OnUpdateOptionsEmulatorSavetypeSram(CCmdUI* pCmdUI);
	afx_msg void OnOptionsEmulatorSavetypeFlash();
	afx_msg void OnUpdateOptionsEmulatorSavetypeFlash(CCmdUI* pCmdUI);
	afx_msg void OnOptionsEmulatorSavetypeEepromsensor();
	afx_msg void OnUpdateOptionsEmulatorSavetypeEepromsensor(CCmdUI* pCmdUI);
	afx_msg void OnOptionsEmulatorSavetypeNone();
	afx_msg void OnUpdateOptionsEmulatorSavetypeNone(CCmdUI* pCmdUI);
	afx_msg void OnOptionsEmulatorSavetypeFlash512k();
	afx_msg void OnUpdateOptionsEmulatorSavetypeFlash512k(CCmdUI* pCmdUI);
	afx_msg void OnOptionsEmulatorSavetypeFlash1m();
	afx_msg void OnUpdateOptionsEmulatorSavetypeFlash1m(CCmdUI* pCmdUI);
	afx_msg void OnOptionsEmulatorSavetypeDetectNow();
	afx_msg void OnOptionsSoundChannel1();
	afx_msg void OnUpdateOptionsSoundChannel1(CCmdUI* pCmdUI);
	afx_msg void OnOptionsSoundChannel2();
	afx_msg void OnUpdateOptionsSoundChannel2(CCmdUI* pCmdUI);
	afx_msg void OnOptionsSoundChannel3();
	afx_msg void OnUpdateOptionsSoundChannel3(CCmdUI* pCmdUI);
	afx_msg void OnOptionsSoundChannel4();
	afx_msg void OnUpdateOptionsSoundChannel4(CCmdUI* pCmdUI);
	afx_msg void OnOptionsSoundDirectsounda();
	afx_msg void OnUpdateOptionsSoundDirectsounda(CCmdUI* pCmdUI);
	afx_msg void OnOptionsSoundDirectsoundb();
	afx_msg void OnUpdateOptionsSoundDirectsoundb(CCmdUI* pCmdUI);
	afx_msg void OnOptionsLanguageSystem();
	afx_msg void OnUpdateOptionsLanguageSystem(CCmdUI* pCmdUI);
	afx_msg void OnOptionsLanguageEnglish();
	afx_msg void OnUpdateOptionsLanguageEnglish(CCmdUI* pCmdUI);
	afx_msg void OnOptionsLanguageOther();
	afx_msg void OnUpdateOptionsLanguageOther(CCmdUI* pCmdUI);
	afx_msg void OnOptionsJoypadConfigure1();
	afx_msg void OnUpdateOptionsJoypadConfigure1(CCmdUI* pCmdUI);
	afx_msg void OnOptionsJoypadConfigure2();
	afx_msg void OnUpdateOptionsJoypadConfigure2(CCmdUI* pCmdUI);
	afx_msg void OnOptionsJoypadConfigure3();
	afx_msg void OnUpdateOptionsJoypadConfigure3(CCmdUI* pCmdUI);
	afx_msg void OnOptionsJoypadConfigure4();
	afx_msg void OnUpdateOptionsJoypadConfigure4(CCmdUI* pCmdUI);
	afx_msg void OnOptionsJoypadMotionconfigure();
	afx_msg void OnUpdateOptionsJoypadMotionconfigure(CCmdUI* pCmdUI);
	afx_msg void OnCheatsSearchforcheats();
	afx_msg void OnUpdateCheatsSearchforcheats(CCmdUI* pCmdUI);
	afx_msg void OnCheatsCheatlist();
	afx_msg void OnUpdateCheatsCheatlist(CCmdUI* pCmdUI);
	afx_msg void OnCheatsAutomaticsaveloadcheats();
	afx_msg void OnCheatsLoadcheatlist();
	afx_msg void OnUpdateCheatsLoadcheatlist(CCmdUI* pCmdUI);
	afx_msg void OnCheatsSavecheatlist();
	afx_msg void OnUpdateCheatsSavecheatlist(CCmdUI* pCmdUI);
	afx_msg void OnToolsDisassemble();
	afx_msg void OnUpdateToolsDisassemble(CCmdUI* pCmdUI);
	afx_msg void OnToolsLogging();
	afx_msg void OnUpdateToolsLogging(CCmdUI* pCmdUI);
	afx_msg void OnToolsIoviewer();
	afx_msg void OnUpdateToolsIoviewer(CCmdUI* pCmdUI);
	afx_msg void OnToolsMapview();
	afx_msg void OnUpdateToolsMapview(CCmdUI* pCmdUI);
	afx_msg void OnToolsMemoryviewer();
	afx_msg void OnUpdateToolsMemoryviewer(CCmdUI* pCmdUI);
	afx_msg void OnToolsOamviewer();
	afx_msg void OnUpdateToolsOamviewer(CCmdUI* pCmdUI);
	afx_msg void OnToolsPaletteview();
	afx_msg void OnUpdateToolsPaletteview(CCmdUI* pCmdUI);
	afx_msg void OnToolsTileviewer();
	afx_msg void OnUpdateToolsTileviewer(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCheatsAutomaticsaveloadcheats(CCmdUI* pCmdUI);
	afx_msg void OnOptionsSoundStartrecording();
	afx_msg void OnUpdateOptionsSoundStartrecording(CCmdUI* pCmdUI);
	afx_msg void OnOptionsSoundStoprecording();
	afx_msg void OnUpdateOptionsSoundStoprecording(CCmdUI* pCmdUI);
	afx_msg void OnToolsRecordStartavirecording();
	afx_msg void OnUpdateToolsRecordStartavirecording(CCmdUI* pCmdUI);
	afx_msg void OnToolsRecordStopavirecording();
	afx_msg void OnUpdateToolsRecordStopavirecording(CCmdUI* pCmdUI);
	afx_msg void OnPaint();
	afx_msg void OnToolsCustomize();
	afx_msg void OnUpdateToolsCustomize(CCmdUI* pCmdUI);
	afx_msg void OnHelpBugreport();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnInitMenu(CMenu* pMenu);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnFileSavegameOldestslot();
	afx_msg void OnUpdateFileSavegameOldestslot(CCmdUI* pCmdUI);
	afx_msg void OnFileLoadgameMostrecent();
	afx_msg void OnUpdateFileLoadgameMostrecent(CCmdUI* pCmdUI);
	afx_msg void OnFileLoadgameAutoloadmostrecent();
	afx_msg void OnUpdateFileLoadgameAutoloadmostrecent(CCmdUI* pCmdUI);
	afx_msg void OnCheatsDisablecheats();
	afx_msg void OnUpdateCheatsDisablecheats(CCmdUI* pCmdUI);
	afx_msg void OnOptionsVideoFullscreenmaxscale();
	afx_msg void OnOptionsEmulatorGameoverrides();
	afx_msg void OnUpdateOptionsEmulatorGameoverrides(CCmdUI* pCmdUI);
	afx_msg void OnHelpGnupubliclicense();
	afx_msg BOOL OnFileRecentFile(UINT nID);
	afx_msg BOOL OnFileLoadSlot(UINT nID);
	afx_msg BOOL OnFileSaveSlot(UINT nID);

	afx_msg void OnOutputapiXaudio2config();
	afx_msg void OnUpdateOutputapiXaudio2config(CCmdUI *pCmdUI);

	afx_msg void OnOptionsVideoRenderoptionsD3dbilinear();
	afx_msg void OnUpdateOptionsVideoRenderoptionsD3dbilinear(CCmdUI* pCmdUI);
	afx_msg void OnRenderapiD3dmotionblur();
	afx_msg void OnUpdateRenderapiD3dmotionblur(CCmdUI *pCmdUI);

	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg void OnEmulatorBiosfiles();
	afx_msg void OnNcRButtonDown(UINT nHitTest, CPoint point);

    afx_msg void OnEnterSizeMove();

	afx_msg void OnAudioCoreSettings();
	afx_msg void OnUpdateAudioCoreSettings(CCmdUI *pCmdUI);
};
