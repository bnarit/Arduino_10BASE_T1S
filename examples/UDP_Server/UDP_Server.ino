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

/**************************************************************************************
 * CONSTANTS
 **************************************************************************************/

static uint8_t const T1S_PLCA_NODE_ID = 1;

static IPAddress const ip_addr     {192, 168,  42, 100 + T1S_PLCA_NODE_ID};
static IPAddress const network_mask{255, 255, 255,   0};
static IPAddress const gateway     {192, 168,  42, 100};

static T1SPlcaSettings const t1s_plca_settings{T1S_PLCA_NODE_ID};
static T1SMacSettings const t1s_default_mac_settings;

static uint16_t const UDP_SERVER_LOCAL_PORT = 8888;
static uint8_t * udp_rx_msg_buf[256] = {0};

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

  // If Power Provider, turn on LOCAL_ENABLE and turn on T1S_DISABLE
  //tc6_inst->digitalWrite(1,1,0);
  // If Power Receiver, turn off LOCAL_ENABLE and turn off T1S_DISABLE
  tc6_inst->digitalWrite(0,0,0);
  // If we want to disable PoDL, turn off LOCAL_ENABLE and turn on T1S_DISABLE
  //tc6_inst->digitalWrite(0,0,0);

  if (!tc6_inst->begin(UDP_SERVER_LOCAL_PORT))
  {
    Serial.println("begin(...) failed for UDP server");
    for (;;) { }
  }
}

void loop()
{
  /* Services the hardware and the protocol stack.
   * Must be called cyclic. The faster the better.
   */
  tc6_inst->service();

  static unsigned long prev_beacon_check = 0;

  auto const now = millis();

  if ((now - prev_beacon_check) > 1000)
  {
    prev_beacon_check = now;
    if (!tc6_inst->getPlcaStatus(OnPlcaStatus))
      Serial.println("getPlcaStatus(...) failed");
  }

  /* Check for incoming UDP packets. */
  int const packet_size = tc6_inst->parsePacket();
  if (packet_size)
  {
    /* Receive incoming UDP packets. */
    Serial.print("Received ");
    Serial.print(packet_size);
    Serial.print(" bytes from ");
    Serial.print(tc6_inst->remoteIP());
    Serial.print(" port ");
    Serial.print(tc6_inst->remotePort());
    Serial.println();

    int len = tc6_inst->read(reinterpret_cast<unsigned char *>(udp_rx_msg_buf), sizeof(udp_rx_msg_buf));
    if (len > 0) {
      udp_rx_msg_buf[len] = 0;
    }
    Serial.print("UDP packet contents: ");
    Serial.print(reinterpret_cast<char *>(udp_rx_msg_buf));
    Serial.println();

    /* Send back a reply, to the IP address and port we got the packet from. */
    tc6_inst->beginPacket(tc6_inst->remoteIP(), tc6_inst->remotePort());
    tc6_inst->write((const uint8_t *)udp_rx_msg_buf, sizeof(udp_rx_msg_buf));
    tc6_inst->endPacket();
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
