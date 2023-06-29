#include <Arduino_10BASE_T1S.h>

#include "tc6-stub.h"
#include "tc6-lwip.h"

/*
 *  arduino-cli compile -b arduino:samd:nano_33_iot -v examples/LAN8651
 * or
 *  arduino-cli compile -b rp2040:rp2040:rpipico -v examples/LAN8651
 */

static uint8_t const BOARD_INSTANCE       = 0;
static bool    const T1S_PLCA_ENABLE      = true;
static uint8_t const T1S_PLCA_NODE_ID     = BOARD_INSTANCE + 1;
static uint8_t const T1S_PLCA_NODE_COUNT  = 8;
static uint8_t const T1S_PLCA_BURST_COUNT = 0;
static uint8_t const T1S_PLCA_BURST_TIMER = 0x80;
static bool    const MAC_PROMISCUOUS_MODE = false;
static bool    const MAC_TX_CUT_THROUGH   = false;
static bool    const MAC_RX_CUT_THROUGH   = false;

static uint8_t const IP[] = {192, 168, 0, 100 + BOARD_INSTANCE};

int8_t lwip_idx = -1;

void setup()
{
  Serial.begin(115200);
  while (!Serial) { }

  delay(1000);

//  uint8_t mac_addr[6] = {0};
//  if (!TC6Stub_Init(0, mac_addr)) {
//    Serial.println("TC6Stub_Init failed.");
//    return;
//  }
//
//  char msg[32] = {0};
//  snprintf(msg, sizeof(msg), "MAC = %02X:%02X:%02X:%02X:%02X:%02X", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
//  Serial.println(msg);

  lwip_idx = TC6LwIP_Init(IP,
                          T1S_PLCA_ENABLE,
                          T1S_PLCA_NODE_ID,
                          T1S_PLCA_NODE_COUNT,
                          T1S_PLCA_BURST_COUNT,
                          T1S_PLCA_BURST_TIMER,
                          MAC_PROMISCUOUS_MODE,
                          MAC_TX_CUT_THROUGH,
                          MAC_RX_CUT_THROUGH);

  if (lwip_idx < 0) {
    Serial.println("'TC6LwIP_Init' failed.");
    return;
  }

  Serial.println("setup complete.");
}

void loop()
{
  /* Services the hardware and the protocol stack.
   * Must be called cyclic. The faster the better.
   */
  TC6LwIP_Service();

  static unsigned long prev_beacon_check = 0;

  auto const now = millis();

  if ((now - prev_beacon_check) > 1000)
  {
    prev_beacon_check = now;
    TC6LwIP_GetPlcaStatus(lwip_idx, OnPlcaStatus);
  }
}


static void OnPlcaStatus(int8_t idx, bool success, bool plcaStatus)
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
