#ifndef MBED_TEXTLCDCONFIG_H
#define MBED_TEXTLCDCONFIG_H

//Select hardware interface options to reduce memory footprint (multiple options allowed)
#define LCD_I2C        1           /* I2C Expander PCF8574/MCP23008 */

//Select options to reduce memory footprint (multiple options allowed)
#define LCD_UDC        1           /* Enable predefined UDC example*/
#define LCD_PRINTF     1           /* Enable Stream implementation */
#define LCD_ICON       0           /* Enable Icon implementation -2.0K codesize*/
#define LCD_ORIENT     0           /* Enable Orientation switch implementation -0.9K codesize*/
#define LCD_BIGFONT    1           /* Enable Big Font implementation -0.6K codesize */
#define LCD_INVERT     0           /* Enable display Invert implementation -0.5K codesize*/
#define LCD_POWER      1           /* Enable Power control implementation -0.1K codesize*/
#define LCD_BLINK      1           /* Enable UDC and Icon Blink control implementation -0.8K codesize*/
#define LCD_CONTRAST   0           /* Enable Contrast control implementation -0.9K codesize*/
#define LCD_TWO_CTRL   0           /* Enable LCD40x4 (two controller) implementation -0.1K codesize*/
#define LCD_FONTSEL    0           /* Enable runtime font select implementation using setFont -0.9K codesize*/

//Select option to activate default fonttable or alternatively use conversion for specific controller versions (eg PCF2116C, PCF2119R, SSD1803, US2066)
#define LCD_DEF_FONT   1           //Default HD44780 font

//Pin Defines for I2C PCF8574/PCF8574A or MCP23008 and SPI 74595 bus expander interfaces
//Different commercially available LCD portexpanders use different wiring conventions.
//LCD and serial portexpanders should be wired according to the tables below.
//
//Select Serial Port Expander Hardware module (one option only)
#define DEFAULT        0
#define ADAFRUIT       0
#define DFROBOT        0
#define LCM1602        1
#define YWROBOT        0
#define GYLCD          0
#define MJKDZ          0
#define SYDZ           0
#define WIDEHK         0
#define LCDPLUG        0


#if ((YWROBOT==1) || (LCM1602==1))
//Definitions for YWROBOT LCM1602 V1 Module mapping between serial port expander pins and LCD controller.
//Slaveaddress may be set by solderbridges (default 0x4E). SDA/SCL has no pullup Resistors onboard.
//
//Note: LCD RW pin must be kept LOW
//      E2 is not available on default hardware and so it does not support LCD40x4 (second controller)
//      BL is used to control backlight.

//I2C bus expander PCF8574 interface
#define LCD_BUS_I2C_RS (1 << 0)
#define LCD_BUS_I2C_RW (1 << 1)
#define LCD_BUS_I2C_E  (1 << 2)
#define LCD_BUS_I2C_BL (1 << 3)
#define LCD_BUS_I2C_D4 (1 << 4)
#define LCD_BUS_I2C_D5 (1 << 5)
#define LCD_BUS_I2C_D6 (1 << 6)
#define LCD_BUS_I2C_D7 (1 << 7)

#define LCD_BUS_I2C_E2 (1 << 1)


//Force I2C portexpander type
#define PCF8574        1
#define MCP23008       0

//Inverted Backlight control
#define BACKLIGHT_INV  0
#endif





//Bitpattern Defines for I2C PCF8574/PCF8574A, MCP23008 and SPI 74595 Bus expanders
//Don't change!
#define LCD_BUS_I2C_MSK (LCD_BUS_I2C_D4 | LCD_BUS_I2C_D5 | LCD_BUS_I2C_D6 | LCD_BUS_I2C_D7)
#if (BACKLIGHT_INV == 1)
#define LCD_BUS_I2C_DEF (0x00 | LCD_BUS_I2C_BL)
#else
#define LCD_BUS_I2C_DEF  0x00
#endif

#define LCD_BUS_SPI_MSK (LCD_BUS_SPI_D4 | LCD_BUS_SPI_D5 | LCD_BUS_SPI_D6 | LCD_BUS_SPI_D7)
#if (BACKLIGHT_INV == 1)
#define LCD_BUS_SPI_DEF (0x00 | LCD_BUS_SPI_BL)
#else
#define LCD_BUS_SPI_DEF  0x00
#endif


/* PCF8574 I2C portexpander slave address */
#define PCF8574_SA0    0x40
#define PCF8574_SA1    0x42
#define PCF8574_SA2    0x44
#define PCF8574_SA3    0x46
#define PCF8574_SA4    0x48
#define PCF8574_SA5    0x4A
#define PCF8574_SA6    0x4C
#define PCF8574_SA7    0x4E

//Some native I2C controllers dont support ACK. Set define to '0' to allow code to proceed even without ACK
//#define LCD_I2C_ACK    0
#define LCD_I2C_ACK    1


// Contrast setting, 6 significant bits (only supported for controllers with extended features)
// Voltage Multiplier setting, 2 or 3 significant bits (only supported for controllers with extended features)
#define LCD_DEF_CONTRAST    0x20

//ST7032 EastRising ERC1602FS-4 display
//Contrast setting 6 significant bits (0..63)
//Voltage Multiplier setting 3 significant bits:
// 0: 1.818V
// 1: 2.222V
// 2: 2.667V
// 3: 3.333V
// 4: 3.636V (ST7032 default)
// 5: 4.000V
// 6: 4.444V
// 7: 5.000V
#define LCD_ST7032_CONTRAST 0x28 
#define LCD_ST7032_RAB      0x04

//ST7036 EA DOGM1603 display
//Contrast setting 6 significant bits
//Voltage Multiplier setting 3 significant bits
#define LCD_ST7036_CONTRAST 0x28
#define LCD_ST7036_RAB      0x04

//SSD1803 EA DOGM204 display
//Contrast setting 6 significant bits
//Voltage Multiplier setting 3 significant bits
#define LCD_SSD1_CONTRAST   0x28
#define LCD_SSD1_RAB        0x06

//US2066/SSD1311 EastRising ER-OLEDM2002-4 display
//Contrast setting 8 significant bits, use 6 for compatibility
#define LCD_US20_CONTRAST   0x3F
//#define LCD_US20_CONTRAST   0x1F

//PCF2113, PCF2119 display
//Contrast setting 6 significant bits
//Voltage Multiplier setting 2 significant bits
#define LCD_PCF2_CONTRAST   0x20
#define LCD_PCF2_S12        0x02

//PT6314 VFD display
//Contrast setting 2 significant bits, use 6 for compatibility
#define LCD_PT63_CONTRAST   0x3F

//SPLC792A is clone of ST7032i
//Contrast setting 6 significant bits (0..63)
//Voltage Multiplier setting 3 significant bits:
// 0: 1.818V
// 1: 2.222V
// 2: 2.667V
// 3: 3.333V (SPLC792A default) 
// 4: 3.636V
// 5: 4.000V
// 6: 4.444V
// 7: 5.000V
#define LCD_SPLC792A_CONTRAST 0x28
#define LCD_SPLC792A_RAB      0x04

#endif //MBED_TEXTLCDCONFIG_H
