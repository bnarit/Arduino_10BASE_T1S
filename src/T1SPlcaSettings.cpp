/*
 *  This file is part of the Arduino_10BASE_T1S library.
 *
 *  Copyright (c) 2024 Arduino SA
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/.
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
           "PLCA\n" \
           "\tnode id     : %d%s\n" \
           "\tnode count  : %d\n" \
           "\tburst count : %d\n" \
           "\tburst timer : %d",
           _node_id,
           (_node_id == 0) ? " (PLCA Coordinator)" : "",
           _node_count,
           _burst_count,
           _burst_timer);

  return p.write(msg);
}
