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

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include "TC6_Io.h"

/**************************************************************************************
 * NAMESPACE
 **************************************************************************************/

namespace TC6
{

/**************************************************************************************
 * CONSTANTS
 **************************************************************************************/

static SPISettings const LAN865x_SPI_SETTING{8 * 1000 * 1000UL, MSBFIRST, SPI_MODE0};

/**************************************************************************************
 * STATIC MEMBER DEFINITION
 **************************************************************************************/

size_t const TC6_Io::MAC_SIZE;
uint8_t const TC6_Io::FALLBACK_MAC[TC6_Io::MAC_SIZE];

/**************************************************************************************
 * CTOR/DTOR
 **************************************************************************************/

TC6_Io::TC6_Io(
#if defined(ARDUINO_ARCH_AVR)
  SPIClass & spi,
#else
  HardwareSPI &spi,
#endif
  int const cs_pin,
  int const reset_pin,
  int const irq_pin)
  : _spi{spi}, _cs_pin{cs_pin}, _reset_pin{reset_pin}, _irq_pin{irq_pin}, _int_in{0}, _int_out{0}, _int_reported{0}
{

}

/**************************************************************************************
 * PUBLIC MEMBER FUNCTIONS
 **************************************************************************************/

bool TC6_Io::begin()
{
  digitalWrite(_cs_pin, HIGH);
  pinMode(_cs_pin, OUTPUT);

  pinMode(_reset_pin, OUTPUT);
  digitalWrite(_reset_pin, LOW);
  delay(100);
  digitalWrite(_reset_pin, HIGH);
  delay(100);

  _spi.begin();

  return true;
}

void TC6_Io::onInterrupt()
{
  _int_in++;
}

bool TC6_Io::is_interrupt_active()
{
  _int_reported = _int_in;
  return (_int_reported != _int_out);
}

void TC6_Io::release_interrupt()
{
  if (digitalRead(_irq_pin) == HIGH)
    _int_out = _int_reported;
}

bool TC6_Io::spi_transaction(uint8_t const *pTx, uint8_t *pRx, uint16_t const len)
{
  digitalWrite(_cs_pin, LOW);
  _spi.beginTransaction(LAN865x_SPI_SETTING);

  for (size_t b = 0; b < len; b++)
    pRx[b] = _spi.transfer(pTx[b]);

  _spi.endTransaction();
  digitalWrite(_cs_pin, HIGH);

  return true;
}

/**************************************************************************************
 * NAMESPACE
 **************************************************************************************/

} /* TC6 */
