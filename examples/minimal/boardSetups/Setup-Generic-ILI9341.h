// See SetupX_Template.h for all options available

// These settings specify the hardware connections (LCD/OLED display pins,
// button pins, etc.) for a particular board configuration.

// This file is configured for:
// An ESP32 devkit module with a 240x320 ILI9341 TFT display.
// Note that pin numbers for hand-wired boards will need to match pins below.

#define BOARD_NAME  "Generic-ILI9341-240x320"

// ----------------------------------------------------------------------------

// This section is somewhat project-specific-- tailor to your requirements:
#define BUTTON_1        0					// menu
#define BUTTON_2        35				// action / sleep
#define WAKE_GPIO				GPIO_SEL_35	// wake gpio (should correspond to sleep button)

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

#define ILI9341_DRIVER

#define TFT_WIDTH  240
#define TFT_HEIGHT 320

#define TFT_MISO 19
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS    5  // 15  // Chip select control pin
#define TFT_DC    17 // 2  // Data Command control pin
#define TFT_RST   16 // 4  // Reset pin (could connect to RST pin)
//#define TFT_RST  -1  // Set TFT_RST to -1 if display RESET is connected to ESP32 board RST
#define TFT_BL    4   // manual wiring of TFT BL (BLK) to this GPIO
#define TFT_BACKLIGHT_ON HIGH  // HIGH or LOW are options

#define TOUCH_CS 22     // Chip select pin (T_CS) of touch screen

#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
// #define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
// #define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
// #define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:.
// #define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
// #define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

#define SMOOTH_FONT

// #define SPI_FREQUENCY  20000000
// #define SPI_FREQUENCY  27000000
#define SPI_FREQUENCY  40000000
// #define SPI_FREQUENCY  80000000

#define SPI_READ_FREQUENCY  20000000

#define SPI_TOUCH_FREQUENCY  2500000
