#ifndef CPICTURE_H
#define CPICTURE_H


#include "../common/Types.h"


/// 32 bit RGBA color as used in modern PC systems
typedef struct {
  u8 b; ///< blue  0-255
  u8 g; ///< green 0-255
  u8 r; ///< red   0-255
  u8 a; ///< alpha 0-255
} COLOR32;


/// A class to manage a picture in COLOR32 format
class CPicture
{
public:
    CPicture();
    ~CPicture();
    void create( unsigned int width, unsigned int height );
    COLOR32 &pixel( unsigned int x, unsigned int y ); ///< get a reference to the pixel at the specified coordinates
    COLOR32 *picture; ///< address of picture data

private:
    unsigned int size; ///< number of bytes that have been allocated at this memory address
    unsigned int width;
    unsigned int height;
};


#endif // CPICTURE_H
