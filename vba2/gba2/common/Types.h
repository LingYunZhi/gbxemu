/*  VisualBoyAdvance 2
    Copyright (C) 2009-2010  VBA development team

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef TYPES_H
#define TYPES_H


#ifndef NULL
  #define NULL 0
#endif


#ifdef _MSC_VER
  typedef unsigned __int8   u8;
  typedef unsigned __int16 u16;
  typedef unsigned __int32 u32;
  typedef unsigned __int64 u64;
  typedef   signed __int8   s8;
  typedef   signed __int16 s16;
  typedef   signed __int32 s32;
  typedef   signed __int64 s64;
#else
  #include <stdint.h>
  typedef uint8_t   u8;
  typedef uint16_t u16;
  typedef uint32_t u32;
  typedef uint64_t u64;
  typedef  int8_t   s8;
  typedef  int16_t s16;
  typedef  int32_t s32;
  typedef  int64_t s64;
#endif


#define BIT0   0x1
#define BIT1   0x2
#define BIT2   0x4
#define BIT3   0x8
#define BIT4   0x10
#define BIT5   0x20
#define BIT6   0x40
#define BIT7   0x80
#define BIT8   0x100
#define BIT9   0x200
#define BIT10  0x400
#define BIT11  0x800
#define BIT12  0x1000
#define BIT13  0x2000
#define BIT14  0x4000
#define BIT15  0x8000


#endif // TYPES_H
