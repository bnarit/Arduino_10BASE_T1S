/*
 *  This file is part of the Arduino_10BASE_T1S library.
 *
 *  Copyright (c) 2024 Arduino SA
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include "MacAddress.h"

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

//MacAddress::MacAddress()
//{
//
//}

/**************************************************************************************
 * PUBLIC MEMBER FUNCTIONS
 **************************************************************************************/

size_t MacAddress::printTo(Print & p) const
{
  char msg[32] = {0};

  uint8_t const * const ptr_mac = (this->data());

  snprintf(msg,
           sizeof(msg),
           "MAC\t%02X:%02X:%02X:%02X:%02X:%02X",
           ptr_mac[0],
           ptr_mac[1],
           ptr_mac[2],
           ptr_mac[3],
           ptr_mac[4],
           ptr_mac[5]);

  return p.write(msg);
}
