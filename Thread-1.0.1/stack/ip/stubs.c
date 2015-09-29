// File: stub-file.c
// Description: The global stub file.
//
// To use:
//
// #define the stub files you want to use in your application, then #include this file.
//
// Example to use the emIsActiveKeySequence() and emIsFrameCounterValid() stubs in your
// application:
//
// #define USE_STUB_emIsActiveKeySequence
// #define USE_STUB_emIsFrameCounterValid
// #include "stack/ip/stubs.c"
//
// Author(s): Nate Smith, nathan.smith@silabs.com
//
// Copyright 2014 by Silicon Laboratories. All rights reserved.             *80*

#ifdef USE_STUB_emGleanShortIdFromInterfaceId
bool emGleanShortIdFromInterfaceId(uint8_t *interfaceId, uint16_t* shortId)
{
  return false;
}
#endif

#ifdef USE_STUB_emIsActiveKeySequence
bool emIsActiveKeySequence(uint8_t keySequence)
{
  return false;
}
#endif

#ifdef USE_STUB_FrameCounterStatus
typedef enum {
 FRAME_COUNTER_NOT_VALID        // not the actual value - its a stub
} FrameCounterStatus;
#endif

#ifdef USE_STUB_emIsFrameCounterValid
FrameCounterStatus emIsFrameCounterValid(uint32_t newFrameCounter,
                                         uint32_t keySequence,
                                         uint32_t nextFrameCounter,
                                         bool usingOldKey)
{
  return FRAME_COUNTER_NOT_VALID;
}
#endif

#ifdef USE_STUB_emIsPriorKeySequence
bool emIsPriorKeySequence(uint8_t keySequence)
{
  return false;
}
#endif

#ifdef USE_STUB_emNodeType
EmberNodeType emNodeType = EMBER_ROUTER;
#endif

#ifdef USE_STUB_halCommonGetInt32uMillisecondTick
uint32_t halCommonGetInt32uMillisecondTick(void)
{
  return 0;
}
#endif

#ifdef USE_STUB_halGetWakeInfo
uint32_t halGetWakeInfo(void)
{
  return 0;
}
#endif

#ifdef USE_STUB_emUseParentLongId
bool emUseParentLongId = false;
#endif

#ifdef USE_STUB_emberGetNodeId
EmberNodeId emberGetNodeId(void)
{
  return 0;
}
#endif

#ifdef USE_STUB_emberGetNodeStatusReturn
void emberGetNodeStatusReturn(EmberStatus status,
                              uint8_t ripId,
                              EmberNodeId nodeId,
                              uint8_t parentRipId,
                              EmberNodeId parentId,
                              const uint8_t *networkFragmentIdentifier,
                              uint32_t networkFrameCounter) {}
#endif

#ifdef USE_STUB_emberMarkApplicationBuffers
void emberMarkApplicationBuffers(void) {}
#endif

#ifdef USE_STUB_emberFreeMemoryForPacketHandler
void emberFreeMemoryForPacketHandler(void *objectRef) {}
#endif

#ifdef USE_STUB_emModifyLongId
void emModifyLongId(const uint8_t *longId, uint8_t *interfaceId) {}
#endif

#ifdef USE_STUB_emUnmodifyInterfaceId
void emUnmodifyInterfaceId(const uint8_t *interfaceId, uint8_t *longId) {}
#endif

#ifdef USE_STUB_emReallyLogLine
void emReallyLogLine(uint8_t logType, PGM_P formatString, ...) {}
#endif

#ifdef USE_STUB_emLogConfigFromName
bool emLogConfigFromName(const char *typeName,
                            uint8_t typeNameLength,
                            bool on,
                            uint8_t port)
{
  return false;
}
#endif

#ifdef USE_STUB_emMacRequestPoll
void emMacRequestPoll(void) {}
#endif

#ifdef USE_STUB_emSetSleepyBroadcast
void emSetSleepyBroadcast(PacketHeader header) {}
#endif

#ifdef USE_STUB_emParentId
uint16_t emParentId;
#endif

#ifdef USE_STUB_emberEventIsScheduled
bool emberEventIsScheduled(Event *event) { return false; }
#endif

#ifdef USE_STUB_ChildUpdateState
typedef enum {
  CHILD_KEEP_ALIVE,
  CHILD_UPDATE,
  PARENT_REBOOT
} ChildUpdateState;
#endif

#ifdef USE_STUB_emSendMleChildUpdate
bool emSendMleChildUpdate(const EmberIpv6Address *destination,
                          ChildUpdateState state)
{
  return false;
}
#endif

#ifdef USE_STUB_emberEventSetInactive
void emberEventSetInactive(Event *event) {}
#endif

#ifdef USE_STUB_emberEventSetDelayMs
void emberEventSetDelayMs(Event *event, uint32_t delay) {}
#endif

#ifdef USE_STUB_emJoinInit
void emJoinInit(void) {}
#endif

#ifdef USE_STUB_emStartAttach
void emStartAttach(void) {}
#endif

#ifdef USE_STUB_emStackConfiguration
uint16_t emStackConfiguration = STACK_CONFIG_FULL_ROUTER;
#endif

#ifdef USE_STUB_emStackEventQueue
EventQueue emStackEventQueue;
#endif

#ifdef USE_STUB_emApiEventDelayUpdatedFromIsrHandler
void emApiEventDelayUpdatedFromIsrHandler(Event *event) {}
#endif

#ifdef USE_STUB_emGetParentIpv6Address
bool emGetParentIpv6Address(EmberIpv6Address *address) { return false; }
#endif

#ifdef USE_STUB_emGetPollTimeout
uint32_t emGetPollTimeout(void) { return 0; }
#endif

#ifdef USE_STUB_emSetLeaderEui64BeaconData
void emSetLeaderEui64BeaconData(const uint8_t *value) {}
#endif

#ifdef USE_STUB_emGetLeaderEui64BeaconPointer
uint8_t allZerosForEmGetLeaderEui64BeaconPointer[8] = {0};
uint8_t *emGetLeaderEui64BeaconPointer(void)
{
  return allZerosForEmGetLeaderEui64BeaconPointer;
}
#endif

#ifdef USE_STUB_emFindNeighborIndex
uint8_t emFindNeighborIndex(PacketHeader header)
{
  return 0xFF;
}
#endif

#ifdef USE_STUB_emNoteLinkQuality
void emNoteLinkQuality(uint8_t neighborIndex) {}
#endif

#ifdef USE_STUB_emRipInit
void emRipInit(void) {}
#endif

#ifdef USE_STUB_emStartRip
void emStartRip(void) {}
#endif

#ifdef USE_STUB_emStartReedAdvertisements
void emStartReedAdvertisements(void) {}
#endif

#ifdef USE_STUB_emSetNetworkStatus
void emSetNetworkStatus(EmberNetworkStatus newStatus) {}
#endif

#ifdef USE_STUB_emLinkIsEstablished
bool emLinkIsEstablished(uint8_t index)
{
  return false;
}
#endif

#ifdef USE_STUB_emBeaconPayloadBuffer
uint8_t emBeaconPayloadBuffer[100] = {0};
#endif

#ifdef USE_STUB_emDefaultRouteToUart
bool emDefaultRouteToUart = false;
#endif

#ifdef USE_STUB_emGetActiveNetworkKeySequenceNumber
bool emGetActiveNetworkKeySequenceNumber(uint32_t *sequenceLoc)
{
  return false;
}
#endif

#ifdef USE_STUB_emJoiningNewFragment
bool emJoiningNewFragment = false;
#endif

#ifdef USE_STUB_emIncrementBorderRouterVersion
void emIncrementBorderRouterVersion(void) {}
#endif

#ifdef USE_STUB_emLocalRipId
uint8_t emLocalRipId(void)
{
  return 0xFF;
}
#endif

#ifdef USE_STUB_emLookupLongId
uint8_t *emLookupLongId(EmberNodeId shortId)
{
  return NULL;
}
#endif

#ifdef USE_STUB_emLookupNextHop
bool emLookupNextHop(const uint8_t *address, uint16_t *nextHop)
{
  return true;
}
#endif

#ifdef USE_STUB_emMacReceiveMode
bool emMacReceiveMode = false;
#endif

#ifdef USE_STUB_emIsAssignedInMask
bool emIsAssignedInMask(const uint8_t *mask, uint8_t ripId)
{
  return false;
}
#endif

#ifdef USE_STUB_emMarkAssignedRipIds
void emMarkAssignedRipIds(uint8_t sequenceNumber, const uint8_t *mask) {}
#endif

#ifdef USE_STUB_emNetworkKeyUpdate
void emNetworkKeyUpdate(uint32_t sequence, const uint8_t *key, uint8_t authCounter) {}
#endif

#ifdef USE_STUB_emGlobalAddressTable
Buffer emGlobalAddressTable = NULL_BUFFER;
#endif

#ifdef USE_STUB_emberGetGlobalPrefixReturn
void emberGetGlobalPrefixReturn(uint8_t borderRouterFlags,
                                const uint8_t *prefix,
                                uint8_t domainId,
                                uint32_t preferredLifetime,
                                uint32_t validLifetime) {}
#endif

#ifdef USE_STUB_emberGetGlobalAddressReturn
void emberGetGlobalAddressReturn(const EmberIpv6Address *address,
                                 uint32_t preferredLifetime,
                                 uint32_t validLifetime,
                                 uint8_t addressFlags) {}
#endif

#ifdef USE_STUB_emberGetDhcpClientReturn
void emberGetDhcpClientReturn(const EmberIpv6Address *address) {}
#endif

#ifdef USE_STUB_emNoteGlobalPrefix
void emNoteGlobalPrefix(uint8_t *prefix) {}
#endif

#ifdef USE_STUB_emRipEntry
typedef struct {
  // The destination id is the index of the entry in the RIP table.
  uint8_t eui64[8];
  uint32_t frameCounter;
  uint8_t rollingLinkMargin;
  uint8_t nextHop;
  uint8_t metric; // rip = bits 0-3, incoming = bits 4-5, outgoing = bits 6-7
  uint8_t flags; // age = bottom nibble, using-old-key = bit 5, mle-in = bit 6,
               // bit 7 unused
} RipEntry;
#endif

#ifdef USE_STUB_emRipTable
RipEntry emRipTable[64] = {0};
#endif

#ifdef USE_STUB_emRouterCount
uint8_t emRouterCount = 0;
#endif

#ifdef USE_STUB_emUnassignRipId
bool emUnassignRipId(const uint8_t *longId)
{
  return true;
}
#endif

#ifdef USE_STUB_emSendDhcpAddressRequest
void emSendDhcpAddressRequest(void *entry, EmberNodeId serverNodeId) {}

#endif

#ifdef USE_STUB_emSendDhcpRouterIdRequest
bool emSendDhcpRouterIdRequest(void) { return false; }
#endif

#ifdef USE_STUB_emberExternalRouteChange
void emberExternalRouteChange(const uint8_t *prefix,
                              uint8_t prefixLength,
                              bool open) {}
#endif

#ifdef USE_STUB_emberDhcpServerChange
void emberDhcpServerChange(const uint8_t *prefix,
                           uint8_t prefixLength,
                           bool available) {}
#endif

#ifdef USE_STUB_emberSlaacServerChange
void emberSlaacServerChange(const uint8_t *prefix,
                            uint8_t prefixLength,
                            bool available) {}
#endif

#ifdef USE_STUB_emberAddressConfigurationChange
void emberAddressConfigurationChange(const EmberIpv6Address *address,
                                     uint32_t validLifetime,
                                     uint32_t preferredLifetime,
                                     uint8_t addressFlags) {}
#endif

#ifdef USE_STUB_emberRequestDhcpAddressReturn
void emberRequestDhcpAddressReturn(EmberStatus status,
                                   const uint8_t *prefix,
                                   uint8_t prefixLength) {}
#endif

#ifdef USE_STUB_emberRequestSlaacAddressReturn
void emberRequestSlaacAddressReturn(EmberStatus status,
                                    const uint8_t *prefix,
                                    uint8_t prefixLength) {}
#endif

#ifdef USE_STUB_emberResignGlobalAddressReturn
void emberResignGlobalAddressReturn(EmberStatus status) {}
#endif

#ifdef USE_STUB_emSendMyNetworkData
void emSendMyNetworkData(EmberNodeId leader, Buffer myNetworkData) {}
#endif

#ifdef USE_STUB_emSendNetworkData
PacketHeader emSendNetworkData(const uint8_t *ipDest, uint8_t options)
{
  return NULL_BUFFER;
}
#endif

#ifdef USE_STUB_emSendNetworkDataRequest
bool emSendNetworkDataRequest(const uint8_t *ipDest,
                                 bool includeIdMask,
                                 uint16_t jitter)
{
  return false;
}
#endif

#ifdef USE_STUB_emAddNetworkDataTlv
bool emAddNetworkDataTlv(uint8_t *tlv) { return false; }
#endif

#ifdef USE_STUB_emNoteNetworkDataChange
void emNoteNetworkDataChange(void) {}
#endif

#ifdef USE_STUB_emProcessNetworkDataChangeRequest
uint16_t emProcessNetworkDataChangeRequest(const uint8_t *ipSource,
                                         uint8_t *data,
                                         uint16_t dataLength)
{
  return 0;
}
#endif

#ifdef USE_STUB_emGetLeaderNodeId
EmberNodeId emGetLeaderNodeId(void) { return 0xFFFE; }
#endif

#ifdef USE_STUB_emPreviousNodeId
EmberNodeId emPreviousNodeId;
#endif

#ifdef USE_STUB_emStoreMulticastSequence
void emStoreMulticastSequence(uint8_t *target) {}
#endif

#ifdef USE_STUB_emStackTask
EmberTaskId emStackTask = 0;
#endif

#ifdef USE_STUB_halButtonIsr
void halButtonIsr(uint8_t button, uint8_t state) {}
#endif

#ifdef USE_STUB_halCommonIdleForMilliseconds
EmberStatus halCommonIdleForMilliseconds(uint32_t *duration)
{
  return EMBER_SUCCESS;
}
#endif

#ifdef USE_STUB_halInternalGetTokenData
void halInternalGetTokenData(void *data, uint16_t addr, uint8_t index, uint8_t len)
{
}
#endif

#ifdef USE_STUB_halInternalSetTokenData
void halInternalSetTokenData(uint16_t token, uint8_t index, void *data, uint8_t len)
{
}
#endif

#ifdef USE_STUB_halStackSymbolDelayAIsr
void halStackSymbolDelayAIsr(void) {}
#endif

#ifdef USE_STUB_emSetTransportChecksum
void emSetTransportChecksum(PacketHeader header, Ipv6Header *ipHeader) {}
#endif

#ifdef USE_STUB_emUsePresharedTlsSessionState
bool emUsePresharedTlsSessionState = false;
#endif

#ifdef USE_STUB_emMleLurkerAcceptHandler
void emMleLurkerAcceptHandler(void) {}
#endif

#ifdef USE_STUB_emMleAttachCompleteHandler
void emMleAttachCompleteHandler(bool success) {}
#endif

#ifdef USE_STUB_MleCommand
// MLE commands.
typedef enum {
  MLE_REQUEST            = 0,
  MLE_ACCEPT             = 1,
  MLE_ACCEPT_AND_REQUEST = 2,
  MLE_REJECT             = 3,
  MLE_ADVERTISEMENT      = 4,
  MLE_UPDATE             = 5,
  MLE_UPDATE_REQUEST     = 6,
  MLE_DATA_REQUEST       = 7,
  MLE_DATA_RESPONSE      = 8,
  MLE_PARENT_REQUEST     = 9,
  MLE_PARENT_RESPONSE    = 10,
  MLE_CHILD_ID_REQUEST   = 11,
  MLE_CHILD_ID_RESPONSE  = 12,
// These are pseudo commands used internally
  MLE_JOIN_REQUEST       = 0xFE,    // unsecured REQUEST with no challenge
  MLE_JOIN_ACCEPT        = 0xFF     // unsecured ACCEPT with no response
} MleCommand;
#endif

#ifdef USE_STUB_MleMessage
typedef struct {
  MleCommand command;
  uint8_t keySequence;            // key sequence number used for decryption

  // TLV data (grouped by size to minimize padding)
  uint8_t mode;
  uint8_t challengeLength;
  uint8_t responseLength;
  uint8_t neighborsLength;
  uint8_t parameterCount;
  uint8_t scanMask;
  bool isLegacy;

  // the address of the source
  EmberNodeId sourceAddress;

  // assigned to us from the source, taken from MLE_SHORT_ADDRESS_TLV
  EmberNodeId assignedShortAddress;

  uint32_t tlvMask;               // which TLVs are present
  uint32_t requestedTlvMask;      // which TLVs are requested from the destination
  uint32_t timeout;      // child timeout, from MLE_TIMEOUT_TLV
  uint8_t *challenge;    // points to the challenge in MLE_CHALLENGE_TLV
  uint8_t *response;     // points to the response in MLE_RESPONSE_TLV
  uint8_t *neighbors;    // points to the neighbors in MLE_NEIGHBOR_TLV
  uint8_t *frameCounter; // points to the frame counter in MLE_REPLAY_COUNTER_TLV
  uint8_t *parameters;   // points to the parameters in MLE_PARAMETER_TLV
  uint8_t *globalPrefix; // points to the global prefix in MLE_GLOBAL_PREFIX_TLV
  uint8_t *leaderData;            // points to start of data
  uint8_t *networkData;           // points to length byte at start of data
  uint8_t *assignedIdMask;        // points to start of data
  uint8_t *connectivity;
  uint8_t senderLongId[8];
  uint8_t rssi;
} MleMessage;
#endif

#ifdef USE_STUB_emMleModeIsLurker
bool emMleModeIsLurker(MleMessage *message)
{
  return (message->mode == BIT(7)
          || (message->isLegacy && message->mode == 0));
}
#endif

#ifdef USE_STUB_emZigbeeNetworkSecurityLevel
uint8_t emZigbeeNetworkSecurityLevel = 4;
#endif

#ifdef USE_STUB_sendLogEvent
void sendLogEvent(char *foo, char *bar) {}
#endif

#ifdef USE_STUB_emCoapInitialize
void emCoapInitialize(void) {}
#endif

#ifdef USE_STUB_emCostDivision
uint8_t emCostDivision(uint8_t cost)
{
  return 1;
}
#endif

#ifdef USE_STUB_emCustomLinkQualities
uint8_t emCustomLinkQualities[5] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
#endif

#ifdef USE_STUB_emEnableMle
bool emEnableMle = true;
#endif

#ifdef USE_STUB_printNeighborTable
void printNeighborTable(void) {}
#endif

#ifdef USE_STUB_emEnableMacEncryption
bool emEnableMacEncryption = true;
#endif

#ifdef USE_STUB_emEncryptDataPolls
bool emEncryptDataPolls = true;
#endif

#ifdef USE_STUB_emFindChild
EmberNodeId emFindChild(const uint8_t *id)
{
  return EMBER_NULL_NODE_ID;
}
#endif

#ifdef USE_STUB_emFindChildIndex
uint8_t emFindChildIndex(uint8_t startIndex, const uint8_t *eui64)
{
  return 0xFF;
}
#endif

#ifdef USE_STUB_emFindChildIndexByAddress
uint8_t emFindChildIndexByAddress(const EmberIpv6Address *address)
{
  return 0xFF;
}
#endif

#ifdef USE_STUB_emInitializeChild
bool emInitializeChild(uint8_t index,
                          EmberNodeId id,
                          const uint8_t *eui64,
                          uint8_t capabilities,
                          uint32_t timeoutSeconds)
{
  return true;
}
#endif

#ifdef USE_STUB_emNoteChildTransaction
void emNoteChildTransaction(uint8_t index) { }
#endif

#ifdef USE_STUB_emRetrySubmit
bool emRetrySubmit(PacketHeader header, uint8_t retries, uint16_t delayMs)
{
  return true;
}
#endif

#ifdef USE_STUB_emSubmitIndirectOrRetry
bool emSubmitIndirectOrRetry(PacketHeader header,
                                uint8_t retries,
                                uint16_t delayMs)
{
  return true;
}
#endif

#ifdef USE_STUB_emReallySubmitIpHeader
bool emReallySubmitIpHeader(PacketHeader header,
                               Ipv6Header *ipHeader,
                               bool allowLoopback,
                               uint8_t retries,
                               uint16_t delayMs)
{
  return true;
}
#endif

#ifdef USE_STUB_emRetryInit
void emRetryInit(void) {}
#endif

#ifdef USE_STUB_emberSerialPrintf
EmberStatus emberSerialPrintf(uint8_t port, PGM_P formatString, ...)
{
  return EMBER_SUCCESS;
}
#endif

#ifdef USE_STUB_emberSerialPrintfLine
EmberStatus emberSerialPrintfLine(uint8_t port, PGM_P formatString, ...)
{
  return EMBER_SUCCESS;
}
#endif

#ifdef USE_STUB_emberSerialWaitSend
EmberStatus emberSerialWaitSend(uint8_t port)
{
  return 0;
}
#endif

#ifdef USE_STUB_emInUseAsNextHop
bool emInUseAsNextHop(uint16_t id)
{
  return false;
}
#endif

#ifdef USE_STUB_emExtendedRetryCount
uint8_t emExtendedRetryCount;
#endif

#ifdef USE_STUB_emBrokenDagParent
void emBrokenDagParent(uint16_t parentId) {}
#endif

#ifdef USE_STUB_emSetPhyRadioChannel
uint8_t emSetPhyRadioChannel(uint8_t radioChannel)
{
  return 0;
}
#endif

#ifdef USE_STUB_emSetOptionalBeaconPayload
void emSetOptionalBeaconPayload(uint8_t *payload, uint8_t length) {}
#endif

#ifdef USE_STUB_printEui
void printEui(uint8_t *eui) {}
#endif

#ifdef USE_STUB_emberCounterHandler
void emberCounterHandler(EmberCounterType type, uint16_t increment) {}
#endif

#ifdef USE_STUB_emDefaultSourceLongId
const uint8_t emDefaultSourceLongId[8] = {
  0x42, 0x8A, 0x2F, 0x98, 0x71, 0x37, 0x44, 0x91
};
#endif

#ifdef USE_STUB_emSendIcmpMessage
bool emSendIcmpMessage(uint8_t type,
                          uint8_t code,
                          const uint8_t *ipDest,
                          const uint8_t *payload,
                          uint16_t payloadLength)
{
  return false;
}
#endif

#ifdef USE_STUB_emSetNodeType
void emSetNodeType(EmberNodeType type) {}
#endif

#ifdef USE_STUB_emberGetPanId
EmberPanId emberGetPanId(void)
{
  return 0;
}
#endif

#ifdef USE_STUB_emSetNodeId
void emSetNodeId(uint16_t nodeId) {}
#endif

#ifdef USE_STUB_emberSleepyChildPollTimeout
uint32_t emberSleepyChildPollTimeout = 300;
#endif

#ifdef USE_STUB_emberEndDevicePollTimeout
uint8_t emberEndDevicePollTimeout = 30;
#endif

#ifdef USE_STUB_emChildFrameCounters
static uint32_t frameCounters[16];
uint32_t *emChildFrameCounters = frameCounters;
#endif

#ifdef USE_STUB_emChildSecondsSinceLastTransaction
uint16_t emChildSecondsSinceLastTransaction(uint8_t index) { return 0; }
#endif

#ifdef USE_STUB_emChildSecondsSinceAttaching
uint16_t emChildSecondsSinceAttaching(uint8_t index) { return 0; }
#endif

#ifdef USE_STUB_emSendMleParentRequest
bool emSendMleParentRequest(uint8_t scanMask)
{
  return false;
}
#endif

#ifdef USE_STUB_emSendMleChildIdRequest
bool emSendMleChildIdRequest(uint8_t *ipDest,
                                const uint8_t *response,
                                uint8_t responseLength,
                                bool amSleepy,
                                bool darkRouterActivationOnly)
{
  return false;
}
#endif

#ifdef USE_STUB_emChildCount
uint8_t emChildCount(void)
{
  return 0;
}
#endif

#ifdef USE_STUB_emChildIdTable
static uint16_t childIdTable[16];
uint16_t *emChildIdTable = childIdTable;
#endif

#ifdef USE_STUB_emChildStatus
static uint16_t childStatus[16];
uint16_t *emChildStatus = childStatus;
#endif

#ifdef USE_STUB_emChildTimeouts
static uint32_t childTimeouts[16];
uint32_t *emChildTimeouts = childTimeouts;
#endif

#ifdef USE_STUB_emChildSetFrameCounter
void emChildSetFrameCounter(uint8_t index,
                            uint32_t frameCounter,
                            uint32_t keySequence)
{
}
#endif

#ifdef USE_STUB_emberNetworkStatus
EmberNetworkStatus emberNetworkStatus(void)
{
  return EMBER_JOINED_NETWORK;
}
#endif

#ifdef USE_STUB_emVerifyLocalServerData
void emVerifyLocalServerData(void) {}
#endif

#ifdef USE_STUB_emInitializeLeaderServerData
void emInitializeLeaderServerData(void) {}
#endif

#ifdef USE_STUB_emMakeLocalServerData
uint8_t *emMakeLocalServerData(uint8_t *finger, uint8_t *end)
{
  return finger;
}
#endif

#ifdef USE_STUB_emIsOurDhcpServerPrefix
bool emIsOurDhcpServerPrefix(const uint8_t *prefix)
{
  return false;
}
#endif

#ifdef USE_STUB_emIsOurExternalRoutePrefix
bool emIsOurExternalRoutePrefix(const uint8_t *prefix)
{
  return false;
}
#endif

#ifdef USE_STUB_emProcessIdResponse
void emProcessIdResponse(uint16_t shortAddress) {}
#endif

#ifdef USE_STUB_emMakeAssignedRipIdMask
bool emMakeAssignedRipIdMask(uint8_t *mask)
{
  memset(mask, 0xFF, ASSIGNED_RIP_ID_MASK_SIZE);
  return ASSIGNED_RIP_ID_MASK_SIZE;
}
#endif

#ifdef USE_STUB_emRipIdSequenceNumber
uint8_t emRipIdSequenceNumber = 0;
#endif

#ifdef USE_STUB_emProcessRipNeighborTlv
void emProcessRipNeighborTlv(MleMessage *message) {}
#endif

#ifdef USE_STUB_JoinState
// Values for emJoinState.
typedef enum {
  JOIN_COMPLETE                  = 0,
} JoinState;
#endif

#ifdef USE_STUB_emJoinState
uint8_t emJoinState = JOIN_COMPLETE;
#endif

#ifdef USE_STUB_AttachState
typedef enum {
  ATTACH_COMPLETE                = 0,
} AttachState;
#endif

#ifdef USE_STUB_emAttachState
uint8_t emAttachState = ATTACH_COMPLETE;
#endif

#ifdef USE_STUB_emEraseChild
void emEraseChild(uint8_t childIndex) {}
#endif

#ifdef USE_STUB_emIsAssignedId
bool emIsAssignedId(EmberNodeId id, const uint8_t *neighborData)
{
  return true;
}
#endif

#ifdef USE_STUB_emCurrentRssi
int8_t emCurrentRssi;
#endif

#ifdef USE_STUB_emGetRoute
uint8_t emGetRoute(uint8_t destIndex, uint8_t *routeCostResult)
{
  *routeCostResult = 0;
  return 0xFF;
}
#endif

#ifdef USE_STUB_emHastenRipAdvertisement
void emHastenRipAdvertisement(void) {}
#endif

#ifdef USE_STUB_emMaybeHastenRipAdvertisement
void emMaybeHastenRipAdvertisement(uint8_t index, uint8_t oldRouteCost) {}
#endif

#ifdef USE_STUB_emRouterRebootSuccess
void emRouterRebootSuccess(void) {}
#endif

#ifdef USE_STUB_emStopLeaderTimeout
void emStopLeaderTimeout(void) {}
#endif

#ifdef USE_STUB_emIpConnections
Buffer emIpConnections;
#endif

#ifdef USE_STUB_emChildInit
void emChildInit(void) {}
#endif

#ifdef USE_STUB_emBeaconPayload
uint8_t *emBeaconPayload(PacketHeader beacon)
{
  return NULL;
}
#endif

#ifdef USE_STUB_emBeaconPayloadSize
uint8_t emBeaconPayloadSize = 0;
#endif

#ifdef USE_STUB_emBorderRouterTableInit
void emBorderRouterTableInit(void) {}
#endif

#ifdef USE_STUB_emEraseHostRegistry
void emEraseHostRegistry(void) {}
#endif

#ifdef USE_STUB_emFormSecurityInit
void emFormSecurityInit(void) {}
#endif

#ifdef USE_STUB_emFragmentState
FragmentState emFragmentState = 0;
#endif

#ifdef USE_STUB_emFragmentInit
void emFragmentInit(void) {}
#endif

#ifdef USE_STUB_emIsRepairingFragment
bool emIsRepairingFragment(void) {}
#endif

#ifdef USE_STUB_emIncomingForMeToUart
bool emIncomingForMeToUart = false;
#endif

#ifdef USE_STUB_emJoinSecurityInit
void emJoinSecurityInit(void) {}
#endif

#ifdef USE_STUB_emLocalPermitJoinUpdate
void emLocalPermitJoinUpdate(uint32_t durationMs) {}
#endif

#ifdef USE_STUB_emMacScanType
uint8_t emMacScanType = 0;
#endif

#ifdef USE_STUB_emRadioGetRandomNumbers
bool emRadioGetRandomNumbers(uint16_t *buffer, uint8_t length)
{
  return true;
}
#endif

#ifdef USE_STUB_emSaveNetwork
void emSaveNetwork(void) {}
#endif

#ifdef USE_STUB_emSecurityToUart
bool emSecurityToUart = false;
#endif

#ifdef USE_STUB_emSendMleJoinRequest
bool emSendMleJoinRequest(uint8_t *ipDest)
{
  return false;
}
#endif

#ifdef USE_STUB_emSendMleRequest
bool emSendMleRequest(uint8_t *ipDest, bool jitter, bool isLegacy)
{
  return false;
}
#endif

#ifdef USE_STUB_emSendMleRouterIdVerifyRequest
bool emSendMleRouterIdVerifyRequest(void)
{
  return false;
}
#endif

#ifdef USE_STUB_emSendMleEndDeviceIdVerifyRequest
bool emSendMleEndDeviceIdVerifyRequest(uint8_t *destIp)
{
  return false;
}
#endif

#ifdef USE_STUB_emSendMleReject
void emSendMleReject(uint8_t *ipDest, MleMessage *message) {}
#endif

#ifdef USE_STUB_emSendPermitJoinUpdate
bool emSendPermitJoinUpdate(uint32_t duration)
{
  return false;
}
#endif

#ifdef USE_STUB_emSetPanId
void emSetPanId(uint16_t panId) {}
#endif

#ifdef USE_STUB_emSetRadioPower
void emSetRadioPower(int8_t power) {}
#endif

#ifdef USE_STUB_emStartDagRoot
bool emStartDagRoot(uint8_t instanceId,
                       const DagConfiguration *configuration,
                       bool dao,
                       uint8_t *pioPrefix)
{
  return false;
}
#endif

#ifdef USE_STUB_emStartLeaderTimeout
void emStartLeaderTimeout(void) {}
#endif

#ifdef USE_STUB_emStartJoinClient
bool emStartJoinClient(const uint8_t *address,
                          const uint8_t *key,
                          uint8_t keyLength)
{
  return false;
}
#endif

#ifdef USE_STUB_emStartPolling
void emStartPolling(void) {}
#endif

#ifdef USE_STUB_emStartRouterSelection
void emStartRouterSelection(void) {}
#endif

#ifdef USE_STUB_emStartLurkerAdvertisements 
void emStartLurkerAdvertisements(void) {}
#endif

#ifdef USE_STUB_emStopLurkerAdvertisements 
void emStopLurkerAdvertisements(void) {}
#endif

#ifdef USE_STUB_emStopRip
void emStopRip(void) {}
#endif

#ifdef USE_STUB_emberAddAddressDataReturn
void emberAddAddressDataReturn(uint16_t shortId) {}
#endif

#ifdef USE_STUB_emberClearAddressCacheReturn
void emberClearAddressCacheReturn(void) {}
#endif

#ifdef USE_STUB_emberLookupAddressDataReturn
void emberLookupAddressDataReturn(uint16_t shortId) {}
#endif

#ifdef USE_STUB_emberActiveScanHandler
void emberActiveScanHandler(const EmberMacBeaconData *beaconData) {}
#endif

#ifdef USE_STUB_emberCommissionNetworkReturn
void emberCommissionNetworkReturn(EmberStatus status) {}
#endif

#ifdef USE_STUB_emberFormNetworkReturn
void emberFormNetworkReturn(EmberStatus status) {}
#endif

#ifdef USE_STUB_emberIpIncomingBeaconHandler
void emberIpIncomingBeaconHandler(PacketHeader header) {}
#endif

#ifdef USE_STUB_emberJoinNetworkReturn
void emberJoinNetworkReturn(EmberStatus status) {}
#endif

#ifdef USE_STUB_emberAttachToNetworkReturn
void emberAttachToNetworkReturn(EmberStatus status) {}
#endif

#ifdef USE_STUB_emberChangeNodeTypeReturn
void emberChangeNodeTypeReturn(EmberStatus status) {}
#endif

#ifdef USE_STUB_emberPermitJoiningReturn
void emberPermitJoiningReturn(EmberStatus status) {}
#endif

#ifdef USE_STUB_emberPermitJoiningHandler
void emberPermitJoiningHandler(bool joiningAllowed) {}
#endif


#ifdef USE_STUB_emberResumeNetworkReturn
void emberResumeNetworkReturn(EmberStatus status) {}
#endif

#ifdef USE_STUB_emberSetRadioPower
void emberSetRadioPower(int8_t power) {}
#endif

#ifdef USE_STUB_emberStartScan
void emberStartScan(EmberNetworkScanType scanType,
                    uint32_t channelMask,
                    uint8_t duration)
{
}
#endif

#ifdef USE_STUB_emAddAddressData
AddressData *emAddAddressData(uint8_t context,
                              const uint8_t *iid,
                              EmberNodeId shortId,
                              bool update)
{
  return NULL;
}
#endif

#ifdef USE_STUB_emUnassignShortId
bool emUnassignShortId(uint8_t context, const uint8_t *iid) { return false; }
#endif

#ifdef USE_STUB_AddressData
typedef struct {
  uint8_t iid[8];
  EmberNodeId shortId;  // 0xFFFF = unused, 0xFFFE = not known
  uint8_t context;        // 6LoWPAN context for the prefix
  uint8_t key;            // retrieval key
  uint8_t flags;          // flags for DHCP, discovery, etc.
  uint8_t head;           // least-recently-used queue
  uint8_t tail;
  uint32_t lastTransactionSeconds;
} AddressData;
#endif

#ifdef USE_STUB_emAddressCache
Vector emAddressCache = {
  NULL_BUFFER,
  sizeof(AddressData),
  0,
  0
};
#endif

#ifdef USE_STUB_emAmThreadCommissioner
bool emAmThreadCommissioner(void)
{
  return false;
}
#endif

#ifdef USE_STUB_emFindNextNetworkDataTlv
uint8_t *emFindNextNetworkDataTlv(uint8_t type, uint8_t *start)
{
  return NULL;
}
#endif

#ifdef USE_STUB_emFindPrefixSubTlv
uint8_t *emFindPrefixSubTlv(uint8_t *prefixTlv, uint8_t innerType)
{
  return NULL;
}
#endif

#ifdef USE_STUB_emRemoveNetworkDataSubTlv
void emRemoveNetworkDataSubTlv(uint8_t *outerTlv, uint8_t *innerTlv) {}
#endif

#ifdef USE_STUB_emSetNetworkDataTlvLength
uint8_t *emSetNetworkDataTlvLength(uint8_t *tlv, uint16_t tlvDataLength)
{
  return NULL;
}
#endif

#ifdef USE_STUB_emberAddAddressData
void emberAddAddressData(const uint8_t *longId, uint16_t shortId) {}
#endif

#ifdef USE_STUB_emFindNearestSlaacPreferredServer
EmberNodeId emFindNearestSlaacPreferredServer(const uint8_t *prefix)
{
  return 0xFFFE;
}
#endif

#ifdef USE_STUB_emFindNearestSlaacValidServer
EmberNodeId emFindNearestSlaacValidServer(const uint8_t *prefix)
{
  return 0xFFFE;
}
#endif

#ifdef USE_STUB_emFindNearestDhcpServer
EmberNodeId emFindNearestDhcpServer(const uint8_t *prefix)
{
  return 0xFFFE;
}
#endif

#ifdef USE_STUB_emStoreIpv6Header
bool emStoreIpv6Header(Ipv6Header *source, PacketHeader header)
{
  return false;
}
#endif

#ifdef USE_STUB_emberUdpMulticastListen
EmberStatus emberUdpMulticastListen(uint16_t port, const uint8_t *multicastAddress)
{
  return 0;
}
#endif

#ifdef USE_STUB_emberInitReturn
void emberInitReturn(EmberStatus status) {}
#endif

#ifdef USE_STUB_emberInitializeHostAddressTable
void emberInitializeHostAddressTable(void) {}
#endif

#ifdef USE_STUB_emberGetHostGlobalAddress
GlobalAddressEntry *emberGetHostGlobalAddress(uint8_t index)
{
  return NULL;
}
#endif

#ifdef USE_STUB_emberAddHostGlobalAddress
EmberStatus emberAddHostGlobalAddress(const EmberIpv6Address *address,
                                      uint32_t preferredLifetime,
                                      uint32_t validLifetime,
                                      uint8_t addressFlags)
{
  return EMBER_SUCCESS;
}
#endif

#ifdef USE_STUB_emberRemoveHostAddress
EmberStatus emberRemoveHostAddress(const EmberIpv6Address *address)
{
  return EMBER_SUCCESS;
}
#endif

#ifdef USE_STUB_emberInitializeListeners
void emberInitializeListeners(void) {}
#endif

#ifdef USE_STUB_emberCloseListeners
void emberCloseListeners(void) {}
#endif

#ifdef USE_STUB_emAllocateTlsState
Buffer emAllocateTlsState(uint8_t fd, uint16_t flags)
{
  return NULL_BUFFER;
}
#endif

#ifdef USE_STUB_emMarkTlsState
void emMarkTlsState(Buffer *tlsBufferLoc)
{
}
#endif

#ifdef USE_STUB_emSavedTlsSessions
Buffer emSavedTlsSessions = NULL_BUFFER;
#endif

#ifdef USE_STUB_TcpConnection
typedef struct {} *TcpConnection;
#endif 

#ifdef USE_STUB_emStartTlsResume
void emStartTlsResume(TcpConnection *connection) {}
#endif

#ifdef USE_STUB_emTlsClose
bool emTlsClose(TcpConnection *connection)
{
  return true;
}
#endif

#ifdef USE_STUB_emTlsSendApplicationBuffer
EmberStatus emTlsSendApplicationBuffer(TcpConnection *connection,
                                       Buffer content)
{
  return 0;
}
#endif

#ifdef USE_STUB_emTlsSendBufferedApplicationData
EmberStatus emTlsSendBufferedApplicationData(TcpConnection *connection,
                                             const uint8_t *data,
                                             uint16_t length,
                                             Buffer moreData,
                                             uint16_t moreLength)
{
  return EMBER_ERR_FATAL;
}
#endif

#ifdef USE_STUB_emTlsStatusHandler
bool emTlsStatusHandler(TcpConnection *connection)
{
  return 0;
}
#endif

#ifdef USE_STUB_emDtlsJoinMode
uint8_t emDtlsJoinMode;
#endif

#ifdef USE_STUB_emParentConnectionHandle
uint8_t emParentConnectionHandle;
#endif

#ifdef USE_STUB_emHandleJoinDtlsMessage
void emHandleJoinDtlsMessage(uint8_t *source, uint8_t *packet, uint16_t length) {}
#endif

#ifdef USE_STUB_emSubmitRelayedJoinPayload
void emSubmitRelayedJoinPayload(void *connection, Buffer payload) {}
#endif

#ifdef USE_STUB_emSendJoinerEntrust
void emSendJoinerEntrust(uint8_t *ipDestination) {}
#endif

#ifdef USE_STUB_emLoseHelper
void emLoseHelper(void) {}
#endif

#ifdef USE_STUB_emHeaderSetTag
void emHeaderSetTag(PacketHeader header, uint8_t tag) {}
#endif

#ifdef USE_STUB_emHeaderMacInfoField
uint8_t emHeaderMacInfoField(PacketHeader header) 
{
  return 0;
}
#endif

#ifdef USE_STUB_emHeaderSetMacInfoField
void emHeaderSetMacInfoField(PacketHeader header, uint8_t info) {}
#endif

#ifdef USE_STUB_emMakeDataHeader
PacketHeader emMakeDataHeader(bool isBroadcast,
                              bool longSource,
                              bool longDest,
                              uint8_t *payload,
                              uint16_t payloadLength)
{
  return NULL_BUFFER;
}
#endif

#ifdef USE_STUB_emMarkBufferPointer
void emMarkBufferPointer(void **pointerLoc) {}
#endif

#ifdef USE_STUB_emSetMacLongDestination
void emSetMacLongDestination(PacketHeader header, const uint8_t *longId) {}
#endif

#ifdef USE_STUB_emSetMacShortDestination
void emSetMacShortDestination(PacketHeader header, EmberNodeId id) {}
#endif

#ifdef USE_STUB_emDhcpIncomingMessageHandler
bool emDhcpIncomingMessageHandler(Ipv6Header *ipHeader)
{
  return false;
}
#endif

#ifdef USE_STUB_emFindNeighborIndexByIpAddress
uint8_t emFindNeighborIndexByIpAddress(const uint8_t *ipAddress)
{
  return 0xFF;
}
#endif

#ifdef USE_STUB_emFindNeighborIndexByShortId
uint8_t emFindNeighborIndexByShortId(uint16_t shortId)
{
  return 0xFF;
}
#endif

#ifdef USE_STUB_emMleIncomingMessageHandler
void emMleIncomingMessageHandler(PacketHeader header, Ipv6Header *ipHeader)
{
}
#endif

#ifdef USE_STUB_emSendDhcpLeaseQuery
bool emSendDhcpLeaseQuery(const uint8_t *goal)
{
  return false;
}
#endif

#ifdef USE_STUB_emNeighborShortId
uint16_t emNeighborShortId(uint8_t index)
{
  return 0xFFFE;
}
#endif

#ifdef USE_STUB_emNeighborHaveShortId
bool emNeighborHaveShortId(uint8_t index)
{
  return false;
}
#endif

#ifdef USE_STUB_emNeighborSetShortId
uint8_t emNeighborSetShortId(uint8_t index, EmberNodeId id)
{
  return 0xFF;
}
#endif

#ifdef USE_STUB_emberTcpAcceptHandler
void emberTcpAcceptHandler(uint16_t port, uint8_t fd) {}
#endif

#ifdef USE_STUB_emberTcpStatusHandler
void emberTcpStatusHandler(uint8_t fd, uint8_t status) {}
#endif

#ifdef USE_STUB_emberUdpListen
EmberStatus emberUdpListen(uint16_t port, const uint8_t *address)
{
  return EMBER_SUCCESS;
}
#endif

#ifdef USE_STUB_emberUdpHandler
void emberUdpHandler(const uint8_t *destination,
                     const uint8_t *source,
                     uint16_t localPort,
                     uint16_t remotePort,
                     const uint8_t *payload,
                     uint16_t payloadLength)
{
}
#endif

#ifdef USE_STUB_emberUdpMulticastHandler
void emberUdpMulticastHandler(EmberUdpPeerData *peer,
                              const uint8_t *destinationIpv6Address,
                              const uint8_t *sourceIpv6Address,
                              uint16_t localPort,
                              uint16_t remotePort,
                              uint8_t flags,
                              const uint8_t *packet,
                              uint16_t length)
{
}
#endif

#ifdef USE_STUB_emGetNetworkKey
uint8_t *emGetNetworkKey(uint32_t sequence)
{
  return NULL;
}
#endif

#ifdef USE_STUB_emGetLegacyNetworkKey
uint8_t *emGetLegacyNetworkKey(void)
{
  return NULL;
}
#endif

#ifdef USE_STUB_emMacHeaderLength
uint8_t emMacHeaderLength(uint8_t *macFrame)
{
  return 0;
}
#endif

#ifdef USE_STUB_emSwitchToNetworkKey
void emSwitchToNetworkKey(uint32_t sequence) {}
#endif

#ifdef USE_STUB_emCcmDecryptPacket
bool emCcmDecryptPacket(const uint8_t *nonce,
                           uint8_t *packet,
                           uint16_t authenticateLength,
                           uint8_t *encrypt,
                           uint16_t encryptLength,
                           uint8_t micLength)
{
  return true;
}
#endif

#ifdef USE_STUB_emEapWriteBuffer
void emEapWriteBuffer(TlsState *tls, Buffer buffer) {}
#endif

#ifdef USE_STUB_emFinishDtlsServerJoin
EmberStatus emFinishDtlsServerJoin(void *connection)
{
  return EMBER_SUCCESS;
}
#endif

#ifdef USE_STUB_emCloseDtlsJoinConnection
void emCloseDtlsJoinConnection(void) {}
#endif

#ifdef USE_STUB_emGetCommissioningMacKey
uint8_t *emGetCommissioningMacKey(const uint8_t *senderEui64)
{
  return NULL;
}
#endif

#ifdef USE_STUB_emGetOutgoingCommissioningMacKey
uint8_t *emGetOutgoingCommissioningMacKey(const uint8_t *senderEui64)
{
  return NULL;
}
#endif

#ifdef USE_STUB_emLoadKeyIntoCore
void emLoadKeyIntoCore(const uint8_t *key) {}
#endif

#ifdef USE_STUB_emReadAndProcessRecord
TlsStatus emReadAndProcessRecord(TlsState *tls, Buffer *incomingQueue)
{
  return TLS_SUCCESS;
}
#endif

#ifdef USE_STUB_emRunHandshake
TlsStatus emRunHandshake(TlsState *tls, uint8_t *incoming, uint16_t length)
{
  return TLS_SUCCESS;
}
#endif

#ifdef USE_STUB_emSha256HashBytes
void emSha256HashBytes(Sha256State *hashState, const uint8_t *bytes, uint16_t count)
{
}
#endif

#ifdef USE_STUB_emSha256Start
void emSha256Start(Sha256State *hashState) {}
#endif

#ifdef USE_STUB_emTlsSetState
void emTlsSetState(TlsState *tls, uint8_t state) {}
#endif

#ifdef USE_STUB_emAddDhcpClient
bool emAddDhcpClient(uint8_t *address, const uint8_t *identifier)
{
  return false;
}
#endif

#ifdef USE_STUB_emCcmEncryptPacket
void emCcmEncryptPacket(const uint8_t *nonce,
                        uint8_t *packet,
                        uint16_t authenticateLength,
                        uint16_t encryptLength,
                        uint8_t micLength)
{
}
#endif

#ifdef USE_STUB_emCompressContextPrefix
bool emCompressContextPrefix(uint8_t *address, uint8_t *context)
{
  return false;
}
#endif

#ifdef USE_STUB_emFe8Prefix
const Bytes8 emFe8Prefix = {{ 0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }};
#endif

#ifdef USE_STUB_emIsLl64
bool emIsLl64(const uint8_t *address)
{
  return false;
}
#endif

#ifdef USE_STUB_emFindAll6lowpanContexts
uint16_t emFindAll6lowpanContexts(void)
{
  return 0;
}
#endif

#ifdef USE_STUB_emFindChildByInterfaceId
bool emFindChildByInterfaceId(const uint8_t *interfaceId, uint16_t *nextHop)
{
  return false;
}
#endif

#ifdef USE_STUB_emGetDhcpClientIid
uint8_t *emGetDhcpClientIid(const uint8_t *destination)
{
  return NULL;
}
#endif

#ifdef USE_STUB_emGetMleKey
uint8_t *emGetMleKey(uint32_t sequence)
{
  return NULL;
}
#endif

#ifdef USE_STUB_emGetLegacyMleKey
uint8_t *emGetLegacyMleKey(void)
{
  return NULL;
}
#endif

#ifdef USE_STUB_emGetMasterKey
uint8_t *emGetMasterKey(uint32_t sequenceNumber)
{
  return NULL;
}
#endif

#ifdef USE_STUB_emSecurityIncrementOutgoingFrameCounter
uint32_t emSecurityIncrementOutgoingFrameCounter(void)
{
  return 1;
}
#endif

#ifdef USE_STUB_emGetSecurityFrameCounter
uint32_t emGetSecurityFrameCounter(void)
{
  return 0;
}
#endif

#ifdef USE_STUB_emGetSequenceFromAuxFrame
uint8_t emGetSequenceFromAuxFrame(const uint8_t *auxFrame, uint32_t *sequence)
{
  return false;
}
#endif

#ifdef USE_STUB_emSendMleChildReboot
bool emSendMleChildReboot(void)
{
  return false;
}
#endif

#ifdef USE_STUB_emGetThreadJoin
bool emGetThreadJoin(void)
{
  return true;
}
#endif

#ifdef USE_STUB_emGetThreadNativeCommission
bool emGetThreadNativeCommission(void)
{
  return true;
}
#endif

#ifdef USE_STUB_emNoteExternalCommissioner
void emNoteExternalCommissioner(EmberUdpConnectionHandle handle) {}
#endif
  
#ifdef USE_STUB_emHandleIncomingTcp
void emHandleIncomingTcp(PacketHeader header, Ipv6Header *ipHeader) {}
#endif

#ifdef USE_STUB_emHandleIncomingUdp
void emHandleIncomingUdp(PacketHeader header, Ipv6Header *ipHeader) {}
#endif

#ifdef USE_STUB_emHandleIncomingCoapCommission
void emHandleIncomingCoapCommission(PacketHeader header, Ipv6Header *ipHeader)
{
}
#endif

#ifdef USE_STUB_emIsDuplicateMulticast
bool emIsDuplicateMulticast(Ipv6Header *ipHeader)
{
  return false;
}
#endif

#ifdef USE_STUB_emIncomingJoinMessageHandler
void emIncomingJoinMessageHandler(PacketHeader header,
                                  Ipv6Header *ipHeader,
                                  bool relayed)
{
}
#endif

#ifdef USE_STUB_emStartAppCommissioning
bool emStartAppCommissioning(void)
{
  return false;
}
#endif

#ifdef USE_STUB_emForwardToCommissioner
void emForwardToCommissioner(PacketHeader header, Ipv6Header *ipHeader) {}
#endif

#ifdef USE_STUB_emForwardToJoiner
void emForwardToJoiner(const uint8_t *joinerIid,
                       uint16_t joinerPort,
                       EmberNodeId joinerRouterNodeId,
                       const uint8_t *kek,
                       Buffer payload)
{
}
#endif

#ifdef USE_STUB_emSteeringDataMatch
bool emSteeringDataMatch(uint8_t *data, uint8_t length) { return false; }
#endif

#ifdef USE_STUB_emBecomeCommissionerReturn
void emberBecomeCommissionerReturn(EmberStatus status) {}
#endif

#ifdef USE_STUB_emberCommissionerStatusHandler
void emberCommissionerStatusHandler(uint16_t flags,
                                    const uint8_t *commissionerName,
                                    uint8_t commissionerNameLength)
{
}
#endif

#ifdef USE_STUB_emSendSteeringDataReturn
void emberSendSteeringDataReturn(EmberStatus status) {}
#endif

#ifdef USE_STUB_emIsSleepyChild
bool emIsSleepyChild(EmberNodeId id)
{
  return false;
}
#endif

#ifdef USE_STUB_emMacCommandHandler
void emMacCommandHandler(PacketHeader header) {}
#endif

#ifdef USE_STUB_emMacSubmitIndirect
EmberStatus emMacSubmitIndirect(PacketHeader header)
{
  return 0;
}
#endif

#ifdef USE_STUB_emMacToNetworkQueue
Buffer emMacToNetworkQueue = NULL_BUFFER;
#endif

#ifdef USE_STUB_emMaybeAddChild
uint8_t emMaybeAddChild(EmberNodeId id,
                      const uint8_t *longId,
                      uint8_t capabilities,
                      uint32_t timeoutSeconds)
{
  return 0xFF;
}
#endif

#ifdef USE_STUB_emMulticastContext
uint8_t *emMulticastContext(uint8_t context)
{
  return NULL;
}
#endif

#ifdef USE_STUB_emNoteIdMapping
void emNoteIdMapping(uint8_t context,
                     const uint8_t *longId,
                     EmberNodeId shortId,
                     uint32_t lastTransactionSeconds) {}
#endif

#ifdef USE_STUB_emProcessIncomingBeacon
void emProcessIncomingBeacon(PacketHeader header) {}
#endif

#ifdef USE_STUB_emRetryTransmit
PacketHeader emRetryTransmit(void)
{
  return NULL_BUFFER;
}
#endif

#ifdef USE_STUB_emRplConnectivityHandler
void emRplConnectivityHandler(bool connected) {}
#endif

#ifdef USE_STUB_emSendIcmpErrorMessage
bool emSendIcmpErrorMessage(uint8_t type,
                               uint8_t code,
                               PacketHeader header,
                               Ipv6Header *ipHeader)
{
  return false;
}
#endif

#ifdef USE_STUB_emSendIcmpErrorMessageToDest
bool emSendIcmpErrorMessageToDest(uint8_t type,
                                     uint8_t code,
                                     uint8_t *ipDest,
                                     PacketHeader header,
                                     Ipv6Header *ipHeader)
{
  return false;
}
#endif

#ifdef USE_STUB_emSetThreadJoin
void emSetThreadJoin(bool on) {}
#endif

#ifdef USE_STUB_emUncompressContextPrefix
bool emUncompressContextPrefix(uint8_t context, uint8_t *address)
{
  return false;
}
#endif

#ifdef USE_STUB_emExtraJoinAttempts
uint8_t emExtraJoinAttempts = 0;
#endif

#ifdef USE_STUB_emEventControlSetDelayMS
void emEventControlSetDelayMS(EmberEventControl*event, uint32_t delay) {}
#endif

#ifdef USE_STUB_emIsDefaultGlobalPrefix
bool emIsDefaultGlobalPrefix(const uint8_t *prefix)
{
  return true;
}
#endif

#ifdef USE_STUB_emIsLegacyUla
bool emIsLegacyUla(const uint8_t *prefix)
{ 
  return false;
}
#endif

#ifdef USE_STUB_emIsFe8Address
bool emIsFe8Address(const uint8_t *address)
{
  return true;
}
#endif

#ifdef USE_STUB_emLocalEui64
EmberEui64 emLocalEui64 = {0};
#endif

#ifdef USE_STUB_emberEui64
const EmberEui64 *emberEui64(void)
{
  return &emLocalEui64;
}
#endif

#ifdef USE_STUB_emMacShortSource
EmberNodeId emMacShortSource(PacketHeader header)
{
  return EMBER_NULL_NODE_ID;
}
#endif

#ifdef USE_STUB_emMacSourceMode
uint16_t emMacSourceMode(PacketHeader header)
{
  return 0;
}
#endif

#ifdef USE_STUB_emMacSourcePointer
uint8_t *emMacSourcePointer(PacketHeader header)
{
  return NULL;
}
#endif

#ifdef USE_STUB_emMleTlvPresent
bool emMleTlvPresent(const MleMessage *message, uint8_t tlv)
{
  return false;
}
#endif

#ifdef USE_STUB_emReallyEndLogLine
void emReallyEndLogLine(uint8_t logType) {}
#endif

#ifdef USE_STUB_emReallyLog
void emReallyLog(uint8_t logType, PGM_P formatString, ...) {}
#endif

#ifdef USE_STUB_emReallyLogBytes
void emReallyLogBytes(uint8_t logType,
                      bool lineStartAndEnd,
                      bool useHex,
                      PGM_P formatString,
                      uint8_t const *bytes,
                      uint16_t length,
                      ...)
{
}
#endif

#ifdef USE_STUB_emReallyStartLogLine
void emReallyStartLogLine(uint8_t logType) {}
#endif

#ifdef USE_STUB_halCommonGetRandomTraced
uint16_t halCommonGetRandomTraced(char *file, int line)
{
  return 0;
}
#endif

#ifdef USE_STUB_emChildUsingOldKey
bool emChildUsingOldKey(const uint8_t *longInterface)
{
  return false;
}
#endif

#ifdef USE_STUB_emNeighborUsingOldKey
bool emNeighborUsingOldKey(const uint8_t *longInterface)
{
  return false;
}
#endif

#ifdef USE_STUB_emberSerialWriteString
EmberStatus emberSerialWriteString(uint8_t port, PGM_P string)
{
  return EMBER_SUCCESS;
}
#endif

#ifdef USE_STUB_emGetLeaderEui64
const uint8_t *emGetLeaderEui64(void)
{
  return NULL;
}
#endif

#ifdef USE_STUB_emContentType
uint8_t emContentType = 0;
#endif

#ifdef USE_STUB_emGetIntValueFromUriData
uint16_t emGetIntValueFromUriData(const UriData *uriData,
                                const uint8_t *key,
                                uint8_t defaultValue)
{
  return 0;
}
#endif

#ifdef USE_STUB_emRssiToCost
uint8_t emRssiToCost(int8_t rssi) {
  return 0;
}
#endif

#ifdef USE_STUB_emRadioIdleMicroForUs
void emRadioIdleMicroForUs(uint32_t delayUs) {}
#endif

#ifdef USE_STUB_emberAbortWakeupReturn
void emberAbortWakeupReturn(EmberStatus status) {}
#endif

#ifdef USE_STUB_emMarkWakeupBuffers
void emMarkWakeupBuffers(void) {}
#endif

#ifdef USE_STUB_emberChildId
EmberNodeId emberChildId(uint8_t index)
{
  return EMBER_NULL_NODE_ID;
}
#endif

#ifdef USE_STUB_emberChildIndex
uint8_t emberChildIndex(EmberNodeId childId)
{
  return 0xFF;
}
#endif

#ifdef USE_STUB_emberChildLongIndex
uint8_t emberChildLongIndex(uint8_t *childLongId)
{
  return 0xFF;
}
#endif

#ifdef USE_STUB_emberEnergyScanHandler
void emberEnergyScanHandler(uint8_t channel, int8_t rssi) {}
#endif

#ifdef USE_STUB_emberGetCcaThresholdReturn
void emberGetCcaThresholdReturn(int8_t threshold) {}
#endif

#ifdef USE_STUB_emberGetNetworkKeyInfoReturn
void emberGetNetworkKeyInfoReturn(EmberStatus status,
                                  uint32_t sequence,
                                  uint8_t state)
{
}
#endif

#ifdef USE_STUB_emberGetRadioPowerReturn
void emberGetRadioPowerReturn(int8_t power) {}
#endif

#ifdef USE_STUB_emberGetRipEntryReturn
void emberGetRipEntryReturn(uint8_t index, const EmberRipEntry *entry) {}
#endif

#ifdef USE_STUB_emberGetTxPowerModeReturn
void emberGetTxPowerModeReturn(uint16_t txPowerMode) {}
#endif

#ifdef USE_STUB_emberListenForWakeupReturn
void emberListenForWakeupReturn(EmberStatus status) {}
#endif

#ifdef USE_STUB_emberRadioNeedsCalibratingHandler
void emberRadioNeedsCalibratingHandler(void) {}
#endif

#ifdef USE_STUB_emberResetMicroHandler
void emberResetMicroHandler(EmberResetCause cause)
{
}
#endif

#ifdef USE_STUB_emberScanReturn
void emberScanReturn(EmberStatus status) {}
#endif

#ifdef USE_STUB_emberSetCcaThresholdReturn
void emberSetCcaThresholdReturn(EmberStatus status) {}
#endif

#ifdef USE_STUB_emberSetNextNetworkKeyReturn
void emberSetNextNetworkKeyReturn(EmberStatus status) {}
#endif

#ifdef USE_STUB_emberSetRadioPowerReturn
void emberSetRadioPowerReturn(EmberStatus status) {}
#endif

#ifdef USE_STUB_emberSetSecurityParametersReturn
void emberSetSecurityParametersReturn(EmberStatus status) {}
#endif

#ifdef USE_STUB_emberSprintfIpAddress
void emberSprintfIpAddress(const uint8_t *ipAddress, uint8_t *to, uint16_t toSize)
{
}
#endif

#ifdef USE_STUB_emberStartWakeupReturn
void emberStartWakeupReturn(EmberStatus status) {}
#endif

#ifdef USE_STUB_emberSwitchToNextNetworkKeyReturn
void emberSwitchToNextNetworkKeyReturn(EmberStatus status) {}
#endif

#ifdef USE_STUB_emberWakeupHandler
void emberWakeupHandler(EmberWakeupReason reason,
                        EmberWakeupState state,
                        uint16_t remainingMs,
                        uint8_t dataByte,
                        uint16_t otaSequence)
{
}
#endif

#ifdef USE_STUB_emberSleepNoTimerReturn
void emApiSleepNoTimerReturn(void)
{
}
#endif

#ifdef USE_STUB_emberWakeupStateReturn
void emApiWakeupStateReturn(uint8_t wakeupState,
                            uint16_t wakeupSequenceNumber)
{
}
#endif

#ifdef USE_STUB_emberSetWakeupSequenceNumberReturn
void emberSetWakeupSequenceNumberReturn(void) {}
#endif

#ifdef USE_STUB_halSleep
void halSleep(SleepModes sleepMode)
{
}
#endif

#ifdef USE_STUB_halSleepForMilliseconds
EmberStatus halSleepForMilliseconds(uint32_t *duration)
{
  return EMBER_SUCCESS;
}
#endif

#ifdef USE_STUB_handleRequest
void handleRequest(UriData *data)
{
}
#endif

#ifdef USE_STUB_simulatedTimePasses
void simulatedTimePasses(void) {}
#endif

#ifdef USE_STUB_simulatedTimePassesUs
void simulatedTimePassesUs(uint32_t delayUs) {}
#endif

#ifdef USE_STUB_simulatedTimePassesMs
void simulatedTimePassesMs(uint32_t delayMs) {}
#endif

#ifdef USE_STUB_simulatorId
uint16_t simulatorId = 0;
#endif

#ifdef USE_STUB_emAddUnicastTableEntry
void emAddUnicastTableEntry(EmberNodeId source, uint8_t sequence) {}
#endif

#ifdef USE_STUB_emAddUdpConnection
UdpConnectionData *emAddUdpConnection
  (const uint8_t *remoteAddress,
   uint16_t localPort,
   uint16_t remotePort,
   uint16_t flags,
   uint16_t recordSize,
   EmberUdpConnectionStatusHandler statusHandler,
   EmberUdpConnectionReadHandler readHandler)
{
  return NULL;
}
#endif

#ifdef USE_STUB_emFindConnection
UdpConnectionData *emFindConnection(const uint8_t *remoteAddress,
                                    uint16_t localPort,
                                    uint16_t remotePort)
{
  return NULL;
}
#endif

#ifdef USE_STUB_emLookupHandle
void *emLookupHandle(uint8_t handle, bool remove)
{
  return NULL;
}
#endif

#ifdef USE_STUB_emUdpSendDtlsRecord
void emUdpSendDtlsRecord(EmberUdpConnectionHandle handle, Buffer tlsRecord) {}
#endif

#ifdef USE_STUB_emAdvanceFragmentPointer
bool emAdvanceFragmentPointer(PacketHeader packet)
{
  return false;
}
#endif

#ifdef USE_STUB_emChildIsFrameCounterValid
bool emChildIsFrameCounterValid(uint32_t frameCounter,
                                   const uint8_t *longId,
                                   uint32_t sequenceNumber)
{
  return false;
}
#endif

#ifdef USE_STUB_emConvert6lowpanToInternal
bool emConvert6lowpanToInternal(PacketHeader header,
                                   uint8_t *headerLength)
{
  return true;
}
#endif

#ifdef USE_STUB_MeshHeader
typedef struct {
  uint8_t hopLimit;
  EmberNodeId source;
  EmberNodeId destination;
} MeshHeader;
#endif

#ifdef USE_STUB_emReadMeshHeader
bool emReadMeshHeader(PacketHeader header, MeshHeader *mesh) { 
  return false;
}
#endif

#ifdef USE_STUB_emWriteMeshHeader
void emWriteMeshHeader(PacketHeader header, MeshHeader *mesh) { }
#endif

#ifdef USE_STUB_emIsInUnicastTable
bool emIsInUnicastTable(EmberNodeId source, uint8_t sequence)
{
  return false;
}
#endif

#ifdef USE_STUB_emMacMicLength
uint8_t emMacMicLength(void)
{
  return 4;
}
#endif

#ifdef USE_STUB_emMoreFragments
bool emMoreFragments(PacketHeader packet)
{
  return false;
}
#endif

#ifdef USE_STUB_emNeighborIsFrameCounterValid
bool emNeighborIsFrameCounterValid(uint32_t frameCounter,
                                      const uint8_t *longId,
                                      bool updateFrameCounter)
{
  return false;
}
#endif

#ifdef USE_STUB_emPhyToMacQueueItemAddedCallback
void emPhyToMacQueueItemAddedCallback(void) {}
#endif

#ifdef USE_STUB_emScanReturn
void emScanReturn(EmberStatus status, uint8_t scanType, uint32_t txFailureMask)
{
  assert(false);
}
#endif

#ifdef USE_STUB_emWakeupKeepIncomingPacketIsr
bool emWakeupKeepIncomingPacketIsr(uint8_t *rawPacket)
{ 
  return true;
}
#endif

#ifdef USE_STUB_emMacTransmitCompleteCallback
void emMacTransmitCompleteCallback(PacketHeader header, EmberStatus status)
{
}
#endif

#ifdef USE_STUB_emMakeNextFragment
uint8_t emMakeNextFragment(PacketHeader header,
                         uint8_t *txBuffer,
                         uint8_t maxLength,
                         uint8_t *destinationLoc)
{
  return 0;
}
#endif

#ifdef USE_STUB_emMakeStackJitMessage
PacketHeader emMakeStackJitMessage(void)
{
  return EMBER_NULL_MESSAGE_BUFFER;
}
#endif

#ifdef USE_STUB_emProcessWakeupMessage
bool emProcessWakeupMessage(PacketHeader header)
{
  return false;
}
#endif

#ifdef USE_STUB_emSetAllSleepyChildFlags
bool emSetAllSleepyChildFlags(uint16_t mask, bool set)
{
  return false;
}
#endif

#ifdef USE_STUB_emWakeupMessageFrameCounter
uint32_t emWakeupMessageFrameCounter(void)
{
  return 0;
}
#endif

#ifdef USE_STUB_emGetFdConnection
void *emGetFdConnection(uint8_t fd)
{
  return NULL;
}
#endif

#ifdef USE_STUB_emPaaAddressNotification
void emPaaAddressNotification(uint32_t sessionId, uint8_t const *newAddress) {}
#endif

#ifdef USE_STUB_emRemoveConnection
void emRemoveConnection(void *connection) {}
#endif

#ifdef USE_STUB_emStartConnectionTimerMs
void emStartConnectionTimerMs(uint16_t delayMs) {}
#endif

#ifdef USE_STUB_emStartConnectionTimerQs
void emStartConnectionTimerQs(void *connection, uint16_t delayQs) {}
#endif

#ifdef USE_STUB_emberTcpClose
EmberStatus emberTcpClose(uint8_t fd)
{
  return EMBER_ERR_FATAL;
}
#endif

#ifdef USE_STUB_emberTcpRemoteIpAddress
Ipv6Address *emberTcpRemoteIpAddress(uint8_t fd)
{
  return NULL;
}
#endif

#ifdef USE_STUB_emberPollForDataReturn
void emberPollForDataReturn(EmberStatus status, bool dataPending) {}
#endif

#ifdef USE_STUB_emLocalIslandId
uint8_t dummyIslandId[ISLAND_ID_SIZE] = {0};
uint8_t *emLocalIslandId(void)
{
  return dummyIslandId;
}
#endif

#ifdef USE_STUB_emSetIslandId
void emSetIslandId(const uint8_t *data)
{
}
#endif

#ifdef USE_STUB_dataHandler
void dataHandler(const uint8_t *packet, uint16_t length) {}
#endif

#ifdef USE_STUB_emMakeIpHeader
PacketHeader emMakeIpHeader(Ipv6Header *ipHeader,
                            uint8_t tag,
                            uint8_t options,
                            const uint8_t *destination,
                            uint8_t hopLimit,
                            uint16_t payloadBufferLength)
{
  return NULL_BUFFER;
}
#endif

#ifdef USE_STUB_emPosixRead
ssize_t emPosixRead(int fd, void *buf, size_t count)
{
  return 0;
}
#endif

#ifdef USE_STUB_emPosixWrite
ssize_t emPosixWrite(int fd, const void *buf, size_t count)
{
  return 0;
}
#endif

#ifdef USE_STUB_emberConfigureDefaultHostAddress
void emberConfigureDefaultHostAddress(const EmberIpv6Address *mlAddress)
{
}
#endif

#ifdef USE_STUB_emberConfigureGlobalHostAddress
void emberConfigureGlobalHostAddress(const EmberIpv6Address *address)
{
}
#endif

#ifdef USE_STUB_emberAssertInfoReturn
void emberAssertInfoReturn(const uint8_t *fileName, uint32_t lineNumber) {}
#endif

#ifdef USE_STUB_emberConfigUartReturn
void emberConfigUartReturn(void) {}
#endif

#ifdef USE_STUB_emberConfigureGatewayReturn
void emberConfigureGatewayReturn(EmberStatus status) {}
#endif

#ifdef USE_STUB_emberEchoReturn
void emberEchoReturn(const uint8_t *data, uint8_t length) {}
#endif

#ifdef USE_STUB_emberGetChannelCalDataTokenReturn
void emberGetChannelCalDataTokenReturn(uint8_t lna,
                                       int8_t tempAtLna,
                                       uint8_t modDac,
                                       int8_t tempAtModDac)
{
}
#endif

#ifdef USE_STUB_emberGetCounterReturn
void emberGetCounterReturn(EmberCounterType type, uint16_t value) { }
#endif

#ifdef USE_STUB_emberGetMulticastEntryReturn
void emberGetMulticastEntryReturn(uint8_t lastSequence,
                                  uint8_t windowBitmask,
                                  uint8_t dwellQs,
                                  const uint8_t *seed)
{
}
#endif

#ifdef USE_STUB_emberGetVersionsReturn
void emberGetVersionsReturn(const uint8_t *versionName,
                            uint16_t binaryManagementVersionNumber,
                            uint16_t stackVersionNumber,
                            uint16_t stackBuildNumber,
                            EmberVersionType versionType,
                            const uint8_t *buildTimestamp)
{
}
#endif

#ifdef USE_STUB_emberIncomingIcmpHandler
void emberIncomingIcmpHandler(Ipv6Header *ipHeader) {}
#endif

#ifdef USE_STUB_emberNcpUdpStormReturn
void emberNcpUdpStormReturn(EmberStatus status) {}
#endif

#ifdef USE_STUB_emberNcpUdpStormCompleteHandler
void emberNcpUdpStormCompleteHandler(void) {}
#endif

#ifdef USE_STUB_emberResetNcpAshReturn
void emberResetNcpAshReturn(void) {}
#endif

#ifdef USE_STUB_emberResetNetworkStateReturn
void emberResetNetworkStateReturn(EmberStatus status) {}
#endif

#ifdef USE_STUB_emberSerialReadByte
EmberStatus emberSerialReadByte(uint8_t port, uint8_t *dataByte)
{
  return EMBER_SUCCESS;
}
#endif

#ifdef USE_STUB_sendDebugMessage
void sendDebugMessage(void) {}
#endif

#ifdef USE_STUB_emberSetTxPowerModeReturn
void emberSetTxPowerModeReturn(EmberStatus status) {}
#endif

#ifdef USE_STUB_emberStartUartStormReturn
void emberStartUartStormReturn(void) {}
#endif

#ifdef USE_STUB_emberStopUartStormReturn
void emberStopUartStormReturn(void) {}
#endif

#ifdef USE_STUB_logEnabled
bool logEnabled = true;
#endif

#ifdef USE_STUB_managementHandler
void managementHandler(SerialLinkMessageType type,
                       const uint8_t *message,
                       uint16_t length)
{
}
#endif

#ifdef USE_STUB_emFindNearestGateway
EmberNodeId emFindNearestGateway(const uint8_t *ipDestination,
                                 const uint8_t *ipSource)
{
  return EMBER_NULL_NODE_ID;
}
#endif

#ifdef USE_STUB_emFindNeighborByIpAddress
EmNextHopType emFindNeighborByIpAddress(const uint8_t *address, uint16_t *nextHop)
{
  return EM_NO_NEXT_HOP;
}
#endif

#ifdef USE_STUB_emFindNeighborByLongId
uint16_t emFindNeighborByLongId(const uint8_t *longId)
{
  return 0xFFFE;
}
#endif

#ifdef USE_STUB_emPrepareRetriedMleMessage
PacketHeader emPrepareRetriedMleMessage(PacketHeader header)
{
  return NULL_BUFFER;
}
#endif

#ifdef USE_STUB_emProcessDuplicateAddressMessage
void emProcessDuplicateAddressMessage(PacketHeader header,
                                      Ipv6Header *ipHeader)
{
}
#endif

#ifdef USE_STUB_emRipLookupNextHop
uint16_t emRipLookupNextHop(uint16_t shortMacId) { return EMBER_NULL_NODE_ID; }
#endif

#ifdef USE_STUB_emSyncNeighbor
void emSyncNeighbor(const uint8_t *destination) {}
#endif

#ifdef USE_STUB_emExpandSequenceNumber
uint32_t emExpandSequenceNumber(uint8_t littleSequence)
{
  return 0;
}
#endif

#ifdef USE_STUB_emThreadEncryptionEnabled
bool emThreadEncryptionEnabled = false;
#endif

#ifdef USE_STUB_halInternalStartSymbolTimer
void halInternalStartSymbolTimer(void) {}
#endif

#ifdef USE_STUB_emButtonTick
void emButtonTick(void) {}
#endif

#ifdef USE_STUB_emNetworkFragmentationEnabled
bool emNetworkFragmentationEnabled = false;
#endif

#ifdef USE_STUB_emAddLurkerNeighbor
uint8_t emAddLurkerNeighbor(const uint8_t *longId)
{
  return 0xFF;
}
#endif

#ifdef USE_STUB_emRipLinkCost
uint8_t emRipLinkCost(uint8_t ripId)
{
  return 0;
}
#endif

#ifdef USE_STUB_emSendDhcpRouterIdRelease
bool emSendDhcpRouterIdRelease(uint16_t shortId, const uint8_t *longId)
{
  return false;
}
#endif

#ifdef USE_STUB_emberConfigureGateway
void emberConfigureGateway(uint8_t borderRouterFlags,
                           const uint8_t *prefix,
                           const uint8_t prefixLength,
                           uint8_t domainId,
                           uint32_t preferredLifetime,
                           uint32_t validLifetime)
{
}
#endif

#ifdef USE_STUB_emGetBinaryCommandIdentifierString
const char *emGetBinaryCommandIdentifierString(uint16_t identifier)
{
  return NULL;
}
#endif

#ifdef USE_STUB_emIslandSequenceNumberUpdated
void emIslandSequenceNumberUpdated(void) {}
#endif

#ifdef USE_STUB_emMacDisassociationNotificationRespond
EmberStatus emMacDisassociationNotificationRespond(const uint8_t *longId,
                                                   EmberNodeId shortAddr)
{
  return 0;
}
#endif

#ifdef USE_STUB_emLookupChildEui64ById
uint8_t *emLookupChildEui64ById(EmberNodeId id)
{
  return NULL;
}
#endif

#ifdef USE_STUB_emNextNwkFrameCounter
uint32_t emNextNwkFrameCounter = 0;
#endif

#ifdef USE_STUB_emSetNwkFrameCounter
void emSetNwkFrameCounter(uint32_t newFrameCounter) {}
#endif

#ifdef USE_STUB_emPrintRipTable
void emPrintRipTable(uint8_t port) {}
#endif

#ifdef USE_STUB_emberDeepSleep
void emberDeepSleep(bool sleep) { }
#endif

#ifdef USE_STUB_emberDeepSleepTick
bool emberDeepSleepTick(void) { return false; }
#endif

#ifdef USE_STUB_emberDeepSleepReturn
void emberDeepSleepReturn(EmberStatus status) {}
#endif

#ifdef USE_STUB_emberDeepSleepCallback
void emberDeepSleepCallback(uint16_t sleepDuration) {}
#endif

#ifdef USE_STUB_emberStackPollForDataReturn
void emberStackPollForDataReturn(EmberStatus status) {}
#endif

#ifdef USE_STUB_emberOkToNapReturn
void emberOkToNapReturn(uint8_t stateMask) {}
#endif

#ifdef USE_STUB_emberSendDoneReturn
void emberSendDoneReturn(void) {}
#endif

#ifdef USE_STUB_emProcessMleParentResponse
void emProcessMleParentResponse(PacketHeader header, MleMessage *message) {}
#endif

#ifdef USE_STUB_emClearAddressCache
void emClearAddressCache(void) {}
#endif

#ifdef USE_STUB_emClearAddressCacheId
void emClearAddressCacheId(EmberNodeId id) {}
#endif

#ifdef USE_STUB_emCurrentIpSource
uint8_t *emCurrentIpSource;
#endif

#ifdef USE_STUB_emCurrentMeshSource
EmberNodeId emCurrentMeshSource = 0xFFFE;
#endif

#ifdef USE_STUB_emMacDecrypt
bool emMacDecrypt(uint8_t *packet,
                     uint8_t *lengthLoc,
                     uint8_t *sourceEui64,
                     bool combineKeyWithNetworkFragmentIdentifier)
{
  return false;
}
#endif

#ifdef USE_STUB_printBytes
void printBytes(const uint8_t *bytes, uint16_t length) {}
#endif

#ifdef USE_STUB_emberSerialPrintBytes
EmberStatus emberSerialPrintBytes(uint8_t port,
                                  PGM_P prefix,
                                  uint8_t *bytes,
                                  uint16_t count)
{
  return EMBER_SUCCESS;
}
#endif

#ifdef USE_STUB_emRouterIndex
uint8_t emRouterIndex(uint8_t ripId)
{
  return 0xFF;
}
#endif

#ifdef USE_STUB_emRouterOrLurkerIndexByLongId
uint8_t emRouterOrLurkerIndexByLongId(const uint8_t *longId, bool isLurker)
{
  return 0xFF;
}
#endif

#ifdef USE_STUB_emIncrementSequenceNumber
void emIncrementSequenceNumber(void) {}
#endif

#ifdef USE_STUB_emEndDeviceRebootSuccess
void emEndDeviceRebootSuccess(void) {}
#endif

#ifdef USE_STUB_emEndDeviceRebootFailed
void emEndDeviceRebootFailed(void) {}
#endif

#ifdef USE_STUB_emStartNewIsland
void emStartNewIsland(void) {}
#endif

#ifdef USE_STUB_printIpAddress
void printIpAddress(uint8_t *address) {}
#endif

#ifdef USE_STUB_emberUartSpeedTestReturn
void emberUartSpeedTestReturn(uint32_t totalBytesSent,
                              uint32_t bytesSent,
                              uint32_t testLengthMs)
{
}
#endif

#ifdef USE_STUB_emStopPolling
void emStopPolling(void) {}
#endif

#ifdef USE_STUB_emLostParent
void emLostParent(void) {}
#endif

#ifdef USE_STUB_emParentLongId
uint8_t emParentLongId[8];
#endif

#ifdef USE_STUB_emParentCostToLeader
uint8_t emParentCostToLeader = 0xFF;
#endif

#ifdef USE_STUB_emFillIslandId
void emFillIslandId(uint8_t *islandId, uint8_t weight)
{
}
#endif

#ifdef USE_STUB_emIsIslandIdBetter
bool emIsIslandIdBetter(const uint8_t *islandId)
{
  return false;
}
#endif

#ifdef USE_STUB_emIsIslandIdBetterOrEqual
bool emIsIslandIdBetterOrEqual(const uint8_t *islandId)
{
  return false;
}
#endif

#ifdef USE_STUB_emIsIslandBetter
bool emIsIslandBetter(const uint8_t *islandId, uint8_t sequence)
{
  return false;
}
#endif

#ifdef USE_STUB_emJoinBetterIsland
void emJoinBetterIsland(void) {}
#endif

#ifdef USE_STUB_emForceParentId
uint16_t emForceParentId = 0xFFFF;
#endif

#ifdef USE_STUB_emForceParentLongId
uint8_t emForceParentLongId[8];
#endif

#ifdef USE_STUB_emSendNetworkFragmentMleAdvertisement
void emSendNetworkFragmentMleAdvertisement(bool retry) {}
#endif

#ifdef USE_STUB_emResetChildsTimer
void emResetChildsTimer(uint8_t index) {}
#endif

#ifdef USE_STUB_emberResetSerialState
void emberResetSerialState(void) {}
#endif

#ifdef USE_STUB_emIsLocalIpAddress
bool emIsLocalIpAddress(const uint8_t *address)
{
  return false;
}
#endif

#ifdef USE_STUB_emStoreDefaultGlobalPrefix
bool emStoreDefaultGlobalPrefix(uint8_t *target)
{
  return true;
}
#endif

#ifdef USE_STUB_emEccSign
bool emEccSign(const uint8_t *hash,
                  const EccPrivateKey *key,
                  uint8_t *r,
                  uint8_t *s)
{
  return false;
}
#endif

#ifdef USE_STUB_emMleInit
void emMleInit(void) {}
#endif

#ifdef USE_STUB_emGenerateEcdh
bool emGenerateEcdh(uint8_t **publicKeyLoc, uint8_t *secret)
{
  return false;
}
#endif

#ifdef USE_STUB_emEccVerify
bool emEccVerify(const uint8_t *hash,
                    EccPublicKey *key,
                    const uint8_t *r,
                    uint16_t rLength,
                    const uint8_t *s,
                    uint16_t sLength)
{
  return false;
}
#endif

#ifdef USE_STUB_emEcdhSharedSecret
bool emEcdhSharedSecret(const EccPublicKey *remotePublicKey,
                           const uint8_t *localSecret,
                           uint8_t *sharedSecret)
{
  return false;
}
#endif

#ifdef USE_STUB_emStoreLongFe8Address
void emStoreLongFe8Address(uint8_t *eui64, uint8_t *target) {}
#endif

#ifdef USE_STUB_emberRunCommandInterpreter
bool emberRunCommandInterpreter(bool commandIsBinary,
                                   EmberCommandState *state,
                                   EmberCommandEntry *commandTable,
                                   EmberCommandErrorHandler *errorHandler,
                                   const uint8_t *input,
                                   uint16_t length)
{
  return true;
}
#endif

#ifdef USE_STUB_driverNcpFd
uint8_t driverNcpFd;
#endif

#ifdef USE_STUB_ipModemLinkMarkBuffers
void ipModemLinkMarkBuffers(void) {}
#endif

#ifdef USE_STUB_emProcessNcpManagementCommand
void emProcessNcpManagementCommand(SerialLinkMessageType type,
                                   const uint8_t *message,
                                   uint16_t length)
{
}
#endif

#ifdef USE_STUB_emGetShortAddress
bool emGetShortAddress(uint8_t context,
                          const uint8_t *iid,
                          EmberNodeId *resultLoc,
                          bool discover)
{
  return false;
}
#endif

#ifdef USE_STUB_emSetSequenceNumber
void emSetSequenceNumber(uint32_t amount) {}
#endif

#ifdef USE_STUB_emberActiveWakeupReturn
void emberActiveWakeupReturn(EmberStatus status) {}
#endif

#ifdef USE_STUB_emMaybeClearRoute
void emMaybeClearRoute(uint16_t destId, uint16_t nextHop) {}
#endif

#ifdef USE_STUB_emJitTransmitComplete
bool emJitTransmitComplete(Buffer header, EmberStatus status)
{
  return false;
}
#endif

#ifdef USE_STUB_emMacRetries
uint8_t emMacRetries;
#endif

#ifdef USE_STUB_txBufferFullHandler
void txBufferFullHandler(const uint8_t *packet,
                         uint16_t packetLength,
                         uint16_t written) {}
#endif

#ifdef USE_STUB_txFailedHandler
void txFailedHandler(uint8_t fd,
                     const uint8_t *packet,
                     uint16_t packetLength,
                     uint16_t written) {}
#endif

#ifdef USE_STUB_emNoteSuccessfulPoll
void emNoteSuccessfulPoll(void) {}
#endif

#ifdef USE_STUB_emBecomeLeader
void emBecomeLeader(void) {}
#endif

#ifdef USE_STUB_emRetryTransmitComplete
bool emRetryTransmitComplete(PacketHeader header, EmberStatus status)
{
  return false;
}
#endif

#ifdef USE_STUB_emUdpJoinPort
uint16_t emUdpJoinPort;
#endif

#ifdef USE_STUB_emUdpCommissionPort
uint16_t emUdpCommissionPort;
#endif

#ifdef USE_STUB_emUdpTransmitComplete
void emUdpTransmitComplete(PacketHeader header, EmberStatus status)
{
}
#endif

#ifdef USE_STUB_emFindNeighborIndexByLongId
uint8_t emFindNeighborIndexByLongId(const uint8_t *longId)
{
  return 0xFF;
}
#endif

#ifdef USE_STUB_emClearNeighbor
void emClearNeighbor(uint16_t shortId) {}
#endif

#ifdef USE_STUB_emRadioSetIdleMode
EmberStatus emRadioSetIdleMode(uint8_t mode)
{
 return EMBER_SUCCESS;
}
#endif

#ifdef USE_STUB_emUseAddressManagement
bool emUseAddressManagement = true;
#endif

#ifdef USE_STUB_emIsAddressManagementPost
bool emIsAddressManagementPost(Ipv6Header *ipHeader)
{
  return false;
}
#endif

#ifdef USE_STUB_emSendAddressQuery
EmberStatus emSendAddressQuery(const EmberIpv6Address *address)
{
  return EMBER_ERR_FATAL;
}
#endif

#ifdef USE_STUB_emSendAddressSolicitOrRelease
EmberStatus emSendAddressSolicitOrRelease(bool isSolicit)
{
  return EMBER_ERR_FATAL;
}
#endif

#ifdef USE_STUB_emSendCommissioningData
void emSendCommissioningData(uint8_t *eui64, uint8_t *key) {}
#endif

#ifdef USE_STUB_emProcessCommissioningData
void emProcessCommissioningData(PacketHeader header, Ipv6Header *ipHeader) {}
#endif

#ifdef USE_STUB_emProcessCommissioningMessage
void emProcessCommissioningMessage(PacketHeader header) {}
#endif

#ifdef USE_STUB_emSetCommissioningMacKey
void emSetCommissioningMacKey(uint8_t *key) {}
#endif

#ifdef USE_STUB_emCommissioningHandshakeComplete
void emCommissioningHandshakeComplete(void) {}
#endif

#ifdef USE_STUB_emCommissionInit
void emCommissionInit(void) {}
#endif

#ifdef USE_STUB_emSetCommissionState
void emSetCommissionState(const uint8_t *state, uint8_t stateLength) {}
#endif

#ifdef USE_STUB_emSetLinkLocalIdentifier
void emSetLinkLocalIdentifier(const uint8_t *extendedId) {}
#endif

#ifdef USE_STUB_emCoapCommissionMessageHandler
void emCoapCommissionMessageHandler(CoapMessage *coap) {}
#endif

#ifdef USE_STUB_makeMessage
Parcel *makeMessage(char *format, ...)
{
  return NULL;
}
#endif

#ifdef USE_STUB_emCoapMarkBuffers
void emCoapMarkBuffers(void) {}
#endif

#ifdef USE_STUB_emCommissioner
Buffer emCommissioner;
#endif

#ifdef USE_STUB_emResetRipTable
void emResetRipTable(void) {}
#endif

#ifdef USE_STUB_emStoreGp16
bool emStoreGp16(uint16_t id, uint8_t *target)
{
  return false;
}
#endif

#ifdef USE_STUB_setRandomDataType
void setRandomDataType(randomDataType dataType) {}
#endif

#ifdef USE_STUB_CoapMessage
typedef struct {} CoapMessage;
#endif

#ifdef USE_STUB_emSubmitCoapMessage
EmberStatus emSubmitCoapMessage(CoapMessage *message,
                                const uint8_t *uri,
                                Buffer payloadBuffer,
                                EmberCoapOptions options)
{
  return EMBER_ERR_FATAL;
}
#endif

#ifdef USE_STUB_emInitializeStackCoapMessage
void emInitializeStackCoapMessage(CoapMessage *message,
                                  uint8_t *payload,
                                  uint16_t payloadLength)
{
}
#endif

#ifdef USE_STUB_emSequenceNumberIsNewer
bool emSequenceNumberIsNewer(uint32_t sequenceNumber) { return false; }
#endif

#ifdef USE_STUB_emJpakeEccGetCkxaOrSkxbData
bool emJpakeEccGetCkxaOrSkxbData(uint8_t *ckxaOrSkxb,
                                    uint16_t ckxaOrSkxbLength,
                                    const uint8_t *sharedSecret,
                                    const uint8_t sharedSecretLength,
                                    bool isClient)
{
  return false;
}
#endif

#ifdef USE_STUB_emJpakeEccVerifyCkxaOrSkxbData
bool emJpakeEccVerifyCkxaOrSkxbData(const uint8_t *ckxaOrSkxb,
                                       const uint8_t *sharedSecret,
                                       const uint8_t sharedSecretLength,
                                       bool originatorIsClient)
{
  return false;
}
#endif

#ifdef USE_STUB_emJPakeEccStart
void emJPakeEccStart(void) {}
#endif

#ifdef USE_STUB_emMacKickStart
void emMacKickStart(void) {}
#endif

#ifdef USE_STUB_emVectorSearch
void *emVectorSearch(Vector *vector,
                     EqualityPredicate predicate,
                     const void *target,
                     uint16_t *indexLoc)
{
  return NULL;
}
#endif

#ifdef USE_STUB_emVectorMatchCount
uint16_t emVectorMatchCount(Vector *vector,
                          EqualityPredicate predicate,
                          const void *target)
{
  return 0;
}
#endif

#ifdef USE_STUB_emVectorAdd
void *emVectorAdd(Vector *vector, uint16_t quanta)
{
  return NULL;
}
#endif

#ifdef USE_STUB_emJPakeEccFinish
bool emJPakeEccFinish(uint8_t *ms,
                         const uint8_t *sharedSecret,
                         const uint8_t sharedSecretLength,
                         bool isClient)
{
  return false;
}
#endif

#ifdef USE_STUB_emJpakeEccGetHxaHxbData
bool emJpakeEccGetHxaHxbData(uint8_t *hxa,
                                uint16_t hxaLength,
                                uint8_t *hxb,
                                uint16_t hxbLength,
                                bool isClient)
{
  return true;
}
#endif

#ifdef USE_STUB_emberCoapDecodeUri
bool emberCoapDecodeUri(const uint8_t *rawUri,
                        uint16_t rawUriLength,
                        uint8_t *convertedUri,
                        uint16_t convertedUriLength)
{
  return false;
}
#endif

#ifdef USE_STUB_emCoapIncomingMessageHandler
void emCoapIncomingMessageHandler(PacketHeader header,
                                  Ipv6Header *ipHeader,
                                  uint8_t dtlsHandle,
                                  uint16_t options,
                                  void *handler)
{
}
#endif

#ifdef USE_STUB_emCoapMessageHandler
bool emCoapMessageHandler(CoapMessage *coap) { return false; }
#endif

#ifdef USE_STUB_emberCoapRespond
bool emberCoapRespond(EmberCoapCode responseCode,
                         const uint8_t *payload,
                         uint16_t payloadLength)
{
  return false;
}
#endif

#ifdef USE_STUB_emSetChildAddresses
bool emSetChildAddresses(uint8_t index,
                            const uint8_t *data,
                            uint8_t dataLength)
{
  return false;
}
#endif

#ifdef USE_STUB_emJpakeEccVerifyHxaHxbData
bool emJpakeEccVerifyHxaHxbData(const uint8_t *hxa,
                                   const uint8_t *hxb,
                                   bool originatorIsClient)
{
  return false;
}
#endif

#ifdef USE_STUB_emMarkUdpBuffers
void emMarkUdpBuffers(void) {}
#endif

#ifdef USE_STUB_emDeriveKey
bool emDeriveKey(uint32_t sequence,
                    const uint8_t *masterKey,
                    uint8_t *derivedKey,
                    uint8_t *legacyKey)
{
  return true;
}
#endif

#ifdef USE_STUB_emStoreLegacyUla
void emStoreLegacyUla(uint8_t *target) {}
#endif

#ifdef USE_STUB_emSetLegacyUla
void emSetLegacyUla(const uint8_t *prefix) {}
#endif

#ifdef USE_STUB_emCallMarkFunctions
void emCallMarkFunctions(void) {}
#endif

#ifdef USE_STUB_emStoreIpSourceAddress
bool emStoreIpSourceAddress(uint8_t *source, const uint8_t *destination)
{
  emberGetLocalIpAddress(0, (EmberIpv6Address *)source);
  return true;
}
#endif

#ifdef USE_STUB_emberGetLocalIpAddress
bool emberGetLocalIpAddress(uint8_t index, EmberIpv6Address *address)
{
  return true;
}
#endif

#ifdef USE_STUB_emberGetRoutingLocatorReturn
void emberGetRoutingLocatorReturn(const EmberIpv6Address *rloc) {}
#endif

#ifdef USE_STUB_emGetCoapCodeName
const uint8_t *emGetCoapCodeName(EmberCoapCode type) { return NULL; }
#endif

#ifdef USE_STUB_emHandleAddressManagementPost
bool emHandleAddressManagementPost(CoapMessage *coap,
                                      const uint8_t *convertedUri)
{
  return false;
}
#endif

#ifdef USE_STUB_ipAddressToString
void ipAddressToString(const uint8_t *address, uint8_t *buffer, uint8_t bufferLength)
{
  strcpy(buffer, "0000:0000:0000:0000:0000:0000:0000:0000");
}
#endif

#ifdef USE_STUB_emberCustomNcpToHostMessageHandler
void emberCustomNcpToHostMessageHandler(const uint8_t *message, uint8_t messageLength) {}
#endif

#ifdef USE_STUB_emberCustomHostToNcpMessageHandler
void emberCustomHostToNcpMessageHandler(const uint8_t *message, uint8_t messageLength) {}
#endif

#ifdef USE_STUB_emberHostToNcpNoOp
void emberHostToNcpNoOp(const uint8_t *bytes, uint8_t bytesLength) {}
#endif

#ifdef USE_STUB_emberNcpToHostNoOp
void emberNcpToHostNoOp(const uint8_t *bytes, uint8_t bytesLength) {}
#endif

#ifdef USE_STUB_emberLeaderDataHandler
void emberLeaderDataHandler(const uint8_t *leaderData) {}
#endif

#ifdef USE_STUB_emGetCompleteNetworkData
bool emGetCompleteNetworkData(void)
{
  return true;
}
#endif

#ifdef USE_STUB_emberGetNetworkDataTlvReturn
void emberGetNetworkDataTlvReturn(uint8_t typeByte,
                                  uint8_t index,
                                  uint8_t versionNumber,
                                  const uint8_t *tlv,
                                  uint8_t tlvLength) {}
#endif

