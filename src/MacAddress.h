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

class MacAddress : public arduino::Printable
{
public:
  MacAddress() : MacAddress(nullptr) { }
  MacAddress(uint8_t const * mac);

  static MacAddress create_from_uid();

  uint8_t * data() { return _data; }
  uint8_t const * data() const { return _data; }


private:
  uint8_t _data[MAC_ADDRESS_NUM_BYTES];
  virtual size_t printTo(Print & p) const override;
};
