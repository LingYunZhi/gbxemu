// gbaSaveTypeDetect.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

using namespace std;

#ifdef _UNICODE
# define _tstring wstring
#else
# define _tstring string
#endif

int _tmain(int argc, _TCHAR* argv[])
{
    if( argc < 2 ) return -1;
    assert( sizeof( unsigned int ) == 4 );
    const unsigned int EEPR = 'E' | ( 'E' << 8 ) | ( 'P' << 16 ) | ( 'R' << 24 );
    const unsigned int SRAM = 'S' | ( 'R' << 8 ) | ( 'A' << 16 ) | ( 'M' << 24 );
    const unsigned int FLAS = 'F' | ( 'L' << 8 ) | ( 'A' << 16 ) | ( 'S' << 24 );
    const TCHAR *answer_str[] = {
        _T( "This cartridge has probably no backup media." ),
        _T( "This cartridge uses EEPROM." ),
        _T( "This cartridge uses SRAM." ),
        _T( "This cartridge uses FLASH (64 KiB)." ),
        _T( "This cartridge uses FLASH (128 KiB)." )
    };
    int answer;

    const size_t max_rom_size = 32 * 1024 * 1024;
    unsigned int *rom = NULL;
    rom = (unsigned int *)malloc(max_rom_size);
    if( rom == NULL ) return -1;
    unsigned int check;
    int address_max;
    char temp[11]; temp[10] = '\0';

    int nfiles = argc - 1;

    FILE *f;
    struct _stat buf;

    _tstring outfilename;
    outfilename = argv[0];
    outfilename.append( _T( ".txt" ) );

    FILE *fout = NULL;
    fout = _tfopen( outfilename.c_str(), _T( "a" ) );
    if( fout == NULL ) return -1;

    for( int i = 0; i < nfiles; i++ ) {
        f = NULL;
        _tstat( argv[i+1], &buf );
        if( buf.st_size > max_rom_size ) continue;
        f = _tfopen( argv[i+1], _T( "rb" ) );
        if( f == NULL ) continue;
        fread( rom, 1, buf.st_size, f );

        // check routine:
        address_max = ( buf.st_size / 4 ) - 3;
        answer = 0;


        for( int address = 0; address < address_max; address++ ) {
            check = rom[address];

            if( EEPR == check ) {
                memcpy( temp, &rom[address], 10 );
                if( 0 == strncmp( temp, "EEPROM_V", 8 ) ) {
                    answer = 1;
                    break;
                }
            }

            if( SRAM == check ) {
                memcpy( temp, &rom[address], 10 );
                if( ( 0 == strncmp( temp, "SRAM_V", 6 ) ) || ( 0 == strncmp( temp, "SRAM_F_V", 8 ) ) ) {
                    answer = 2;
                    break;
                }
            }

            if( FLAS == check ) {
                memcpy( temp, &rom[address], 10 );
                if( ( 0 == strncmp( temp, "FLASH_V", 7 ) ) || ( 0 == strncmp( temp, "FLASH512_V", 10 ) ) ) {
                    answer = 3;
                    break;
                }
                if( 0 == strncmp( temp, "FLASH1M_V", 9 ) ) {
                    answer = 4;
                    break;
                }
            }
        }

        _ftprintf( fout, _T( "File: %s\nResult: %s\n\n" ), argv[i+1], answer_str[answer] );
        fclose( f );
    }
    
    fclose( fout );
    free( rom );

	return 0;
}

