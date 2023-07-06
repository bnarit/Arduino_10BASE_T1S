#include <Arduino_10BASE_T1S.h>

#include "tc6-stub.h"
#include "tc6-lwip.h"

static bool    const T1S_PLCA_ENABLE      = true;
static uint8_t const T1S_PLCA_NODE_ID     = 1;
static uint8_t const T1S_PLCA_NODE_COUNT  = 8;
static uint8_t const T1S_PLCA_BURST_COUNT = 0;
static uint8_t const T1S_PLCA_BURST_TIMER = 0x80;
static bool    const MAC_PROMISCUOUS_MODE = false;
static bool    const MAC_TX_CUT_THROUGH   = false;
static bool    const MAC_RX_CUT_THROUGH   = false;

static uint8_t const IP[] = {192, 168, 42, 100 + T1S_PLCA_NODE_ID};

int8_t lwip_idx = -1;

void setup()
{
  Serial.begin(115200);
  while (!Serial) { }
  delay(1000);

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

  uint8_t MAC[6] = {0};
  TC6LwIP_GetMac(lwip_idx, reinterpret_cast<uint8_t **>(&MAC));

  char board_info_msg[256] = {0};
  snprintf(board_info_msg,
           sizeof(board_info_msg),
           "\tIP\t%d.%d.%d.%d\n" \
           "\tMAC\t%02X:%02X:%02X:%02X:%02X:%02X\n" \
           "\tPLCA\n" \
           "\t\tnode id     : %d\n" \
           "\t\tnode count  : %d\n" \
           "\t\tburst count : %d\n" \
           "\t\tburst timer : %d\n" \
           "\tMAC\n" \
           "\t\tpromisc. mode : %d\n" \
           "\t\ttx cut through: %d\n" \
           "\t\trx cut through: %d",
           IP[0], IP[1], IP[2], IP[3],
           MAC[0], MAC[1], MAC[2], MAC[3], MAC[4], MAC[5],
           T1S_PLCA_NODE_ID,
           T1S_PLCA_NODE_COUNT,
           T1S_PLCA_BURST_COUNT,
           T1S_PLCA_BURST_TIMER,
           MAC_PROMISCUOUS_MODE,
           MAC_TX_CUT_THROUGH,
           MAC_RX_CUT_THROUGH);

  Serial.println(board_info_msg);
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
