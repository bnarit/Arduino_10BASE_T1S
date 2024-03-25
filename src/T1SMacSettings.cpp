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

#include "T1SMacSettings.h"

/**************************************************************************************
 * CTOR/DTOR
 **************************************************************************************/

T1SMacSettings::T1SMacSettings(bool const mac_promiscuous_mode,
                               bool const mac_tx_cut_through,
                               bool const mac_rx_cut_through)
: _mac_promiscuous_mode{mac_promiscuous_mode}
, _mac_tx_cut_through{mac_tx_cut_through}
, _mac_rx_cut_through{mac_rx_cut_through}
{

}

/**************************************************************************************
 * PUBLIC MEMBER FUNCTIONS
 **************************************************************************************/

size_t T1SMacSettings::printTo(Print & p) const
{
  char msg[128] = {0};

  snprintf(msg,
           sizeof(msg),
           "MAC\n" \
           "\tpromisc. mode : %d\n" \
           "\ttx cut through: %d\n" \
           "\trx cut through: %d",
           _mac_promiscuous_mode,
           _mac_tx_cut_through,
           _mac_rx_cut_through);

  return p.write(msg);
}
