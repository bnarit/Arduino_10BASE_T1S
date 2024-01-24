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

#include <stdint.h>

#include <Print.h>
#include <Printable.h>

/**************************************************************************************
 * CONSTANTS
 **************************************************************************************/

static size_t constexpr MAC_ADDRESS_NUM_BYTES = 6;

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

class MacAddress
#if defined(ARDUINO_ARCH_AVR)
  : public Printable
#else
  : public arduino::Printable
#endif
{
public:
  uint8_t * data() { return _data; }
  uint8_t const * data() const { return _data; }
private:
  uint8_t _data[MAC_ADDRESS_NUM_BYTES];
  virtual size_t printTo(Print & p) const override;
};
