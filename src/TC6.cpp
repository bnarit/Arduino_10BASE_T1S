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

#include "TC6.h"

#include <map>

#include "tc6-lwip.h"

/**************************************************************************************
 * GLOBAL CONSTANTS
 **************************************************************************************/

static std::map<int8_t, TC6::OnPlcaStatusFunc> on_plca_status_func_map;

/**************************************************************************************
 * CTOR/DTOR
 **************************************************************************************/

TC6::TC6()
: _idx(-1)
{

}

TC6::~TC6()
{
  on_plca_status_func_map.erase(on_plca_status_func_map.find(_idx));
}

bool TC6::begin(uint8_t const ip[4],
                bool const enable_plca,
                uint8_t const node_id,
                uint8_t const node_count,
                uint8_t const burst_count,
                uint8_t const burst_timer,
                bool const mac_promiscuous_mode,
                bool const mac_tx_cut_through,
                bool const mac_rx_cut_through)
{
  _idx = TC6LwIP_Init(ip,
                      enable_plca,
                      node_id,
                      node_count,
                      burst_count,
                      burst_timer,
                      mac_promiscuous_mode,
                      mac_tx_cut_through,
                      mac_rx_cut_through);
  return (_idx >= 0);
}

void TC6::service()
{
  TC6LwIP_Service();
}

bool TC6::getPlcaStatus(OnPlcaStatusFunc on_plca_status)
{
  on_plca_status_func_map[_idx] = on_plca_status;

  return TC6LwIP_GetPlcaStatus(_idx,
                               +[](int8_t idx, bool success, bool plca_status)
                               {
                                 auto const citer = on_plca_status_func_map.find(idx);

                                 if (citer == on_plca_status_func_map.end())
                                   return;

                                  citer->second(success, plca_status);
                               });
}

bool TC6::sendWouldBlock()
{
  return TC6LwIP_SendWouldBlock(_idx);
}

TC6::MACAddr TC6::getMacAddr()
{
  MACAddr mac = {0};
  TC6LwIP_GetMac(_idx, mac.data());
  return mac;
}