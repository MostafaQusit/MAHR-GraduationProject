#ifndef MAHR_PINSLIST_H_
#define MAHR_PINSLIST_H_

/* Pinout Map:
                                                                     _________________
3.3V_OUTPUT                                                       __|                 |__ GND
Enable (PU)                                                       __|                 |__ GPIO23: VSPI_MOSI/SPI_MOSI
GPIO36: RTC_GPIO00/ADC1_CH00/S_VP   (IN)                          __|                 |__ GPIO22: I2C_SCL/U0RTS
GPIO39: RTC_GPIO03/ADC1_CH03/S_VP   (IN)                          __|                 |__ GPIO01: U0TXD (PU) ×
GPIO34: RTC_GPIO04/ADC1_CH06/VDET_1 (IN)                          __|                 |__ GPIO03: U0RXD (PU) ×
GPIO35: RTC_GPIO05/ADC1_CH07/VDET_2 (IN)                          __|                 |__ GPIO21: I2C_SDA
GPIO32: RTC_GPIO09/ADC1_CH04/TOUCH9/XTAL_32K_P                    __|                 |__ GND
GPIO33: RTC_GPIO08/ADC1_CH05/TOUCH8/XTAL_32K_N                    __|                 |__ GPIO19: VSPI_MISO/U0CTS
GPIO25: RTC_GPIO06/ADC2_CH08/DAC_1                                __|                 |__ GPIO18: VSPI_SCK
GPIO26: RTC_GPIO07/ADC2_CH09/DAC_2                                __|                 |__ GPIO05: VSPI_SS/SDIO (PU) !
GPIO27: RTC_GPIO17/ADC2_CH07/TOUCH7                               __|                 |__ GPIO17: U2TXD
GPIO14: RTC_GPIO16/ADC2_CH06/TOUCH6/HSPI_SCK /MTMS           (PU) __|                 |__ GPIO16: U2RXD
GPIO12: RTC_GPIO15/ADC2_CH05/TOUCH5/HSPI_MISO/MTDS/VDD_Flash (PD) __|                 |__ GPIO04: RTC_GPIO10/ADC2_CH00/TOUCH0      (PD)
GND                                                               __|                 |__ GPIO00: RTC_GPIO11/ADC2_CH01/TOUCH1/BOOT (PU) !
GPIO13: RTC_GPIO14/ADC2_CH04/TOUCH4/HSPI_MOSI/MTCK           (PD) __|                 |__ GPIO02: RTC_GPIO12/ADC2_CH02/TOUCH2      (PD) !
GPIO09: D2 /U1RXD (PU) ×                                          __|                 |__ GPIO15: RTC_GPIO13/ADC2_CH03/TOUCH3/LOG  (PU) !
GPIO10: D3 /U1TXD (PU) ×                                          __|                 |__ GPIO08: D1 /U2CTS (PU) ×
GPIO11: CMD/U1RTS (PU) ×                                          __|                 |__ GPIO07: D0 /U2RTS (PU) ×
5.0V_INTPUT                                                       __|                 |__ GPIO06: SCK/U1CTS (PU) ×
                                                                     _________________

Notes:
    - During Wifi working, All Channels of ADC2 will not work.
    - All pins with (×) are not recommended to use them, so avoid them.
    - Pins with (!) Pay attention as their behavior can be unpredictable, mainly during boot, Don’t use them unless you absolutely need to :
        * GPIO00: Must be HIGH during boot and LOW for programming!
        * GPIO02: Must be LOW during boot!
        * GPIO12: Must be LOW during boot!
        * GPIO05: Must be HIGH during boot! (tip: nothing happen if LOW)
        * GPIO15: Must be HIGH during boot! (perevent startup log if LOW)
*/

// ESP32 (MASTER):
    // SPI:
        //built-in macros.
        #define SPI_MOSI    23
        #define SPI_MISO    19
        #define SPI_SCK     18
        #define SPI_SS1      5
        #define SPI_SS2      4
    /********************************************/
    // IMU(MPU9255):
        //built-in macros.
        #define IMU_SDA     21
        #define IMU_SCL     22
    /********************************************/
    // Ultrasonics:
        #define USTX_FL     26  // Far  Left
        #define USRX_FL     34

        #define USTX_NL     27  // Near Left
        #define USRX_NL     35

        #define USTX_NR     32  // Near Right
        #define USRX_NR     36

        #define USTX_FR     33  // Far  Right
        #define USRX_FR     39
    /********************************************/
    // GSM Module :
        #define GSM_TX      16
        #define GSM_RX      17
        #define GSM_RST     25
        #define GSM_DTR     13
        #define GSM_RING    14

/*******************************************************************/
// ESP32 (SLAVE 1):
    // SPI:
        //built-in macros.
        #define SPI_MOSI    23
        #define SPI_MISO    19
        #define SPI_SCK     18
        #define SPI_SS       5
    /********************************************/
    // DC motors:
        // Right side:
        #define MOTOR_RIGHT_PWM 27
        #define MOTOR_RIGHT_DIR 26
        // Left side:
        #define MOTOR_LEFT_PWM  33
        #define MOTOR_LEFT_DIR  32
    /********************************************/
    // Stepper motor: (z-axis)
        #define STEPPER_STP     14 //21
        #define STEPPER_DIR     15 //22

        #define UPPER_LS         4 //21
        #define LOWER_LS        13
    /********************************************/
    // Encoders:
        // Right side:
        #define ENCODER_RIGHT_A 34
        #define ENCODER_RIGHT_B 35
        // Left side:
        #define ENCODER_LEFT_A  36
        #define ENCODER_LEFT_B  39
    /********************************************/
    // MP3:
        #define MP3_TX          16
        #define MP3_RX          17
        #define MP3_RST         25

/*******************************************************************/
// ESP32 (SLAVE 2):
    // soon...

#endif