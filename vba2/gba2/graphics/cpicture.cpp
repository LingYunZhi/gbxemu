#include "cpicture.h"
#include <assert.h>


CPicture::CPicture()
{
  picture = NULL;
  size = 0;
}


CPicture::~CPicture()
{
  if( picture != NULL ) {
    delete [] picture;
  }
}


void CPicture::create( unsigned int width, unsigned int height ) {
  const unsigned int newSize = width * height;
  assert( newSize != 0 );
  // no need to reallocate memory if the size is the same
  if( newSize != size ) {
    if( picture != NULL ) {
      delete [] picture;
    }
    picture = new COLOR32[newSize];
    assert( picture != NULL ); // catch out of memory
  }
  size = newSize;
  this->width = width;
  this->height = height;
}


COLOR32 &CPicture::pixel( unsigned int x, unsigned int y ) {
  return picture[ x + ( y * width ) ];
}
