/*
 * This file is part of the Arduino_10BASE_T1S library.
 * Copyright (c) 2023 Arduino SA.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include "T1SPlcaSettings.h"

/**************************************************************************************
 * CTOR/DTOR
 **************************************************************************************/

T1SPlcaSettings::T1SPlcaSettings(uint8_t const node_id,
                                 uint8_t const node_count,
                                 uint8_t const burst_count,
                                 uint8_t const burst_timer)
: _node_id{node_id}
, _node_count{node_count}
, _burst_count{burst_count}
, _burst_timer{burst_timer}
{

}

/**************************************************************************************
 * PUBLIC MEMBER FUNCTIONS
 **************************************************************************************/

size_t T1SPlcaSettings::printTo(Print & p) const
{
  char msg[128] = {0};
  snprintf(msg,
           sizeof(msg),
           "\tPLCA\n" \
           "\t\tnode id     : %d\n" \
           "\t\tnode count  : %d\n" \
           "\t\tburst count : %d\n" \
           "\t\tburst timer : %d",
           _node_id,
           _node_count,
           _burst_count,
           _burst_timer);

  return p.write(msg);
}
