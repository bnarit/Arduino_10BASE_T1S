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
           "\tMAC\n" \
           "\t\tpromisc. mode : %d\n" \
           "\t\ttx cut through: %d\n" \
           "\t\trx cut through: %d",
           _mac_promiscuous_mode,
           _mac_tx_cut_through,
           _mac_rx_cut_through);

  return p.write(msg);
}
