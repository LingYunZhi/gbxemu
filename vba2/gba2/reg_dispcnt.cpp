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


#include "reg_dispcnt.h"


#include <assert.h>


Reg_DISPCNT::Reg_DISPCNT()
{
  m_data = 0;
  update();
}


void Reg_DISPCNT::update() {
  bg_mode = m_data & 7;
  assert( bg_mode <= 5 );
  frame = (m_data & BIT4) != 0;
  obj_not_while_hblank = (m_data & BIT5) != 0;
  obj_mapping_1d = (m_data & BIT6) != 0;
  forced_blank = (m_data & BIT7) != 0;
  displayBG0 = (m_data & BIT8) != 0;
  displayBG1 = (m_data & BIT9) != 0;
  displayBG2 = (m_data & BIT10) != 0;
  displayBG3 = (m_data & BIT11) != 0;
  displayOBJ = (m_data & BIT12) != 0;
  displayWIN0 = (m_data & BIT13) != 0;
  displayWIN1 = (m_data & BIT14) != 0;
  displayOBJWIN = (m_data & BIT15) != 0;
}
