/*
 * File: dispatch.h
 * Description: definitions for IP packet dispatching.
 * Author(s): Matteo Paris, matteo@ember.com
 *
 * Copyright 2013 by Silicon Laboratories. All rights reserved.             *80*
 */

typedef struct {
  uint8_t prefix[16];    // By convention bits after prefixLength are zero.
  uint8_t prefixLength;  // Length in bits.
  uint8_t status;
  uint16_t nextHop;
} RouteTableEntry;

#define ROUTE_TABLE_SIZE 1

#define ROUTE_TABLE_STATUS_EMPTY  0
#define ROUTE_TABLE_STATUS_ACTIVE 1

#define EXTENDED_RETRY_COUNT_DEFAULT 2
#define EXTENDED_RETRY_MIN_DELAY_MS 16
#define EXTENDED_RETRY_LOG_JITTER 5
#define EXTENDED_RETRY_TIMEOUT_MS 250
extern uint8_t emExtendedRetryCount;
extern uint8_t emMulticastRetryCount;

// Cache of the source addresses from the packet we are currently
// processing.  Used for sending replies.
extern uint8_t *emCurrentIpSource;
extern EmberNodeId emCurrentMeshSource;

void emInitializeRouteTable(void);

bool emSetRouteTableEntry(uint8_t index,
                             uint8_t *prefix,
                             uint8_t prefixLength,
                             uint16_t nextHop);

#define emDeleteRouteTableEntry(index) emSetRouteTableEntry(0, NULL, 0, 0)

EmNextHopType emLookupNextHop(const uint8_t *address, uint16_t *nextHop);

#define ALLOW_LOOPBACK     true
#define NO_LOOPBACK       false
#define NO_DELAY              0
#define USE_DEFAULT_RETRIES 255

#define emSubmitIpHeader(header, ipHeader)                              \
  (emReallySubmitIpHeader((header),                                     \
                          (ipHeader),                                   \
                          ALLOW_LOOPBACK,                               \
                          USE_DEFAULT_RETRIES, NO_DELAY))

#define emSubmitIpHeaderNoRetries(header, ipHeader) \
  (emReallySubmitIpHeader((header), (ipHeader), ALLOW_LOOPBACK, 0, NO_DELAY))

// This sets the transport checksum, does any necessary loopback, and
// calls emSubmitIndirectOrRetry().
bool emReallySubmitIpHeader(PacketHeader header,
                               Ipv6Header *ipHeader,
                               bool allowLoopback,
                               uint8_t retries,
                               uint16_t delayMs);

// Used by folks that don't have an Ipv6Header to pass to
// emReallySumbitIpHeader().
bool emSubmitIndirectOrRetry(PacketHeader header,
                                uint8_t retries,
                                uint16_t delayMs,
                                bool isMulticast);

// Ditto, because those folks need to set their own checksum.
void emSetTransportChecksum(PacketHeader header, Ipv6Header *ipHeader);

void emNetworkCheckIncomingQueue(void);

// The incoming RSSI of the message currently being processed.
extern int8_t emCurrentRssi;

// Callback for incoming beacons.
void emProcessIncomingBeacon(PacketHeader header);
void emberIpIncomingBeaconHandler(PacketHeader header); // not a public API

// Mode for passing MAC data directly to the app.
extern bool emMacReceiveMode;

extern bool emDefaultRouteToUart;
extern bool emIncomingForMeToUart;
extern bool emSecurityToUart;

void emHandleIncomingUdp(PacketHeader header, Ipv6Header *ipHeader);
