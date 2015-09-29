// Copyright 2015 Silicon Laboratories, Inc.

#include PLATFORM_HEADER
#include CONFIGURATION_HEADER
#include EMBER_AF_API_STACK
#include EMBER_AF_API_HAL
#include "thread-bookkeeping.h"

// If serial functionality is enabled, we will initiaze the serial ports during
// startup.  This has to happen after the HAL is initiazed.
#ifdef EMBER_AF_API_SERIAL
  #include EMBER_AF_API_SERIAL
  #define SERIAL_INIT EMBER_AF_SERIAL_PORT_INIT
#else
  #define SERIAL_INIT()
#endif

// If printing is enabled, we will print some diagnostic information about the
// most recent reset and also during runtime.  On some platforms, extended
// diagnostic information is available.
#if defined(EMBER_AF_API_SERIAL) && defined(EMBER_AF_PRINT_ENABLE)
  #ifdef EMBER_AF_API_DIAGNOSTIC_CORTEXM3
    #include EMBER_AF_API_DIAGNOSTIC_CORTEXM3
  #endif
  static void printResetInformation(void);
  #define PRINT_RESET_INFORMATION printResetInformation
  #define emberAfGuaranteedPrint(...) \
    emberSerialGuaranteedPrintf(APP_SERIAL, __VA_ARGS__)
  #define emberAfGuaranteedPrintln(format, ...) \
    emberSerialGuaranteedPrintf(APP_SERIAL, format "\r\n", __VA_ARGS__);
#else
  #define PRINT_RESET_INFORMATION()
  #define emberAfGuaranteedPrint(...)
  #define emberAfGuaranteedPrintln(...)
#endif

#ifdef EMBER_AF_API_MANAGEMENT_CLIENT
  #include EMBER_AF_API_MANAGEMENT_CLIENT
  #define PROCESS_MANAGEMENT_COMMAND processManagementCommand
#else
  #define PROCESS_MANAGEMENT_COMMAND()
#endif

// Our entry point is typically main(), except during testing.
#ifdef EMBER_TEST
  #define MAIN nodeMain
#else
  #define MAIN main
#endif

static bool init = false;
static void loop(void);

extern const EmberEventData emAppEvents[];
EmberTaskId emAppTask;

int MAIN(MAIN_FUNCTION_PARAMETERS)
{
  // Let the application and plugins do early initialization.  This function is
  // generated.
  emberAfMain(MAIN_FUNCTION_ARGUMENTS);

  // Initialize the HAL and enable interrupts.
  halInit();
  INTERRUPTS_ON();

  // Initialize the serial ports.
  SERIAL_INIT();

  // Display diagnostic information about the most recent reset.
  PRINT_RESET_INFORMATION();

  // Initialize a task for the application and plugin events and enable idling.
  emAppTask = emberTaskInit(emAppEvents);
  emberTaskEnableIdling(true);

  // Initialize the stack and wait until it finishes.  We do this so that the
  // application doesn't get ahead of the stack.
  emberInit();
  while (!init) {
    halResetWatchdog();
    PROCESS_MANAGEMENT_COMMAND();
  }
  init = false;

  // Initialize the application and plugins.  This function is generated.
  emberAfInit();

  // Run the application loop, which usually never terminates.
  loop();

  return 0;
}

static void loop(void)
{
  while (true) {
    // Reset the watchdog timer to prevent a timeout.
    halResetWatchdog();

    PROCESS_MANAGEMENT_COMMAND();

    if (init) {
      init = false;
      emberAfInit();
    }

    // Let the stack, application, and plugins run periodic tasks.  This
    // function is generated.
    emberAfTick();

    // Run the application and plugin events.
    emberRunTask(emAppTask);

    simulatedTimePassesMs(emberMsToNextEvent(emAppEvents, MAX_INT32U_VALUE));
  }
}

#ifdef EMBER_AF_PRINT_ENABLE

static void printResetInformation(void)
{
  // Information about the most recent reset is printed during startup to aid
  // in debugging.
  emberAfGuaranteedPrintln("Reset info: 0x%x (%p)",
                           halGetResetInfo(),
                           halGetResetString());
#ifdef EMBER_AF_API_DIAGNOSTIC_CORTEXM3
  emberAfGuaranteedPrintln("Extended reset info: 0x%2x (%p)",
                           halGetExtendedResetInfo(),
                           halGetExtendedResetString());
  if (halResetWasCrash()) {
    halPrintCrashSummary(APP_SERIAL);
    halPrintCrashDetails(APP_SERIAL);
    halPrintCrashData(APP_SERIAL);
  }
#endif // EMBER_AF_API_DIAGNOSTIC_CORTEXM3
}

#endif // EMBER_AF_PRINT_ENABLE

void emberInitReturn(EmberStatus status)
{
  // If initialization fails, we have to assert because something is wrong.
  emberAfGuaranteedPrintln("Init: 0x%x", status);
  assert(status == EMBER_SUCCESS);
  init = true;

#ifdef EMBER_AF_API_TFTP_BOOTLOAD_TARGET
  emInitializeTftpServer();
  emInitializeTftpBootloader();
#endif
}

// TODO: This should not be necessary in the application.  See EMIPSTACK-324.
bool ipModemLinkPreparingForPowerDown(void)
{
  return false;
}
