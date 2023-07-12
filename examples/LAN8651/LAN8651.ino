#include <Arduino_10BASE_T1S.h>

#include "udp_perf_client.h"

static uint8_t const T1S_PLCA_NODE_ID     = 1;
static uint8_t const T1S_PLCA_NODE_COUNT  = 8;
static uint8_t const T1S_PLCA_BURST_COUNT = 0;
static uint8_t const T1S_PLCA_BURST_TIMER = 0x80;
static bool    const MAC_PROMISCUOUS_MODE = false;
static bool    const MAC_TX_CUT_THROUGH   = false;
static bool    const MAC_RX_CUT_THROUGH   = false;

static uint8_t const IP[] = {192, 168, 42, 100 + T1S_PLCA_NODE_ID};

static T1SPlcaSettings const t1s_plca_settings{T1S_PLCA_NODE_ID, T1S_PLCA_NODE_COUNT, T1S_PLCA_BURST_COUNT, T1S_PLCA_BURST_TIMER};
static T1SMacSettings const t1s_mac_settings{MAC_PROMISCUOUS_MODE, MAC_TX_CUT_THROUGH, MAC_RX_CUT_THROUGH};

TC6 tc6_inst;

void setup()
{
  Serial.begin(115200);
  while (!Serial) { }
  delay(1000);

  if (!tc6_inst.begin(IP,
                      t1s_plca_settings,
                      t1s_mac_settings))
  {
    Serial.println("'TC6::begin(...)' failed.");
    return;
  }

  auto const MAC = tc6_inst.getMacAddr();

  char board_info_msg[256] = {0};
  snprintf(board_info_msg,
           sizeof(board_info_msg),
           "\tIP\t%d.%d.%d.%d\n" \
           "\tMAC\t%02X:%02X:%02X:%02X:%02X:%02X\n",
           IP[0], IP[1], IP[2], IP[3],
           MAC[0], MAC[1], MAC[2], MAC[3], MAC[4], MAC[5]);

  Serial.println(board_info_msg);
  Serial.println(t1s_plca_settings);
  Serial.println(t1s_mac_settings);

  iperf_init();
  iperf_print_app_header();
  iperf_start_application();
}

void loop()
{
  /* Services the hardware and the protocol stack.
   * Must be called cyclic. The faster the better.
   */
  tc6_inst.service();

  iperf_service();

  static unsigned long prev_beacon_check = 0;

  auto const now = millis();

  if ((now - prev_beacon_check) > 1000)
  {
    prev_beacon_check = now;
    tc6_inst.getPlcaStatus(OnPlcaStatus);
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
