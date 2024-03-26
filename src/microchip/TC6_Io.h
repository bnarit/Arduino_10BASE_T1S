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

#include <SPI.h>

/**************************************************************************************
 * NAMESPACE
 **************************************************************************************/

namespace TC6
{

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

class TC6_Io
{
public:
  static size_t constexpr
  MAC_SIZE = 6;
  static uint8_t constexpr
  FALLBACK_MAC[MAC_SIZE] = { 0x00u, 0x80u, 0xC2u, 0x00u, 0x01u, 0xCCu };

  TC6_Io(HardwareSPI & spi,
         int const cs_pin,
         int const reset_pin,
         int const irq_pin);

  virtual bool begin();

  void onInterrupt();

  bool is_interrupt_active();

  void release_interrupt();

  bool spi_transaction(uint8_t const *pTx, uint8_t *pRx, uint16_t const len);


private:
  HardwareSPI & _spi;
  int const _cs_pin;
  int const _reset_pin;
  int const _irq_pin;
  volatile uint8_t _int_in;
  volatile uint8_t _int_out;
  volatile uint8_t _int_reported;
};

/**************************************************************************************
 * NAMESPACE
 **************************************************************************************/

} /* TC6 */
