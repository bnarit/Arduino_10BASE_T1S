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

#include <deque>
#include <vector>

#include <api/Udp.h>
#include <api/IPAddress.h>

#include "lib/liblwip/include/lwip/udp.h"
#include "lib/liblwip/include/lwip/ip_addr.h"

#include "MacAddress.h"
#include "T1SMacSettings.h"
#include "T1SPlcaSettings.h"

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

class Arduino_10BASE_T1S_UDP : public UDP
{
public:
           Arduino_10BASE_T1S_UDP();
  virtual ~Arduino_10BASE_T1S_UDP();


  /* arduino:UDP */
  virtual uint8_t begin(uint16_t port) override;
  virtual void stop() override;

  virtual int beginPacket(IPAddress ip, uint16_t port) override;
  virtual int beginPacket(const char *host, uint16_t port) override;
  virtual int endPacket() override;
  virtual size_t write(uint8_t data) override;
  virtual size_t write(const uint8_t * buffer, size_t size) override;

  virtual int parsePacket() override;
  virtual int available() override;
  virtual int read() override;
  virtual int read(unsigned char* buffer, size_t len) override;
  virtual int read(char* buffer, size_t len) override;
  virtual int peek() override;
  virtual void flush() override;

  virtual IPAddress remoteIP() override;
  virtual uint16_t remotePort() override;


  /* This function MUST not be called from the user of this library,
   * it's used for internal purposes only.
   */
  void onUdpRawRecv(struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, uint16_t port);


private:
  struct udp_pcb * _udp_pcb;

  IPAddress _remote_ip;
  uint16_t _remote_port;
  std::deque<uint8_t> _rx_data;

  IPAddress _send_to_ip;
  uint16_t _send_to_port;
  std::vector<uint8_t> _tx_data;
};
