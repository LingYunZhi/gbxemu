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


#include "sound_output_qt.h"
#include <QtMultimedia/QAudioOutput>
#include <QtMultimedia/QAudioFormat>
#include <QtMultimedia/QAudioDeviceInfo>
#include <QIODevice>
#include <QThread>


// buffer size in units of 1/60 seconds
#define BUFFER_SIZE 6


// hack: expose protected QThread::usleep() function to public
class CSleep : QThread {
  Q_OBJECT
public:
  static void usleep ( unsigned long usecs ) {
    QThread::usleep( usecs );
  }
};


sound_output_qt::sound_output_qt( QAudioDeviceInfo &outputDevice, QObject *parent )
  : QObject( parent ),
  m_selectedDevice( outputDevice )
{
  m_device = NULL;
  m_buffer = NULL;
  m_enableAudioSync = true;
  m_initialized = false;
  if( m_selectedDevice.isNull() ) {
    m_selectedDevice = QAudioDeviceInfo::defaultOutputDevice();
    Q_ASSERT( m_selectedDevice.isNull() == false );
  }
}


bool sound_output_qt::init( long sampleRate ) {
  if( m_initialized ) return false;
  m_initialized = true;

  // size in bytes of 1/60 seconds of audio data
  const int audio_frame_size = ( sampleRate * 2 * 2 ) / 60;
  const int desired_buffer_size = BUFFER_SIZE * audio_frame_size;

  QAudioFormat format;
  format.setCodec( "audio/pcm" );
  format.setFrequency( sampleRate );
  format.setChannels( 2 );
  format.setSampleType( QAudioFormat::SignedInt );
  format.setSampleSize( 16 );
  format.setByteOrder( QAudioFormat::LittleEndian ); // TODO: is this correct on other architectures?
  Q_ASSERT( format.isValid() );

  m_device = new QAudioOutput( m_selectedDevice, format, this );
  Q_ASSERT( m_device != NULL );

  m_device->setBufferSize( desired_buffer_size );

  m_buffer = m_device->start();
  Q_ASSERT( m_buffer != NULL );

  const int delta_buffer_size = m_device->bufferSize() - desired_buffer_size;
  qDebug( "audio buffer size delta = %i bytes", delta_buffer_size );

  Q_ASSERT( m_device->bufferSize() >= audio_frame_size );

  return true;
}


void sound_output_qt::pause() {
  if( m_device == NULL ) return;

  qDebug( "pausing sound..." );

//  device->suspend();
}


void sound_output_qt::reset() {
  if( m_device == NULL ) return;

  qDebug( "resetting sound..." );

//  device->reset();
}


void sound_output_qt::resume() {
  if( m_device == NULL ) return;

  qDebug( "resuming sound..." );

//  device->resume();
}


void sound_output_qt::write( u16 *finalWave, int length ) {
  if( m_buffer == NULL ) return;

  if( m_enableAudioSync ) {
    quint64 wait_count = 0;
    while( m_device->bytesFree() < length ) {
      // synchronize process/thread to audio
      CSleep::usleep( 1000000 / 60 );
      if( (++wait_count) > (60*3) ) {
        // if we waited more than three seconds, something went terribly wrong
        Q_ASSERT( false );
        break; // prevent application hang
      }
    }
  }

  const qint64 bytes_written = m_buffer->write( (const char *)finalWave, length );
//  Q_ASSERT( bytes_written == length ); // only when enableAudioSync == true
}


void sound_output_qt::setThrottle( unsigned short throttle ) {
  // TODO: implement
}
