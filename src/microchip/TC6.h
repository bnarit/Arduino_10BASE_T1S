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

#pragma once

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include "../Arduino_10BASE_T1S_UDP.h"

#include <cstdint>

#include <memory>

#include "lib/liblwip/include/lwip/netif.h"

#include "microchip/lib/libtc6/inc/tc6.h"

#include "../MacAddress.h"

#include "TC6_Io_Generic.h"

/**************************************************************************************
 * TYPEDEF
 **************************************************************************************/

typedef void (*TC6LwIP_On_PlcaStatus)(int8_t idx, bool success, bool plcaStatus);

typedef struct
{
  TC6_t *tc6;
  struct pbuf *pbuf;
  TC6LwIP_On_PlcaStatus pStatusCallback;
  uint16_t rxLen;
  bool rxInvalid;
  bool tc6NeedService;
} TC6Lib_t;

typedef struct
{
  char ipAddr[16];
  struct netif netint;
  uint8_t mac[6];
} LwIp_t;

typedef struct
{
  TC6Lib_t tc;
  LwIp_t ip;
  std::shared_ptr<TC6_Io_Base> io;
} TC6LwIP_t;

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

class TC6 : public Arduino_10BASE_T1S_UDP
{
public:
  TC6(std::shared_ptr<TC6_Io_Base> const tc6_io);
  virtual ~TC6();


  virtual bool begin(IPAddress const ip_addr,
                     T1SPlcaSettings const t1s_plca_settings,
                     T1SMacSettings const t1s_mac_settings) override;


  void service();

  bool getPlcaStatus(TC6LwIP_On_PlcaStatus on_plca_status);

  bool sendWouldBlock();

  MacAddress getMacAddr();


private:
  std::shared_ptr<TC6_Io_Base> const _tc6_io;
  int8_t _idx;

  TC6LwIP_t _lw;
};
