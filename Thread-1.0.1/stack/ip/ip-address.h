/*
 * File: ip-address.h
 * Description: IPv6 address utilities
 * Author(s): Richard Kelsey, Matteo Paris
 *
 * Copyright 2013 Silicon Laboratories, Inc.                                *80*
 */

// This is used on both hosts and SOCs.  It does not include any reference
// to the local node ID, which is not available on hosts.  That code is in
// ip-header.c.

//------------------------------------------------------------------------------
// For global prefixes without an associated border router.
#define NULL_BORDER_ROUTER_INDEX 0xFF

// Same thing again, except that we have a full IPv6 address.
typedef struct {
  uint32_t preferredLifetime;     // in seconds
  uint32_t validLifetime;         // in seconds
  uint16_t requestTimeout;        // Seconds until we can retry
                                // a failed address request.
  uint8_t addressFlags;
  uint8_t prefixTlvFlags;
  uint8_t address[16];
  uint8_t domainId;
  // Keeping track of request failures might need to keep track of
  // multiple servers.  On the other hand, multiple servers and
  // failures should both be rare, so simpler is probably better.
  uint8_t serverLongId[8];
  uint8_t context;
} GlobalAddressEntry;

// We have to enforce this limit ourselves in order to avoid overflowing
// when converting lifetimes from seconds to milliseconds.
#define MAX_LIFETIME_DELAY_SEC (EMBER_MAX_EVENT_DELAY_MS / 1000)

// The actual SLAAC (preferred or valid) lifetime is not advertised, so
// we use this value.  We don't actually time out SLAAC addresses, but we
// need some value to report in DHCP LEASEQUERY replies.
#define SLAAC_LIFETIME_DELAY_SEC (30 * 60)

// Wait five minutes before trying again.  Errors should be rare (DHCP
// itself retries the request multiple times), and we want to give plenty
// of time for the network and/or gateway data to stabilize.
#define DHCP_REQUEST_TIMEOUT_SECONDS 300

void emGlobalPrefixTableInit(void);
bool emIsDefaultGlobalPrefix(const uint8_t *prefix);
void emSetDefaultGlobalPrefix(const uint8_t *suppliedPrefix);
bool emStoreDefaultGlobalPrefix(uint8_t *target);
void emSetLegacyUla(const uint8_t *prefix);
void emStoreLegacyUla(uint8_t *target);
bool emIsLegacyUla(const uint8_t *prefix);
bool emHaveLegacyUla(void);
bool emNeedAddressRequest(GlobalAddressEntry *entry);
bool emStoreIpSourceAddress(uint8_t *source, const uint8_t *destination);

//----------------------------------------------------------------

GlobalAddressEntry *emGetGatewayAddressEntry(const uint8_t *prefix);
GlobalAddressEntry *emGetPrefixAddressEntry(const uint8_t *prefix);

GlobalAddressEntry *emAddGlobalAddress(uint8_t *address,
                                       uint8_t domainId,
                                       uint32_t preferredLifetime,
                                       uint32_t validLifetime,
                                       const uint8_t *serverLongId,
                                       uint8_t addressFlags,
                                       uint8_t prefixTlvFlags);

//----------------------------------------------------------------
// For printing out the global prefix entries.
void emPrintGlobalAddresses(void);

//------------------------------------------------------------------------------

// Unicast addresses

bool emStoreGp16(uint16_t id, uint8_t *target);
bool emStoreGp64(const uint8_t *longId, uint8_t *target);
bool emIsGp16(const uint8_t *address, uint16_t *returnShort);
bool emIsGp64(const uint8_t *address);
bool emIsLoopbackAddress(const uint8_t *address);
#define emIsUnspecifiedAddress(addr) (emIsMemoryZero((addr), 16))
bool emStoreDefaultIpAddress(uint8_t *target);
bool emIsLocalIpAddress(const uint8_t *address);
bool emIsUnicastForMe(const uint8_t *address);
bool emIsOurExternalRoutePrefix(const uint8_t *prefix);
bool emIsOurDhcpServerPrefix(const uint8_t *prefix);
bool emIsOurSlaacServerPrefix(const uint8_t *prefix);

// Link local addresses

extern const Bytes8 emFe8Prefix;
void emStoreLongFe8Address(uint8_t *longId, uint8_t *target);
void emStoreShortFe8Address(uint16_t id, uint8_t *target);
void emStoreDefaultFe8Address(uint8_t *target);
bool emIsFe8Address(const uint8_t *address);
bool emIsLl16(const uint8_t *address, uint16_t *returnShort);
bool emIsLl64(const uint8_t *address);
bool emIsLocalLl64(const uint8_t *address);

// Mesh local identifier
extern uint8_t emMeshLocalIdentifier[8];
bool emberIsMeshLocalIdentifier(const uint8_t *identifier);
void emSetMeshLocalIdentifier(const uint8_t *identifier);

extern uint8_t emLinkLocalIdentifier[8];
void emSetLinkLocalIdentifier(const uint8_t *identifier);

// Interface Ids

void emStoreShortInterfaceId(uint16_t id, uint8_t *target);
#define emStoreLongInterfaceId(longId, target) (emModifyLongId((longId), (target)))
bool emIsLocalLongInterfaceId(const uint8_t* interfaceId);
bool emIsLocalShortInterfaceId(const uint8_t *interfaceId);
bool emGleanShortIdFromInterfaceId(const uint8_t *interfaceId, uint16_t* shortId);
void emModifyLongId(const uint8_t *longId, uint8_t *target);
void emUnmodifyInterfaceId(const uint8_t *interfaceId, uint8_t *longId);

// Multicast addresses

#define emIsMulticastAddress(ipAddress) ((ipAddress)[0] == 0xFF)
bool emIsAllNodesMulticastAddress(const uint8_t *address);
bool emIsFf01MulticastAddress(const uint8_t *address);
bool emIsFf02MulticastAddress(const uint8_t *address);
bool emIsFf03MulticastAddress(const uint8_t *address);
extern const Bytes16 emFf02AllNodesMulticastAddress;
extern const Bytes16 emFf02AllRoutersMulticastAddress;
extern const Bytes16 emFf03AllNodesMulticastAddress;
extern const Bytes16 emFf03AllRoutersMulticastAddress;
extern const Bytes16 emFf05AllNodesMulticastAddress;
extern const Bytes16 emFf05AllRoutersMulticastAddress;
extern const Bytes16 emFf02AllRplNodesMulticastAddress;
extern const Bytes16 emFf03AllDhcpMulticastAddress;
bool emIsMulticastForMe(const uint8_t *address);

#ifdef EMBER_TEST
// For installing a global prefix for testing.
void emSetTestPrefix(const uint8_t *prefix);
#endif

