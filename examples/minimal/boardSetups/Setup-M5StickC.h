// See SetupX_Template.h for all options available

// These settings specify the hardware connections (LCD/OLED display pins,
// button pins, etc.) for a particular board configuration.

// This file is configured for:
// An M5StickC (ESP32-PICO with a built-in 80x160 display)

#define BOARD_NAME  "M5StickC"

// ----------------------------------------------------------------------------

// This section is somewhat project-specific-- tailor to your requirements:
#define BUTTON_1				37
#define BUTTON_2				39
#define WAKE_GPIO				GPIO_SEL_39
#define USE_IMU		1
#define IMU_ORIENTATION_E_N_U
#define IMU_NO_MAG								// no compass on MPU6886
#define IMU_ADDRESS 0x68
#define AXP192_ADDRESS 0x34				// for backlight/power control
#define I2C_SDA     21
#define I2C_SCL     22

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

#define TFT_WIDTH  80
#define TFT_HEIGHT 160

#define ST7735_GREENTAB160x80

// #define CGRAM_OFFSET      // Library will add offsets required
#define TFT_RGB_ORDER TFT_BGR  // Colour order Blue-Green-Red

//#define TFT_MISO -1

#define TFT_MOSI            15
#define TFT_SCLK            13
#define TFT_CS              5
#define TFT_DC              23
#define TFT_RST             18

// #define TFT_BL          -1  // Display backlight control pin

// #define TFT_INVOFF  0x20    //
// #define TFT_INVON   0x21    //

// #define TFT_BACKLIGHT_ON HIGH  // HIGH or LOW are options

#define LOAD_GLCD
#define LOAD_FONT2
// #define LOAD_FONT4
// #define LOAD_FONT6
// #define LOAD_FONT7
// #define LOAD_FONT8
// #define LOAD_GFXFF

#define SMOOTH_FONT

// #define SPI_FREQUENCY  20000000  // 
#define SPI_FREQUENCY  27000000  // recommended for M5StickC
// #define SPI_FREQUENCY  35000000  // M5StickC: framerate increase, seems to work, but...
// #define SPI_FREQUENCY  40000000  // M5StickC: results in screen offset (bottom edge noise)
// #define SPI_FREQUENCY  80000000  // M5StickC: black screen

#define SPI_READ_FREQUENCY  20000000
#define SPI_TOUCH_FREQUENCY  2500000

// #define SUPPORT_TRANSACTIONS
