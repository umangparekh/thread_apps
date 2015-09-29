/** @file hal/micro/cortexm3/efm32/regs.h
 *
 * @brief
 * This file pulls in the appropriate register
 * headers based on the specific Cortex-M3 being
 * compiled.
 *
 * THIS IS A GENERATED FILE.  DO NOT EDIT.
 *
 * <!-- Copyright 2015 Silicon Laboratories, Inc.                        *80*-->
 */

#if defined(CORTEXM3_EFM32LG990F256)
  #include "efm32lg990f256/regs.h"
#elif defined(CORTEXM3_EFR32)
  #include "efr32/regs.h"
#elif defined(CORTEXM3_EZR32LG230F256)
  #include "ezr32lg230f256/regs.h"
#else
  #error Unknown Cortex-M3 micro
#endif
