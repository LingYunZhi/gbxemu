#include "../System.h"
#include <stdlib.h>
#include <memory.h>

/*
 * Thanks to Kawaks' Mr. K for the code

   Incorporated into vba by Anthony Di Franco
*/

static u8 *frm1 = NULL;
static u8 *frm2 = NULL;
static u8 *frm3 = NULL;

extern int RGB_LOW_BITS_MASK;
extern u32 qRGB_COLOR_MASK[2];

static void Init()
{
  frm1 = (u8 *)calloc(322*242,4);
  // 1 frame ago
  frm2 = (u8 *)calloc(322*242,4);
  // 2 frames ago
  frm3 = (u8 *)calloc(322*242,4);
  // 3 frames ago
}

void InterframeCleanup()
{
  if(frm1)
    free(frm1);
  if(frm2)
    free(frm2);
  if(frm3)
    free(frm3);
  frm1 = frm2 = frm3 = NULL;
}

void SmartIB(u8 *srcPtr, u32 srcPitch, int width, int height)
{
  if(frm1 == NULL) {
    Init();
  }

  u16 colorMask = ~RGB_LOW_BITS_MASK;

  u16 *src0 = (u16 *)srcPtr;
  u16 *src1 = (u16 *)frm1;
  u16 *src2 = (u16 *)frm2;
  u16 *src3 = (u16 *)frm3;

  int sPitch = srcPitch >> 1;

  int pos = 0;
  for (int j = 0; j < height;  j++)
    for (int i = 0; i < sPitch; i++) {
      u16 color = src0[pos];
      src0[pos] =
        (src1[pos] != src2[pos]) &&
        (src3[pos] != color) &&
        ((color == src2[pos]) || (src1[pos] == src3[pos]))
        ? (((color & colorMask) >> 1) + ((src1[pos] & colorMask) >> 1)) :
        color;
      src3[pos] = color; /* oldest buffer now holds newest frame */
      pos++;
    }

  /* Swap buffers around */
  u8 *temp = frm1;
  frm1 = frm3;
  frm3 = frm2;
  frm2 = temp;
}

void SmartIB32(u8 *srcPtr, u32 srcPitch, int width, int height)
{
  if(frm1 == NULL) {
    Init();
  }

  u32 *src0 = (u32 *)srcPtr;
  u32 *src1 = (u32 *)frm1;
  u32 *src2 = (u32 *)frm2;
  u32 *src3 = (u32 *)frm3;

  u32 colorMask = 0xfefefe;

  int sPitch = srcPitch >> 2;
  int pos = 0;

  for (int j = 0; j < height;  j++)
    for (int i = 0; i < sPitch; i++) {
      u32 color = src0[pos];
      src0[pos] =
        (src1[pos] != src2[pos]) &&
        (src3[pos] != color) &&
        ((color == src2[pos]) || (src1[pos] == src3[pos]))
        ? (((color & colorMask) >> 1) + ((src1[pos] & colorMask) >> 1)) :
        color;
      src3[pos] = color; /* oldest buffer now holds newest frame */
      pos++;
    }

  /* Swap buffers around */
  u8 *temp = frm1;
  frm1 = frm3;
  frm3 = frm2;
  frm2 = temp;
}

void MotionBlurIB(u8 *srcPtr, u32 srcPitch, int width, int height)
{
  if(frm1 == NULL) {
    Init();
  }

  u16 colorMask = ~RGB_LOW_BITS_MASK;

  u16 *src0 = (u16 *)srcPtr;
  u16 *src1 = (u16 *)frm1;

  int sPitch = srcPitch >> 1;

  int pos = 0;
  for (int j = 0; j < height;  j++)
    for (int i = 0; i < sPitch; i++) {
      u16 color = src0[pos];
      src0[pos] =
        (((color & colorMask) >> 1) + ((src1[pos] & colorMask) >> 1));
      src1[pos] = color;
      pos++;
    }
}

void MotionBlurIB32(u8 *srcPtr, u32 srcPitch, int width, int height)
{
  if(frm1 == NULL) {
    Init();
  }

  u32 *src0 = (u32 *)srcPtr;
  u32 *src1 = (u32 *)frm1;

  u32 colorMask = 0xfefefe;

  int sPitch = srcPitch >> 2;
  int pos = 0;

  for (int j = 0; j < height;  j++)
    for (int i = 0; i < sPitch; i++) {
      u32 color = src0[pos];
      src0[pos] = (((color & colorMask) >> 1) +
                   ((src1[pos] & colorMask) >> 1));
      src1[pos] = color;
      pos++;
    }
}
