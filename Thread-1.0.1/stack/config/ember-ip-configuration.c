/** @file ember-ip-configuration.c
 * @brief User-configurable stack memory allocation and convenience stubs
 * for little-used callbacks.
 *
 * \b Note: Application developers should \b not modify any portion
 * of this file. Doing so may lead to mysterious bugs. Allocations should be
 * adjusted only with macros in a custom CONFIGURATION_HEADER.
 *
 * <!--Copyright 2009 by Ember Corporation. All rights reserved.         *80*-->
 */
#include PLATFORM_HEADER
#include "stack/core/ember-stack.h"
#include "stack/include/ember-types.h"
#include "stack/include/wakeup.h"
#include "hal/hal.h"
#include "config/config.h"
#include "stack/config/ember-configuration-defaults.h"
#include "stack/ip/zigbee/child-data.h"
#include "app/tmsp/tmsp-enum.h"

// Declare heap memory contents based on EMBER_HEAP_SIZE
#if defined(CORTEXM3)
  VAR_AT_SEGMENT(NO_STRIPPING uint16_t heapMemory[(EMBER_HEAP_SIZE) / 2], __EMHEAP__);
#elif defined(EMBER_STACK_COBRA)
  // on Cobra, the heap is allocated the entire unused portion of memory by the linker
#else
  uint16_t heapMemory[(EMBER_HEAP_SIZE) / 2];
  const uint32_t heapMemorySize = EMBER_HEAP_SIZE;
#endif

#if (! defined (RTOS) || defined (IP_MODEM_LIBRARY))
uint8_t emberChildTableSize = EMBER_CHILD_TABLE_SIZE;
static ChildStatusFlags childStatus[EMBER_CHILD_TABLE_SIZE];
ChildStatusFlags *emChildStatus = childStatus;
static EmberNodeId childIdTable[EMBER_CHILD_TABLE_SIZE + 1];
EmberNodeId *emChildIdTable = childIdTable;
static uint8_t childLongIdTable[(EMBER_CHILD_TABLE_SIZE + 1) * 8];
uint8_t *emChildLongIdTable = childLongIdTable;
static uint32_t childTimers[EMBER_CHILD_TABLE_SIZE];
uint32_t *emChildTimers = childTimers;
static uint32_t childTimeouts[EMBER_CHILD_TABLE_SIZE];
uint32_t *emChildTimeouts = childTimeouts;
static uint32_t childFrameCounters[EMBER_CHILD_TABLE_SIZE];
uint32_t *emChildFrameCounters = childFrameCounters;
static uint16_t childTransactionTimers[EMBER_CHILD_TABLE_SIZE];
uint16_t *emChildLastTransactionTimesSec = childTransactionTimers;
static uint16_t childSecondSinceAttaching[EMBER_CHILD_TABLE_SIZE];
uint16_t *emChildAttachDurationSec = childSecondSinceAttaching;
uint8_t emMaxEndDeviceChildren = EMBER_CHILD_TABLE_SIZE;
uint16_t emberMacIndirectTimeout = EMBER_INDIRECT_TRANSMISSION_TIMEOUT;
uint32_t emberSleepyChildPollTimeout = EMBER_SLEEPY_CHILD_POLL_TIMEOUT;
uint8_t emberEndDevicePollTimeout = EMBER_END_DEVICE_POLL_TIMEOUT;
uint8_t emZigbeeNetworkSecurityLevel = EMBER_SECURITY_LEVEL;
// Needed by some phy files
uint8_t emberReservedMobileChildEntries = 0;

PGM uint8_t emTaskCount = EMBER_TASK_COUNT;
EmberTaskControl emTasks[EMBER_TASK_COUNT];

const EmberVersion emberVersion = {
  EMBER_MAJOR_VERSION,
  EMBER_MINOR_VERSION,
  EMBER_PATCH_VERSION,
  EMBER_VERSION_TYPE,
  EMBER_BUILD_NUMBER,
  EMBER_CHANGE_NUMBER,
};

#ifndef EMBER_APPLICATION_HAS_GET_VERSIONS
void emApiGetVersions(void)
{
  static const uint8_t emBuildTimestamp[] = __DATE__" "__TIME__;
  emApiGetVersionsReturn(EMBER_VERSION_NAME,
                         EMBER_MANAGEMENT_VERSION,
                         EMBER_FULL_VERSION,
                         EMBER_BUILD_NUMBER,
                         EMBER_VERSION_TYPE,
                         emBuildTimestamp);
}
#endif

#ifndef EMBER_APPLICATION_HAS_APP_NAME
const char * const emAppName = "";
#endif
#endif // ! defined (RTOS) || defined (IP_MODEM_LIBRARY)

#ifndef EMBER_APPLICATION_HAS_SIM_NOTIFY_SERIAL_RX
bool simNotifySerialRx(const uint8_t *data, uint16_t length)
{
  return true;
}
#endif

#ifndef EMBER_APPLICATION_HAS_EXTERNAL_MEMORY_ALLOCATION
void *emberAllocateMemoryForPacketHandler(uint32_t size, void **objectRef)
{
  return NULL;
}
void emberFreeMemoryForPacketHandler(void *objectRef) { }
#endif

// Framework applications, which are distinguished by the __THREAD_CONFIG__
// define, have generated stubs for any unimplemented handlers and returns,
// making these stubs unnecessary.
#ifndef __THREAD_CONFIG__

#ifndef EMBER_APPLICATION_HAS_POLL_HANDLER
void emberPollHandler(EmberNodeId childId, bool transmitExpected)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_COUNTER_HANDLER
  void emApiCounterHandler(EmberCounterType type, uint16_t increment) { }
  #if (!defined(UNIX_HOST) && !defined(UNIX_HOST_SIM))
    void emApiGetCounter(EmberCounterType type) { }
    void emApiClearCounters(void) { }
  #endif
#endif

#ifndef EMBER_APPLICATION_HAS_WAKEUP_HANDLER
void emApiWakeupHandler(EmberWakeupReason reason,
                        EmberWakeupState state,
                        uint16_t remainingMs,
                        uint8_t dataByte,
                        uint16_t otaSequence)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_NETWORK_STATUS_CHANGED_HANDLER
void emApiNetworkStatusChangedHandler(EmberNetworkStatus oldStatus,
                                      EmberNetworkStatus newStatus)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_TCP_ACCEPT_HANDLER
void emberTcpAcceptHandler(uint16_t port, uint8_t fd)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_TCP_STATUS_HANDLER
void emberTcpStatusHandler(uint8_t fd, uint8_t status)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_TCP_READ_HANDLER
void emberTcpReadHandler(uint8_t fd, uint8_t *incoming, uint16_t length)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_UDP_HANDLER
void emApiUdpHandler(const uint8_t *destination,
                     const uint8_t *source,
                     uint16_t localPort,
                     uint16_t remotePort,
                     const uint8_t *payload,
                     uint16_t payloadLength)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_UDP_MULTICAST_HANDLER
void emApiUdpMulticastHandler(const uint8_t *destinationIpv6Address,
                              const uint8_t *sourceIpv6Address,
                              uint16_t localPort,
                              uint16_t remotePort,
                              uint8_t flags,
                              const uint8_t *packet,
                              uint16_t length)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_INCOMING_ICMP_HANDLER
void emApiIncomingIcmpHandler(Ipv6Header *ipHeader)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_MAC_PASSTHROUGH_FILTER_HANDLER
bool emApiMacPassthroughFilterHandler(uint8_t *macHeader)
{
  return false;
}
#endif

#ifndef EMBER_APPLICATION_HAS_MAC_PASSTHROUGH_MESSAGE_HANDLER
void emApiMacPassthroughMessageHandler(PacketHeader header)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_BUTTON_TICK
void emButtonTick(void)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_GET_VERSIONS_RETURN
void emApiGetVersionsReturn(const uint8_t *versionName,
                            uint16_t managementVersionNumber,
                            uint16_t stackVersionNumber,
                            uint16_t stackBuildNumber,
                            EmberVersionType versionType,
                            const uint8_t *buildTimestamp)
{
  assert(false);
}
#endif

#ifndef EMBER_APPLICATION_HAS_SWITCH_TO_NEXT_NETWORK_KEY_HANDLER
void emApiSwitchToNextNetworkKeyHandler(EmberStatus status)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_ASH_STATUS_HANDLER
#include "hal/micro/generic/ash-v3.h" // for AshState

void emberAshStatusHandler(AshState state)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_NETWORK_STATE_CHANGED_HANDLER
void emNetworkStateChangedHandler(void) {}
#endif

#ifndef EMBER_APPLICATION_HAS_EVENT_DELAY_UPDATED_FROM_ISR_HANDLER
void emApiEventDelayUpdatedFromIsrHandler(Event *event) {}
#endif

#ifndef EMBER_APPLICATION_HAS_START_HOST_JOIN_CLIENT_HANDLER
void emberStartHostJoinClientHandler(const uint8_t *parentAddress) {}
#endif

#endif // __THREAD_CONFIG__
