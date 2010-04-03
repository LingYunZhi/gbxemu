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


#ifndef GRAPHICSCHIP_H
#define GRAPHICSCHIP_H


#include "genericmemory.h"


class GraphicsChip
{
public:
  GraphicsChip();
  ~GraphicsChip();

private:
  // palette RAM [1 KiB]
  GenericMemory palram;
  // video RAM [96 KiB]
  GenericMemory vram;
  // object attribute memory [1 KiB]
  GenericMemory oam;
};


#endif // GRAPHICSCHIP_H
