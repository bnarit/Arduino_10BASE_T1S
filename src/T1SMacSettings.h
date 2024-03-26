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
 * CLASS DECLARATION
 **************************************************************************************/

class T1SMacSettings : public arduino::Printable
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
