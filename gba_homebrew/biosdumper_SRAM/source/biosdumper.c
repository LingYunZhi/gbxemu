/*---------------------------------------------------------------------------------


  This is the modified BIOS reading example from devkitPro/ARM.
  
  I (Spacy) made it write the BIOS to SRAM, instead of a file.
  This way, I can dump it with my EZ-3in1-Addon for my NDS lite.
  You have to strip the created .sav file (dumped with rudlophs ExpLoader or AKAIO)
  down to 16 Kilo Bytes and save it as gba.bin.
  The CRC32 checksum should be A6473709.

---------------------------------------------------------------------------------*/


#include <gba.h>
#include <stdio.h>
#include <stdlib.h>


//---------------------------------------------------------------------------------
void waitForever() {
//---------------------------------------------------------------------------------
	while (1)
		VBlankIntrWait();
}

//---------------------------------------------------------------------------------
// Program entry point
//---------------------------------------------------------------------------------
int main(void) {
//---------------------------------------------------------------------------------


	// the vblank interrupt must be enabled for VBlankIntrWait() to work
	// since the default dispatcher handles the bios flags no vblank handler
	// is required
	irqInit();
	irqEnable(IRQ_VBLANK);

	consoleDemoInit();

	iprintf("GBA Bios Dumper\n\n");

	u32 *bios = (u32 *)malloc(0x4000);
	
	if ( bios ) {
		iprintf("Memory allocated\n");
	} else {
		iprintf("Memory allocation failure!\n");
		waitForever();
	}

	int i;
	
	iprintf("dumping ");

	for (i=0; i<0x4000; i+=4)
	{
		// The MidiKey2Freq bios call allows us to read from bios
		// the lower bits are inaccurate, so just get it four times :)
		u32 a = MidiKey2Freq((WaveData *)(i-4), 180-12, 0) * 2;
		u32 b = MidiKey2Freq((WaveData *)(i-3), 180-12, 0) * 2;
		u32 c = MidiKey2Freq((WaveData *)(i-2), 180-12, 0) * 2;
		u32 d = MidiKey2Freq((WaveData *)(i-1), 180-12, 0) * 2;

		// rebuild a 32bit word from the 4 words we read
		u32 abcd =	( a & 0xff000000 ) |
					( d & 0xff000000 ) >> 8 |
					( c & 0xff000000 ) >> 16 |
					( b & 0xff000000 ) >> 24;
		bios[i/4] = abcd;

		//print a dot every 256 bytes
		if ( (i & 0xff) == 0 ) iprintf("."); 

	}
	
	iprintf("\nBios dumped, transfering it to SRAM\n");
	
	u16 *WAITCNT = (u16*)0x4000204;
	// set SRAM waitstate
	*WAITCNT |= 3; // SRAM -> 8 cycles
	
	u8 *biosByte = (u8 *)bios;
	u8 *sram = (u8*)SRAM;
	for( i = 0; i < 0x4000; i++ ) {
		*sram = *biosByte;
		sram++;
		biosByte++;
	}
	
	
	iprintf("\ndone :)\n");
	
	waitForever();

	return 0;
}


