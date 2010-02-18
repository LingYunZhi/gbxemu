#include <stdio.h>
#include <assert.h>


class Digit {
public:
  Digit( char *alphabet, int alphabet_size, Digit *carryDigit );
  char getChar();
  void increment();
  bool unhandledCarryOccured;

private:
  int m_value;
  char *m_alphabet;
  int m_alphabet_size;
  Digit *m_carryDigit; // 0-pointer means no carry
};


Digit::Digit( char *alphabet, int alphabet_size, Digit *carryDigit ) {
  m_value = 0;
  assert( alphabet != 0 );
  m_alphabet = alphabet;
  assert( alphabet_size != 0 );
  m_alphabet_size = alphabet_size;
  m_carryDigit = carryDigit;
  unhandledCarryOccured = false;
}


char Digit::getChar() {
  assert( m_value < m_alphabet_size );
  return m_alphabet[m_value];
}


void Digit::increment() {
  m_value++;
  if( m_value >= m_alphabet_size ) {
    // carry occured
    m_value = 0;
    if( m_carryDigit ) {
      m_carryDigit->increment();
    } else {
      unhandledCarryOccured = true;
    }
  }
}


void printDigits( char *str, Digit **digits, int nDigits ) {
  for( int i = 0; i < nDigits; i++ ) {
    str[i] = digits[i]->getChar();
  }
}


int main() {
  char alphabet[] = "abcdefghijklmnopqrstuvwxyz";
  int alphabet_size = sizeof( alphabet ) - 1;


  const int nDigits = 8;
  Digit *digits[nDigits];
  for( int i = 0; i < nDigits; i++ ) {
    digits[i] = new Digit( alphabet, alphabet_size, i ? digits[i-1] : (Digit *)0 );
  }

  Digit *first = digits[nDigits-1];
  Digit *last = digits[0];

  char *str = new char[nDigits+1];
  str[nDigits] = '\0';
  FILE *f = fopen( "passwords.txt", "w" );
  assert( f != NULL );

  do {
    printDigits( str, digits, nDigits );
    fprintf( f, "%s\n", str );

    first->increment();
  } while( !last->unhandledCarryOccured );

  fclose( f );
  delete[] str;

  printf( "done\n" );
  return 0;
}
