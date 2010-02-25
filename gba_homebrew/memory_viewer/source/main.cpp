#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <stdio.h>
#include <stdlib.h>


void printMemory( u32 address ) {
  iprintf(CON_CLS());
  iprintf("Address: %.8X\n\n     ", address);

  // print first row
  int x;
  for( x = 0; x < 8; x++ ) {
    iprintf("+%.2X",x);
  }

  // print data
  u8 *mem = (u8 *)address;
  for( u32 y = 0; y < 16; y++ ) {
    iprintf("\n%.4X:",(address+(y<<3))&0xFFFF);
    x = 8;
    while( x-- ) {
      iprintf(" %.2X", *(mem++));
    }
  }
}


int main(void) {
  u32 address = 0;
  // the vblank interrupt must be enabled for VBlankIntrWait() to work
  // since the default dispatcher handles the bios flags no vblank handler
  // is required
  irqInit();
  irqEnable(IRQ_VBLANK);

  consoleDemoInit();

  bool update = true;
  while (1) {
    scanKeys();
    setRepeat( 20, 5 );
    const u16 keys = keysDownRepeat();
    if( keys & KEY_DOWN ) {
      address+=8;
      update = true;
    }
    if( keys & KEY_UP ) {
      address-=8;
      update = true;
    }
    if( keys & KEY_RIGHT ) {
      address+=256;
      update = true;
    }
    if( keys & KEY_LEFT ) {
      address-=256;
      update = true;
    }
    if( keys & KEY_R ) {
      address+=0x01000000;
      address&=0xFF000000;
      update = true;
    }
    if( keys & KEY_L ) {
      address-=0x01000000;
      address&=0xFF000000;
      update = true;
    }
    if( keys & KEY_A ) {
      address=0;
      update = true;
    }
    if( keys & KEY_B ) {
      address=0x01000000;
      update = true;
    }
    if( update ) {
      update = false;
      printMemory( address );
    }
    VBlankIntrWait();
  }
}
