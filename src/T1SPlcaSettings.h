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

class T1SPlcaSettings : public arduino::Printable
{
private:
  uint8_t _node_id;
  uint8_t _node_count;
  uint8_t _burst_count;
  uint8_t _burst_timer;

public:
  static uint8_t const DEFAULT_NODE_ID     =   0;
  static uint8_t const DEFAULT_NODE_COUNT  =   8;
  static uint8_t const DEFAULT_BURST_COUNT =   0;
  static uint8_t const DEFAULT_BURST_TIMER = 128;


  T1SPlcaSettings() : T1SPlcaSettings(DEFAULT_NODE_ID, DEFAULT_NODE_COUNT, DEFAULT_BURST_COUNT, DEFAULT_BURST_TIMER) { }
  T1SPlcaSettings(uint8_t const node_id) : T1SPlcaSettings(node_id, DEFAULT_NODE_COUNT, DEFAULT_BURST_COUNT, DEFAULT_BURST_TIMER) { }
  T1SPlcaSettings(uint8_t const node_id,
                  uint8_t const node_count,
                  uint8_t const burst_count,
                  uint8_t const burst_timer);


  virtual size_t printTo(Print & p) const override;


  uint8_t node_id()     const { return _node_id; }
  uint8_t node_count()  const { return _node_count; }
  uint8_t burst_count() const { return _burst_count; }
  uint8_t burst_timer() const { return _burst_timer; }
};
