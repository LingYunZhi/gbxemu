#include <stdio.h>
#include <string.h>


int main() {
  for( int len = 8; len <= 63; len++ ) {
    char *str = new char[len+1];
    str[len] = '\0';
    for( int c = 0x20; c < 0x7F; c++ ) {
      memset( str, c, len );
      printf( "%s\n", str );
    }
    delete[] str;
  }
}
