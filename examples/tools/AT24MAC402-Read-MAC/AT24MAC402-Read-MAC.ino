/*
 * This example has been tested with Arduino Nano 33 IoT and with
 * Mikroe Two-Wire ETH Click board (MicroChip LAN8651).
 *
 * arduino-cli compile -b arduino:samd:nano_33_iot -v examples/tools/AT24MAC402-Read-MAC -u -p /dev/ttyACM0
 *
 * Author:
 *  Alexander Entinger
 */

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <Wire.h>

#include <Arduino_10BASE_T1S.h>

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

  /* Obtain MAC address stored on EEPROM of Mikroe
   * Two-Wire ETH Click board.
   */
  MacAddress mac_addr;
  if (!get_mac_address(mac_addr.data())) {
    Serial.println("'get_mac_address(...)' failed.");
    for(;;) { }
  }

  Serial.print("AT24MAC402 MAC address: ");
  Serial.println(mac_addr);
}

void loop() {
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
