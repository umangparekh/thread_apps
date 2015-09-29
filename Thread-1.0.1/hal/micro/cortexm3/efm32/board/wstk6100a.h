/** @file hal/micro/cortexm3/efm32/board/wstk6100a.h
 * See @ref board for detailed documentation.
 *
 * <!-- Copyright 2013 Silicon Laboratories, Inc.                        *80*-->
 */

/** @addtogroup board
 *  @brief Functions and definitions specific to the breakout board.
 *
 *@{
 */

#ifndef __BOARD_H__
#define __BOARD_H__

#include "bspconfig.h"

/** @name LED Definitions
 *
 * The following are used to aid in the abstraction with the LED
 * connections.  The microcontroller-specific sources use these
 * definitions so they are able to work across a variety of boards
 * which could have different connections.  The names and ports/pins
 * used below are intended to match with a schematic of the system to
 * provide the abstraction.
 *
 * The ::HalBoardLedPins enum values should always be used when manipulating the
 * state of LEDs, as they directly refer to the GPIOs to which the LEDs are
 * connected.
 *
 * \b Note: LEDs 0 and 1 are on the RCM.
 *
 * \b Note: LED 2 is on the breakout board (dev0680).
 *
 * \b Note: LED 3 simply redirects to LED 2.
 */
//@{

/**
 * @brief Assign each GPIO with an LED connected to a convenient name.
 * ::BOARD_ACTIVITY_LED and ::BOARD_HEARTBEAT_LED provide a further layer of
 * abstraction on top of the 3 LEDs for verbose coding.
 */
enum HalBoardLedPins {
  BOARDLED0 = 0,
  BOARDLED1 = 1,
  BOARDLED2 = BOARDLED0,
  BOARDLED3 = BOARDLED1,
  BOARD_ACTIVITY_LED  = BOARDLED0,
  BOARD_HEARTBEAT_LED = BOARDLED1
};

/** @} END OF LED DEFINITIONS  */

/** @name Button Definitions
 *
 * The following are used to aid in the abstraction with the Button
 * connections.  The microcontroller-specific sources use these
 * definitions so they are able to work across a variety of boards
 * which could have different connections.  The names and ports/pins
 * used below are intended to match with a schematic of the system to
 * provide the abstraction.
 *
 * The BUTTONn macros should always be used with manipulating the buttons
 * as they directly refer to the GPIOs to which the buttons are connected.
 *
 * @note The GPIO number must match the IRQ letter
 */
//@{
/**
 * @brief The interrupt service routine for all buttons.
 */
#define BUTTON_ISR buttonIrqCallback
/**
 * @brief The actual GPIO BUTTON0 is connected to.  This define should
 * be used whenever referencing BUTTON0.
 */
#define BUTTON0         (6)
/**
 * @brief The GPIO port for BUTTON0.
 */
#define BUTTON0_PORT        (gpioPortF)

/**
 * @brief The actual GPIO BUTTON1 is connected to.  This define should
 * be used whenever referencing BUTTON1, such as controlling if pieces
 * are compiled in.
 * Remember that other pieces that might want to use IRQC.
 */
#define BUTTON1         (7)
/**
 * @brief The GPIO input register for BUTTON1.
 */
#define BUTTON1_PORT        (gpioPortF)
//@} //END OF BUTTON DEFINITIONS

/** @name Temperature sensor ADC channel
 *
 * Define the analog input channel connected to the LM-20 temperature sensor.
 * The scale factor compensates for different platform input ranges.
 * PB5/ADC0 must be an analog input.
 * PC7 must be an output and set to a high level to power the sensor.
 *
 *@{
 */
/**
 * @brief The analog input channel to use for the temperature sensor.
 */
#define TEMP_SENSOR_ADC_CHANNEL ADC_SOURCE_ADC0_VREF2
/**
 * @brief The scale factor to compensate for different input ranges.
 */
#define TEMP_SENSOR_SCALE_FACTOR 1
/** @} END OF TEMPERATURE SENSOR ADC CHANNEL DEFINITIONS */

/** @name Packet Trace
 *
 * When ::PACKET_TRACE is defined, ::GPIO_PACFGH will automatically be setup by
 * halInit() to enable Packet Trace support on PA4 and PA5,
 * in addition to the configuration specified below.
 *
 * @note This define will override any settings for PA4 and PA5.
 */
//@{
/**
 * @brief This define does not equate to anything.  It is used as a
 * trigger to enable Packet Trace support on the breakout board (dev0680).
 */
#define PACKET_TRACE  // We do have PACKET_TRACE support

// We have to define a location for clock and dout even if we're in UART mode
// so set these to match the values used on the WSTK
#define RADIO_PTI_DCLK_ROUTELOC 6
#define RADIO_PTI_DOUT_ROUTELOC 6

//@} //END OF PACKET TRACE DEFINITIONS


/** @brief Radio HoldOff not supported
 */
#define halInternalInitRadioHoldOff() /* no-op */


/* Board Controller Version */
#define BCP_VERSION   2

#define halInternalInitBoard() do {                                \
 halInternalPowerUpBoard();                                        \
 halInternalEnableHFXO();                                          \
 halInternalConfigPti();                                           \
 halInternalEnableDCDC();                                          \
} while(0)


#define halInternalPowerUpBoard() do {                             \
BSP_LedsInit();                                                    \
} while(0)

#define halInternalPowerDownBoard() do {                           \
} while(0)

#define halInternalEnableHFXO() do {                               \
  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_WSTK_DEFAULT;       \
  CMU_HFXOInit(&hfxoInit);                                         \
} while(0)

#ifndef DISABLE_PTI
#define halInternalConfigPti() do {                                \
  RADIO_PTIInit_t ptiInit = RADIO_PTI_INIT;                        \
  RADIO_PTI_Init(&ptiInit);                                        \
} while(0)
#else
#define halInternalConfigPti() do {                                \
  RADIO_PTIInit_t ptiInit = RADIO_PTI_INIT;                        \
  ptiInit.mode = RADIO_PTI_MODE_DISABLED;                          \
  RADIO_PTI_Init(&ptiInit);                                        \
} while(0)
#endif

/* Set power config 2 - DCDCTODVDD - needed for the SLWSTK6100A_EFR32MG hardware configuration.
   Use low-noise DCDC mode in EM0 and low-power mode in EM2, 3 and 4. */
// this uses a hack that we need now since the actual code doesnt work
#ifdef DISABLE_DCDC
#define halInternalEnableDCDC()
#else
#define halInternalEnableDCDC() do {                           \
  EMU->PWRCFG = EMU_PWRCFG_PWRCFG_DCDCTODVDD;                  \
  EMU->DCDCCTRL = EMU_DCDCCTRL_DCDCMODE_LOWNOISE |             \
                  EMU_DCDCCTRL_DCDCMODEEM23_EM23LOWPOWER |     \
                  EMU_DCDCCTRL_DCDCMODEEM4_EM4LOWPOWER;        \
} while(0)
#endif
/**
 * Enable VCOM Pin
 */
#define halEnableVCOM()  do {                                      \
    GPIO_PinModeSet(gpioPortA, 5, gpioModePushPull, 1);            \
    GPIO_PinOutSet(gpioPortA, 5);                                  \
  } while (0)

#endif //__BOARD_H__

/**
 *  Instantiate variables that are used in the board header
 *  @TODO: This should probably involve some configurator tool and not global
 *  variables, but this is the quickest way to make things work for now.
 */
#define DEFINE_BOARD_GPIO_CFG_VARIABLES()

/** @} END Board Specific Functions */

/** @} END addtogroup */
