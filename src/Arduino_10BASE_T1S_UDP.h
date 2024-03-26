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

#include <api/Udp.h>
#include <api/IPAddress.h>

#include "MacAddress.h"
#include "T1SMacSettings.h"
#include "T1SPlcaSettings.h"

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

class Arduino_10BASE_T1S_UDP : public UDP
{
public:
  virtual ~Arduino_10BASE_T1S_UDP() { }

  virtual bool begin(IPAddress const ip_addr,
                     IPAddress const network_mask,
                     IPAddress const gateway,
                     MacAddress const mac_addr,
                     T1SPlcaSettings const t1s_plca_settings,
                     T1SMacSettings const t1s_mac_settings) = 0;

  virtual void service() = 0;

  /* arduino:UDP */
  virtual uint8_t begin(uint16_t port) = 0;
  virtual void stop() = 0;

  virtual int beginPacket(IPAddress ip, uint16_t port) = 0;
  virtual int beginPacket(const char *host, uint16_t port) = 0;
  virtual int endPacket() = 0;
  virtual size_t write(uint8_t data) = 0;
  virtual size_t write(const uint8_t * buffer, size_t size) = 0;

  virtual int parsePacket() = 0;
  virtual int available() = 0;
  virtual int read() = 0;
  virtual int read(unsigned char* buffer, size_t len) = 0;
  virtual int read(char* buffer, size_t len) = 0;
  virtual int peek() = 0;
  virtual void flush() = 0;

  virtual IPAddress remoteIP() = 0;
  virtual uint16_t remotePort() = 0;
};
