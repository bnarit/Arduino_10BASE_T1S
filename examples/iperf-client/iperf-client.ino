/*
 * This example has been tested with the Arduino 10BASE-T1S (T1TOS) shield.
 *
 * Author:
 *  Alexander Entinger
 */

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <Arduino_10BASE_T1S.h>

#include <SPI.h>
#include <Wire.h>

#include "udp_perf_client.h"

/**************************************************************************************
 * CONSTANTS
 **************************************************************************************/

static uint8_t const T1S_PLCA_NODE_ID     = 1;
static uint8_t const T1S_PLCA_NODE_COUNT  = 8;
static uint8_t const T1S_PLCA_BURST_COUNT = 0;
static uint8_t const T1S_PLCA_BURST_TIMER = 0x80;
static bool    const MAC_PROMISCUOUS_MODE = false;
static bool    const MAC_TX_CUT_THROUGH   = false;
static bool    const MAC_RX_CUT_THROUGH   = false;

static IPAddress const ip_addr     {192, 168,  42, 100 + T1S_PLCA_NODE_ID};
static IPAddress const network_mask{255, 255, 255,   0};
static IPAddress const gateway     {192, 168,  42, 100};

static T1SPlcaSettings const t1s_plca_settings{T1S_PLCA_NODE_ID, T1S_PLCA_NODE_COUNT, T1S_PLCA_BURST_COUNT, T1S_PLCA_BURST_TIMER};
static T1SMacSettings const t1s_mac_settings{MAC_PROMISCUOUS_MODE, MAC_TX_CUT_THROUGH, MAC_RX_CUT_THROUGH};

/**************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************/

auto const tc6_io = new TC6::TC6_Io(
#if defined(ARDUINO_GIGA) || defined(ARDUINO_PORTENTA_C33)
  SPI1
#else
  SPI
#endif
  , CS_PIN
  , RESET_PIN
  , IRQ_PIN);
auto const tc6_inst = new TC6::TC6_Arduino_10BASE_T1S(tc6_io);

/**************************************************************************************
 * SETUP/LOOP
 **************************************************************************************/

void setup()
{
  Serial.begin(115200);
  while (!Serial) { }
  delay(1000);

  /* I2C (Wire) is needed to obtain an individual MAC
   * address from the AT24MAC402 EEPROM located on the
   * Mikroe Two-Wire ETH click board.
   */
  Wire.begin();

  /* Initialize digital IO interface for interfacing
   * with the LAN8651.
   */
  pinMode(IRQ_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(IRQ_PIN),
                  []() { tc6_io->onInterrupt(); },
                  FALLING);

  /* Initialize IO module. */
  if (!tc6_io->begin())
  {
    Serial.println("'TC6_Io::begin(...)' failed.");
    for (;;) { }
  }

  MacAddress const mac_addr = MacAddress::create_from_uid();

  if (!tc6_inst->begin(  ip_addr
                       , network_mask
                       , gateway
                       , mac_addr
                       , t1s_plca_settings
                       , t1s_mac_settings))
  {
    Serial.println("'TC6::begin(...)' failed.");
    for (;;) { }
  }

  Serial.print("IP\t");
  Serial.println(ip_addr);
  Serial.println(mac_addr);
  Serial.println(t1s_plca_settings);
  Serial.println(t1s_mac_settings);

  iperf_init(tc6_inst);
  iperf_print_app_header();
  iperf_start_application();
}

void loop()
{
  /* Services the hardware and the protocol stack.
   * Must be called cyclic. The faster the better.
   */
  tc6_inst->service();

  iperf_service();

  static unsigned long prev_beacon_check = 0;

  auto const now = millis();

  if ((now - prev_beacon_check) > 1000)
  {
    prev_beacon_check = now;
    if (!tc6_inst->getPlcaStatus(OnPlcaStatus))
      Serial.println("getPlcaStatus(...) failed");
  }
}


static void OnPlcaStatus(bool success, bool plcaStatus)
{
  if (!success)
  {
    Serial.println("PLCA status register read failed");
    return;
  }

  if (plcaStatus)
    Serial.println("PLCA Mode active");
  else {
    Serial.println("CSMA/CD fallback");
    tc6_inst->enablePlca();
  }
}
