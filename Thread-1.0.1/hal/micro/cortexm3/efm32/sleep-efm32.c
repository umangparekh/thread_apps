/** @file hal/micro/cortexm3/sleep.c
 *
 * @brief EM3XX micro specific sleep functions.
 *
 * <!-- Copyright 2013 Silicon Laboratories, Inc.                       *80* -->
 */

#include PLATFORM_HEADER
#include "hal/micro/micro-common.h"
#include "hal/micro/micro.h"
#include "hal/micro/cortexm3/micro-common.h"
#include "hal/micro/cortexm3/memmap.h"
#include "hal/micro/cortexm3/mfg-token.h"
#ifdef MINIMAL_HAL
  #define BYPASS_MPU(blah) blah
#else
  #include BOARD_HEADER
  #include "hal/micro/cortexm3/mpu.h"
#endif //defined(MINIMAL_HAL)
#include "hal/micro/cortexm3/diagnostic.h"
#include "hal/micro/cortexm3/wake-gpio.h"

#include "em_emu.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "rtcdriver.h"
#include "serial/com.h"

//[[
//This function is responsible for manually putting every piece of the Low
//Voltage domain through a fake reset cycle.  This function is intended to
//mimick a Low Voltage domain reset, with the except being any piece of
//the chip that is enabled via cstartup (since cstartup will not be called
//after this function).
void halInternalMimickLvReset(void)
{
}


#ifdef SLEEP_TRACE //WBB350FIXME -- Find a less intrusive technique
  extern bool sleepTraceOn;
  extern uint8_t sleepTraceBuffer[];
  extern uint8_t sleepTraceIndex;
  extern uint8_t sleepTraceDelayPosition;
  #define SLEEP_TRACE_ADD_MARKER(byte)                 \
    do {                                               \
      if(sleepTraceOn) {                               \
        if(sleepTraceIndex<50) {                       \
          sleepTraceBuffer[sleepTraceIndex] = byte;    \
        }                                              \
        sleepTraceIndex++;                             \
      }                                                \
    } while(0)
  #define SLEEP_TRACE_1SEC_DELAY(position)             \
    do {                                               \
      if(sleepTraceDelayPosition==position) {          \
        uint8_t delayCnt=(20*1);                         \
        while(delayCnt-->0) {                          \
          halCommonDelayMicroseconds(50000);           \
        }                                              \
      }                                                \
    } while(0)
#else //SLEEP_TRACE
  #define SLEEP_TRACE_ADD_MARKER(byte) do{}while(0)
  #define SLEEP_TRACE_1SEC_DELAY(position) do{}while(0)
#endif //SLEEP_TRACE
//]]

// halInternalWakeEvent temporarily mapped to GPIO->IF 
static uint32_t halInternalWakeEvent=0;

uint32_t halGetWakeInfo(void)
{
  return halInternalWakeEvent;
}

void halInternalSleep(SleepModes sleepMode)
{
  //SLEEPMODE_POWERDOWN and SLEEPMODE_POWERSAVE are deprecated.  Remap them
  //to their appropriate, new mode name.
  if(sleepMode == SLEEPMODE_POWERDOWN) {
    sleepMode = SLEEPMODE_MAINTAINTIMER;
  } else if(sleepMode == SLEEPMODE_POWERSAVE) {
    sleepMode = SLEEPMODE_WAKETIMER;
  }

  halInternalWakeEvent = 0; //clear old wake events

  bool restoreWatchdog = halInternalWatchDogEnabled();

  bool skipSleep = false;
  //disable watchdog while sleeping (since we can't reset it asleep)
  halInternalDisableWatchDog(MICRO_DISABLE_WATCH_DOG_KEY);
  //[[
  SLEEP_TRACE_ADD_MARKER('A');
  //]]
  //Normal ATOMIC/INTERRUPTS_OFF/INTERRUPTS_ON uses the BASEPRI mask
  //to juggle priority levels so that the fault handlers can always
  //be serviced.  But, the WFI instruction is only capable of
  //working with the PRIMASK bit.  Therefore, we have to switch from
  //using BASEPRI to PRIMASK to keep interrupts disabled so that the
  //WFI can return on an interrupt
  //Globally disable interrupts with PRIMASK
  _setPriMask();
  //Bring the BASEPRI up to 0 to allow interrupts (but still disabled
  //with PRIMASK)
  INTERRUPTS_ON();
  COM_InternalPowerDown();

  if(!skipSleep) 
  {
#ifdef BOOTLOADER_OPEN
          #error no bootloader support yet
#else

    
    switch(sleepMode)
    {
      case SLEEPMODE_IDLE:
        //[[
        SLEEP_TRACE_ADD_MARKER('B');
        //]]
        EMU_EnterEM1();
        break;
      // there is no difference between wake/maintain timer
      case SLEEPMODE_WAKETIMER:
      case SLEEPMODE_MAINTAINTIMER:
        //[[
        SLEEP_TRACE_ADD_MARKER('C');
        //]]
        EMU_EnterEM2(true);
        break;
      case SLEEPMODE_NOTIMER:
        //[[
        SLEEP_TRACE_ADD_MARKER('E');
        //]]
        EMU_EnterEM3(true);
        break;
      default:
        //Oops!  Invalid sleepMode parameter.
        assert(0);
        break;
    }
    //[[
    SLEEP_TRACE_ADD_MARKER('F');
    //]]
#endif // BOOTLOADER_OPEN
  }
  else 
  {
    //Record the fact that we skipped sleep
    // halInternalWakeEvent |= BIT32(SLEEPSKIPPED_INTERNAL_WAKE_EVENT_BIT);
  }

  halInternalWakeEvent|=GPIO_IntGet();
  //restart watchdog if it was running when we entered sleep
  //do this before dispatching interrupts while we still have tight
  //control of code execution
  if(restoreWatchdog) {
    halInternalEnableWatchDog();
  }
  
  COM_InternalPowerUp();
  //The WFI instruction does not actually clear the PRIMASK bit, it
  //only allows the PRIMASK bit to be bypassed.  Therefore, we must
  //manually clear PRIMASK to reenable all interrupts.
  _clearPriMask();
  //[[
  SLEEP_TRACE_ADD_MARKER('G');
  //]]
}


static uint32_t savedGPIO_IEN;
void halSleepWithOptions(SleepModes sleepMode, WakeMask gpioWakeBitMask)
{
  savedGPIO_IEN = GPIO->IEN;
  GPIO->IEN = gpioWakeBitMask;
  halInternalSleep(sleepMode);
  GPIO->IEN = savedGPIO_IEN;
}

void halSleep(SleepModes sleepMode)
{
  halInternalSleep(sleepMode);
}
