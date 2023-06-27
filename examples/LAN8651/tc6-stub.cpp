//DOM-IGNORE-BEGIN
/*
Copyright (C) 2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

The software and documentation is provided by microchip and its contributors
"as is" and any express, implied or statutory warranties, including, but not
limited to, the implied warranties of merchantability, fitness for a particular
purpose and non-infringement of third party intellectual property rights are
disclaimed to the fullest extent permitted by law. In no event shall microchip
or its contributors be liable for any direct, indirect, incidental, special,
exemplary, or consequential damages (including, but not limited to, procurement
of substitute goods or services; loss of use, data, or profits; or business
interruption) however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise) arising in any way
out of the use of the software and documentation, even if advised of the
possibility of such damage.

Except as expressly permitted hereunder and subject to the applicable license terms
for any third-party software incorporated in the software and any applicable open
source software license terms, no license or other rights, whether express or
implied, are granted under any patent or other intellectual property rights of
Microchip or any third party.
*/
//DOM-IGNORE-END
/*******************************************************************************
  MCU specific stub code for OpenAlliance TC6 10BASE-T1S MACPHY via SPI protocol

  Company:
    Microchip Technology Inc.

  File Name:
    tc6-stub.c

  Summary:
    MCU specifc stub code

  Description:
    This file acts as a bridge between the TC6 library and the Board Support Package
    for the dedicated MCU
*******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
//#include "definitions.h"
//#include "tc6-conf.h"
//#include "tc6.h"
#include <SPI.h>
#include <Wire.h>
#include <Arduino.h>
#include <Arduino_10BASE_T1S.h>

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/*                          USER ADJUSTABLE                             */
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

static const uint8_t FALLBACK_MAC[] = {0x00u, 0x80u, 0xC2u, 0x00u, 0x01u, 0xCCu};

#if defined(ARDUINO_SAMD_NANO_33_IOT)
static int const IRQ_PIN   =  2;
static int const RESET_PIN =  9;
static int const CS_PIN    = 10;
#elif defined(ARDUINO_RASPBERRY_PI_PICO)
static int const IRQ_PIN   = 15;
static int const RESET_PIN = 14;
static int const CS_PIN    =  5;
#else
# error "No pins defined for your board"
#endif

static SPISettings const LAN865x_SPI_SETTING{1*1000*1000UL, MSBFIRST, SPI_MODE0};

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/*                      DEFINES AND LOCAL VARIABLES                     */
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

typedef struct
{
    uint8_t mac[6];
    uint8_t intIn;
    uint8_t intOut;
    uint8_t intReported;
} Stub_Local_t;

static Stub_Local_t d = { 0 };

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/*                      PRIVATE FUNCTION PROTOTYPES                     */
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

static bool GetMacAddress(Stub_Local_t *ps);

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/*                         PUBLIC FUNCTIONS                             */
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

bool TC6Stub_Init(uint8_t /* idx */, uint8_t pMac[6])
{
    d.intIn = 0;
    d.intOut = 0;
    d.intReported = 0;

    digitalWrite(CS_PIN, HIGH);
    pinMode(CS_PIN, OUTPUT);

    digitalWrite(RESET_PIN, HIGH);
    pinMode(RESET_PIN, OUTPUT);

    pinMode(IRQ_PIN, INPUT_PULLUP);

#if defined(ARDUINO_RASPBERRY_PI_PICO)
    SPI.setSCK(2);
    SPI.setTX (3);
    SPI.setRX (4);
    SPI.setCS (5);
#endif
    SPI.begin();

#if defined(ARDUINO_RASPBERRY_PI_PICO)
    Wire.setSDA(0);
    Wire.setSCL(1);
#endif
    Wire.begin();

    if (GetMacAddress(&d)) {
      memcpy(pMac, d.mac, 6u);
    } else {
      memcpy(pMac, FALLBACK_MAC, 6u);
    }

    digitalWrite(RESET_PIN, LOW);
    delay(100);
    digitalWrite(RESET_PIN, HIGH);
    delay(100);

    attachInterrupt(digitalPinToInterrupt(IRQ_PIN),
                    []() -> void
                    {
                      d.intIn++;
                      Serial.println(d.intIn);
                    },
                    FALLING);

    return true;
}

bool TC6Stub_IntActive(uint8_t /* idx */)
{
    Serial.println("TC6Stub_IntActive");
    Stub_Local_t *ps = &d;
    ps->intReported = ps->intIn;
    return (ps->intReported != ps->intOut);
}

void TC6Stub_ReleaseInt(uint8_t /* idx */)
{
    Serial.println("TC6Stub_IntActive");
    Stub_Local_t *ps = &d;
    if (digitalRead(IRQ_PIN) == HIGH) {
        ps->intOut = ps->intReported;
    }
}

uint32_t TC6Stub_GetTick(void)
{
    return millis();
}

bool TC6Stub_SpiTransaction(uint8_t idx, uint8_t *pTx, uint8_t *pRx, uint16_t len)
{
    digitalWrite(CS_PIN, LOW);
    SPI.beginTransaction(LAN865x_SPI_SETTING);

    for (size_t b = 0; b < len; b++)
      pRx[b] = SPI.transfer(pTx[b]);

    SPI.endTransaction();
    digitalWrite(CS_PIN, HIGH);

#if 1
    Serial.print("TX = ");
    for (size_t b = 0; b < len; b++)
    {
      char msg[8] = {0};
      snprintf(msg, sizeof(msg), "%02X ", pTx[b]);
      Serial.print(msg);
    }
    Serial.println();

    Serial.print("RX = ");
    for (size_t b = 0; b < len; b++)
    {
      char msg[8] = {0};
      snprintf(msg, sizeof(msg), "%02X ", pRx[b]);
      Serial.print(msg);
    }
    Serial.println();
#endif

  return true;
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/*                  PRIVATE FUNCTION IMPLEMENTATIONS                    */
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

static bool GetMacAddress(Stub_Local_t *ps)
{
    uint8_t MAC_EEPROM_I2C_SLAVE_ADDR = 0x58;
    uint8_t MAC_EEPROM_EUI_REG_ADDR = 0x9A;
    bool success = false;

    Wire.beginTransmission(MAC_EEPROM_I2C_SLAVE_ADDR);
    Wire.write(MAC_EEPROM_EUI_REG_ADDR);
    Wire.endTransmission();

    Wire.requestFrom(MAC_EEPROM_I2C_SLAVE_ADDR, sizeof(ps->mac));

    uint32_t const start = TC6Stub_GetTick();

    size_t bytes_read = 0;
    while (bytes_read < sizeof(ps->mac) && ((TC6Stub_GetTick() - start) < 1000)) {
      if (Wire.available()) {
        ps->mac[bytes_read] = Wire.read();
        bytes_read++;
      }
    }

    success = (bytes_read == sizeof(ps->mac));
    return success;
}
