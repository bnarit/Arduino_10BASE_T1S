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

#include "TC6_Io_Generic.h"

#include "lib/libtc6/inc/tc6.h"

/**************************************************************************************
 * CONSTANTS
 **************************************************************************************/

static SPISettings const LAN865x_SPI_SETTING{8*1000*1000UL, MSBFIRST, SPI_MODE0};

/**************************************************************************************
 * CTOR/DTOR
 **************************************************************************************/

TC6_Io_Generic::TC6_Io_Generic(HardwareSPI & spi, HardwareI2C & wire, int const cs_pin, int const reset_pin, int const irq_pin)
: _spi{spi}
, _wire{wire}
, _cs_pin{cs_pin}
, _reset_pin{reset_pin}
, _irq_pin{irq_pin}
, _int_in{0}
, _int_out{0}
, _int_reported{0}
{

}

/**************************************************************************************
 * PUBLIC MEMBER FUNCTIONS
 **************************************************************************************/

bool TC6_Io_Generic::init()
{
  digitalWrite(_cs_pin, HIGH);
  pinMode(_cs_pin, OUTPUT);

  pinMode(_reset_pin, OUTPUT);
  digitalWrite(_reset_pin, LOW);
  delay(100);
  digitalWrite(_reset_pin, HIGH);
  delay(100);

  _spi.begin();
  _wire.begin();

  return true;
}

bool TC6_Io_Generic::is_interrupt_active()
{
  _int_reported = _int_in;
  return (_int_reported != _int_out);
}

void TC6_Io_Generic::release_interrupt()
{
  if (digitalRead(_irq_pin) == HIGH)
    _int_out = _int_reported;
}

bool TC6_Io_Generic::spi_transaction(uint8_t const * pTx, uint8_t * pRx, uint16_t const len)
{
  digitalWrite(_cs_pin, LOW);
  _spi.beginTransaction(LAN865x_SPI_SETTING);

  for (size_t b = 0; b < len; b++)
    pRx[b] = _spi.transfer(pTx[b]);

  _spi.endTransaction();
  digitalWrite(_cs_pin, HIGH);

  TC6_SpiBufferDone(0 /* tc6instance */, true /* success */);

  return true;
}

bool TC6_Io_Generic::get_mac_address(uint8_t * p_mac)
{
  uint8_t MAC_EEPROM_I2C_SLAVE_ADDR = 0x58;
  uint8_t MAC_EEPROM_EUI_REG_ADDR = 0x9A;
  bool success = false;

  _wire.beginTransmission(MAC_EEPROM_I2C_SLAVE_ADDR);
  _wire.write(MAC_EEPROM_EUI_REG_ADDR);
  _wire.endTransmission();

  _wire.requestFrom(MAC_EEPROM_I2C_SLAVE_ADDR, MAC_SIZE);

  uint32_t const start = millis();

  size_t bytes_read = 0;
  while (bytes_read < MAC_SIZE && ((millis() - start) < 1000)) {
    if (_wire.available()) {
      p_mac[bytes_read] = _wire.read();
      bytes_read++;
    }
  }

  success = (bytes_read == MAC_SIZE);
  return success;
}

void TC6_Io_Generic::onInterrupt()
{
  _int_in++;
}
