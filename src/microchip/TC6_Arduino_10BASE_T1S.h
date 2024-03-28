/*
 *  This file is part of the Arduino_10BASE_T1S library.
 *
 *  Copyright (c) 2024 Arduino SA
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include "../Arduino_10BASE_T1S_PHY_Interface.h"

#include <stdint.h>

#include "lib/liblwip/include/lwip/netif.h"

#include "microchip/lib/libtc6/inc/tc6.h"

#include "TC6_Io.h"

/**************************************************************************************
 * TYPEDEF
 **************************************************************************************/

typedef void (*TC6LwIP_On_PlcaStatus)(bool success, bool plcaStatus);

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
  TC6::TC6_Io * io;
} TC6LwIP_t;

/**************************************************************************************
 * NAMESPACE
 **************************************************************************************/

namespace TC6
{

/**************************************************************************************
 * TYPEDEF
 **************************************************************************************/

enum class DIO { A0, A1 };

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

class TC6_Arduino_10BASE_T1S : public Arduino_10BASE_T1S_PHY_Interface
{
public:
  TC6_Arduino_10BASE_T1S(TC6_Io * tc6_io);

  virtual ~TC6_Arduino_10BASE_T1S();


  virtual bool begin(IPAddress const ip_addr,
                     IPAddress const network_mask,
                     IPAddress const gateway,
                     MacAddress const mac_addr,
                     T1SPlcaSettings const t1s_plca_settings,
                     T1SMacSettings const t1s_mac_settings) override;


  virtual void service() override;

  void digitalWrite(DIO const dio, bool const value);

  bool getPlcaStatus(TC6LwIP_On_PlcaStatus on_plca_status);
  bool enablePlca();

  bool sendWouldBlock();


private:
  TC6_Io * _tc6_io;
  TC6LwIP_t _lw;
  T1SPlcaSettings _t1s_plca_settings;

  void digitalWrite_A0(bool const value);
  void digitalWrite_A1(bool const value);
};

/**************************************************************************************
 * NAMESPACE
 **************************************************************************************/

} /* TC6 */
