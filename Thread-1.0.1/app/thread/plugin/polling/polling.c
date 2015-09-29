// Copyright 2015 Silicon Laboratories, Inc.

#include PLATFORM_HEADER
#include CONFIGURATION_HEADER
#include EMBER_AF_API_STACK
#include EMBER_AF_API_HAL
#ifdef EMBER_AF_API_DEBUG_PRINT
  #include EMBER_AF_API_DEBUG_PRINT
#endif
#include "thread-bookkeeping.h"
#include "thread-callbacks.h"

#define SHORT_POLL_INTERVAL_MS                   \
  (EMBER_AF_PLUGIN_POLLING_SHORT_POLL_INTERVAL_S \
   * MILLISECOND_TICKS_PER_SECOND)
#define LONG_POLL_INTERVAL_MS                   \
  (EMBER_AF_PLUGIN_POLLING_LONG_POLL_INTERVAL_S \
   * MILLISECOND_TICKS_PER_SECOND)

EmberEventControl emberAfPluginPollingEventControl;

static uint32_t pollIntervalMs = 0;

static bool okToLongPoll(void)
{
#ifndef EMBER_HOST
  // If the stack expects messages that must be polled for, we short poll.
  EmberIdleRadioState radioStateResult;
  emberStackIdleTimeMs(&radioStateResult);
  if (radioStateResult == IDLE_WITH_POLLING) {
    return false;
  }
#endif

  // Otherwise, the decision to short poll or long poll is left to the
  // application and plugins.  This function is generated.
  return emberAfOkToLongPoll();
}

void emberAfPluginPollingInitCallback(void)
{
  pollIntervalMs = 0;
  emberEventControlSetInactive(emberAfPluginPollingEventControl);
}

void emberAfPluginPollingTickCallback(void)
{
  // Only sleepy end devices can poll, and only if they are actually joined.
  // If we aren't sleepy or aren't joined, we stop polling.  If we are sleepy
  // and are joined, we schedule a poll whenever the polling interval changes,
  // including when we go from not polling to polling.  On the host, we tell
  // the NCP which interval we want and then it handles the actual polling.  On
  // the SoC, we schedule an event to poll ourselves.
  EmberNetworkParameters parameters = {{0}};
  uint32_t newPollIntervalMs = 0;
  emberGetNetworkParameters(&parameters);
  if (parameters.nodeType == EMBER_SLEEPY_END_DEVICE
      && emberNetworkStatus() == EMBER_JOINED_NETWORK) {
    newPollIntervalMs = (okToLongPoll()
                         ? LONG_POLL_INTERVAL_MS
                         : SHORT_POLL_INTERVAL_MS);
  }

  if (pollIntervalMs != newPollIntervalMs) {
    pollIntervalMs = newPollIntervalMs;
#ifdef EMBER_HOST
    emberStackPollForData(pollIntervalMs);
#else
    if (pollIntervalMs == 0) {
      emberEventControlSetInactive(emberAfPluginPollingEventControl);
    } else {
      emberEventControlSetActive(emberAfPluginPollingEventControl);
    }
#endif
  }
}

void emberAfPluginPollingEventHandler(void)
{
  // On the SoC, when the event fires, we poll.  The event is left inactive
  // here, and will be rescheduled when the status of the poll comes back.
  // This event should never fire on the host, but it is harmless if it does.
  emberEventControlSetInactive(emberAfPluginPollingEventControl);
  emberPollForData();
}

void emberPollForDataReturn(EmberStatus status, bool dataPending)
{
  // After a success poll on the SoC, we reschedule the event based on the data
  // pending  If this poll resulted in a message for us, we assume there might
  // be more waiting for us, so we poll right away.  Otherwise, we go back to
  // our normal polling interval.  If this poll failed, we reset our interval,
  // which will cause it to be recalculated.  It is possible that polling
  // failed due to a change in network state, and the recalculation will catch
  // that.  On the host, with the NCP handling polling, this function is only
  // called for failures.
  if (status == EMBER_SUCCESS) {
#ifndef EMBER_HOST
    if (dataPending) {
      emberEventControlSetActive(emberAfPluginPollingEventControl);
    } else {
      emberEventControlSetDelayMS(emberAfPluginPollingEventControl,
                                  pollIntervalMs);
    }
#endif
  } else {
    pollIntervalMs = 0;
    emberAfDebugPrintln("ERR: Poll failed: 0x%x", status);
  }
}

void emberStackPollForDataReturn(EmberStatus status)
{
  // If configuring the NCP for polling failed, we reset our interval, which
  // will cause it to be recalculated.  It is possible that polling failed due
  // to a change in network state, and the recalculation will catch that.
  if (status != EMBER_SUCCESS) {
    pollIntervalMs = 0;
    emberAfDebugPrintln("ERR: Stack poll failed: 0x%x", status);
  }
}
