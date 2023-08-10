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

#include <Arduino.h>

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

class TC6_Io_Base
{
public:
  virtual ~TC6_Io_Base() { }

  virtual bool init(uint8_t pMac[6]) = 0;

  virtual bool is_interrupt_active() = 0;
  virtual void release_interrupt() = 0;

  virtual bool spi_transaction(uint8_t const * pTx, uint8_t * pRx, uint16_t const len) = 0;
  virtual bool get_mac_address(uint8_t * p_mac) = 0;

  virtual void onInterrupt() = 0;

  uint32_t get_tick() const {
    return millis();
  }
};
