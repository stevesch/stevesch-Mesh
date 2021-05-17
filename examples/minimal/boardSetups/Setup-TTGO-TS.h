// See SetupX_Template.h for all options available

// These settings specify the hardware connections (LCD/OLED display pins,
// button pins, etc.) for a particular board configuration.

// This file is configured for:
// A TTGO TS (ESP32 with a built-in 1.8" 128x160 display)
// NOTE: variations of similar boards exist.  Some require different settings
// according to the "tab color" of the protective cover that came on the display.
// See ST7735_GREENTAB2, ST7735_GREENTAB below

#define BOARD_NAME  "TTGO-TS-128x160"

// ----------------------------------------------------------------------------

// This section is somewhat project-specific-- tailor to your requirements:

// 35=Left, 34=Center, 39=Right
#define BUTTON_1        35					// menu
#define BUTTON_2        34					// send/sleep (should correspond to WAKE_GPIO)
#define BUTTON_3				39					// action
#define WAKE_GPIO				GPIO_SEL_34	// wake gpio (should correspond to sleep button)

#define USE_IMU		1
#define IMU_ADDRESS 0x68
#define IMU_ORIENTATION_E_N_D_GYRO_REVERSED
#define I2C_SDA     19
#define I2C_SCL     18

// ----------------------------------------------------------------------------
// This section has board-specific advanced TFT options:
#define ENABLE_TFT_DMA          1
#define ENABLE_MULTI_CORE_COPY  0
// DMA, if available, typically results in best framerate.  Multi-core-copy
// is next fastest, and a standard (typically slowest) single-core copy
// is performed if both are disabled.

// for debugging or low-memory situations where flicker is not a concern:
#define DISABLE_BACKBUFFER      0

// ----------------------------------------------------------------------------

#define ST7735_DRIVER

#define TFT_WIDTH  128
#define TFT_HEIGHT 160

#define ST7735_GREENTAB2
// Other TFT options:
// #define ST7735_GREENTAB
// #define ST7735_GREENTAB2       // Use if you get random pixels on two edges of green tab display
// #define ST7735_GREENTAB3       // Use if you get random pixels on edge(s) of 128x128 screen
// #define ST7735_GREENTAB128     // Use if you only get part of 128x128 screen in rotation 0 & 1
// #define ST7735_GREENTAB160x80  // Use if you only get part of 128x128 screen in rotation 0 & 1
// #define ST7735_REDTAB160x80    // for https://www.aliexpress.com/item/ShengYang-1pcs-IPS-0-96-inch-7P-SPI-HD-65K-Full-Color-OLED-Module-ST7735-Drive/32918394604.html
// #define ST7735_REDTAB
// #define ST7735_BLACKTAB        // Display with no offsets

#define TFT_MISO    -1
#define TFT_MOSI    23
#define TFT_SCLK    5
#define TFT_CS      16  // Chip select control pin
#define TFT_DC      17  // Data Command control pin
// #define TFT_RST     -1  // Reset pin (could connect to RST pin)
#define TFT_RST     9  // Reset pin (could connect to RST pin)
#define TFT_BL      27

#define TFT_BACKLIGHT_ON HIGH  // HIGH or LOW are options

#define LOAD_GLCD  // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2 // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
// #define LOAD_FONT4 // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
// #define LOAD_FONT6 // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
// #define LOAD_FONT7 // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:.
// #define LOAD_FONT8 // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
//#define LOAD_FONT8N // Font 8. Alternative to Font 8 above, slightly narrower, so 3 digits fit a 160 pixel TFT
// #define LOAD_GFXFF // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

#define SMOOTH_FONT

// #define SPI_FREQUENCY  20000000 // Maximum to use SPIFFS
#define SPI_FREQUENCY  27000000 //

#define SUPPORT_TRANSACTIONS
