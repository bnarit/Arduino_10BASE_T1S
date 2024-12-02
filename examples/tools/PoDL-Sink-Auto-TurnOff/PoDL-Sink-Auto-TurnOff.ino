/*
 * This example is used to test T1TOS PoDL sink capabilities.
 *
 * This targets T1TOS v0.1.
 *
 * Author:
 *  Alexander Entinger
 */

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <Arduino_10BASE_T1S.h>

#include <SPI.h>

/**************************************************************************************
 * CONSTANTS
 **************************************************************************************/

static uint8_t const T1S_PLCA_NODE_ID = 1; /* Doubles as PLCA coordinator. */

static IPAddress const ip_addr     {192, 168,  42, 100 + T1S_PLCA_NODE_ID};
static IPAddress const network_mask{255, 255, 255,   0};
static IPAddress const gateway     {192, 168,  42, 100};

static T1SPlcaSettings const t1s_plca_settings{T1S_PLCA_NODE_ID};
static T1SMacSettings const t1s_default_mac_settings;

/**************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************/

#if defined(ARDUINO_GIGA) || defined(ARDUINO_PORTENTA_C33)
  Arduino_10BASE_T1S_PHY_TC6(SPI1, CS_PIN, RESET_PIN, IRQ_PIN);
#else
  Arduino_10BASE_T1S_PHY_TC6(SPI, CS_PIN, RESET_PIN, IRQ_PIN);
#endif

/**************************************************************************************
 * SETUP/LOOP
 **************************************************************************************/

void setup()
{
  Serial.begin(115200);
  while (!Serial) { }
  delay(1000);

  /* Initialize digital IO interface for interfacing
   * with the LAN8651.
   */
  pinMode(IRQ_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(IRQ_PIN),
                  []() { t1s_io.onInterrupt(); },
                  FALLING);

  /* Initialize IO module. */
  if (!t1s_io.begin())
  {
    Serial.println("'TC6_Io::begin(...)' failed.");
    for (;;) { }
  }

  MacAddress const mac_addr = MacAddress::create_from_uid();

  if (!t1s_phy.begin(ip_addr
    , network_mask
    , gateway
    , mac_addr
    , t1s_plca_settings
    , t1s_default_mac_settings))
  {
    Serial.println("'TC6::begin(...)' failed.");
    for (;;) { }
  }

  Serial.print("IP\t");
  Serial.println(ip_addr);
  Serial.println(mac_addr);
  Serial.println(t1s_plca_settings);
  Serial.println(t1s_default_mac_settings);

  /* A0 -> LOCAL_ENABLE -> DO NOT feed power from board to network. */
  t1s_phy.digitalWrite(TC6::DIO::A0, false);
  /* A1 -> T1S_DISABLE -> Open the switch connecting network to board by pulling EN LOW. */
  t1s_phy.digitalWrite(TC6::DIO::A1, true);

  Serial.println("PoDL-Sink-Auto-TurnOff");
}

void loop()
{
  /* Services the hardware and the protocol stack.
   * Must be called cyclic. The faster the better.
   */
  t1s_phy.service();
}
