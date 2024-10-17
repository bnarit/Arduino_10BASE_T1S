/*
 * This example has been tested with the Arduino 10BASE-T1S (T1TOS) shield and
 * can be used to control the value of various DIO output pins.
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

static uint8_t const T1S_PLCA_NODE_ID = 0; /* Doubles as PLCA coordinator. */

static IPAddress const ip_addr     {192, 168,  42, 100 + T1S_PLCA_NODE_ID};
static IPAddress const network_mask{255, 255, 255,   0};
static IPAddress const gateway     {192, 168,  42, 100};

static T1SPlcaSettings const t1s_plca_settings{T1S_PLCA_NODE_ID};
static T1SMacSettings const t1s_default_mac_settings;

/* Modify this function call parameter if you want to test a different LAN8651 DIO.
 */
static auto const DIO_PIN = TC6::DIO::A0;

/**************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************/

auto const tc6_io = new TC6::TC6_Io(
#if defined(ARDUINO_GIGA) || defined(ARDUINO_PORTENTA_C33)
  SPI1
#else
  SPI
#endif
  , CS_PIN
  , RESET_PIN
  , IRQ_PIN);
auto const tc6_inst = new TC6::TC6_Arduino_10BASE_T1S(tc6_io);

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
}

void loop()
{
  /* Services the hardware and the protocol stack.
   * Must be called cyclic. The faster the better.
   */
  tc6_inst->service();

  static unsigned long prev_dio_toogle = 0;
  auto const now = millis();

  if ((now - prev_dio_toogle) > 5000)
  {
    prev_dio_toogle = now;

    static bool dio_val = false;

    Serial.print("DIO ");
    Serial.print(toStr(DIO_PIN));
    Serial.print(" = ");
    Serial.println(dio_val);

    tc6_inst->digitalWrite(DIO_PIN, dio_val);
    dio_val = !dio_val;
  }
}

String toStr(TC6::DIO const dio)
{
  if (dio == TC6::DIO::A0)
    return String("A0");
  else if (dio == TC6::DIO::A1)
    return String("A1");
  else
    return String("");
}