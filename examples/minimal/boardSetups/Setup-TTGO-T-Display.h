// See SetupX_Template.h for all options available

// These settings specify the hardware connections (LCD/OLED display pins,
// button pins, etc.) for a particular board configuration.

// This file is configured for:
// A TTGO T-Display (ESP32 with a built-in 135x240 display)

#define BOARD_NAME  "TTGO-T-Display"

// ----------------------------------------------------------------------------

// This section is somewhat project-specific-- tailor to your requirements:
#define BUTTON_1        0					// menu
#define BUTTON_2        35				// action / sleep
#define WAKE_GPIO				GPIO_SEL_35	// wake gpio (should correspond to sleep button)

// ----------------------------------------------------------------------------
// This section has board-specific advanced TFT options:
#define ENABLE_TFT_DMA          0
#define ENABLE_MULTI_CORE_COPY  1
// DMA, if available, typically results in best framerate.  Multi-core-copy
// is next fastest, and a standard (typically slowest) single-core copy
// is performed if both are disabled.

// for debugging or low-memory situations where flicker is not a concern:
#define DISABLE_BACKBUFFER      0

// ----------------------------------------------------------------------------

#define ST7789_DRIVER

#define TFT_WIDTH  135
#define TFT_HEIGHT 240

#define CGRAM_OFFSET      // Library will add offsets required

//#define TFT_MISO -1

#define TFT_MOSI            19
#define TFT_SCLK            18
#define TFT_CS              5
#define TFT_DC              16
#define TFT_RST             23

#define TFT_BL          4  // Display backlight control pin

#define TFT_INVOFF  0x20    //
#define TFT_INVON   0x21    //

#define TFT_BACKLIGHT_ON HIGH  // HIGH or LOW are options

#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF

#define SMOOTH_FONT

//#define SPI_FREQUENCY  27000000
// #define SPI_FREQUENCY  40000000  // Maximum for some common displays like ILI9341
#define SPI_FREQUENCY  80000000     // Significant framerate increase for multiple objects on TTGO T-Display

#define SPI_READ_FREQUENCY  6000000 // 6 MHz is the maximum SPI read speed for the ST7789V
