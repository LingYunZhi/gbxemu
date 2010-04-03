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


#ifndef REG_DISPCNT_H
#define REG_DISPCNT_H


#include "common/Types.h"
#include "common/genericioregister.h"


class Reg_DISPCNT : protected GenericIORegister
{
public:
  Reg_DISPCNT();
  u8 bg_mode; // 0 - 5
  bool frame; // 0 - 1
  bool obj_not_while_hblank; // false - true
  bool obj_mapping_1d; // false - true
  bool forced_blank; // false - true
  bool displayBG0; // false - true
  bool displayBG1; // false - true
  bool displayBG2; // false - true
  bool displayBG3; // false - true
  bool displayOBJ; // false - true
  bool displayWIN0; // false - true
  bool displayWIN1; // false - true
  bool displayOBJWIN; // false - true

private:
  void update();
};


#endif // REG_DISPCNT_H
