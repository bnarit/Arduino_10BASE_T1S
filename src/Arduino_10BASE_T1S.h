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

#include "Arduino_10BASE_T1S_PHY_Interface.h"
#include "Arduino_10BASE_T1S_UDP.h"

#include "microchip/TC6_Arduino_10BASE_T1S.h"

#include "lib/liblwip/include/lwip/netif.h"
#include "lib/liblwip/include/lwip/init.h"
#include "lib/liblwip/include/lwip/timeouts.h"
#include "lib/liblwip/include/netif/etharp.h"
#include "lib/liblwip/include/lwip/mem.h"
#include "lib/liblwip/include/lwip/memp.h"
#include "lib/liblwip/include/lwip/udp.h"

/**************************************************************************************
 * CONSTANTS
 **************************************************************************************/

#if defined(ARDUINO_SAMD_NANO_33_IOT)
static int const CS_PIN    = 10;
static int const RESET_PIN =  9;
static int const IRQ_PIN   =  2;
#elif defined(ARDUINO_SAMD_ZERO) || defined(ARDUINO_MINIMA) || defined(ARDUINO_UNOWIFIR4) || defined(ARDUINO_AVR_UNO_WIFI_REV2)
/* Those are all boards with the Arduino Uno form factor for the T1S shield. */
static int const CS_PIN    =  9;
static int const RESET_PIN =  4;
static int const IRQ_PIN   =  2;
#else
# error "No pins defined for your board"
#endif
