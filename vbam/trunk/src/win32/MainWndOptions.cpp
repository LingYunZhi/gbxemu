#include "stdafx.h"
#include "VBA.h"
#include "MainWnd.h"

#include "Directories.h"
#include "FileDlg.h"
#include "Joypad.h"
#include "MaxScale.h"
#include "Reg.h"
#include "WinResUtil.h"
#include "XAudio2_Config.h"
#include "BIOSDialog.h"
#include "AudioCoreSettingsDlg.h"

#include "../System.h"
#include "../gba/GBA.h"
#include "../gba/Globals.h"
#include "../gba/Sound.h"

#include "../version.h"

#include <tchar.h>


void MainWnd::OnOptionsVideoVsync()
{
	theApp.syncToVideo = !theApp.syncToVideo;
	if( theApp.display ) {
		theApp.display->setOption( _T("syncToVideo"), theApp.syncToVideo );
	}
}


void MainWnd::OnUpdateOptionsVideoVsync(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(theApp.syncToVideo);
}


void MainWnd::OnUpdateOptionsVideoX1(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(theApp.videoOption == VIDEO_1X);
}

void MainWnd::OnUpdateOptionsVideoX2(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(theApp.videoOption == VIDEO_2X);
}

void MainWnd::OnUpdateOptionsVideoX3(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(theApp.videoOption == VIDEO_3X);
}

void MainWnd::OnUpdateOptionsVideoX4(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(theApp.videoOption == VIDEO_4X);
}

BOOL MainWnd::OnOptionVideoSize(UINT nID)
{
	theApp.updateVideoSize(nID);
	return TRUE;
}

void MainWnd::OnOptionsVideoFullscreen()
{
  IDisplay::VIDEO_MODE mode;
  ZeroMemory( &mode, sizeof(IDisplay::VIDEO_MODE) );

  if( theApp.display->selectFullScreenMode( mode ) ) {
	  if( ( mode.width != theApp.fsWidth ) ||
		  ( mode.height != theApp.fsHeight ) ||
		  ( mode.bitDepth != theApp.fsColorDepth ) ||
		  ( mode.frequency != theApp.fsFrequency ) ||
		  ( mode.adapter != theApp.fsAdapter ) ||
		  ( theApp.videoOption != VIDEO_OTHER ) )
	  {
		  theApp.fsForceChange = true;
		  theApp.fsWidth = mode.width;
		  theApp.fsHeight = mode.height;
		  theApp.fsFrequency = mode.frequency;
		  theApp.fsColorDepth = mode.bitDepth;
		  theApp.fsAdapter = mode.adapter;
		  theApp.updateVideoSize( ID_OPTIONS_VIDEO_FULLSCREEN );
	  }
  }
  theApp.winAccelMgr.UpdateMenu(theApp.menu);
}


void MainWnd::OnUpdateOptionsVideoFullscreen(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(theApp.videoOption == VIDEO_OTHER);
}


void MainWnd::OnOptionsVideoFullscreenstretchtofit()
{
	theApp.fullScreenStretch = !theApp.fullScreenStretch;
	if( theApp.display ) {
		theApp.display->setOption( _T("fullScreenStretch"), theApp.fullScreenStretch );
	}
}


void MainWnd::OnUpdateOptionsVideoFullscreenstretchtofit(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(theApp.fullScreenStretch);
}

BOOL MainWnd::OnVideoLayer(UINT nID)
{
  layerSettings ^= 0x0100 <<
    ((nID & 0xFFFF) - ID_OPTIONS_VIDEO_LAYERS_BG0);
  layerEnable = DISPCNT & layerSettings;
  CPUUpdateRenderBuffers(false);

  // inform the user about the change
  CString msg;
  int thisLayer = -1;
  switch( nID ) {
	  case ID_OPTIONS_VIDEO_LAYERS_BG0:
		  msg = "BG0";
		  thisLayer = 256;
		  break;
	  case ID_OPTIONS_VIDEO_LAYERS_BG1:
		  msg = "BG1";
		  thisLayer = 512;
		  break;
	  case ID_OPTIONS_VIDEO_LAYERS_BG2:
		  msg = "BG2";
		  thisLayer = 1024;
		  break;
	  case ID_OPTIONS_VIDEO_LAYERS_BG3:
		  msg = "BG3";
		  thisLayer = 2048;
		  break;
	  case ID_OPTIONS_VIDEO_LAYERS_OBJ:
		  msg = "OBJ";
		  thisLayer = 4096;
		  break;
	  case ID_OPTIONS_VIDEO_LAYERS_WIN0:
		  msg = "WIN0";
		  thisLayer = 8192;
		  break;
	  case ID_OPTIONS_VIDEO_LAYERS_WIN1:
		  msg = "WIN1";
		  thisLayer = 16384;
		  break;
	  case ID_OPTIONS_VIDEO_LAYERS_OBJWIN:
		  msg = "OBJWIN";
		  thisLayer = 32768;
		  break;
	  default:
		  ASSERT( false );
  }
  if( layerSettings & thisLayer ) {
	  msg.Append( " enabled" );
  } else {
	  msg.Append( " disabled" );
  }
  systemScreenMessage( msg );

  return TRUE;
}

void MainWnd::OnVideoLayerReset()
{
	layerSettings = 0xFF00;
	layerEnable = DISPCNT & layerSettings;
	CPUUpdateRenderBuffers(false);
	systemScreenMessage( "All layers enabled" );
}

void MainWnd::OnUpdateVideoLayer(CCmdUI *pCmdUI)
{
  pCmdUI->SetCheck((layerSettings >> (8 + pCmdUI->m_nID - ID_OPTIONS_VIDEO_LAYERS_BG0)) & 1);
  switch(pCmdUI->m_nID) {
  case ID_OPTIONS_VIDEO_LAYERS_BG1:
  case ID_OPTIONS_VIDEO_LAYERS_BG2:
  case ID_OPTIONS_VIDEO_LAYERS_BG3:
  case ID_OPTIONS_VIDEO_LAYERS_WIN1:
  case ID_OPTIONS_VIDEO_LAYERS_OBJWIN:
    pCmdUI->Enable(theApp.cartridgeType == IMAGE_GBA);
    break;
  }
}


void MainWnd::OnOptionsVideoRenderoptionsD3dbilinear()
{
    theApp.gpuBilinear = !theApp.gpuBilinear;

	if( theApp.display ) {
        theApp.display->setOption( _T("gpuBilinear"), theApp.gpuBilinear ? 1 : 0 );
	}
}


void MainWnd::OnUpdateOptionsVideoRenderoptionsD3dbilinear(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck( theApp.gpuBilinear ? 1 : 0 );
}


void MainWnd::OnOptionsEmulatorDirectories()
{
  Directories dlg;
  dlg.DoModal();
}

void MainWnd::OnOptionsEmulatorDisablestatusmessages()
{
  theApp.disableStatusMessage = !theApp.disableStatusMessage;
}

void MainWnd::OnUpdateOptionsEmulatorDisablestatusmessages(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(theApp.disableStatusMessage);
}

void MainWnd::OnOptionsEmulatorSynchronize()
{
  syncToAudio = !syncToAudio;
}

void MainWnd::OnUpdateOptionsEmulatorSynchronize(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(syncToAudio);
}

void MainWnd::OnOptionsEmulatorPausewheninactive()
{
  theApp.pauseWhenInactive = !theApp.pauseWhenInactive;
}

void MainWnd::OnUpdateOptionsEmulatorPausewheninactive(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(theApp.pauseWhenInactive);
}


BOOL MainWnd::OnOptionsEmulatorShowSpeed(UINT nID)
{
  switch(nID) {
  case ID_OPTIONS_EMULATOR_SHOWSPEED_NONE:
    theApp.showSpeed = 0;
    SetWindowText( VBA_NAME_AND_SUBVERSION );
    break;
  case ID_OPTIONS_EMULATOR_SHOWSPEED_PERCENTAGE:
    theApp.showSpeed = 1;
    break;
  case ID_OPTIONS_EMULATOR_SHOWSPEED_DETAILED:
    theApp.showSpeed = 2;
    break;
  case ID_OPTIONS_EMULATOR_SHOWSPEED_TRANSPARENT:
    theApp.showSpeedTransparent = !theApp.showSpeedTransparent;
    break;
  default:
    return FALSE;
  }
  return TRUE;
}

void MainWnd::OnUpdateOptionsEmulatorShowSpeed(CCmdUI *pCmdUI)
{
  switch(pCmdUI->m_nID) {
  case ID_OPTIONS_EMULATOR_SHOWSPEED_NONE:
    pCmdUI->SetCheck(theApp.showSpeed == 0);
    break;
  case ID_OPTIONS_EMULATOR_SHOWSPEED_PERCENTAGE:
    pCmdUI->SetCheck(theApp.showSpeed == 1);
    break;
  case ID_OPTIONS_EMULATOR_SHOWSPEED_DETAILED:
    pCmdUI->SetCheck(theApp.showSpeed == 2);
    break;
  case ID_OPTIONS_EMULATOR_SHOWSPEED_TRANSPARENT:
    pCmdUI->SetCheck(theApp.showSpeedTransparent);
    break;
  }
}


void MainWnd::OnAudioCoreSettings()
{
	AudioCoreSettingsDlg dlg;

	//dlg.m_enabled = gb_effects_config.enabled;
	//dlg.m_surround = gb_effects_config.surround;
	//dlg.m_echo = gb_effects_config.echo;
	//dlg.m_stereo = gb_effects_config.stereo;
	dlg.m_volume = soundGetVolume();
	//dlg.m_declicking = gbSoundGetDeclicking();
	dlg.m_sound_interpolation = soundInterpolation;
	dlg.m_sound_filtering = soundFiltering;
	dlg.m_sample_rate = soundGetSampleRate();

	if( IDOK == dlg.DoModal() ) {
		//gb_effects_config_t _new;

		//_new.enabled = dlg.m_enabled;
		//_new.surround = dlg.m_surround;
		//_new.echo = dlg.m_echo;
		//_new.stereo = dlg.m_stereo;

		//gbSoundConfigEffects( _new );

		soundSetVolume( dlg.m_volume );

		//gbSoundSetDeclicking( dlg.m_declicking );

		soundInterpolation = dlg.m_sound_interpolation;

		soundFiltering = dlg.m_sound_filtering;
		
		if( theApp.cartridgeType == IMAGE_GBA ) {
			soundSetSampleRate( dlg.m_sample_rate );
		}
	}
}

void MainWnd::OnUpdateAudioCoreSettings(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( ( !theApp.soundRecording ) ? TRUE : FALSE );
}

void MainWnd::updateSoundChannels(UINT id)
{
  int flag = 0;

  if(id == ID_OPTIONS_SOUND_CHANNEL1)
    flag = 1;

  if(id == ID_OPTIONS_SOUND_CHANNEL2)
    flag = 2;

  if(id == ID_OPTIONS_SOUND_CHANNEL3)
    flag = 4;

  if(id == ID_OPTIONS_SOUND_CHANNEL4)
    flag = 8;

  if(id == ID_OPTIONS_SOUND_DIRECTSOUNDA)
    flag = 256;

  if(id == ID_OPTIONS_SOUND_DIRECTSOUNDB)
    flag = 512;

  int active = soundGetEnable() & 0x30f;

  if(active & flag)
    active &= (~flag);
  else
    active |= flag;

  soundSetEnable(active);
}

void MainWnd::OnOptionsSoundChannel1()
{
  updateSoundChannels(ID_OPTIONS_SOUND_CHANNEL1);
}

void MainWnd::OnUpdateOptionsSoundChannel1(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(soundGetEnable() & 1);
}

void MainWnd::OnOptionsSoundChannel2()
{
  updateSoundChannels(ID_OPTIONS_SOUND_CHANNEL2);
}

void MainWnd::OnUpdateOptionsSoundChannel2(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(soundGetEnable() & 2);
}

void MainWnd::OnOptionsSoundChannel3()
{
  updateSoundChannels(ID_OPTIONS_SOUND_CHANNEL3);
}

void MainWnd::OnUpdateOptionsSoundChannel3(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(soundGetEnable() & 4);
}

void MainWnd::OnOptionsSoundChannel4()
{
  updateSoundChannels(ID_OPTIONS_SOUND_CHANNEL4);
}

void MainWnd::OnUpdateOptionsSoundChannel4(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(soundGetEnable() & 8);
}

void MainWnd::OnOptionsSoundDirectsounda()
{
  updateSoundChannels(ID_OPTIONS_SOUND_DIRECTSOUNDA);
}

void MainWnd::OnUpdateOptionsSoundDirectsounda(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(soundGetEnable() & 256);
  pCmdUI->Enable(theApp.cartridgeType == IMAGE_GBA);
}

void MainWnd::OnOptionsSoundDirectsoundb()
{
  updateSoundChannels(ID_OPTIONS_SOUND_DIRECTSOUNDB);
}

void MainWnd::OnUpdateOptionsSoundDirectsoundb(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(soundGetEnable() & 512);
  pCmdUI->Enable(theApp.cartridgeType == IMAGE_GBA);
}


void MainWnd::OnOptionsLanguageSystem()
{
  theApp.winSetLanguageOption(0, false);
  theApp.winAccelMgr.UpdateMenu(theApp.menu);
}

void MainWnd::OnUpdateOptionsLanguageSystem(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(theApp.languageOption == 0);
}

void MainWnd::OnOptionsLanguageEnglish()
{
  theApp.winSetLanguageOption(1, false);
  theApp.winAccelMgr.UpdateMenu(theApp.menu);
}

void MainWnd::OnUpdateOptionsLanguageEnglish(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(theApp.languageOption == 1);
}

void MainWnd::OnOptionsLanguageOther()
{
  theApp.winSetLanguageOption(2, false);
  theApp.winAccelMgr.UpdateMenu(theApp.menu);
}

void MainWnd::OnUpdateOptionsLanguageOther(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(theApp.languageOption == 2);
}


void MainWnd::OnOptionsJoypadConfigure1()
{
  JoypadConfig dlg(0);
  dlg.DoModal();
}

void MainWnd::OnUpdateOptionsJoypadConfigure1(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(theApp.videoOption != VIDEO_320x240);
}

void MainWnd::OnOptionsJoypadConfigure2()
{
  JoypadConfig dlg(1);
  dlg.DoModal();
}

void MainWnd::OnUpdateOptionsJoypadConfigure2(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(theApp.videoOption != VIDEO_320x240);
}

void MainWnd::OnOptionsJoypadConfigure3()
{
  JoypadConfig dlg(2);
  dlg.DoModal();
}

void MainWnd::OnUpdateOptionsJoypadConfigure3(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(theApp.videoOption != VIDEO_320x240);
}

void MainWnd::OnOptionsJoypadConfigure4()
{
  JoypadConfig dlg(3);
  dlg.DoModal();
}

void MainWnd::OnUpdateOptionsJoypadConfigure4(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(theApp.videoOption != VIDEO_320x240);
}

BOOL MainWnd::OnOptionsJoypadDefault(UINT nID)
{
  theApp.joypadDefault = nID - ID_OPTIONS_JOYPAD_DEFAULTJOYPAD_1;
  return TRUE;
}

void MainWnd::OnUpdateOptionsJoypadDefault(CCmdUI *pCmdUI)
{
  pCmdUI->SetCheck(theApp.joypadDefault == (int)(pCmdUI->m_nID - ID_OPTIONS_JOYPAD_DEFAULTJOYPAD_1));
}

void MainWnd::OnOptionsJoypadMotionconfigure()
{
  MotionConfig dlg;
  dlg.DoModal();
}

void MainWnd::OnUpdateOptionsJoypadMotionconfigure(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(theApp.videoOption != VIDEO_320x240);
}

BOOL MainWnd::OnOptionsJoypadAutofire(UINT nID)
{
  switch(nID) {
  case ID_OPTIONS_JOYPAD_AUTOFIRE_A:
    if(theApp.autoFire & 1) {
      theApp.autoFire &= ~1;
      systemScreenMessage(winResLoadString(IDS_AUTOFIRE_A_DISABLED));
    } else {
      theApp.autoFire |= 1;
      systemScreenMessage(winResLoadString(IDS_AUTOFIRE_A));
    }
    break;
  case ID_OPTIONS_JOYPAD_AUTOFIRE_B:
    if(theApp.autoFire & 2) {
      theApp.autoFire &= ~2;
      systemScreenMessage(winResLoadString(IDS_AUTOFIRE_B_DISABLED));
    } else {
      theApp.autoFire |= 2;
      systemScreenMessage(winResLoadString(IDS_AUTOFIRE_B));
    }
    break;
  case ID_OPTIONS_JOYPAD_AUTOFIRE_L:
    if(theApp.autoFire & 512) {
      theApp.autoFire &= ~512;
      systemScreenMessage(winResLoadString(IDS_AUTOFIRE_L_DISABLED));
    } else {
      theApp.autoFire |= 512;
      systemScreenMessage(winResLoadString(IDS_AUTOFIRE_L));
    }
    break;
  case ID_OPTIONS_JOYPAD_AUTOFIRE_R:
    if(theApp.autoFire & 256) {
      theApp.autoFire &= ~256;
      systemScreenMessage(winResLoadString(IDS_AUTOFIRE_R_DISABLED));
    } else {
      theApp.autoFire |= 256;
      systemScreenMessage(winResLoadString(IDS_AUTOFIRE_R));
    }
    break;
  default:
    return FALSE;
  }
  return TRUE;
}

void MainWnd::OnUpdateOptionsJoypadAutofire(CCmdUI *pCmdUI)
{
  bool check = true;
  switch(pCmdUI->m_nID) {
  case ID_OPTIONS_JOYPAD_AUTOFIRE_A:
    check = (theApp.autoFire & 1) != 0;
    break;
  case ID_OPTIONS_JOYPAD_AUTOFIRE_B:
    check = (theApp.autoFire & 2) != 0;
    break;
  case ID_OPTIONS_JOYPAD_AUTOFIRE_L:
    check = (theApp.autoFire & 512) != 0;
    break;
  case ID_OPTIONS_JOYPAD_AUTOFIRE_R:
    check = (theApp.autoFire & 256) != 0;
    break;
  }
  pCmdUI->SetCheck(check);
}

void MainWnd::OnOptionsVideoFullscreenmaxscale()
{
  MaxScale dlg;

  dlg.DoModal();

  if( theApp.display ) {
	  theApp.display->setOption( _T("maxScale"), theApp.maxScale );
  }
}


void MainWnd::OnOutputapiXaudio2config()
{
	XAudio2_Config dlg;

	dlg.m_selected_device_index = theApp.xa2Device;
	dlg.m_buffer_count = theApp.xa2BufferCount;
	dlg.m_enable_upmixing = theApp.xa2Upmixing;

	if( dlg.DoModal() == IDOK ) {
		soundShutdown();

		theApp.xa2Device = dlg.m_selected_device_index;
		theApp.xa2BufferCount = dlg.m_buffer_count;
		theApp.xa2Upmixing = dlg.m_enable_upmixing;

		soundInit();
	}
}

void MainWnd::OnUpdateOutputapiXaudio2config(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(!theApp.soundRecording);
}

void MainWnd::OnRenderapiD3dmotionblur()
{
	theApp.gpuMotionBlur = !theApp.gpuMotionBlur;
	if( theApp.display ) {
		theApp.display->setOption( _T("gpuMotionBlur"), theApp.gpuMotionBlur ? 1 : 0 );
	}
}

void MainWnd::OnUpdateRenderapiD3dmotionblur(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( theApp.gpuMotionBlur ? 1 : 0 );
}

void MainWnd::OnEmulatorBiosfiles()
{
	BIOSDialog dlg;
	dlg.m_enableBIOS_GBA = theApp.useBiosFileGBA ? TRUE : FALSE;
	dlg.m_skipLogo = theApp.skipBiosFile ? TRUE : FALSE;
	dlg.m_pathGBA = theApp.biosFileNameGBA;

	if( IDOK == dlg.DoModal() ) {
		theApp.useBiosFileGBA = dlg.m_enableBIOS_GBA == TRUE;
		theApp.skipBiosFile = dlg.m_skipLogo == TRUE;
		theApp.biosFileNameGBA = dlg.m_pathGBA;
	}
}
