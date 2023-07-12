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
