/*
 * File: multicast.h
 * Description: Multicast forwarding from draft-hui-6man-trickle-mcast.
 * Author(s): Matteo Paris
 *
 * Copyright 2010 by Ember Corporation. All rights reserved.                *80*
 */

// Returns true if the packet has a hop-by-hop multicast option and an
// entry in the multicast table.  Also returns true if the packet should
// have a multicast option but doesn't, or if there is no space left
// in the multicast table for it.
bool emIsDuplicateMulticast(Ipv6Header *ipHeader);

void emStoreMulticastSequence(uint8_t *target);

void emMulticastTableInit(void);

#define MULTICAST_RETRY_COUNT_DEFAULT 2

// Thread uses a Trickle timer for multicasts.  If used exactly
// according to the specs, this would transmit once btween 250ms and
// 500ms after the initial reception, again between 750ms and 100ms
// after the initial reception, and finally once between 1250ms and
// 1500ms.  In other words, it transmits at a random time in the
// second half of each 500ms interval.
//
// This is silly.  The plan is to change it so that it transmits at
// one randomly chosen moment in each 500ms interval.  This doesn't
// map well to using the retry table, which starts each new delay
// at the time of the previous transmission.
//
// So we fake by making the first delay 0..500ms, and the second
// and third ones 250..750ms.  This gives roughly the right distribution,
// although the final transmission may be late by up to 500ms.

#define MULTICAST_MIN_INITIAL_INTERVAL_MS 0
#define MULTICAST_INITIAL_LOG_JITTER 9       // For 512ms jitter.
#define MULTICAST_MIN_INTERVAL_MS 250
#define MULTICAST_LOG_JITTER 9               // For 512ms jitter.

#ifdef EMBER_RIP_STACK
  #define DWELL_TIME_QS 40
#else
  #define DWELL_TIME_QS 64
#endif

//------------------------------------------------------------------------------

// The windowBitmask records which of the previous 8 sequence numbers since
// lastSequence have been received; the low bit is the highest sequence number.
// Entries are deleted when dwellQs reaches 0.
typedef struct {
  uint8_t lastSequence;
  uint8_t windowBitmask;
  uint8_t dwellQs;
  uint8_t seedLength;
  uint8_t seed[0];
} MulticastTableEntry;

#define MULTICAST_TABLE_SIZE 8

// Exported so someone can trace it.
extern Buffer emMulticastTable;

MulticastTableEntry *emGetMulticastTableEntry(uint16_t i);
void emRestoreMulticastSequence(void);
