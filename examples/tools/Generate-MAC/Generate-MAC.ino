/*
 * This example has been tested with Arduino Uno Minima / WiFi R4.
 *   - arduino-cli compile -b arduino:renesas_uno:unor4wifi -v examples/tools/Generate-MAC -u -p /dev/ttyACM0
 *   - arduino-cli compile -b arduino:renesas_uno:minima -v examples/tools/Generate-MAC -u -p /dev/ttyACM0
 */

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <Arduino_10BASE_T1S.h>

/**************************************************************************************
 * SETUP/LOOP
 **************************************************************************************/

void setup()
{
  Serial.begin(115200);
  while (!Serial) { }
  delay(1000);

  MacAddress const mac_addr = MacAddress::create_from_uid();

  Serial.println(mac_addr);
}

void loop() {

}
