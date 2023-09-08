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

#include "microchip/TC6_Arduino_10BASE_T1S_UDP.h"

#include "lib/liblwip/include/lwip/netif.h"
#include "lib/liblwip/include/lwip/init.h"
#include "lib/liblwip/include/lwip/timeouts.h"
#include "lib/liblwip/include/netif/etharp.h"
#include "lib/liblwip/include/lwip/mem.h"
#include "lib/liblwip/include/lwip/memp.h"
#include "lib/liblwip/include/lwip/udp.h"
