#include "axp192.h"
#ifdef AXP192_ADDRESS

#include <Arduino.h>
// #include <TFT_eSPI.h>
// #include <SPI.h>
#include <Wire.h>

uint8_t axpReadRegister(uint8_t reg)
{
  Wire.beginTransmission(AXP192_ADDRESS);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom(AXP192_ADDRESS, 1);
  return Wire.read();
}

void axpWriteRegister(uint8_t reg, uint8_t data)
{
  Wire.beginTransmission(AXP192_ADDRESS);
  Wire.write(reg);
  Wire.write(data);
  Wire.endTransmission();
}

// Can turn LCD Backlight OFF for power saving
void axpSetLDO2(bool state)
{
  uint8_t buf = axpReadRegister(0x12);
  uint8_t data = state ? (buf | (1 << 2)) : (buf & ~(1 << 2));
  axpWriteRegister(0x12, data);
}

void axpSetup(
    bool disableLDO2, bool disableLDO3, bool disableRTC,
    bool disableDCDC1, bool disableDCDC3, bool disableLDO0)
{
  // Set LDO2 & LDO3(TFT_LED & TFT) 3.0V
  axpWriteRegister(0x28, 0xcc);

  // Set ADC sample rate to 200hz
  axpWriteRegister(0x84, 0b11110010);

  // Set ADC to All Enable
  axpWriteRegister(0x82, 0xff);

  // Bat charge voltage to 4.2, Current 100MA
  axpWriteRegister(0x33, 0xc0);

  // Depending on configuration enable LDO2, LDO3, DCDC1, DCDC3.
  uint8_t buf = (axpReadRegister(0x12) & 0xef) | 0x4D;
  if (disableLDO3)
    buf &= ~(1 << 3);
  if (disableLDO2)
    buf &= ~(1 << 2);
  if (disableDCDC3)
    buf &= ~(1 << 1);
  if (disableDCDC1)
    buf &= ~(1 << 0);
  axpWriteRegister(0x12, buf);

  // 128ms power on, 4s power off
  axpWriteRegister(0x36, 0x0C);

  if (!disableLDO0)
  {
    // Set MIC voltage to 2.8V
    axpWriteRegister(0x91, 0xA0);

    // Set GPIO0 to LDO
    axpWriteRegister(0x90, 0x02);
  }
  else
  {
    axpWriteRegister(0x90, 0x07); // GPIO0 floating
  }

  // Disable vbus hold limit
  axpWriteRegister(0x30, 0x80);

  // Set temperature protection
  axpWriteRegister(0x39, 0xfc);

  // Enable RTC BAT charge
  axpWriteRegister(0x35, 0xa2 & (disableRTC ? 0x7F : 0xFF));

  // Enable bat detection
  axpWriteRegister(0x32, 0x46);

  // Set Power off voltage 3.0v
  axpWriteRegister(0x31, (axpReadRegister(0x31) & 0xf8) | (1 << 2));
}

bool axpSetBacklight(bool lightOn)
{
  // uint8_t b0 =
  axpReadRegister(0x12);
  axpSetLDO2(lightOn);
  // uint8_t b1 =
  axpReadRegister(0x12);
  Serial.printf("setBacklight via axp192 %s\n", lightOn ? "ON" : "OFF");
  // Serial.printf("  b0: ");
  // Serial.print(b0, BIN);
  // Serial.print("\n");
  // Serial.printf("  b1: ");
  // Serial.print(b1, BIN);
  // Serial.print("\n");
  return lightOn;
}

#endif
