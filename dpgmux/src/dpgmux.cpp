/*
    dpgmux - A (de)multiplexer for DPG files
    Copyright (C) 2007 spacy (André Köhler)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "stdafx.h"


#define CHUNK_SIZE 1024 // in bytes - affects stream copying and searching
	// has to be >= 20


#define DPG0_ID 0x30475044 // ASCII "DPG0"
#define DPG1_ID 0x31475044 // ASCII "DPG1"
#define DPG2_ID 0x32475044 // ASCII "DPG2"
#define DPG3_ID 0x33475044 // ASCII "DPG3"

#define MP2  0
#define GSM1 1
#define GSM2 2
#define OGG  3

#define RGB15 0
#define RGB18 1
#define RGB21 2
#define RGB24 3


struct DPG_HEADER
{
	unsigned int version_id;     // DPG0_ID  DPG1_ID  DPG2_ID  DPG3_ID
	unsigned int frames_total;   // total number of frames in video stream
	unsigned int fps;            // frames per second (in file * 0x100)
	unsigned int audio_freq;     // samplerate of audio stream
	unsigned int audio_id;       // MP2  GSM1(channel)  GSM2(channels)  OGG
	unsigned int audio_offset;   // address of audio stream
	unsigned int audio_size;     // size of audio stream in bytes
	unsigned int video_offset;   // address of video stream
	unsigned int video_size;     // size of video stream in bytes
	// version_id >= DPG2
	unsigned int goplist_offset; // address of GOP list
	unsigned int goplist_size;   // size of GOP list
	// version_id >= DPG1
	unsigned int pixel_format;   // RGB15  RGB18  RGB21  RGB24
	// DPG2 and DPG3 use the same header
};


struct GOPLIST_ENTRY
{
	unsigned int frame_index; // frame number at which this MPEG-1 sequence header appears
	unsigned int frame_offset; // address of this MPEG-1 sequence header
};


void print_usage()
{
	_tprintf( _T("Usage:\ndpgmux.exe file [file2]\n") );
	_tprintf( _T("  to demultiplex dpg: specify only dpg file\n") );
	_tprintf( _T("  to create dpg: specify m1v and mp2 file (MPEG-1 streams)\n") );
}


unsigned int readint( FILE *file )
{
	//read 4 bytes and combine them using little endian byte order
	return fgetc( file ) + (fgetc( file ) << 8) + (fgetc( file ) << 16) + (fgetc( file ) << 24);
}


int read_header( _TCHAR *filename, DPG_HEADER *header )
{
	// read DPG header
	FILE *dpg;
	if( _tfopen_s( &dpg, filename, _T("rb") ) != 0 )
	{
		_tprintf( _T("! ERROR: Can not open DPG input file\n") );
		return -1;
	}

	_tprintf( _T("\n  JOB: Reading DPG header\n") );

	// DPG version
	header->version_id = readint( dpg );
	switch( header->version_id )
	{
	case DPG0_ID:
		_tprintf( _T("INFO: DPG version 0 (oldest)\n") );
		break;
	case DPG1_ID:
		_tprintf( _T("INFO: DPG version 1 (old)\n") );
		break;
	case DPG2_ID:
		_tprintf( _T("INFO: DPG version 2 (old)\n") );
		break;
	case DPG3_ID:
		_tprintf( _T("INFO: DPG version 3 (newest)\n") );
		break;
	default:
		_tprintf( _T("! ERROR: Unknown DPG version\n") );
		fclose( dpg );
		return -1;
		break;
	}

	// total frames
	header->frames_total = readint( dpg );
	if( header->frames_total == 0 )
	{
		_tprintf( _T("WARNING: total number of video frames = 0\n") );
	}
	_tprintf( _T("INFO: Video stream has a total of %i frames\n"), header->frames_total );

	// frames per second
	header->fps = readint( dpg ) / 0x100;
	if( header->fps == 0 )
	{
		_tprintf( _T("WARNING: frames per second = 0\n") );
	}
	_tprintf( _T("INFO: Video stream has %i frames per second\n"), header->fps );

	// play time
	int seconds = header->frames_total / header->fps;
	_tprintf( _T("INFO: Resulting play time: %i:%i (Minutes:Seconds)\n"), seconds / 60, seconds % 60 );

	// audio frequency
	header->audio_freq = readint( dpg );
	if( header->audio_freq == 0 )
	{
		_tprintf( _T("WARNING: audio frequency = 0\n") );
	}
	_tprintf( _T("INFO: Audio stream has a frequency of %i Hz\n"), header->audio_freq );

	// audio ID
	header->audio_id = readint( dpg );
	switch( header->audio_id )
	{
	case MP2:
		_tprintf( _T("INFO: Audio stream is MPEG-1 Audio Layer 2\n") );
		break;
	case GSM1:
		_tprintf( _T("INFO: Audio stream is GSM (mono)\n") );
		break;
	case GSM2:
		_tprintf( _T("INFO: Audio stream is GSM (stereo)\n") );
		break;
	case OGG:
		_tprintf( _T("INFO: Audio stream is OGG Vorbis\n") );
		break;
	default:
		_tprintf( _T("WARNING: Unknown audio ID\n") );
		break;
	}

	// audio offset
	header->audio_offset = readint( dpg );
	if( header->audio_offset == 0 )
	{
		_tprintf( _T("WARNING: audio offset = 0\n") );
	}
	_tprintf( _T("INFO: Audio stream starts at 0x%X\n"), header->audio_offset );

	// audio size
	header->audio_size = readint( dpg );
	if( header->audio_size == 0 )
	{
		_tprintf( _T("WARNING: audio size = 0\n") );
	}
	_tprintf( _T("INFO: Audio stream has an approximate size of %i MB\n"), header->audio_size / 0x100000 );

	// video offset
	header->video_offset = readint( dpg );
	if( header->video_offset == 0 )
	{
		_tprintf( _T("WARNING: video offset = 0\n") );
	}
	_tprintf( _T("INFO: Video stream starts at 0x%X\n"), header->video_offset );

	// video size
	header->video_size = readint( dpg );
	if( header->video_size == 0 )
	{
		_tprintf( _T("WARNING: video size = 0\n") );
	}
	_tprintf( _T("INFO: Video stream has an approximate size of %i MB\n"), header->video_size / 0x100000 );

	// GOP list
	switch( header->version_id )
	{
	case DPG0_ID:
	case DPG1_ID:
		header->goplist_offset = 0;
		header->goplist_size = 0;
		_tprintf( _T("INFO: Video GOP list no supported by this DPG version\n") );
		break;
	case DPG2_ID:
	case DPG3_ID:
		header->goplist_offset = readint( dpg );
		if( header->goplist_offset == 0 )
		{
			_tprintf( _T("WARNING: video GOP list offset = 0\n") );
		}
		_tprintf( _T("INFO: Video GOP list starts at 0x%X\n"), header->goplist_offset );

		header->goplist_size = readint( dpg );
		if( header->goplist_size == 0 )
		{
			_tprintf( _T("WARNING: video GOP list size = 0\n") );
		}
		if( header->goplist_size < 0x400 )
		{
			_tprintf( _T("INFO: Video GOP list has a size of %i Byte\n"), header->goplist_size );
		}
		else
		{
			_tprintf( _T("INFO: Video GOP list has an approximate size of %i KB\n"), header->goplist_size / 0x400 );
		}
		break;
	}

	// pixel format
	switch( header->version_id )
	{
	case DPG0_ID:
		header->pixel_format = RGB24;
		break;
	case DPG1_ID:
	case DPG2_ID:
	case DPG3_ID:
		header->pixel_format = readint( dpg );
		break;
	}
	
	switch( header->pixel_format )
	{
	case RGB15:
		_tprintf( _T("INFO: Video stream's pixel format is RGB15\n") );
		break;
	case RGB18:
		_tprintf( _T("INFO: Video stream's pixel format is RGB18\n") );
		break;
	case RGB21:
		_tprintf( _T("INFO: Video stream's pixel format is RGB21\n") );
		break;
	case RGB24:
		_tprintf( _T("INFO: Video stream's pixel format is RGB24\n") );
		break;
	default:
		_tprintf( _T("WARNING: Unknown pixel format ID\n") );
		break;
	}


	fclose( dpg );

	return 0;
}


bool file_exists( _TCHAR *filename )
{
	return ( _taccess( filename, 0 ) != -1 );
}


void copy_stream( FILE *in, FILE *out, unsigned int count )
{
	_tprintf( _T("PROGRESS: ") );
	unsigned int chunk_count = count / CHUNK_SIZE;
	unsigned char *chunk = (unsigned char *)malloc( CHUNK_SIZE );
	time_t start = time( NULL );
	for( unsigned int c = 0; c < chunk_count; c++ )
	{
		// copy stream chunkwise
		// (this is MUCH faster than doing it bytewise)
		fread( chunk, 1, CHUNK_SIZE, in );
		fwrite( chunk, 1, CHUNK_SIZE, out );
		time_t current = time( NULL );
		if( current - start > 0 )
		{
			// print status information every second
			_tprintf( _T("%.0f%%, "), (float)c / (float)chunk_count * 100.0f );
			start = current;
		}
	}
	// copy remaining bytes < CHUNK_SIZE
	fread( chunk, 1, count % CHUNK_SIZE, in );
	fwrite( chunk, 1, count % CHUNK_SIZE, out );
	_tprintf( _T("100%%\n") );
	free( chunk );
}


int demultiplex( _TCHAR *filename )
{
	// read header
	DPG_HEADER header;
	if( read_header( filename, &header ) == -1 )
	{
		_tprintf( _T("! ERROR: Failed reading header, stop demultiplexing\n") );
		return -1;
	}

	// prepare
	bool error;
	_TCHAR drive[_MAX_DRIVE];
	_TCHAR dir[_MAX_DIR];
	_TCHAR basename[_MAX_FNAME];
	_TCHAR file_ext[_MAX_EXT];
	_tsplitpath_s( filename, drive, _countof(drive), dir, _countof(dir), basename, _countof(basename), file_ext, _countof(file_ext) );

	_tprintf( _T("\n->JOB: Demultiplexing '%s%s'\n"), basename, file_ext );

	_stat buf;
	_tstat( filename, &buf ); // file size is in buf.st_size

	// check for errors about video stream in header
	error = false;
	error = error || (header.frames_total == 0);
	error = error || (header.fps == 0);
	error = error || (header.video_offset == 0);
	error = error || (header.video_size == 0);
	error = error || ((header.video_offset + header.video_size) > (unsigned int)buf.st_size);
	if( error )
	{
		_tprintf( _T("! ERROR: DPG header's video section has errors, skip video demultiplexing\n") );
	}
	else
	{
		_TCHAR out_video[_MAX_PATH];
		_tmakepath_s( out_video, _countof(out_video), drive, dir, basename, _T(".m1v") );
		if( file_exists( out_video ) )
		{
			_tprintf( _T("! ERROR: Video output file does already exist\n") );
			return -1;
		}
		FILE *in;
		if( _tfopen_s( &in, filename, _T("rb") ) != 0 )
		{
			_tprintf( _T("! ERROR: Can not open video input file\n") );
			return -1;

		}
		FILE *out;
		if( _tfopen_s( &out, out_video, _T("wb") ) != 0 )
		{
			_tprintf( _T("! ERROR: Can not create video output file\n") );
			fclose( in );
			return -1;
		}
		else
		{
			// create video file
			_tprintf( _T("VIDEO: \n") );
			fseek( in, header.video_offset, SEEK_SET );
			copy_stream( in, out, header.video_size );
			fclose( out );
			fclose( in );
		}
	}

	// check for errors about audio stream in header
	error = false;
	error = error || (header.audio_freq == 0);
	error = error || (header.audio_id == GSM1); // GSM unsupported
	error = error || (header.audio_id == GSM2); // GSM unsupported
	error = error || (header.audio_id > OGG);
	error = error || (header.audio_offset == 0);
	error = error || (header.audio_size == 0);
	error = error || ((header.audio_offset + header.audio_size) > (unsigned int)buf.st_size);
	if( error )
	{
		_tprintf( _T("! ERROR: DPG header's audio section has errors, skip audio demultiplexing\n") );
	}
	else
	{
		_TCHAR out_audio[_MAX_PATH];
		if( header.audio_id == MP2 )
		{
			_tmakepath_s( out_audio, _countof(out_audio), drive, dir, basename, _T(".mp2") );
		}
		if( header.audio_id == OGG )
		{
			_tmakepath_s( out_audio, _countof(out_audio), drive, dir, basename, _T(".ogg") );
		}
		if( file_exists( out_audio ) )
		{
			_tprintf( _T("! ERROR: Audio output file does already exist\n") );
			return -1;
		}
		FILE *in;
		if( _tfopen_s( &in, filename, _T("rb") ) != 0 )
		{
			_tprintf( _T("! ERROR: Can not open audio input file\n") );
			return -1;

		}
		FILE *out;
		if( _tfopen_s( &out, out_audio, _T("wb") ) != 0 )
		{
			_tprintf( _T("! ERROR: Can not create audio output file\n") );
			fclose( in );
			return -1;
		}
		else
		{
			// create audio file
			fseek( in, header.audio_offset, SEEK_SET );
			_tprintf( _T("AUDIO: \n") );
			copy_stream( in, out, header.audio_size );
			fclose( out );
			fclose( in );
		}
	}
	return 0;
}


void inspect_chunk( unsigned char *chunk, unsigned int chunk_offset, unsigned int chunk_size,
				   GOPLIST_ENTRY **goplist, unsigned int *goplistsize, unsigned int *goplistcount, unsigned int *frameindex )
{
	// We overlap the chunks by 8 Bytes.
	// If we would find something in a chunk's last 8 Bytes, we would not be able
	// to get the necessary data Bytes out of it, because they are in the next chunk.
	for( unsigned int i = 0; i < (chunk_size - 8); i++ )
	{
		if( (chunk[i] == 0x00) && (chunk[i+1] == 0x00) )
		{
			// MPEG start code found
			if(	(chunk[i+2] == 0x01) && (chunk[i+3] == 0x00) )
			{
				// picture header found
				// read picture data using big endian byte order
				unsigned int picdata = (chunk[i+4] << 24) | (chunk[i+5] << 16) | (chunk[i+6] << 8) | chunk[i+7];
				unsigned int picdata_refidx = picdata >> 22; // bTemporalReference / 10 bit
				unsigned int picdata_type = (picdata >> 19) & 7; // bPictureCodingType / 3 bit
				// keep track of the frame index
				if( (picdata_refidx < 0x400) &&
					( (picdata_type == 1/*I frame*/) || (picdata_type == 2/*P frame*/) || (picdata_type == 3/*B frame*/) ) )
				{
					(*frameindex)++;
				}
				
				// skip the picture header (8.85 Bytes [70 bits] startcode inclusive)
				i += 8; // 8 because 'for' will add 1, too
			}
			else
			{
				if( (chunk[i+2] == 0x01) && (chunk[i+3] == 0xB3) )
				{
					// sequence header found
					(*goplistcount)++;
					if( *goplistcount > *goplistsize )
					{
						(*goplistsize) += 0x80; // increase the GOP list by 1 KB
						*goplist = (GOPLIST_ENTRY *)realloc( *goplist, (sizeof(GOPLIST_ENTRY) * *goplistsize) );
					}
					// set this GOP header's address
					(*goplist)[ *goplistcount - 1 ].frame_offset = chunk_offset + i;
					// set the corresponding frame number
					(*goplist)[ *goplistcount - 1 ].frame_index = *frameindex;

					// skip the sequence header (14 Bytes startcode inclusive)
					i += 13; // 13 because 'for' will add 1, too
				}
			}
		}
	}
}


int multiplex( _TCHAR *video_file, _TCHAR *audio_file )
{
	// header - audio - video - gop list

	// prepare
	DPG_HEADER header;
	_stat buf;
	_tstat( video_file, &buf ); // video file size is in buf.st_size
	_stat buf2;
	_tstat( audio_file, &buf2 ); // audio file size is in buf2.st_size
	_TCHAR drive[_MAX_DRIVE];
	_TCHAR dir[_MAX_DIR];
	_TCHAR basename[_MAX_FNAME];
	_TCHAR file_ext[_MAX_EXT];
	_tsplitpath_s( video_file, drive, _countof(drive), dir, _countof(dir), basename, _countof(basename), file_ext, _countof(file_ext) );
	_TCHAR out_dpg[_MAX_PATH];
	_tmakepath_s( out_dpg, _countof(out_dpg), drive, dir, basename, _T(".dpg") );
#ifndef _DEBUG
	if( file_exists( out_dpg ) )
	{
		_tprintf( _T("! ERROR: DPG output file does already exist\n") );
		return -1;
	}
#endif

	_tprintf( _T("\n->JOB: Multiplexing '%s.dpg'\n"), basename );

	// calculate header
	_tprintf( _T("\n  JOB: Getting information on input files\n") );
	_tprintf( _T("STATUS: Initializing 'MediaInfo.dll'") );
	MediaInfo mi;
	mi.Option( _T("Internet"), _T("No") );
	String tmp = mi.Option(_T("Info_Version"), _T("0.7.4.7;MediaInfoDLL_Example_MSVC;"));
	const _TCHAR *mi_version = tmp.c_str();
	_tprintf( _T(" - %s\n"), mi_version );
	_tprintf( _T("ADVERTISEMENT: Visit http://mediainfo.sourceforge.net\n"), mi_version );
	if( _tcscmp( mi_version, _T("") ) == 0 )
	{
		_tprintf( _T("! ERROR: dpgmux is not compatible with this version, you must use v0.7.4.7\n") );
		return -1;
	}

	_tprintf( _T("STATUS: Opening audio\n") );
	mi.Open( audio_file );
	tmp = mi.Get( Stream_Audio, 0, 28 ); // SamplingRate
	_tprintf( _T("INFO: SamplingRate: %s\n"), tmp.c_str() );
	header.audio_freq = (unsigned int)_tstoi( tmp.c_str() );
	tmp = mi.Get( Stream_Audio, 0, 7 ); // Codec
	_tprintf( _T("INFO: Codec: %s\n"), tmp.c_str() );
	if( _tcscmp( tmp.c_str(), _T("MPA1L2") ) == 0 )
	{
		header.audio_id = MP2;
	}
	else
	{
		if( _tcscmp( tmp.c_str(), _T("Vorbis") ) == 0 )
		{
			header.audio_id = OGG;
		}
		else
		{
			_tprintf( _T("! ERROR: DPG only supports MP2 or OGG audio stream\n") );
			return -1;
		}
	}
	mi.Close();

	_tprintf( _T("STATUS: Opening video\n") );
	mi.Open( video_file );
	tmp = mi.Get( Stream_Video, 0, 43 ); // FrameCount
	_tprintf( _T("INFO: FrameCount: %s\n"), tmp.c_str() );
	header.frames_total = (unsigned int)_tstoi( tmp.c_str() );
	tmp = mi.Get( Stream_Video, 0, 36 ); // FrameRate
	_tprintf( _T("INFO: FrameRate: %s\n"), tmp.c_str() );
	//header.fps = (unsigned int)(_tstof( tmp.c_str() ) * 0x100);
	double new_fps = _tstof( tmp.c_str() );
	new_fps = new_fps * 0x100;
	double d = pow( 10.0 , 0 ); // round to 0 decimal places (that's closer to the
	new_fps = floor( new_fps * d + 0.5) / d; // original frame rate than just cutting them off)
	_tprintf( _T("INFO: Next best FrameRate: %f\n"), new_fps / 0x100 );
	header.fps = (unsigned int)new_fps;

	header.pixel_format = RGB24; // TODO: detect somehow
	mi.Close();

	header.version_id = DPG3_ID;
	header.audio_offset = (unsigned int)(ceilf( (float)sizeof(DPG_HEADER) / 4.0f ) * 4.0f ); // pdding: align to 32bit sections
	header.audio_size = buf2.st_size;
	header.video_offset = (unsigned int)(ceilf( (float)(header.audio_offset + header.audio_size) / 4.0f ) * 4.0f );
	header.video_size = buf.st_size;
	header.goplist_offset = (unsigned int)(ceilf( (float)(header.video_offset + header.video_size) / 4.0f ) * 4.0f );
	header.goplist_size = 0;

	FILE *out;
	if( _tfopen_s( &out, out_dpg, _T("wb") ) != 0 )
	{
		_tprintf( _T("! ERROR: Can not open DPG output file\n") );
		return -1;

	}

	// write header
	_tprintf( _T("\n  JOB: Copying header\n") );
	fwrite( &header, sizeof(header), 1, out );

	// write audio stream
	_tprintf( _T("\n  JOB: Copying audio\n") );
	FILE *in;
	if( _tfopen_s( &in, audio_file, _T("rb") ) != 0 )
	{
		_tprintf( _T("! ERROR: Can not open audio input file\n") );
		return -1;
	}
	fseek( out, header.audio_offset, SEEK_SET );
	copy_stream( in, out, header.audio_size );
	fclose( in );

	// create GOP list while copying video stream completely into dpg file
	_tprintf( _T("\n  JOB: Copying video & creating GOP list\n") );
	fseek( out, header.video_offset, SEEK_SET );

	// initially prepare for a 32KB GOP list
	// the size will be increased if necessary
	unsigned int goplistsize = 0x1000;
	GOPLIST_ENTRY *goplist = (GOPLIST_ENTRY *)malloc( sizeof(GOPLIST_ENTRY) * goplistsize );

	unsigned int goplistcount = 0;
	unsigned int frameindex = 0;
	
	FILE *video;
	if( _tfopen_s( &video, video_file, _T("rb") ) != 0 )
	{
		_tprintf( _T("! ERROR: Can not open video input file\n") );
		return -1;
	}

	_tprintf( _T("PROGRESS: ") );
	unsigned int chunk_count = buf.st_size / CHUNK_SIZE;
	unsigned char *chunk = (unsigned char *)malloc( CHUNK_SIZE );
	time_t start = time( NULL );
	for( unsigned int c = 0; c < chunk_count; c++ )
	{
		// inspect video stream chunkwise
		// (this is faster than doing it bytewise)
		long wobinich = ftell( video );
		fread( chunk, 1, CHUNK_SIZE, video );
		fwrite( chunk, 1, CHUNK_SIZE, out );

		inspect_chunk( chunk, ftell( video ) - CHUNK_SIZE, CHUNK_SIZE, &goplist, &goplistsize, &goplistcount, &frameindex );

		fseek( video, -8, SEEK_CUR ); // next time continue where we stopped last
		fseek( out, -8, SEEK_CUR );
		time_t current = time( NULL );
		if( current - start > 0 )
		{
			// print status information every second
			_tprintf( _T("%.0f%%, "), (float)c / (float)chunk_count * 100.0f );
			start = current;
		}
	}
	// inspect remaining bytes
	free( chunk );
	unsigned int last_chunk_size = buf.st_size - ((CHUNK_SIZE - 8) * chunk_count);
	chunk = (unsigned char *)malloc( last_chunk_size );
	fread( chunk, 1, last_chunk_size, video );
	fwrite( chunk, 1, last_chunk_size, out );
	inspect_chunk( chunk, ftell( video ) - last_chunk_size, last_chunk_size, &goplist, &goplistsize, &goplistcount, &frameindex );
	free( chunk );
	_tprintf( _T("100%%\n") );
	fclose( video );

	// write GOP list
	_tprintf( _T("\n  JOB: Copying GOP list\n") );
	fseek( out, header.goplist_offset, SEEK_SET );
	fwrite( goplist, sizeof(GOPLIST_ENTRY), goplistcount, out );

	// write GOP list size to header
	header.goplist_size = sizeof(GOPLIST_ENTRY) * goplistcount;
	fseek( out, 40, SEEK_SET );
	fwrite( &(header.goplist_size), sizeof(unsigned int), 1, out );

	fclose( out );
	free( goplist );

	return 0;
}


int _tmain( int argc, _TCHAR* argv[] )
{
	_tprintf( _T("dpgmux v1.001\ndpgmux comes with ABSOLUTELY NO WARRANTY; see 'gpl.txt'\n\n") );

	char mode = '0';
	bool audio_first;
	_TCHAR file_name[_MAX_PATH];
	_TCHAR file_name2[_MAX_PATH];
	_TCHAR file_ext[_MAX_EXT];
	_TCHAR file_ext2[_MAX_EXT];

	if( argc == 2 )
	{
		// determine the given file's extension
		_tcscpy_s( file_name, _countof(file_name), argv[1] );
		_tsplitpath_s( file_name, NULL, 0, NULL, 0, NULL, 0, file_ext, _countof(file_ext) );

		if( _tcsicmp( file_ext, _T(".dpg") ) == 0 )
		{
			// we got a dpg file, set mode to demultiplex dpg
			mode = 'd';
		}
	}

	if( argc == 3 )
	{
		_tcscpy_s( file_name, _MAX_PATH, argv[1] );
		_tcscpy_s( file_name2, _MAX_PATH, argv[2] );
		_tsplitpath_s( file_name, NULL, 0, NULL, 0, NULL, 0, file_ext, _countof(file_ext) );
		_tsplitpath_s( file_name2, NULL, 0, NULL, 0, NULL, 0, file_ext2, _countof(file_ext2) );

		if( ( _tcsicmp( file_ext, _T(".m1v") ) == 0 ) && ( _tcsicmp( file_ext2, _T(".mp2") ) == 0 ) )
		{
				// we got source streams, set mode to multiplex dpg
				mode = 'm';
				audio_first = false;
		}

		if( ( _tcsicmp( file_ext, _T(".mp2") ) == 0 ) && ( _tcsicmp( file_ext2, _T(".m1v") ) == 0 ) )
		{
				mode = 'm';
				// audio stream was specified as first file
				audio_first = true;
		}

		if( ( _tcsicmp( file_ext, _T(".m1v") ) == 0 ) && ( _tcsicmp( file_ext2, _T(".ogg") ) == 0 ) )
		{
				mode = 'm';
				audio_first = false;
		}

		if( ( _tcsicmp( file_ext, _T(".ogg") ) == 0 ) && ( _tcsicmp( file_ext2, _T(".m1v") ) == 0 ) )
		{
				mode = 'm';
				audio_first = true;
		}
	}

	if( ((mode == 'd') || (mode == 'm')) && !file_exists( file_name ) )
	{
		_tprintf( _T("! ERROR: First file does not exists!\n\n") );
		system( "pause" );
		return -1;
	}

	if( (mode == 'm') && !file_exists( file_name2 ) )
	{
		_tprintf( _T("! ERROR: Second file does not exists!\n\n") );
		system( "pause" );
		return -1;
	}

	switch( mode )
	{
	case '0':
		_tprintf( _T("! ERROR: Wrong parameters!\n\n") );
		print_usage();
		system( "pause" );
		return -1;
		break;
	case 'd':
		if( demultiplex( file_name ) == -1 )
		{
			_tprintf( _T("! ERROR: Demultiplexing failed, exiting\n\n") );
			system( "pause" );
			return -1;
		}
		else
		{
			_tprintf( _T("FINISHED\n\n") );
		}
		break;
	case 'm':
		if( multiplex( audio_first ? file_name2 : file_name, audio_first ? file_name : file_name2 ) == -1 )
		{
			_tprintf( _T("! ERROR: Multiplexing failed, exiting\n\n") );
			system( "pause" );
			return -1;
		}
		else
		{
			_tprintf( _T("FINISHED\n\n") );
		}
		break;
	}

#ifdef _DEBUG
	system( "pause" );
#endif

	return 0;
}
