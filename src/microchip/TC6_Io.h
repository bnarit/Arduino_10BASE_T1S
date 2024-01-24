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

  TC6_Io(
#if defined(ARDUINO_ARCH_AVR)
         SPIClass & spi,
#else
         HardwareSPI & spi,
#endif
         int const cs_pin,
         int const reset_pin,
         int const irq_pin);

  virtual bool begin();

  void onInterrupt();

  bool is_interrupt_active();

  void release_interrupt();

  bool spi_transaction(uint8_t const *pTx, uint8_t *pRx, uint16_t const len);


private:
#if defined(ARDUINO_ARCH_AVR)
  SPIClass & _spi;
#else
  HardwareSPI & _spi;
#endif
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
