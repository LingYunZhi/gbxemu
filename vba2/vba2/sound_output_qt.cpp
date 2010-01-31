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
#include <QAudioOutput>
#include <QAudioFormat>
#include <string.h> // for memcpy


sound_buffer_qt::sound_buffer_qt( qint64 bufferSize, QObject *parent )
  : QIODevice( parent )
{
  m_bufferBytesFilled = 0;

  m_bufferSize = bufferSize;

  m_buffer = NULL;
  m_buffer = new quint16[bufferSize];
  Q_ASSERT( m_buffer != NULL );

  open( QIODevice::ReadWrite );
}


sound_buffer_qt::~sound_buffer_qt() {
  close();

  if( m_buffer != NULL ) {
    delete [] m_buffer;
  }
}


qint64 sound_buffer_qt::readData( char *data, qint64 maxlen ) {
  if( m_buffer == NULL ) return 0;

  if( maxlen > m_bufferBytesFilled ) maxlen = m_bufferBytesFilled;
//  Q_ASSERT( maxlen != 0 );
  memcpy( data, m_buffer, maxlen );
  m_bufferBytesFilled -= maxlen;

  return maxlen;
}


qint64 sound_buffer_qt::writeData( const char *data, qint64 len ) {
  if( m_buffer == NULL ) return 0;

  const qint64 bufferSpace = m_bufferSize - m_bufferBytesFilled;
  if( len > bufferSpace ) len = bufferSpace;
  if( bufferSpace == 0 ) {
    // TODO: synchronize
  } else {
    memcpy( m_buffer, data, len );
  }
  m_bufferBytesFilled += len;

  return len;
}


// ##########


sound_output_qt::sound_output_qt( QObject *parent )
  : QObject( parent )
{
  device = NULL;
  buffer = NULL;
}


sound_output_qt::~sound_output_qt()
{
  if( device != NULL ) {
    device->stop();
    delete device;
  }

  if( buffer != NULL ) {
    delete buffer;
  }
}


bool sound_output_qt::init( long sampleRate ) {
  if( device != NULL ) {
    delete device;
  }
  QAudioFormat format;
  format.setCodec( "audio/pcm" );
  format.setFrequency( sampleRate );
  format.setChannels( 2 );
  format.setSampleType( QAudioFormat::SignedInt );
  format.setSampleSize( 16 );
  Q_ASSERT( format.isValid() );

  device = new QAudioOutput( format, this );
  Q_ASSERT( device != NULL );

  const qint64 bufferSize = sampleRate * 2 * 2; // one second; for example: 44100 * 16 bit * stereo
  buffer = new sound_buffer_qt( bufferSize, this );
  Q_ASSERT( buffer != NULL );

  return true;
}


void sound_output_qt::pause() {
  if( device == NULL ) return;

  device->suspend();
}


void sound_output_qt::reset() {
  if( device == NULL ) return;

  device->reset();
}


void sound_output_qt::resume() {
  if( device == NULL ) return;

  device->resume();
}


void sound_output_qt::write( u16 *finalWave, int length ) {
  if( buffer == NULL ) return;
  buffer->writeData( (const char *)finalWave, length );

  if( device == NULL ) return;
  if( QAudio::StoppedState == device->state() ) {
    // for smooth playback, this should only happen once at the beginning
    device->start( buffer );
  }
}


void sound_output_qt::setThrottle( unsigned short throttle ) {
  // TODO: implement
}
