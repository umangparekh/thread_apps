// This file is generated by Ember Desktop.  Please do not edit manually.
//
//

#include PLATFORM_HEADER
#include CONFIGURATION_HEADER
#include EMBER_AF_API_EMBER_TYPES

// OkToSleep function declarations.
bool emberAfPluginIdleSleepOkToSleepCallback(uint32_t durationMs);

bool emberAfOkToSleep(uint32_t durationMs)
{
  return emberAfPluginIdleSleepOkToSleepCallback(durationMs);
}

// WakeUp function declarations.
void emberAfPluginIdleSleepWakeUpCallback(uint32_t durationMs);

void emberAfPluginIdleSleepWakeUp(uint32_t durationMs)
{
  emberAfPluginIdleSleepWakeUpCallback(durationMs);
}

// OkToIdle function declarations.
bool emberAfPluginIdleSleepOkToIdleCallback(uint32_t durationMs);

bool emberAfOkToIdle(uint32_t durationMs)
{
  return emberAfPluginIdleSleepOkToIdleCallback(durationMs);
}

// Active function declarations.
void emberAfPluginIdleSleepActiveCallback(uint32_t durationMs);

void emberAfPluginIdleSleepActive(uint32_t durationMs)
{
  emberAfPluginIdleSleepActiveCallback(durationMs);
}

// Main function declarations.
void emberAfMainCallback(MAIN_FUNCTION_PARAMETERS);

void emberAfMain(MAIN_FUNCTION_PARAMETERS)
{
  emberAfMainCallback(MAIN_FUNCTION_ARGUMENTS);
}

// Init function declarations.
void emberAfInitCallback(void);
void emberCommandReaderInit(void);
void emberAfTftpBootloaderInit(void);

void emberAfInit(void)
{
  emberAfInitCallback();
  emberCommandReaderInit();
  emberAfTftpBootloaderInit();
}

// Tick function declarations.
void emberTick(void);
void emberAfTickCallback(void);
void emberAfPluginCliTickCallback(void);
void emberAfPluginHeartbeatTickCallback(void);
void emberAfPluginIdleSleepTickCallback(void);
void emberSerialBufferTick(void);

void emberAfTick(void)
{
  emberTick();
  emberAfTickCallback();
  emberAfPluginCliTickCallback();
  emberAfPluginHeartbeatTickCallback();
  emberAfPluginIdleSleepTickCallback();
  emberSerialBufferTick();
}

// MarkApplicationBuffers function declarations.
void emberAfMarkApplicationBuffersCallback(void);

void emberMarkApplicationBuffers(void)
{
  emberAfMarkApplicationBuffersCallback();
}