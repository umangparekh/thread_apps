// File: child-data.h
//
// Description: ZigBee IP child table
//
// Author(s): Richard Kelsey <kelsey@ember.com>
//
// Copyright 2010 by Ember Corporation.  All rights reserved.               *80*

// Entries in the child table token have three fields: the child's EUI64,
// the child's node ID, and a capabilities byte.
#define CHILD_ENTRY_EUI64_OFFSET         0
#define CHILD_ENTRY_NODE_ID_OFFSET       8
#define CHILD_ENTRY_CAPABILITIES_OFFSET 10

#define CHILD_ENTRY_SIZE 11
#define MIN_CHILD_TIMEOUT_SECONDS 30

// Moved to include/child.h to give the application access.
//extern uint8_t emMaxEndDeviceChildren;

// Various per-child timers are allocated by the application via
// ember-ip-configuration.c.
extern uint8_t emberChildTableSize;

// These should be combined into a struct.  The short ID may have to be
// separate because of the time requirements on emberChildIndex()
// (in phy/child-table.c).
extern EmberNodeId *emChildIdTable;     // short ID
extern uint8_t *emChildLongIdTable;       // long ID
extern ChildStatusFlags *emChildStatus; // status flags
extern uint32_t *emChildTimers;
extern uint32_t *emChildTimeouts;
extern uint32_t *emChildFrameCounters;
extern uint16_t *emChildLastTransactionTimesSec;
extern uint16_t *emChildAttachDurationSec;
extern Buffer emChildAddressTable;

// The one-byte timer for each end-device child gives the
// elapsed time since the last poll.  The units depend on the current
// flag values.  If CHILD_EXPECTING_JIT is set, the time is in
// milliseconds and is used to clear CHILD_EXPECTING_JIT if we
// fail to send a message in time.  Otherwise, it is in seconds if
// CHILD_IS_MOBILE is set and minutes if not.

//----------------------------------------------------------------
// Child status information

// We have 16 bits bytes of per-child information in RAM.
#define CHILD_IS_PRESENT                 0x0001
#define CHILD_LINK_ESTABLISHED           0x0002
#define CHILD_IS_SLEEPY                  0x0004
#define CHILD_PENDING_MAC_SHORT_INDIRECT 0x0008
#define CHILD_PENDING_MAC_LONG_INDIRECT  0x0010
#define CHILD_USING_OLD_KEY              0x0020
#define CHILD_HAS_OLD_NETWORK_DATA       0x0040
#define CHILD_STABLE_DATA_ONLY           0x0080

#define CHILD_PENDING_APPLICATION_JIT    0x0100
#define CHILD_PENDING_SLEEPY_BROADCAST   0x0200
#define CHILD_EXPECTING_JIT              0x0400
#define CHILD_PENDING_UNICAST_ALARM      0x0800
#define CHILD_PENDING_BROADCAST_ALARM    0x1000

#define CHILD_SECURE_DATA_REQUESTS       0x2000
#define CHILD_IS_REED                    0x4000

// Pick out all of the flags that indicate that a MAC indirect message is
// pending.
#define CHILD_HAS_PENDING_MESSAGE       \
 (CHILD_PENDING_MAC_SHORT_INDIRECT      \
  | CHILD_PENDING_MAC_LONG_INDIRECT     \
  | CHILD_PENDING_APPLICATION_JIT       \
  | CHILD_PENDING_SLEEPY_BROADCAST      \
  | CHILD_HAS_OLD_NETWORK_DATA)

#define emHaveChild(i)          (emChildStatusFlag((i), CHILD_IS_PRESENT))
#define emIsSleepyChildIndex(i) (emChildStatusFlag((i), CHILD_IS_SLEEPY))
#define emIsReedChildIndex(i)   (emChildStatusFlag((i), CHILD_IS_REED))

#define emChildHasPendingMessage(childIndex) \
 ((emChildStatus[(childIndex)] & CHILD_HAS_PENDING_MESSAGE) != 0)

#define emChildStatusFlag(childIndex, mask)     \
  (emChildStatus[(childIndex)] & (mask))

#define emSetChildStatusFlag(childIndex, mask)	\
  (emChildStatus[(childIndex)] |= (mask))

#define emClearChildStatusFlag(childIndex, mask)\
  (emChildStatus[(childIndex)] &= ~(mask))

// Set or clear 'mask' bits for all sleepy children.
bool emSetAllSleepyChildFlags(ChildStatusFlags mask, bool set);

// True if any child as all of the given flags set.
bool emAnyChildHasFlagsSet(ChildStatusFlags mask);

// True if any sleepy child has this flag set.
#define emIsSleepyChildFlagSet(mask) \
 (emAnyChildHasFlagsSet((mask) | CHILD_IS_SLEEPY))

//----------------------------------------------------------------
// Adding and removing entries from the child table.

uint8_t emMaybeAddChild(EmberNodeId id,
                      const uint8_t *longId,
                      uint8_t capabilities,
                      uint32_t timeoutSeconds);

bool emInitializeChild(uint8_t index,
                          EmberNodeId id,
                          const uint8_t *longId,
                          uint8_t capabilities,
                          uint32_t timeoutSeconds);

void emSetChildId(uint8_t childIndex, EmberNodeId newId);
void emEraseChild(uint8_t childIndex);
void emEraseChildTable(void);

void emNoteChildTransaction(uint8_t index);
uint16_t emChildSecondsSinceLastTransaction(uint8_t index);
uint16_t emChildSecondsSinceAttaching(uint8_t index);

#define CHILD_CHANGE_LEAVING     0x00
#define CHILD_CHANGE_JOINING     0x01
#define CHILD_CHANGE_NO_CALLBACK 0x02   // do not call emberChildJoinHandler()

void emNoteChildChange(uint8_t childIndex, uint8_t options);

// Looking up a child.
EmberNodeId emFindChild(const uint8_t *longId);
uint8_t emFindChildIndex(uint8_t startIndex, const uint8_t *longId);
uint8_t emFindChildIndexByAddress(const EmberIpv6Address *address);
#define emFindFreeChildIndex(startIndex) (emFindChildIndex((startIndex), NULL))
bool emFindChildByInterfaceId(const uint8_t *interfaceId, uint16_t *nextHop);
bool emChildUsingOldKey(const uint8_t *longInterface);

bool emSetChildAddresses(uint8_t index,
                            const uint8_t* data,
                            uint8_t dataLength);

// These are also defined in mac/802.15.4/command.h, but not everyone includes
// that and the router bit name defined there is not perspicuous.
#define ROUTER_CAPABILITY          0x02
#define RX_ON_WHEN_IDLE_CAPABILITY 0x08

// This returns the child's index only if the capabilities match.
// If they don't match, the child is removed from the table.
uint8_t emFindChildWithCapabilities(const uint8_t *longId, uint8_t capabilities);

// Used by the routing code, which doesn't care about indexes.
bool emIsMobileChild(EmberNodeId id);
uint8_t *emLookupChildEui64ById(EmberNodeId id);
void emResetChildFrameCounters(void);
bool emChildIsFrameCounterValid(uint32_t frameCounter,
                                   const uint8_t *eui64,
                                   uint32_t sequenceNumber);
void emChildSetFrameCounter(uint8_t index,
                            uint32_t frameCounter,
                            uint32_t keySequence);

uint8_t emGetChildCapabilities(uint8_t index);
bool emIsSleepyChild(EmberNodeId id);
bool emIsEndDeviceChild(EmberNodeId id);

// Restart the timeout clock for end devices.
void emNoteSuccessfulPoll(void);

// Just what it says.
uint32_t emMsSinceLastSuccessfulPoll(void);

// Handy utility.
#define copyEui64(to, from)      MEMCOPY((to), (from), EUI64_SIZE)

void emChildInit(void);

// Used to time out neglectful parents who aren't around when their
// children need them.
void emStartParentAging(void);

// Reset the child's timeout.
void emResetChildsTimer(uint8_t index);

// start polling
void emStartPolling(void);

// stop polling
void emStopPolling(void);

// get the poll timeout
uint32_t emGetPollTimeout(void);

uint8_t emChildCount(void);

// we've lost our parent, rejoin
void emLostParent(void);

#ifdef EMBER_TEST
  EmberStatus emRemoveChild(const uint8_t *longId);
#endif

void emChildAgingInit(void);

extern uint32_t emKeepaliveFlurryIntervalS;
