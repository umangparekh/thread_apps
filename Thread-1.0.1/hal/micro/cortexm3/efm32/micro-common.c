// -----------------------------------------------------------------------------
// @file
// @brief EFM micro specific HAL functions common to full and minimal hal
// 
// @author Silicon Laboratories Inc.
// @version 1.0.0
// 
// @section License
// <b>(C) Copyright 2014 Silicon Laboratories, http://www.silabs.com</b>
// 
// This file is licensed under the Silabs License Agreement. See the file
// "Silabs_License_Agreement.txt" for details. Before using this software for
// any purpose, you must agree to the terms of that agreement.
// 
// -----------------------------------------------------------------------------

#include PLATFORM_HEADER
#include "em_device.h"
#include "em_wdog.h"
#include "em_cmu.h"

#include "include/error.h"
#include "hal/micro/micro-common.h"
#include "hal/micro/cortexm3/efm32/micro-common.h"
#include "hal/micro/adc.h"
#include "ustimer.h"

#if defined(BOARD_HEADER) && !defined(MINIMAL_HAL)
  #include BOARD_HEADER
#endif //defined(BOARD_HEADER)

//NOTE:  The reason ENABLE_OSC32K gets translated into an uint8_t is so that
//       haltest can override the build time configuration and force use of
//       a different OSC32K mode when running the "RTC" command which is used
//       to check proper connectivity of the 32kHz XTAL or external clock.
#define DO_EXPAND(sym)  1 ## sym
#define EXPAND(sym)     DO_EXPAND(sym)
#define CHECK_BLANK_DEFINE(sym) (1 == EXPAND(sym))

#ifdef  ENABLE_OSC32K
  #if CHECK_BLANK_DEFINE(ENABLE_OSC32K) // Empty #define => OSC32K_CRYSTAL
    #undef  ENABLE_OSC32K
    #define ENABLE_OSC32K OSC32K_CRYSTAL // Default if ENABLE_OSC32K is blank
  #endif
#else//!ENABLE_OSC32K
  #define ENABLE_OSC32K OSC32K_DISABLE // Default if BOARD_HEADER doesn't say
#endif//ENABLE_OSC32K
uint8_t useOsc32k = ENABLE_OSC32K;
#ifndef OSC32K_STARTUP_DELAY_MS
  #define OSC32K_STARTUP_DELAY_MS (0) // Not all BOARD_HEADERs define this
#endif//OSC32K_STARTUP_DELAY_MS

#if     DISABLE_OSC24M
  // For compatibility, DISABLE_OSC24M takes precedence over ENABLE_OSC24M
  #undef  ENABLE_OSC24M
  #define ENABLE_OSC24M 0
#endif//DISABLE_OSC24M

#ifndef ENABLE_OSC24M
  #define  ENABLE_OSC24M 24 // Default is 24 MHz Xtal
#endif//ENABLE_OSC24M

//[[
#if     (defined(EMBER_EMU_TEST) || defined(EMBER_EMU))
  extern bool halInternalIAmAnEmulator(void); // In hal library
  #define HALF_SPEED_SYSCLK halInternalIAmAnEmulator()
#else//!(defined(EMBER_EMU_TEST) || defined(EMBER_EMU))
//]]
  #define HALF_SPEED_SYSCLK (false)
//[[
#endif//(defined(EMBER_EMU_TEST) || defined(EMBER_EMU))
//]]

#if     BTL_SYSCLK_KNOWN

// serial-ota-bootloader must use 24 MHz Xtal
// This saves some much needed code space so serial-ota-bootloader fits

#else//!BTL_SYSCLK_KNOWN

// Expose configuration to library code
//const int16_t halSystemXtalKHz = ENABLE_OSC24M * 1000;

// -----------------------------------------------------------------------------
// Clock Functions
uint16_t halSystemClockKHz(void)
{
  return 0;
}

uint16_t halMcuClockKHz(void)
{
  return 0;
}

uint16_t halPeripheralClockKHz()
{
  return 0;
}

#endif//BTL_SYSCLK_KNOWN

extern void halInternalClocksCalibrateFastRc(void); // Belongs in a clocks.h
extern void halInternalClocksCalibrateSlowRc(void); // Belongs in a clocks.h

void halInternalCalibrateFastRc(void)
{
}

void halInternalCalibrateSlowRc(void)
{
}
// -----------------------------------------------------------------------------
// Watchdog Functions
void halInternalEnableWatchDog( void )
{
  /* Turn on the oscillator which will be used. */
  CMU_ClockEnable(cmuClock_CORELE, true);
  CMU_OscillatorEnable(cmuOsc_ULFRCO, true, true);
  WDOG_Init_TypeDef init = WDOG_INIT_DEFAULT;
  init.clkSel = wdogClkSelULFRCO;
  init.perSel = wdogPeriod_2k;
  WDOG_Enable( true );
  WDOG_Init(&init);
}

void halInternalResetWatchDog( void )
{
  WDOG_Feed();
}

void halInternalDisableWatchDog( uint8_t magicKey )
{
  if ( magicKey == MICRO_DISABLE_WATCH_DOG_KEY ) 
  {
    /* The watchdog will hang if you attempt to disable it when already disabled */
    if (halInternalWatchDogEnabled())
    {
      WDOG_Enable( false );
    }
  }
}

bool halInternalWatchDogEnabled( void )
{
  if ( WDOG->CTRL & WDOG_CTRL_EN )
  {
    return true;
  }
  else
  {
    return false;
  }
}

void halGpioSetConfig(uint32_t gpio, uint32_t config)
{
}

//[[ Most of the system-timer functionality is part of the hal-library
//  This functionality is kept public because it depends on configuration
//  options defined in the BOARD_HEADER.  Only for the full HAL, though. In
//  the minimal HAL if the user has to supply the two ifdefs they do so
//  in whatever manner they choose.
//]]
uint16_t halMeasureVdd(ADCRateType rate)
{
  return 0;
}
