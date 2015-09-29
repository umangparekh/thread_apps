// ember-stack.h

#ifndef EMBER_STACK_H
#define EMBER_STACK_H

// Get the stack configuration from the application first
#include PLATFORM_HEADER  //DOLATER: macro-ize this

#include "include/define-ember-api.h"

// Include the public definitions
#include "include/ember.h"
#ifdef EMBER_WAKEUP_STACK
  #include "ip/wakeup-specific.h"
#endif
#include "include/packet-buffer.h"    // We shouldn't need both packet-buffer.h
                                      // and buffer-management.h.
#include "framework/eui64.h"
#include "framework/buffer-management.h"
// I am not sure that this should be here, but I don't know how many files
// would need to include it separately.   -Richard Kelsey
#include "framework/buffer-queue.h"
#include "core/log.h"
#include "include/undefine-ember-api.h"

// For back-compatibility with the hal, which is shared with znet.
#ifndef DOXYGEN_SHOULD_SKIP_THIS
  #define emberGetEui64() ((uint8_t *)&emLocalEui64)
  #define emberIsLocalEui64(eui64)                \
          (MEMCOMPARE((eui64), (uint8_t *)&emLocalEui64, EUI64_SIZE) == 0)
#endif

// This can be modified by the application in ember-configuration.c
extern uint8_t emZigbeeNetworkSecurityLevel;
extern uint16_t emberMacIndirectTimeout;
extern uint8_t emberEndDevicePollTimeout;
extern uint32_t emberSleepyChildPollTimeout;
extern uint8_t emberReservedMobileChildEntries;

//----------------------------------------------------------------
// Handy macros for exposing things only for testing.
// HIDDEN expands to 'static' in platform builds and disappears in test builds.
// ONLY_IF_EMBER_TEST(x) is x in test builds and disappears in platform builds.

#ifdef EMBER_TEST
#define HIDDEN
#define ONLY_IF_EMBER_TEST(x) x
#else
#define HIDDEN static
#define ONLY_IF_EMBER_TEST(x)
#endif


#ifdef LEAF_STACK
  #define NOT_IN_LEAF_STACK(x)
#else
  #define NOT_IN_LEAF_STACK(x) x
#endif


//**************************************************
// Stack private types

// A bool to allow common code to distinguish between RIP and RPL stacks.
#ifdef EMBER_RIP_STACK
 #define RIP_STACK true
 #define RPL_STACK false
#else
 #define RIP_STACK false
 #define RPL_STACK true
#endif

#ifdef EMBER_WAKEUP_STACK
  #define WAKEUP_STACK true
#else
  #define WAKEUP_STACK false
#endif

#define BROADCAST_ADDRESS     0xFFFF
#define EM_BROADCAST_PAN_ID   0xFFFF
#define EM_USE_LONG_ADDRESS   0xFFFE
#define NO_SHORT_ID           0xFFFE

#define emIsValidNodeId(id) ((id) < EM_USE_LONG_ADDRESS)

// Power management status
enum
{
  AWAKE,
  SLEEPY,
  ASLEEP
};

typedef uint8_t PowerStatus;

#define NO_OPTIONS 0            // just generally useful

//----------------------------------------------------------------
// Stack configuration

// The 'option' test command depends on the order of the flag bits.
// If you change the bits you must also change common-test-commands.c.
enum {
  STACK_CONFIG_FULL_ROUTER   = 0x0001,
  STACK_CONFIG_RECEIVE_RIP   = 0x0002,
  STACK_CONFIG_LONG_ID_ONLY  = 0x0004,
  STACK_CONFIG_LURKER_NETWORK = 0x0008,

  // status values
  STACK_CONFIG_NETWORK_IS_UP = 0x8000
};

extern uint16_t emStackConfiguration;

#define emSetStackConfig(x) (emStackConfiguration |= (x))
#define emCheckStackConfig(x) ((emStackConfiguration & (x)) == (x))

#define emAmRouter()      (emCheckStackConfig(STACK_CONFIG_FULL_ROUTER))
#define emReceiveRip()    (emCheckStackConfig(STACK_CONFIG_RECEIVE_RIP))
#define emUseLongIdOnly() (emCheckStackConfig(STACK_CONFIG_LONG_ID_ONLY))
#define emNetworkIsUp()   (emCheckStackConfig(STACK_CONFIG_NETWORK_IS_UP))
#define emOnLurkerNetwork() (WAKEUP_STACK \
                             && emCheckStackConfig(STACK_CONFIG_LURKER_NETWORK))

#define emAmRouting() \
  (emCheckStackConfig(STACK_CONFIG_NETWORK_IS_UP | STACK_CONFIG_FULL_ROUTER))

void emSetNetworkStatus(EmberNetworkStatus newStatus);
void emSaveNetwork(void);
void emBecomeLeader(void);

extern uint8_t emMaxEndDeviceChildren;    // maximum for this node
extern uint8_t emEndDeviceChildCount;     // how many we have
extern EmberNodeId emParentId;
extern uint8_t emParentLongId[8];

// Maximum number of addresses that a parent will allow a sleepy child to
// register.  This is the MLE64 address plus three GUA.
#define MAX_CHILD_ADDRESS_COUNT 4

// The '+ 0' prevents anyone from accidentally assigning to these.
#define emberChildCount()          (emEndDeviceChildCount + 0)
#define emberRouterChildCount()    0
#define emberMaxChildCount()       (emMaxEndDeviceChildren + 0)
#define emberMaxRouterChildCount() 0
#define emberGetParentNodeId()     (emParentId          + 0)

//----------------------------------------------------------------
void emResetNetworkState(void);

EmberNodeId emberGetNodeId(void);

void emSetNodeId(uint16_t nodeId);
void emSetPanId(uint16_t panId);
void emSetRadioPower(int8_t power);

void emNetworkStateChangedHandler(void);  // Use for ip-modem.
//----------------------------------------------------------------
// Private clusters for our application profile.  The public ones are
// in ember-types.h and start from 0x0000.  Replies use the cluster
// with the high-bit (0x8000) set, as the ZDO does.
#define EMBER_MOBILE_JOIN_CLUSTER           0x4000
#define EMBER_MOBILE_REJOIN_CLUSTER         0x4001

#define DEFAULT_PREFIX_BYTES 8
#define DEFAULT_PREFIX_BITS (DEFAULT_PREFIX_BYTES << 3)

uint8_t *emLookupLongId(uint16_t shortId);

//**************************************************
// Other Stack public stuff
EmberStatus emberSendRawMessage(uint8_t *contents, uint8_t length);

//**************************************************
// Other Stack private stuff

// modulo increment and decrement
// useful for circular buffers
#define MOD_INC(num, modulus)      \
   (((num) == ((modulus) - 1)) ? 0 : ((num) + 1))

#define MOD_DEC(num, modulus)      \
   (((num) == 0) ? ((modulus) - 1) : ((num) - 1))

// Returns true if the first 'count' bytes pointed to by 'bytes' are
// all 'target'.
XAP2B_PAGEZERO_ON
bool emMemoryByteCompare(const uint8_t *bytes, uint8_t count, uint8_t target);
XAP2B_PAGEZERO_OFF
#define emIsMemoryZero(bytes, count) (emMemoryByteCompare((uint8_t *)(bytes), (count), 0))
#define isNullEui64(eui64) (emIsMemoryZero((eui64), EUI64_SIZE))

// For comparing structs with a 'contents' field, like keys or dagIds.
#define equalContents(a, b, len) \
  (MEMCOMPARE((a)->contents, (b)->contents, len) == 0)

// Returns true if t1 is a later time than t2.
#define timeGTint8u(t1, t2)                   \
  (! timeGTorEqualInt8u(t2, t1))

#undef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))

// Choose a random number between 'low' and 'high'.
#define randomRange(low, high) ((low) + (halCommonGetRandom() % ((high) - (low))))

// Random number between 0 and 2^log - 1.
#define expRandom(log) (halCommonGetRandom() & (BIT((log)) - 1))

// Random number between  -2^logJitter and 2^logJitter - 1
#define expJitter(logJitter) (expRandom((logJitter)+1) - BIT((logJitter)))

// Decrements the contents of delayTicksLoc by ticksPassed and, if the
// result is greater than zero, sets the contents of minTicksNeededLoc
// to be the minimum of its current contents and the new contents of
// delayTicksLoc.

void emDecrementTicks(uint16_t *delayTicksLoc,
                      uint16_t ticksPassed,
                      uint16_t *minTicksNeededLoc);

extern EmberTaskId emStackTask;
void emPrintStackEvents(uint8_t port);
void emPrintStackBuffers(uint8_t port);

typedef uint8_t PacketDispatchType;
typedef uint8_t MacFrameType;
typedef uint8_t MacAddressMode;
typedef uint8_t HeaderOptionType;

// MAC transmit priority.
// High priority headers go on the front of the queue.
// Normal priority headers go on the back of the queue.
// Only beacon requests and orphan notifications can be
// sent during a scan.  They are submitted with SCAN_OKAY
// and go on the front of the queue.

enum {
  TRANSMIT_PRIORITY_HIGH,
  TRANSMIT_PRIORITY_NORMAL,
  TRANSMIT_PRIORITY_SCAN_OKAY
};
typedef uint8_t TransmitPriority;

// MAC scan types.  Note:  this is an expanded list that includes internal as
// well as external scan types.
//
// The RADIO_OFF scan is not really a scan at all.  It is used to
// temporarilly turn off the radio in order to use the TX and RX
// buffers for other purposes.
// Start the scan with:
//   emberStartScan(EM_START_RADIO_OFF_SCAN, 0, 0);
// Then wait for emMacScanType to be set to EM_RADIO_OFF_SCAN,
// at which point the radio is off.  There may be a brief
// delay while the radio finishes transmitting.
//
// Call emberStopScan() to restart the radio and the MAC.

enum {
  EM_STACK_ENERGY_SCAN = EMBER_ACTIVE_SCAN + 1,
  EM_STACK_ACTIVE_SCAN,
  EM_PASSIVE_SCAN,
  EM_ORPHAN_SCAN,
  EM_START_RADIO_OFF_SCAN,    // Fake scan that is used to turn off the radio.
  EM_RADIO_OFF_SCAN,          // Indicates that the radio is off.
  EM_STACK_ZLL_ACTIVE_SCAN,
  EM_SCAN_STOPPING,
  EM_SCAN_IDLE,
};

// Defined in routing/zigbee/association.c, but used widely.
extern EmberNodeType emNodeType;

// An enumeration in zigbee/aps-security.h.
//typedef uint8_t ZigbeeKeyType;

// Security key and key sequence number are closely related
// under Zigbee.
#define SECURITY_BLOCK_SIZE         16 // Key, Nonce, and standalone block size

typedef struct {
  uint8_t key[SECURITY_BLOCK_SIZE];
  uint8_t keySequenceNumber;
} EmberSecurityKey;

typedef struct RetryEntryS {
  PacketHeader header;
  uint8_t attempts;  // Top/bottom nibble is successful/remaining attempts.
  uint16_t timer;
  uint16_t startTime;
} RetryEntry;

typedef enum {
  DHCPV6_REPLY,
  PARENT_ADDRESS_REGISTRATION,
} ChildPendingReplies;

// Bitmask indexed by ChildPendingReplies.  If the i'th bit is set then
// the corresponding reply is expected and the stack must check for
// inocming messages.
extern uint16_t emPendingReplies;

extern EventQueue emStackEventQueue;

#ifdef IP_MODEM_LIBRARY
  // For RTOS stack thread: an event queue event that runs all stack events.
  extern Event emStackEvent;
#endif

typedef struct {
  EmberMacBeaconData apiBeaconData;
  uint16_t joinPort;
  uint16_t commissionPort;
  bool allowingCommission;
} MacBeaconData;

//----------------------------------------------------------------
// Node Type Macros.

#define LURKER_NODE_TYPE_BIT 0x80

void emSetNodeType(EmberNodeType type);

#define emNodeTypeIsLurker()                   \
  (EMBER_LURKER == emNodeType)

#define emNodeTypeIsRouter()                   \
  (EMBER_ROUTER == emNodeType || EMBER_COORDINATOR == emNodeType)

#define emNodeTypeIsEndDevice()                \
  (EMBER_END_DEVICE == emNodeType              \
   || EMBER_SLEEPY_END_DEVICE == emNodeType    \
   || EMBER_MOBILE_END_DEVICE == emNodeType)

#define emNodeTypeIsSleepy()                   \
  (EMBER_SLEEPY_END_DEVICE == emNodeType       \
   || EMBER_MOBILE_END_DEVICE == emNodeType)

#define emAmDarkRouter()                                    \
  (EMBER_ROUTER == emNodeType                               \
   && ! emCheckStackConfig(STACK_CONFIG_FULL_ROUTER))

#define emAmEndDevice()                        \
  (EMBER_END_DEVICE == emNodeType              \
   || emNodeTypeIsSleepy()                     \
   || emAmDarkRouter())

#define emNodeTypeMaxValue()                   \
  (EMBER_COMMISSIONER)


#ifdef EMBER_TEST
// This function asserts that the length of the freelist equals
// emPacketBufferFreeCount.
uint8_t emFreePacketBufferCount(void);
void printPacketBuffers(EmberMessageBuffer buffer);
void simPrintBytes(char *prefix, uint8_t *bytes, uint16_t count);
void simPrintBuffer(char *prefix, Buffer buffer);
void simPrintStartLine(void);
#endif

//----------------------------------------------------------------
// Internal buffer utilities.

// uint8_t emNormalizeBufferIndex(EmberMessageBuffer *bufferLoc, uint16_t index);
// void emCopyToNormalizedBuffers(PGM_P contents,
//                                EmberMessageBuffer buffer,
//                                uint8_t startIndex,
//                                uint8_t length,
//                                uint8_t direction);
// bool emSetLinkedBuffersLength(EmberMessageBuffer buffer,
//                                  uint16_t oldLength,
//                                  uint16_t newLength);
// extern uint8_t emAvailableStackBuffer;

//----------------------------------------------------------------
// A simple printf()-like facility for creating messages.  For now this only
// works for messages that don't cross buffer boundaries.
//
// This has not been released to customers, mostly because of the restriction
// about crossing buffer boundaries.
//
// The code here uses format strings to specify how values are to be encoded.
// Each character in a format string gives the format for one
// value.  The formats are case-insenstive.  The current format characters are:
// '1'      A one-byte unsigned value.
// '2'      A two-byte unsigned value encoded as the less-significant byte
//          followed by the more-significant byte.
// '4'      A 4-byte unsigned value encoded as the least-significant byte
//          to the most significant byte.  This use takes precedence over the
//          formatting ranges described below.
// '3'-'9'  A pointer to three to nine bytes (usually 8 for an EUI64) that
//          is to be copied into the packet.
// 'A'-'G'  A pointer to ten to sixteen bytes that is to be copied into the
//          packet.
// 's'      A sequence of unsigned bytes.  When encoding the first supplied
//          value is a pointer to the data and the second value is the number
//          of bytes.
// [Not yet implemented:
// 'p'      Same as 's' except that the bytes are in program space.
// ]

XAP2B_PAGEZERO_ON
EmberMessageBuffer emMakeMessage(uint8_t startIndex, PGM_P format, ...);
XAP2B_PAGEZERO_OFF

EmberMessageBuffer emMakeMessageUsingVaList(uint8_t startIndex,
                                            PGM_P format,
                                            va_list argPointer);

// Similar to emMakeMessageUsingVaList(), except that it operates
// on a flat buffer (up to maxLength).  The buffer is assumed to be
// big enough to hold the message
uint8_t emWriteMessage(uint8_t* buffer,
                     uint8_t maxLength,
                     uint8_t startIndex,
                     PGM_P format,
                     ...);
uint8_t emWriteMessageUsingVaList(uint8_t* buffer,
                                uint8_t maxLength,
                                uint8_t startIndex,
                                PGM_P format,
                                va_list argPointer);


uint8_t emParseBuffer(EmberMessageBuffer message,
                    uint8_t startIndex,
                    PGM_P format,
                    ...);

uint8_t emParseBufferFromVaList(EmberMessageBuffer message,
                              uint8_t startIndex,
                              PGM_P format,
                              va_list elements);

//----------------------------------------------------------------
// Message buffer queue functions.
//
// The link fields point from head to tail, except that the final tail's link
// points back to the head, forming a loop.  This allows us to add to the tail
// and remove from the head in constant time.

extern Buffer emMacToNetworkQueue;
extern Buffer emNetworkToApplicationQueue;

//----------------------------------------------------------------
// Application registered callbacks (see stack/include/network-management.h)

extern bool (*emDropCallback)(PacketHeader header, Ipv6Header *ipHeader);
extern void (*emSerialTransmitCallback)(PacketHeader header);

//----------------------------------------------------------------
void emNoteTimeHandler(const char *label);
#define emNoteTime(label)
// #define emNoteTime(label) emNoteTimeHandler(label)

#if defined EMBER_TEST
#define EMBER_TEST_EXTERNAL
#define EMBER_TEST_EXTERNAL_PGM
#else
#define EMBER_TEST_EXTERNAL static
#define EMBER_TEST_EXTERNAL_PGM PGM static
#endif

#ifdef EMBER_TEST
const char *emErrorString(EmberStatus errorCode);
extern uint16_t simulatorId;
extern uint16_t rebootCount;
void simPrint(char *format, ...);
void debugSimPrint(char *format, ...);
void debugPrintTextAndHex(const char* text,
                          const uint8_t* hexData,
                          uint8_t length,
                          uint8_t spaceEveryXChars,
                          bool finalCr);

typedef struct ParcelS {
  uint32_t tag;           // for safety
  int length;
  uint8_t contents[0];
} Parcel;

void scriptTestCheckpoint(char* string);

#elif !defined(EMBER_TEST) && !defined(EMBER_SCRIPTED_TEST)
  #define debugSimPrint(...)
  #define scriptTestCheckpoint(string)
  #define debugPrintTextAndHex(...)
#endif

//------------------------------------------------------------------------------
// Buffer Usage.
//
// Find out where buffers have been squirreled away by ferreting through various
// queues. The information is recorded in emBufferUsage[], 16 bits per buffer.

//#define EM_BUFFER_USAGE
#ifdef EM_BUFFER_USAGE
extern uint16_t emBufferUsage[];
// Asserts that all buffers are accounted for.
void emCheckBufferUsage(void);
enum {
  // On the free list.
  EM_BUFFER_USAGE_FREE             = 0,  // 0x0001
  // The payload of a packet header.
  EM_BUFFER_USAGE_PAYLOAD          = 1,  // 0x??02 (high byte is header)
  // Linked from another buffer.
  EM_BUFFER_USAGE_LINK             = 2,  // 0x??04 (high byte is linker)
  // In emSerialTxQueues.
  EM_BUFFER_USAGE_SERIAL           = 3,  // 0x0008
  // In endpointDataQueue or sourceRoute (EM260).
  EM_BUFFER_USAGE_EZSP             = 4,  // 0x0010
  // In callbackQueue (EM260).
  EM_BUFFER_USAGE_Q_EZSP_CALLBACK  = 5,  // 0x0020
  // In ASH queue (EM260).
  EM_BUFFER_USAGE_Q_ASH            = 6,  // 0x0040
  // In emPhyToMacQueue.
  EM_BUFFER_USAGE_Q_PHY_TO_MAC     = 7,  // 0x0080

  // In emMacToNetworkQueue.
  EM_BUFFER_USAGE_Q_MAC_TO_NWK     = 8,  // 0x0100
  // In emNetworkToApplicationQueue.
  EM_BUFFER_USAGE_Q_NWK_TO_APP     = 9,  // 0x0200
  // In longIndirectPool (MAC).
  EM_BUFFER_USAGE_Q_LONG_INDIRECT  = 10, // 0x0400
  // In shortIndirectPool (MAC).
  EM_BUFFER_USAGE_Q_SHORT_INDIRECT = 11, // 0x0800
  // In retryEntries.
  EM_BUFFER_USAGE_RETRY            = 12, // 0x1000
  // In pendingAckedMessages (APS).
  EM_BUFFER_USAGE_APS              = 13, // 0x2000
  // In transmitQueue (MAC).
  EM_BUFFER_USAGE_MAC              = 14, // 0x4000
  // CBKE library (storage of generated keys)
  EM_BUFFER_USAGE_CBKE             = 15  // 0x8000
};
#endif

// next hop types
typedef enum {
  EM_NO_NEXT_HOP    = 0,
  EM_SHORT_NEXT_HOP = 1,
  EM_LONG_NEXT_HOP  = 2
} EmNextHopType;

#endif // EMBER_STACK_H
