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

#if defined(ARDUINO_ARCH_AVR)
# include "Udp.h"
# include "IPAddress.h"
#else
# include <api/Udp.h>
# include <api/IPAddress.h>
#endif

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
