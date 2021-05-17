#include <Arduino.h>
// This is a test example for Render

#include <Wire.h>
#include <stevesch-Mesh.h>
#include <stevesch-Display.h>

#ifdef AXP192_ADDRESS
	// AXP192 power management chip support (M5StickC/M5StickC Plus)
#include "board-hw/axp192.h"
#endif

#include "simpleRenderer.h"

long tLastLoop = 0;
constexpr float kMaxDeltaTime = 0.100f;
float dtLastLoop = 0.0f;

void setup()
{
#ifdef I2C_SDA
  // mostly for M5StickC/M5StickC Plus:
	Wire.begin(I2C_SDA, I2C_SCL);
	#ifdef AXP192_ADDRESS
	Wire.setClock(400000);
	axpSetup();
	#endif
#endif

  Serial.begin(115200);
  while (!Serial);
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
