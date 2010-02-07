/*  VisualBoyAdvance 2
    Copyright (C) 2009-2010  VBA development team

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "settingssheet_audiooutput.h"
#include "ui_settingssheet_audiooutput.h"

#include <QtMultimedia/QAudioDeviceInfo>
#include <QVariant>


SettingsSheet_AudioOutput::SettingsSheet_AudioOutput( CAppSettings &settings, QWidget *parent )
  : SettingsSheet( settings, parent ),
  ui(new Ui::SettingsSheet_AudioOutput)
{
  ui->setupUi(this);

  const QList<QAudioDeviceInfo> deviceInfo = QAudioDeviceInfo::availableDevices( QAudio::AudioOutput );

  const int nDevices = deviceInfo.size();
  for( int i = 0; i < nDevices; i++ ) {
    const QAudioDeviceInfo &currentInfo = deviceInfo.at(i);
    ui->comboBox_device->addItem( currentInfo.deviceName(), qVariantFromValue(currentInfo) );
    if( m_settings.s_soundOutputDevice.deviceName() == currentInfo.deviceName() ) {
      // select memorized device
      ui->comboBox_device->setCurrentIndex( i );
    }
  }
}


SettingsSheet_AudioOutput::~SettingsSheet_AudioOutput()
{
  delete ui;
}


void SettingsSheet_AudioOutput::applySettings()
{
  m_settings.s_soundOutputDevice = ui->comboBox_device->itemData( ui->comboBox_device->currentIndex() ).value<QAudioDeviceInfo>();
}

RETRANSLATE_CODE( SettingsSheet_AudioOutput )
