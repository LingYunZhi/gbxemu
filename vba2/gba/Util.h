#pragma once

#include "System.h"

enum IMAGE_TYPE {
  IMAGE_UNKNOWN = -1,
  IMAGE_GBA     = 0,
  //IMAGE_GB      = 1
};

// save game
typedef struct {
  void *address;
  int size;
} variable_desc;

void utilPutDword(u8 *p, u32 value);
void utilPutWord(u8 *p, u16 value);
bool utilWriteBMPFile(const char *, int, int, u16 *);
void utilApplyIPS(const char *ips, u8 **rom, int *size);
bool utilIsGBAImage(const char *);
bool utilIsGBImage(const char *);
bool utilIsGzipFile(const char *);
void utilStripDoubleExtension(const char *, char *);
IMAGE_TYPE utilFindType(const char *);

bool utilFileExists( const char *filename );
