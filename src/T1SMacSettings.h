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
 * CLASS DECLARATION
 **************************************************************************************/

class T1SMacSettings
#if defined(ARDUINO_ARCH_AVR)
  : public Printable
#else
  : public arduino::Printable
#endif
{
private:
  bool const _mac_promiscuous_mode;
  bool const _mac_tx_cut_through;
  bool const _mac_rx_cut_through;

public:
  static bool const DEFAULT_MAC_PROMISCUOUS_MODE = false;
  static bool const DEFAULT_MAC_TX_CUT_THROUGH   = false;
  static bool const DEFAULT_MAC_RX_CUT_THROUGH   = false;


  T1SMacSettings() : T1SMacSettings(DEFAULT_MAC_PROMISCUOUS_MODE, DEFAULT_MAC_TX_CUT_THROUGH, DEFAULT_MAC_RX_CUT_THROUGH) { }
  T1SMacSettings(bool const mac_promiscuous_mode,
                 bool const mac_tx_cut_through,
                 bool const mac_rx_cut_through);

  virtual size_t printTo(Print & p) const override;

  uint8_t mac_promiscuous_mode() const { return _mac_promiscuous_mode; }
  uint8_t mac_tx_cut_through()   const { return _mac_tx_cut_through; }
  uint8_t mac_rx_cut_through()   const { return _mac_rx_cut_through; }
};
