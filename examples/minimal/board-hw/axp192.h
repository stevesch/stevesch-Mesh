// AXP192 power management chip support (M5StickC)
#ifdef AXP192_ADDRESS
#include <FreeRTOS.h>

void axpSetup(
    bool disableLDO2 = false, bool disableLDO3 = false, bool disableRTC = false,
    bool disableDCDC1 = false, bool disableDCDC3 = false, bool disableLDO0 = false);

uint8_t axpReadRegister(uint8_t reg);
void axpWriteRegister(uint8_t reg, uint8_t data);
void axpSetLDO2(bool state);

bool axpSetBacklight(bool lightOn);

#endif
