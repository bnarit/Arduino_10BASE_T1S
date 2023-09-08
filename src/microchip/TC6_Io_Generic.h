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

#include "TC6_Io_Base.h"

#include <api/HardwareSPI.h>

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

class TC6_Io_Generic : public TC6_Io_Base
{
public:
  TC6_Io_Generic(HardwareSPI & spi,
                 int const cs_pin,
                 int const reset_pin,
                 int const irq_pin);
  virtual ~TC6_Io_Generic() { }

  virtual bool init() override;

  virtual bool is_interrupt_active() override;
  virtual void release_interrupt() override;

  virtual bool spi_transaction(uint8_t const * pTx, uint8_t * pRx, uint16_t const len) override;

  virtual void onInterrupt() override;


private:
  HardwareSPI & _spi;
  int const _cs_pin;
  int const _reset_pin;
  int const _irq_pin;
  volatile uint8_t _int_in;
  volatile uint8_t _int_out;
  volatile uint8_t _int_reported;
};
