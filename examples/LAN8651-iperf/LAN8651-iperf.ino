/*
 * This example has been tested with Arduino Nano 33 IoT and with
 * Mikroe Two-Wire ETH Click board (MicroChip LAN8651). For further
 * information take a look at the README.
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

#if defined(ARDUINO_SAMD_NANO_33_IOT)
static int const CS_PIN    = 10;
static int const RESET_PIN =  9;
static int const IRQ_PIN   =  2;
#elif defined(ARDUINO_SAMD_ZERO) || defined(ARDUINO_MINIMA) || defined(ARDUINO_UNOWIFIR4) || defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_UNO_WIFI_REV2)
/* Those are all boards with the Arduino Uno form factor for the T1S shield. */
static int const CS_PIN    =  9;
static int const RESET_PIN =  4;
static int const IRQ_PIN   =  2;
#else
# error "No pins defined for your board"
#endif

/**************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************/

auto const tc6_io = new TC6::TC6_Io
  ( SPI
  , CS_PIN
  , RESET_PIN
  , IRQ_PIN);
auto const tc6_inst = new TC6::TC6_Arduino_10BASE_T1S_UDP(tc6_io);

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

  /* Obtain MAC address stored on EEPROM of Mikroe
   * Two-Wire ETH Click board.
   */
  MacAddress mac_addr;
  if (!get_mac_address(mac_addr.data()))
  {
    Serial.println("'get_mac_address(...)' failed, using fallback MAC address.");
    memcpy(mac_addr.data(), TC6::TC6_Io::FALLBACK_MAC, MAC_ADDRESS_NUM_BYTES);
  }

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
    tc6_inst->getPlcaStatus(OnPlcaStatus);
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
  else
    Serial.println("CSMA/CD fallback");
}

static bool get_mac_address(uint8_t * p_mac)
{
  static uint8_t const MAC_EEPROM_I2C_SLAVE_ADDR = 0x58;
  static uint8_t const MAC_EEPROM_EUI_REG_ADDR = 0x9A;
  static uint8_t const MAC_EEPROM_MAC_SIZE = 6;
  bool success = false;

  Wire.beginTransmission(MAC_EEPROM_I2C_SLAVE_ADDR);
  Wire.write(MAC_EEPROM_EUI_REG_ADDR);
  Wire.endTransmission();

  Wire.requestFrom(MAC_EEPROM_I2C_SLAVE_ADDR, MAC_EEPROM_MAC_SIZE);

  uint32_t const start = millis();

  size_t bytes_read = 0;
  while (bytes_read < MAC_EEPROM_MAC_SIZE && ((millis() - start) < 1000)) {
    if (Wire.available()) {
      p_mac[bytes_read] = Wire.read();
      bytes_read++;
    }
  }

  success = (bytes_read == MAC_EEPROM_MAC_SIZE);
  return success;
}
