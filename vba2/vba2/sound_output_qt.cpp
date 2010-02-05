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
#include <QIODevice>


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
  }
}


bool sound_output_qt::init( long sampleRate ) {
  static bool initialized = false;
  Q_ASSERT( initialized == false );
  initialized = true;

  QAudioFormat format;
  format.setCodec( "audio/pcm" );
  format.setFrequency( sampleRate );
  format.setChannels( 2 );
  format.setSampleType( QAudioFormat::SignedInt );
  format.setSampleSize( 16 );
  format.setByteOrder( QAudioFormat::LittleEndian );
  Q_ASSERT( format.isValid() );

  device = new QAudioOutput( format, this );
  Q_ASSERT( device != NULL );

  buffer = device->start();
  Q_ASSERT( buffer != NULL );

  return true;
}


void sound_output_qt::pause() {
  if( device == NULL ) return;

  qDebug( "pausing sound..." );

//  device->suspend();
}


void sound_output_qt::reset() {
  if( device == NULL ) return;

  qDebug( "resetting sound..." );

//  device->reset();
}


void sound_output_qt::resume() {
  if( device == NULL ) return;

  qDebug( "resuming sound..." );

//  device->resume();
}


void sound_output_qt::write( u16 *finalWave, int length ) {
  if( buffer == NULL ) return;

  const qint64 bytes_written = buffer->write( (const char *)finalWave, length );
  // TODO: sync to audio
}


void sound_output_qt::setThrottle( unsigned short throttle ) {
  // TODO: implement
}
