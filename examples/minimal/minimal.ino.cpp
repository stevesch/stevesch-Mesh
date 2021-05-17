# 1 "C:\\Users\\Steve\\AppData\\Local\\Temp\\tmp_t7so6ko"
#include <Arduino.h>
# 1 "C:/Users/Steve/Projects/Arduino/libraries/stevesch-Mesh/examples/minimal/minimal.ino"
# 11 "C:/Users/Steve/Projects/Arduino/libraries/stevesch-Mesh/examples/minimal/minimal.ino"
#include <Arduino.h>

#include <Wire.h>
#include <stevesch-Mesh.h>
#include <stevesch-Display.h>

#ifdef AXP192_ADDRESS

#include "board-hw/axp192.h"
#endif

#include "simpleRenderer.h"

long tLastLoop = 0;
constexpr float kMaxDeltaTime = 0.100f;
float dtLastLoop = 0.0f;
void setup();
void loop();
#line 28 "C:/Users/Steve/Projects/Arduino/libraries/stevesch-Mesh/examples/minimal/minimal.ino"
void setup()
{
#ifdef I2C_SDA

  Wire.begin(I2C_SDA, I2C_SCL);
#ifdef AXP192_ADDRESS
  Wire.setClock(400000);
  axpSetup();
#endif
#endif

  Serial.begin(115200);
  while (!Serial)
    ;
  Serial.println("Setup initializing...");

  simpleRendererSetup();

  Serial.println("Setup complete.");
}

void loop()
{
  long tNow = micros();
  long delta = tNow - tLastLoop;
  float dt = 1.0e-6f * delta;
  dtLastLoop = dt;
  tLastLoop = tNow;
  if (dt > kMaxDeltaTime)
  {
    dt = kMaxDeltaTime;
  }

  simpleRendererLoop(dt);
}