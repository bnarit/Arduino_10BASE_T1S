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

#include <stdint.h>

#include <deque>
#include <vector>

#include "lib/liblwip/include/lwip/udp.h"
#include "lib/liblwip/include/lwip/netif.h"
#include "lib/liblwip/include/lwip/ip_addr.h"

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
 * CLASS DECLARATION
 **************************************************************************************/

class TC6_Arduino_10BASE_T1S_UDP : public Arduino_10BASE_T1S_UDP
{
public:
  TC6_Arduino_10BASE_T1S_UDP(TC6_Io * tc6_io);

  virtual ~TC6_Arduino_10BASE_T1S_UDP();


  virtual bool begin(IPAddress const ip_addr,
                     IPAddress const network_mask,
                     IPAddress const gateway,
                     MacAddress const mac_addr,
                     T1SPlcaSettings const t1s_plca_settings,
                     T1SMacSettings const t1s_mac_settings) override;


  virtual void service() override;

  void digitalWrite(bool dioa0, bool dioa1, bool dioa2);

  bool getPlcaStatus(TC6LwIP_On_PlcaStatus on_plca_status);
  bool enablePlca();

  bool sendWouldBlock();

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
  TC6_Io * _tc6_io;
  TC6LwIP_t _lw;
  T1SPlcaSettings _t1s_plca_settings;

  /* arduino:UDP */
  struct udp_pcb * _udp_pcb;

  IPAddress _remote_ip;
  uint16_t _remote_port;
  std::deque<uint8_t> _rx_data;

  IPAddress _send_to_ip;
  uint16_t _send_to_port;
  std::vector<uint8_t> _tx_data;
};

/**************************************************************************************
 * NAMESPACE
 **************************************************************************************/

} /* TC6 */
