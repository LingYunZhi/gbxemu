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


#ifndef CVERBOSE_H
#define CVERBOSE_H


#include "common/Types.h"


class CVerbose
{
public:
  CVerbose() { m_verboseMask = 0; }

  // add message to verbose channel
  void log( const u32 channel, const char *message, ... ) {}

  // modify the verbose channels to be output/logged
  void setVerboseMask( u32 mask ) { m_verboseMask = mask; }
  u32 getVerboseMask() { return m_verboseMask; }

  // ### verbose channels ###
  // software interrupt
  static const u32 VERBOSE_SWI              = 0x00000001;
  // unaligned memory access
  static const u32 VERBOSE_UNALIGNED_MEMORY = 0x00000002;
  // write to bad memory area
  static const u32 VERBOSE_ILLEGAL_WRITE    = 0x00000004;
  // read from bad memory area
  static const u32 VERBOSE_ILLEGAL_READ     = 0x00000008;
  // first DMA channel
  static const u32 VERBOSE_DMA0             = 0x00000010;
  // second DMA channel
  static const u32 VERBOSE_DMA1             = 0x00000020;
  // third DMA channel
  static const u32 VERBOSE_DMA2             = 0x00000040;
  // fourth DMA channel
  static const u32 VERBOSE_DMA3             = 0x00000080;
  // something else
  static const u32 VERBOSE_OTHER            = 0x00000100;

 private:
   u32 m_verboseMask;
};


#endif // CVERBOSE_H
