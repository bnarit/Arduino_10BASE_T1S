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

#include <list>
#include <deque>
#include <vector>
#include <memory>

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
  void bufferSize(int size) {
    _rx_pkt_list_size = size;
  }

private:
  /* LWIP */
  struct udp_pcb * _udp_pcb;

  /* UDP TRANSMISSION */
  IPAddress _send_to_ip;
  uint16_t _send_to_port;
  std::vector<uint8_t> _tx_data;
  int _rx_pkt_list_size = 10;
  /* UDP RECEPTION */
  class UdpRxPacket
  {
  private:
    IPAddress const _remote_ip;
    uint16_t const _remote_port;
    size_t const _rx_data_len;
    std::deque<uint8_t> _rx_data;

  public:
    UdpRxPacket(
      IPAddress const remote_ip,
      uint16_t const remote_port,
      uint8_t const * p_data,
      size_t const data_len)
      : _remote_ip(remote_ip)
      , _remote_port(remote_port)
      , _rx_data_len(data_len)
      , _rx_data(p_data, p_data + data_len)
    {
    }

    typedef std::shared_ptr<UdpRxPacket> SharedPtr;

    IPAddress remoteIP() const { return _remote_ip; }
    uint16_t remotePort() const { return _remote_port; }
    size_t totalSize() const { return _rx_data_len; }

    int available()
    {
      return _rx_data.size();
    }

    int read()
    {
      uint8_t const data = _rx_data.front();
      _rx_data.pop_front();
      return data;
    }

    int read(unsigned char* buffer, size_t len)
    {
      size_t bytes_read = 0;
      for (; bytes_read < len && !_rx_data.empty(); bytes_read++)
      {
        buffer[bytes_read] = _rx_data.front();
        _rx_data.pop_front();
      }
      return bytes_read;
    }

    int read(char* buffer, size_t len)
    {
      return read((unsigned char*)buffer, len);
    }

    int peek()
    {
      return _rx_data.front();
    }
  };
  std::list<UdpRxPacket::SharedPtr> _rx_pkt_list;
  UdpRxPacket::SharedPtr _rx_pkt;
};
