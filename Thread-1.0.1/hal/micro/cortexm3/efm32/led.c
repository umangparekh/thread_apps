/** @file hal/micro/cortexm3/led.c
 *  @brief LED manipulation routines; stack and example APIs
 *
 * <!-- Author(s): Brooks Barrett -->
 * <!-- Copyright 2007 by Ember Corporation. All rights reserved.       *80*-->
 */

#include PLATFORM_HEADER
#if !defined(MINIMAL_HAL) && defined(BOARD_HEADER)
  // full hal needs the board header to get pulled in
  #include "hal/micro/micro.h"
  #include BOARD_HEADER
#endif
#include "hal/micro/led.h"

// EFR32
#include "bsp.h"


#define GPIO_PxCLR_BASE (GPIO_PACLR_ADDR)
#define GPIO_PxSET_BASE (GPIO_PASET_ADDR)
#define GPIO_PxOUT_BASE (GPIO_PAOUT_ADDR)
// Each port is offset from the previous port by the same amount
#define GPIO_Px_OFFSET  (GPIO_PBCFGL_ADDR-GPIO_PACFGL_ADDR)

void halInternalInitLed(void)
{
  /* Initialize LEDs on-board */
  BSP_LedsInit();
}

void halSetLed(HalBoardLed led)
{
	BSP_LedSet(led);
}

void halClearLed(HalBoardLed led)
{
	BSP_LedClear(led);
}

void halToggleLed(HalBoardLed led)
{
	BSP_LedToggle(led);
}

#ifndef MINIMAL_HAL
void halStackIndicateActivity(bool turnOn)
{
 #if     NO_LED
  // Don't touch LEDs when built with NO_LED
 #else//!NO_LED
  if(turnOn) {
    halSetLed(BOARD_ACTIVITY_LED);
  } else {
    halClearLed(BOARD_ACTIVITY_LED);
  }
 #endif//NO_LED
}
#endif //MINIMAL_HAL
