/** @file ember-configuration.c
 * @brief User-configurable stack memory allocation and convenience stubs 
 * for little-used callbacks. 
 * 
 *
 * \b Note: Application developers should \b not modify any portion
 * of this file. Doing so may lead to mysterious bugs. Allocations should be 
 * adjusted only with macros in a custom CONFIGURATION_HEADER. 
 *
 * <!--Copyright 2008 by Ember Corporation. All rights reserved.         *80*-->
 */
#include PLATFORM_HEADER 
#include "stack/include/ember.h"
#include "stack/include/error.h"
#include "stack/include/ember-static-struct.h" // Required typedefs
//[[WBB350FIXME -- We need a unified, platform independent reboot mechanism ]]
#ifdef XAP2B
  #include "hal/micro/xap2b/em250/pcb.h"
#elif defined (CORTEXM3)
  #include "hal/micro/micro.h"
#endif

// *****************************************
// Memory Allocations & declarations
// *****************************************

extern uint8_t emAvailableMemory[];
#ifdef XAP2B
  #define align(value) ((value) + ((value) & 1))
#else
  #define align(value) (value)
#endif

//------------------------------------------------------------------------------
// API Version

const uint8_t emApiVersion 
  = (EMBER_API_MAJOR_VERSION << 4) + EMBER_API_MINOR_VERSION;

//------------------------------------------------------------------------------
// Packet Buffers

uint8_t emPacketBufferCount = EMBER_PACKET_BUFFER_COUNT;
uint8_t emPacketBufferFreeCount = EMBER_PACKET_BUFFER_COUNT;

// The actual memory for buffers.
uint8_t *emPacketBufferData = &emAvailableMemory[0];
#define END_emPacketBufferData          \
  (align(EMBER_PACKET_BUFFER_COUNT * 32))

uint8_t *emMessageBufferLengths = &emAvailableMemory[END_emPacketBufferData];
#define END_emMessageBufferLengths      \
  (END_emPacketBufferData + align(EMBER_PACKET_BUFFER_COUNT))

uint8_t *emMessageBufferReferenceCounts = &emAvailableMemory[END_emMessageBufferLengths];
#define END_emMessageBufferReferenceCounts      \
  (END_emMessageBufferLengths + align(EMBER_PACKET_BUFFER_COUNT))

uint8_t *emPacketBufferLinks = &emAvailableMemory[END_emMessageBufferReferenceCounts];
#define END_emPacketBufferLinks      \
  (END_emMessageBufferReferenceCounts + align(EMBER_PACKET_BUFFER_COUNT))

uint8_t *emPacketBufferQueueLinks = &emAvailableMemory[END_emPacketBufferLinks];
#define END_emPacketBufferQueueLinks      \
  (END_emPacketBufferLinks + align(EMBER_PACKET_BUFFER_COUNT))

//------------------------------------------------------------------------------
// NWK Layer

#ifdef EMBER_DISABLE_RELAY
uint8_t emAllowRelay = false;
#else
uint8_t emAllowRelay = true;
#endif

// emChildIdTable must be sized one element larger than EMBER_CHILD_TABLE_SIZE
// to allow emberChildIndex() to perform an optimized search when setting the
// frame pending bit.  emberChildTableSize and EMBER_CHILD_TABLE_SIZE still
// correspond to the number of children, not the number of child table elements.
EmberNodeId *emChildIdTable = (EmberNodeId *) &emAvailableMemory[END_emPacketBufferQueueLinks];
uint8_t emberChildTableSize = EMBER_CHILD_TABLE_SIZE;
#define END_emChildIdTable              \
 (END_emPacketBufferQueueLinks + align( (EMBER_CHILD_TABLE_SIZE+1) * sizeof(EmberNodeId)))

uint16_t *emChildStatus = (uint16_t *) &emAvailableMemory[END_emChildIdTable];
#define END_emChildStatus               \
 (END_emChildIdTable + align(EMBER_CHILD_TABLE_SIZE * sizeof(uint16_t)))

uint32_t *emChildTimers = (uint32_t *) &emAvailableMemory[END_emChildStatus];
#define END_emChildTimers               \
 (END_emChildStatus + align(EMBER_CHILD_TABLE_SIZE * sizeof(uint32_t)))

uint8_t *emUnicastAlarmData = (uint8_t *) &emAvailableMemory[END_emChildTimers];
uint8_t emUnicastAlarmDataSize = EMBER_UNICAST_ALARM_DATA_SIZE;
#define END_emUnicastAlarmData          \
 (END_emChildTimers+ align(EMBER_CHILD_TABLE_SIZE * EMBER_UNICAST_ALARM_DATA_SIZE))

uint8_t *emBroadcastAlarmData = (uint8_t *) &emAvailableMemory[END_emUnicastAlarmData];
uint8_t emBroadcastAlarmDataSize = EMBER_BROADCAST_ALARM_DATA_SIZE;
#define END_emBroadcastAlarmData        \
 (END_emUnicastAlarmData + align(EMBER_BROADCAST_ALARM_DATA_SIZE))

EmRouteTableEntry *emRouteData = (EmRouteTableEntry *) &emAvailableMemory[END_emBroadcastAlarmData];
uint8_t emRouteTableSize = EMBER_ROUTE_TABLE_SIZE;
#define END_emRouteData        \
 (END_emBroadcastAlarmData + align(EMBER_ROUTE_TABLE_SIZE * sizeof(EmRouteTableEntry)))

uint8_t *emRouteRecordTable = (uint8_t *) &emAvailableMemory[END_emRouteData];
#define END_emRouteRecordTable          \
  (END_emRouteData + align(((EMBER_CHILD_TABLE_SIZE + 7) >> 3)) * EMBER_ROUTE_TABLE_SIZE)

EmDiscoveryTableEntry *emDiscoveryTable = (EmDiscoveryTableEntry *) &emAvailableMemory[END_emRouteRecordTable];
uint8_t emDiscoveryTableSize = EMBER_DISCOVERY_TABLE_SIZE;
#define END_emDiscoveryTable        \
 (END_emRouteRecordTable + align(EMBER_DISCOVERY_TABLE_SIZE * sizeof(EmDiscoveryTableEntry)))

EmberMulticastTableEntry *emberMulticastTable = (EmberMulticastTableEntry *) &emAvailableMemory[END_emDiscoveryTable];
uint8_t emberMulticastTableSize = EMBER_MULTICAST_TABLE_SIZE;
#define END_emberMulticastTable        \
 (END_emDiscoveryTable + align(EMBER_MULTICAST_TABLE_SIZE * sizeof(EmberMulticastTableEntry)))

#ifdef EMBER_PARAMETERIZED_BROADCAST_TABLE
EmBroadcastTableEntry *emBroadcastTable = (EmBroadcastTableEntry *) &emAvailableMemory[END_emberMulticastTable];
#define END_emberBroadcastTable        \
 (END_emberMulticastTable + align(EMBER_BROADCAST_TABLE_SIZE * sizeof(EmBroadcastTableEntry)))
uint8_t emBroadcastTableSize = EMBER_BROADCAST_TABLE_SIZE;
#else
#define END_emberBroadcastTable END_emberMulticastTable
#endif

//------------------------------------------------------------------------------
// Network descriptor (multi-network support)

#if !defined(EMBER_MULTI_NETWORK_STRIPPED)
EmberNetworkInfo* emNetworkDescriptor = (EmberNetworkInfo *) &emAvailableMemory[END_emberBroadcastTable];
uint8_t emSupportedNetworks = EMBER_SUPPORTED_NETWORKS;
#define END_emNetworkDescriptor         \
    (END_emberBroadcastTable + align( (EMBER_SUPPORTED_NETWORKS) * sizeof(EmberNetworkInfo)))
#else
uint8_t emSupportedNetworks = 1;
#define END_emNetworkDescriptor END_emberBroadcastTable
#endif // !defined(EMBER_MULTI_NETWORK_STRIPPED)

//------------------------------------------------------------------------------
// Neighbor Table

EmNeighborTableEntry *emNeighborData = (EmNeighborTableEntry *) &emAvailableMemory[END_emNetworkDescriptor];
uint8_t emRouterNeighborTableSize = EMBER_NEIGHBOR_TABLE_SIZE;
#define END_emNeighborData        \
 (END_emNetworkDescriptor + align((EMBER_NEIGHBOR_TABLE_SIZE + EMBER_SUPPORTED_NETWORKS) * sizeof(EmNeighborTableEntry)))

uint32_t *emFrameCountersTable = (uint32_t *) &emAvailableMemory[END_emNeighborData];
#define END_emFrameCountersTable        \
 (END_emNeighborData + align((EMBER_NEIGHBOR_TABLE_SIZE + EMBER_CHILD_TABLE_SIZE + EMBER_SUPPORTED_NETWORKS) * sizeof(uint32_t)))

//------------------------------------------------------------------------------
// NWK Retry Queue

EmRetryQueueEntry *emRetryQueue = (EmRetryQueueEntry *) &emAvailableMemory[END_emFrameCountersTable];
uint8_t emRetryQueueSize = EMBER_RETRY_QUEUE_SIZE;
#define END_emRetryQueue  \
 (END_emFrameCountersTable + align(EMBER_RETRY_QUEUE_SIZE * sizeof(EmRetryQueueEntry)))


//------------------------------------------------------------------------------
// RF4CE stack tables

EmberRf4cePairingTableEntry *emRf4cePairingTable = (EmberRf4cePairingTableEntry *) &emAvailableMemory[END_emRetryQueue];
uint8_t emRf4cePairingTableSize = EMBER_RF4CE_PAIRING_TABLE_SIZE;
#define END_emRf4cePairingTable         \
  (END_emRetryQueue + align(EMBER_RF4CE_PAIRING_TABLE_SIZE * sizeof(EmberRf4cePairingTableEntry)))

EmRf4ceOutgoingPacketInfoEntry *emRf4cePendingOutgoingPacketTable = (EmRf4ceOutgoingPacketInfoEntry *) &emAvailableMemory[END_emRf4cePairingTable];
uint8_t emRf4cePendingOutgoingPacketTableSize = EMBER_RF4CE_PENDING_OUTGOING_PACKET_TABLE_SIZE;
#define END_emRf4cePendingOutgoingPacketTable     \
  (END_emRf4cePairingTable + align(EMBER_RF4CE_PENDING_OUTGOING_PACKET_TABLE_SIZE * sizeof(EmRf4ceOutgoingPacketInfoEntry)))

//------------------------------------------------------------------------------
// Binding Table

uint16_t *emBindingRemoteNode = (uint16_t *) &emAvailableMemory[END_emRf4cePendingOutgoingPacketTable];
uint8_t emberBindingTableSize = EMBER_BINDING_TABLE_SIZE;
#define END_emBindingRemoteNode        \
 (END_emRf4cePendingOutgoingPacketTable + align(EMBER_BINDING_TABLE_SIZE * sizeof(uint16_t)))

uint8_t *emBindingFlags = &emAvailableMemory[END_emBindingRemoteNode];
#define END_emBindingFlags        \
 (END_emBindingRemoteNode + align(EMBER_BINDING_TABLE_SIZE))

//------------------------------------------------------------------------------
//End Device Timeouts

uint8_t *emEndDeviceTimeout = &emAvailableMemory[END_emBindingFlags];
#define END_emEndDeviceTimeout \
 (END_emBindingFlags + align(EMBER_CHILD_TABLE_SIZE * sizeof(uint8_t)))

//------------------------------------------------------------------------------

// APS Layer

uint8_t emAddressTableSize = EMBER_ADDRESS_TABLE_SIZE;
EmAddressTableEntry *emAddressTable = (EmAddressTableEntry *) &emAvailableMemory[END_emEndDeviceTimeout];
#define END_emAddressTable        \
 (END_emEndDeviceTimeout + align(EMBER_ADDRESS_TABLE_SIZE * sizeof(EmAddressTableEntry)))

uint8_t emMaxApsUnicastMessages = EMBER_APS_UNICAST_MESSAGE_COUNT;
EmApsUnicastMessageData *emApsUnicastMessageData = (EmApsUnicastMessageData *) &emAvailableMemory[END_emAddressTable];
#define END_emApsUnicastMessageData        \
 (END_emAddressTable + align(EMBER_APS_UNICAST_MESSAGE_COUNT * sizeof(EmApsUnicastMessageData)))

uint16_t emberApsAckTimeoutMs = 
 ((EMBER_APSC_MAX_ACK_WAIT_HOPS_MULTIPLIER_MS
   * EMBER_MAX_HOPS)
  + EMBER_APSC_MAX_ACK_WAIT_TERMINAL_SECURITY_MS);

uint8_t emFragmentDelayMs = EMBER_FRAGMENT_DELAY_MS;
uint8_t emberFragmentWindowSize = EMBER_FRAGMENT_WINDOW_SIZE;

uint8_t emberKeyTableSize = EMBER_KEY_TABLE_SIZE;
uint32_t* emIncomingApsFrameCounters = (uint32_t*)&emAvailableMemory[END_emApsUnicastMessageData];
#define END_emIncomingApsFrameCounters \
  (END_emApsUnicastMessageData + align(EMBER_KEY_TABLE_SIZE * sizeof(uint32_t)))

EmberLinkKeyRequestPolicy emberTrustCenterLinkKeyRequestPolicy = 
  EMBER_DENY_KEY_REQUESTS;
EmberLinkKeyRequestPolicy emberAppLinkKeyRequestPolicy = 
  EMBER_ALLOW_KEY_REQUESTS;

uint8_t emCertificateTableSize = EMBER_CERTIFICATE_TABLE_SIZE;

uint8_t emAppZdoConfigurationFlags =
  0
  
// Define this in order to receive supported ZDO request messages via
// the incomingMessageHandler callback.  A supported ZDO request is one that
// is handled by the EmberZNet stack.  The stack will continue to handle the
// request and send the appropriate ZDO response even if this configuration
// option is enabled.
#ifdef EMBER_APPLICATION_RECEIVES_SUPPORTED_ZDO_REQUESTS
  | EMBER_APP_RECEIVES_SUPPORTED_ZDO_REQUESTS
#endif

// Define this in order to receive unsupported ZDO request messages via
// the incomingMessageHandler callback.  An unsupported ZDO request is one that
// is not handled by the EmberZNet stack, other than to send a 'not supported'
// ZDO response.  If this configuration option is enabled, the stack will no
// longer send any ZDO response, and it is the application's responsibility
// to do so.  To see if a response is required, the application must check
// the APS options bitfield within the emberIncomingMessageHandler callback to see
// if the EMBER_APS_OPTION_ZDO_RESPONSE_REQUIRED flag is set.
#ifdef EMBER_APPLICATION_HANDLES_UNSUPPORTED_ZDO_REQUESTS
  | EMBER_APP_HANDLES_UNSUPPORTED_ZDO_REQUESTS
#endif

// Define this in order to receive the following ZDO request 
// messages via the emberIncomingMessageHandler callback: SIMPLE_DESCRIPTOR_REQUEST,
// MATCH_DESCRIPTORS_REQUEST, and ACTIVE_ENDPOINTS_REQUEST.  If this 
// configuration option is enabled, the stack will no longer send any ZDO
// response, and it is the application's responsibility to do so.
// To see if a response is required, the application must check
// the APS options bitfield within the emberIncomingMessageHandler callback to see
// if the EMBER_APS_OPTION_ZDO_RESPONSE_REQUIRED flag is set.
#ifdef EMBER_APPLICATION_HANDLES_ENDPOINT_ZDO_REQUESTS
  | EMBER_APP_HANDLES_ZDO_ENDPOINT_REQUESTS
#endif

// Define this in order to receive the following ZDO request
// messages via the emberIncomingMessageHandler callback: BINDING_TABLE_REQUEST, 
// BIND_REQUEST, and UNBIND_REQUEST.  If this
// configuration option is enabled, the stack will no longer send any ZDO
// response, and it is the application's responsibility to do so.
// To see if a response is required, the application must check
// the APS options bitfield within the emberIncomingMessageHandler callback
// to see if the EMBER_APS_OPTION_ZDO_RESPONSE_REQUIRED flag is set.
#ifdef EMBER_APPLICATION_HANDLES_BINDING_ZDO_REQUESTS
  | EMBER_APP_HANDLES_ZDO_BINDING_REQUESTS
#endif
;

//------------------------------------------------------------------------------
// Memory Allocation
#ifndef RESERVED_AVAILABLE_MEMORY
  #define RESERVED_AVAILABLE_MEMORY 0
#endif
#define END_stackMemory  END_emIncomingApsFrameCounters + RESERVED_AVAILABLE_MEMORY

// On the XAP2B platform, emAvailableMemory is allocated automatically to fill
// the available space. On other platforms, we must allocate it here.
#if defined(XAP2B)
  extern uint8_t emAvailableMemoryTop[];
  const uint16_t emMinAvailableMemorySize = END_stackMemory;
#elif defined (CORTEXM3)
  VAR_AT_SEGMENT(uint8_t emAvailableMemory[END_stackMemory], __EMHEAP__);
#elif defined(EMBER_TEST)
  uint8_t emAvailableMemory[END_stackMemory];
  const uint16_t emAvailableMemorySize = END_stackMemory;
#else
  #error "Unknown platform."
#endif

void emCheckAvailableMemory(void)
{
#ifdef XAP2B
  uint16_t emAvailableMemorySize = emAvailableMemoryTop - emAvailableMemory;
  assert(END_stackMemory <= emAvailableMemorySize);
#endif
}

// *****************************************
// Non-dynamically configurable structures
// *****************************************
PGM uint8_t emTaskCount = EMBER_TASK_COUNT;
EmberTaskControl emTasks[EMBER_TASK_COUNT];


// *****************************************
// Stack Profile Parameters
// *****************************************

PGM uint8_t emberStackProfileId[8] = { 0, };

uint8_t emDefaultStackProfile = EMBER_STACK_PROFILE;
uint8_t emDefaultSecurityLevel = EMBER_SECURITY_LEVEL;
uint8_t emMaxEndDeviceChildren = EMBER_MAX_END_DEVICE_CHILDREN;
uint8_t emMaxHops = EMBER_MAX_HOPS;
uint16_t emberMacIndirectTimeout = EMBER_INDIRECT_TRANSMISSION_TIMEOUT;
uint8_t emberReservedMobileChildEntries = EMBER_RESERVED_MOBILE_CHILD_ENTRIES;
uint8_t emberMobileNodePollTimeout = EMBER_MOBILE_NODE_POLL_TIMEOUT;
uint8_t emberEndDevicePollTimeout = EMBER_END_DEVICE_POLL_TIMEOUT;
uint8_t emberEndDevicePollTimeoutShift = EMBER_END_DEVICE_POLL_TIMEOUT_SHIFT;
uint8_t emEndDeviceBindTimeout = EMBER_END_DEVICE_BIND_TIMEOUT;
uint8_t emRequestKeyTimeout = EMBER_REQUEST_KEY_TIMEOUT;
uint8_t emPanIdConflictReportThreshold = EMBER_PAN_ID_CONFLICT_REPORT_THRESHOLD;

#ifndef EMBER_NO_STACK
uint8_t emEndDeviceConfiguration = EMBER_END_DEVICE_CONFIG_PERSIST_DATA_ON_PARENT;
#endif

// Normally multicasts do NOT go to the sleepy address (0xFFFF), they go to
// RxOnWhenIdle=true (0xFFFD).  This can be changed, but doing so is not
// ZigBee Pro Compliant and is possibly NOT interoperable.
bool emSendMulticastsToSleepyAddress = EMBER_SEND_MULTICASTS_TO_SLEEPY_ADDRESS;


// *****************************************
// ZigBee Light Link
// *****************************************

#ifndef XAP2B
  EmberZllPolicy emZllPolicy = EMBER_ZLL_POLICY_DISABLED;

  // The number of groups required by the ZLL application.
  uint8_t emZllGroupAddressesNeeded = EMBER_ZLL_GROUP_ADDRESSES;
  int8_t emZllRssiThreshold = EMBER_ZLL_RSSI_THRESHOLD;
#endif

// *****************************************
// Convenience Stubs
// *****************************************
  
// This macro is #define'd at the top of NCP framework applications.
// Since appbuilder generates stubs to callbacks, these convinience stubs
// are not needed for framework apps.
#ifndef __NCP_CONFIG__

#ifndef EMBER_APPLICATION_HAS_TRUST_CENTER_JOIN_HANDLER
EmberJoinDecision emberDefaultTrustCenterDecision = EMBER_USE_PRECONFIGURED_KEY;

EmberJoinDecision emberTrustCenterJoinHandler(EmberNodeId newNodeId,
                                              EmberEUI64 newNodeEui64,
                                              EmberDeviceUpdate status,
                                              EmberNodeId parentOfNewNode)
{
  if (status == EMBER_STANDARD_SECURITY_SECURED_REJOIN
      || status == EMBER_DEVICE_LEFT
      || status == EMBER_HIGH_SECURITY_SECURED_REJOIN)
    return EMBER_NO_ACTION;

  return emberDefaultTrustCenterDecision;
}
#endif

#ifndef EMBER_APPLICATION_HAS_SWITCH_KEY_HANDLER
void emberSwitchNetworkKeyHandler(uint8_t sequenceNumber)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_ZIGBEE_KEY_ESTABLISHMENT_HANDLER
void emberZigbeeKeyEstablishmentHandler(EmberEUI64 partner, EmberKeyStatus status)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_CHILD_JOIN_HANDLER
void emberChildJoinHandler(uint8_t index, bool joining)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_POLL_COMPLETE_HANDLER
void emberPollCompleteHandler(EmberStatus status)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_BOOTLOAD_HANDLERS
void emberIncomingBootloadMessageHandler(EmberEUI64 longId,
                                         EmberMessageBuffer message)
{
}
void emberBootloadTransmitCompleteHandler(EmberMessageBuffer message,
                                          EmberStatus status)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_MAC_FILTER_MATCH_MESSAGE_HANDLER
void emberMacFilterMatchMessageHandler(const EmberMacFilterMatchStruct* macFilterMatchStruct)
{
  emberMacPassthroughMessageHandler(macFilterMatchStruct->legacyPassthroughType,
                                    macFilterMatchStruct->message);
}
#endif

#ifndef EMBER_APPLICATION_HAS_MAC_PASSTHROUGH_HANDLER
void emberMacPassthroughMessageHandler(EmberMacPassthroughType messageType,
                                       EmberMessageBuffer message)
{
}
#endif
#ifndef EMBER_APPLICATION_HAS_RAW_HANDLER
void emberRawTransmitCompleteHandler(EmberMessageBuffer message,
                                     EmberStatus status)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_INCOMING_MFG_TEST_MESSAGE_HANDLER
void emberIncomingMfgTestMessageHandler(uint8_t messageType, 
                                        uint8_t dataLength, 
                                        uint8_t *data) {}
#endif

#ifndef EMBER_APPLICATION_HAS_ENERGY_SCAN_RESULT_HANDLER
void emberEnergyScanResultHandler(uint8_t channel, int8_t maxRssiValue) {}
#endif

#ifndef EMBER_APPLICATION_HAS_DEBUG_HANDLER
void emberDebugHandler(EmberMessageBuffer message) {}
#endif

#ifndef EMBER_APPLICATION_HAS_POLL_HANDLER
void emberPollHandler(EmberNodeId childId, bool transmitExpected)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_REMOTE_BINDING_HANDLER
EmberStatus emberRemoteSetBindingHandler(EmberBindingTableEntry *entry)
{
  // Don't let anyone mess with our bindings.
  return EMBER_INVALID_BINDING_INDEX;
}
EmberStatus emberRemoteDeleteBindingHandler(uint8_t index)
{
  // Don't let anyone mess with our bindings.
  return EMBER_INVALID_BINDING_INDEX;
}
#endif

#ifndef EMBER_APPLICATION_HAS_BUTTON_HANDLER
void halButtonIsr(uint8_t button, uint8_t state)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_SOURCE_ROUTING
void emberIncomingRouteRecordHandler(EmberNodeId source,
                                     EmberEUI64 sourceEui,
                                     uint8_t relayCount,
                                     EmberMessageBuffer header,
                                     uint8_t relayListIndex)
{
}
uint8_t emberAppendSourceRouteHandler(EmberNodeId destination,
                                   EmberMessageBuffer header)
{
  return 0;
}
#endif

#ifndef EMBER_APPLICATION_HAS_INCOMING_MANY_TO_ONE_ROUTE_REQUEST_HANDLER
void emberIncomingManyToOneRouteRequestHandler(EmberNodeId source,
                                               EmberEUI64 longId,
                                               uint8_t cost)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_INCOMING_ROUTE_ERROR_HANDLER
void emberIncomingRouteErrorHandler(EmberStatus status, 
                                    EmberNodeId target)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_GET_ENDPOINT
uint8_t emberGetEndpoint(uint8_t index)
{
  return emberEndpoints[index].endpoint;
}

bool emberGetEndpointDescription(uint8_t endpoint,
                                    EmberEndpointDescription *result)
{ 
  uint8_t i;
  EmberEndpoint *endpoints = emberEndpoints;
  for (i = 0; i < emberEndpointCount; i++, endpoints++) {
    if (endpoints->endpoint == endpoint) {
      EmberEndpointDescription PGM * d = endpoints->description;
      result->profileId                   = d->profileId;
      result->deviceId                    = d->deviceId;
      result->deviceVersion               = d->deviceVersion;
      result->inputClusterCount           = d->inputClusterCount;
      result->outputClusterCount          = d->outputClusterCount;
      return true;
    }
  }
  return false;
}

uint16_t emberGetEndpointCluster(uint8_t endpoint,
                               EmberClusterListId listId,
                               uint8_t listIndex)
{
  uint8_t i;
  EmberEndpoint *endpoints = emberEndpoints;
  for (i = 0; i < emberEndpointCount; i++, endpoints++) {
    if (endpoints->endpoint == endpoint) {
      switch (listId) {
      case EMBER_INPUT_CLUSTER_LIST:
        return endpoints->inputClusterList[listIndex];
      case EMBER_OUTPUT_CLUSTER_LIST:
        return endpoints->outputClusterList[listIndex];
      default: {
      }
      }
    }
  }
  return 0;
}

#endif // defined EMBER_APPLICATION_HAS_GET_ENDPOINT


// Inform the application that an orphan notification has been received.
// This is generally not useful for applications. It could be useful in
// testing and is included for this purpose.
#ifndef EMBER_APPLICATION_HAS_ORPHAN_NOTIFICATION_HANDLER
void emberOrphanNotificationHandler(EmberEUI64 longId)
{
  return;
}
#endif

#ifndef EMBER_APPLICATION_HAS_COUNTER_HANDLER
void emberCounterHandler(EmberCounterType type, uint8_t data)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_COUNTER_ROLLOVER_HANDLER
void emberCounterRolloverHandler(EmberCounterType type)
{

}
#endif

#ifndef EMBER_APPLICATION_HAS_INCOMING_COMMAND_HANDLER
void emberIncomingCommandHandler(EmberZigbeeCommandType commandType,
                                 EmberMessageBuffer commandBuffer,
                                 uint8_t indexOfCommand,
                                 bool encrypted)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_STACK_TOKEN_CHANGED_HANDLER
void emberStackTokenChangedHandler(uint16_t tokenAddress)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_ID_CONFLICT_HANDLER
void emberIdConflictHandler(EmberNodeId conflictingId)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_MAC_PASSTHROUGH_FILTER_HANDLER
bool emberMacPassthroughFilterHandler(uint8_t *macHeader)
{
  return false;
}
#endif

#ifndef EMBER_APPLICATION_HAS_ZLL_ADDRESS_ASSIGNMENT_HANDLER
void emberZllAddressAssignmentHandler(const EmberZllAddressAssignment* addressInfo){}
#endif

#ifndef EMBER_APPLICATION_HAS_ZLL_NETWORK_FOUND_HANDLER
void emberZllNetworkFoundHandler(const EmberZllNetwork* networkInfo,
                                 const EmberZllDeviceInfoRecord* deviceInfo){ }
#endif

#ifndef EMBER_APPLICATION_HAS_ZLL_SCAN_COMPLETE_HANDLER
void emberZllScanCompleteHandler(EmberStatus status){}
#endif

#ifndef EMBER_APPLICATION_HAS_ZLL_TOUCH_LINK_TARGET_HANDLER
void emberZllTouchLinkTargetHandler(const EmberZllNetwork* networkInfo){}
#endif

#ifndef EMBER_APPLICATION_HAS_RF4CE_MESSAGE_SENT_HANDLER
void emberRf4ceMessageSentHandler(EmberStatus status,
                                  uint8_t pairingIndex,
                                  EmberRf4ceTxOption txOptions,
                                  uint8_t profileId,
                                  uint16_t vendorId,
                                  uint8_t messageTag,
                                  uint8_t messageLength,
                                  uint8_t *message){}
#endif

#ifndef EMBER_APPLICATION_HAS_RF4CE_INCOMING_MESSAGE_HANDLER
void emberRf4ceIncomingMessageHandler(uint8_t pairingIndex,
                                      uint8_t profileId,
                                      uint16_t vendorId,
                                      EmberRf4ceTxOption txOptions,
                                      uint8_t messageLength,
                                      uint8_t *message){}
#endif

#ifndef EMBER_APPLICATION_HAS_RF4CE_DISCOVERY_REQUEST_HANDLER
bool emberRf4ceDiscoveryRequestHandler(EmberEUI64 srcIeeeAddr,
                                          uint8_t nodeCapabilities,
                                          EmberRf4ceVendorInfo *vendorInfo,
                                          EmberRf4ceApplicationInfo *appInfo,
                                          uint8_t searchDevType,
                                          uint8_t rxLinkQuality){
  return true;
}
#endif

#ifndef EMBER_APPLICATION_HAS_RF4CE_DISCOVERY_RESPONSE_HANDLER
bool emberRf4ceDiscoveryResponseHandler(bool atCapacity,
                                           uint8_t channel,
                                           EmberPanId panId,
                                           EmberEUI64 srcIeeeAddr,
                                           uint8_t nodeCapabilities,
                                           EmberRf4ceVendorInfo *vendorInfo,
                                           EmberRf4ceApplicationInfo *appInfo,
                                           uint8_t rxLinkQuality,
                                           uint8_t discRequestLqi){
  return true;
}
#endif

#ifndef EMBER_APPLICATION_HAS_RF4CE_AUTO_DISCOVERY_RESPONSE_COMPLETE_HANDLER
void emberRf4ceAutoDiscoveryResponseCompleteHandler(EmberStatus status,
                                                    EmberEUI64 srcIeeeAddr,
                                                    uint8_t nodeCapabilities,
                                                    EmberRf4ceVendorInfo *vendorInfo,
                                                    EmberRf4ceApplicationInfo *appInfo,
                                                    uint8_t searchDevType){}
#endif

#ifndef EMBER_APPLICATION_HAS_RF4CE_PAIR_COMPLETE_HANDLER
void emberRf4cePairCompleteHandler(EmberStatus status,
                                   uint8_t pairingIndex,
                                   EmberRf4ceVendorInfo *vendorInfo,
                                   EmberRf4ceApplicationInfo *appInfo){}
#endif

#ifndef EMBER_APPLICATION_HAS_RF4CE_DISCOVERY_COMPLETE_HANDLER
void emberRf4ceDiscoveryCompleteHandler(EmberStatus status){}
#endif

#ifndef EMBER_APPLICATION_HAS_RF4CE_PAIR_REQUEST_HANDLER
bool emberRf4cePairRequestHandler(EmberStatus status,
                                     uint8_t pairingIndex,
                                     EmberEUI64 srcIeeeAddr,
                                     uint8_t nodeCapabilities,
                                     EmberRf4ceVendorInfo *vendorInfo,
                                     EmberRf4ceApplicationInfo *appInfo,
                                     uint8_t keyExchangeTransferCount){
  return true;
}
#endif

#ifndef EMBER_APPLICATION_HAS_RF4CE_UNPAIR_HANDLER
void emberRf4ceUnpairHandler(uint8_t pairingIndex){}
#endif

#ifndef EMBER_APPLICATION_HAS_RF4CE_UNPAIR_COMPLETE_HANDLER
void emberRf4ceUnpairCompleteHandler(uint8_t pairingIndex){}
#endif

#ifndef EMBER_APPLICATION_HAS_CALCULATE_SMACS_HANDLER
void emberCalculateSmacsHandler(EmberStatus status,
                                EmberSmacData *initiatorSmac,
                                EmberSmacData *responderSmac){}
#endif

#ifndef EMBER_APPLICATION_HAS_GENERATE_CBKE_KEYS_HANDLER
void emberGenerateCbkeKeysHandler(EmberStatus status,
                                  EmberPublicKeyData *ephemeralPublicKey){}
#endif

#ifndef EMBER_APPLICATION_HAS_CALCULATE_SMACS_HANDLER_283K1
void emberCalculateSmacsHandler283k1(EmberStatus status,
                                     EmberSmacData *initiatorSmac,
                                     EmberSmacData *responderSmac){}
#endif

#ifndef EMBER_APPLICATION_HAS_GENERATE_CBKE_KEYS_HANDLER_283K1
void emberGenerateCbkeKeysHandler283k1(EmberStatus status,
                                       EmberPublicKey283k1Data *ephemeralPublicKey){}
#endif

#ifndef EMBER_APPLICATION_HAS_DSA_SIGN_HANDLER
void emberDsaSignHandler(EmberStatus status,
                         EmberMessageBuffer signedMessage){}
#endif

#ifndef EMBER_APPLICATION_HAS_DSA_VERIFY_HANDLER
void emberDsaVerifyHandler(EmberStatus status){}
#endif

#ifndef AVR_ATMEGA
#ifndef EMBER_APPLICATION_HAS_CUSTOM_RADIO_CALIBRATION_CALLBACK
// See stack-info.h for more information.
void emberRadioNeedsCalibratingHandler(void)
{
  // TODO: Failsafe any critical processes or peripherals.
  emberCalibrateCurrentChannel();
}
#endif//EMBER_APPLICATION_HAS_CUSTOM_RADIO_CALIBRATION_CALLBACK
#endif//AVR_ATMEGA

#if defined (XAP2B) || defined (CORTEXM3)
#ifndef EMBER_APPLICATION_HAS_CUSTOM_SIM_EEPROM_CALLBACK
#include "hal/micro/sim-eeprom.h"
// The Simulated EEPROM Callback function.
void halSimEepromCallback(EmberStatus status)
{
  switch (status) {
  case EMBER_SIM_EEPROM_ERASE_PAGE_GREEN:
    //SimEE is asking for one page to be erased.
#ifdef EMBER_STACK_COBRA
    HalUARTRestrain();
#endif
    halSimEepromErasePage();
    break;
  case EMBER_SIM_EEPROM_ERASE_PAGE_RED:
  case EMBER_SIM_EEPROM_FULL:
  { //SimEE says we're out of room!  Erase all pages now or data
    //currently being written will be dropped.
    bool erasedSome = false;
#ifdef EMBER_STACK_COBRA
    HalUARTRestrain();
#endif
    while(halSimEepromErasePage()) { erasedSome = true; }
    if(erasedSome) {
      break;
    }
    //If nothing got erased, then we have a situation where page
    //rotation is stuck because live tokens still exist in the
    //page we want to erase.  In this case we must do a repair to
    //get all live tokens into one virtual page. [BugzId:14392]
    //Fall into...
  }
  case EMBER_ERR_FLASH_WRITE_INHIBITED:
  case EMBER_ERR_FLASH_VERIFY_FAILED:
  { //Something went wrong while writing a token.  There is stale data and the
    //token the app expected to write did not get written.  Also there may
    //now be "stray" data written in the flash that could inhibit future token
    //writes.  To deal with stray/stale data, we must repair the Simulated
    //EEPROM.  Because the expected token write failed and will not be retried,
    //it is best to reset the chip and let normal boot sequences take over.
    //Since halInternalSimEeRepair() could potentially result in another write
    //failure, we use a simple semaphore to prevent recursion.
    static bool repairActive = false;
    if(!repairActive) {
      repairActive = true;
      halInternalSimEeRepair(false);
      switch (status) {
      case EMBER_SIM_EEPROM_FULL:
        //Don't reboot - return to let SimEE code retry the token write
        //[BugzId:14392]
        break;
      case EMBER_ERR_FLASH_VERIFY_FAILED:
        //[[WBB350FIXME -It would be nice to have a unified, ]]
        //[[             platform independant reboot machanism]]
        #if defined (XAP2B)
          halInternalSysReset(CE_REBOOT_F_VERIFY);
        #elif defined (CORTEXM3)
          halInternalSysReset(RESET_FLASH_VERIFY);
        #else
          assert(0);
        #endif
        break;
      case EMBER_ERR_FLASH_WRITE_INHIBITED:
        #if defined (XAP2B)
          halInternalSysReset(CE_REBOOT_F_INHIBIT);
        #elif defined (CORTEXM3)
          halInternalSysReset(RESET_FLASH_INHIBIT);
        #else
          assert(0);
        #endif
        break;
      default:
        assert(0);
        break;
      }
      repairActive = false;
    }
    break;
  }
  case EMBER_SIM_EEPROM_REPAIRING:
    // While there's nothing for an app to do when the SimEE is going to
    // repair itself (SimEE has to be fully functional for the rest of the
    // system to work), alert the application to the fact that repairing
    // is occuring.  There are debugging scenarios where an app might want
    // to know that repairing is happening; such as monitoring frequency.
    // NOTE:  Common situations will trigger an expected repair, such as
    //        using an erased chip or changing token definitions.
    break;
  default:
    // this condition indicates an unexpected problem.
    assert(0);
    break;
  }
}

#endif//EMBER_APPLICATION_HAS_CUSTOM_SIM_EEPROM_CALLBACK

#ifndef CUSTOM_EM250_TEST_APPLICATION
/* Sample system call */
uint32_t emberTest (unsigned int arg, ... )
{
  return arg;
}
#endif//CUSTOM_EM250_TEST_APPLICATION

#ifndef EMBER_APPLICATION_HAS_CUSTOM_ISRS
uint16_t microGenericIsr ( uint16_t interrupt, uint16_t pcbContext )
{
  return interrupt;
}

uint16_t halInternalSc2Isr(uint16_t interrupt, uint16_t pcbContext)
{
  return interrupt;
}
#endif//EMBER_APPLICATION_HAS_CUSTOM_ISRS
#endif//XAP2B

#endif /* __NCP_CONFIG__ */
