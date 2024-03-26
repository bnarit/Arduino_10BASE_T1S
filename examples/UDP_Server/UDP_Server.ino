/*
 * This example has been tested with the Arduino 10BASE-T1S (T1TOS) shield.
 *
 * This example contains a UDP echo server that displays each received message
 * and echos it back to its sender.
 *
 * Can be tested on Linux with a 10BASE-T1S USB dongle and
 *   echo -n "hello" >/dev/udp/192.168.42.100/8888
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

static uint8_t const T1S_PLCA_NODE_ID = 0; /* The UDP server doubles as PLCA coordinator. */

static IPAddress const ip_addr     {192, 168,  42, 100 + T1S_PLCA_NODE_ID};
static IPAddress const network_mask{255, 255, 255,   0};
static IPAddress const gateway     {192, 168,  42, 100};

static T1SPlcaSettings const t1s_plca_settings{T1S_PLCA_NODE_ID};
static T1SMacSettings const t1s_default_mac_settings;

static uint16_t const UDP_SERVER_LOCAL_PORT = 8888;
static uint8_t udp_rx_msg_buf[256] = {0};

/**************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************/

auto const tc6_io = new TC6::TC6_Io
  ( SPI
    , CS_PIN
    , RESET_PIN
    , IRQ_PIN);
auto const tc6_inst = new TC6::TC6_Arduino_10BASE_T1S(tc6_io);
Arduino_10BASE_T1S_UDP udp_server;

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
                  []() { tc6_io->onInterrupt(); },
                  FALLING);

  /* Initialize IO module. */
  if (!tc6_io->begin())
  {
    Serial.println("'TC6_Io::begin(...)' failed.");
    for (;;) { }
  }

  MacAddress const mac_addr = MacAddress::create_from_uid();

  if (!tc6_inst->begin(ip_addr
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

  if (!udp_server.begin(UDP_SERVER_LOCAL_PORT))
  {
    Serial.println("begin(...) failed for UDP server");
    for (;;) { }
  }

  Serial.println("UDP_Server");
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
  int const packet_size = udp_server.parsePacket();
  if (packet_size)
  {
    /* Receive incoming UDP packets. */
    Serial.print("Received ");
    Serial.print(packet_size);
    Serial.print(" bytes from ");
    Serial.print(udp_server.remoteIP());
    Serial.print(" port ");
    Serial.print(udp_server.remotePort());
    Serial.println();

    int const bytes_read = udp_server.read(udp_rx_msg_buf, sizeof(udp_rx_msg_buf));
    if (bytes_read > 0) {
      udp_rx_msg_buf[bytes_read] = 0;
    }
    Serial.print("UDP_Server received packet content: \"");
    Serial.print(reinterpret_cast<char *>(udp_rx_msg_buf));
    Serial.println("\"");

    /* Send back a reply, to the IP address and port we got the packet from. */
    udp_server.beginPacket(udp_server.remoteIP(), udp_server.remotePort());
    udp_server.write((const uint8_t *)udp_rx_msg_buf, packet_size);
    udp_server.endPacket();
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
