#include <Arduino_10BASE-T1S.h>

#include "tc6-stub.h"

void setup()
{
  Serial.begin(115200);
  while (!Serial) { }

  uint8_t mac_addr[6] = {0};
  if (!TC6Stub_Init(0, mac_addr)) {
    Serial.println("TC6Stub_Init failed.");
    return;
  }

  char msg[32] = {0};
  snprintf(msg, sizeof(msg), "MAC = %02X:%02X:%02X:%02X:%02X:%02X", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(msg);
}

void loop()
{

}
