/*
 * File: dtls-join.h
 * Description: joining using DTLS
 *
 * Copyright 2015 Silicon Laboratories, Inc.                                *80*
 */

enum {
  DLTS_JOIN_THREAD_COMMISSIONING,       // the default
  DLTS_JOIN_JPAKE_TEST
};

extern uint8_t emDtlsJoinMode;
extern uint16_t emUdpJoinPort;
extern uint16_t emUdpCommissionPort;

// Thread wants use to require a cookie for DTLS join handshakes.
extern bool emDtlsJoinRequireCookie;

void emSetJoinSecuritySuites(uint16_t suites);

void emCloseDtlsJoinConnection(void);

// Process a message that arrives from a joiner, either directly over the
// radio (relayed == false) or via the commissioning app (relayed == true).
// The value of 'relayed' is used only to determine how to send any
// response.
void emIncomingJoinMessageHandler(PacketHeader header,
                                  Ipv6Header *ipHeader,
                                  bool relayed);


// Process a joiner message that was relayed via the commissioning protocol.
// The "message" is the transport payload of the original message sent by
// the joiner.
void emRelayedJoinMessageHandler(const uint8_t *joinerIid,
                                 uint16_t joinerPort,
                                 EmberNodeId joinerRouterId,
                                 const uint8_t *message,
                                 uint16_t messageLength,
                                 Buffer messageBuffer);

// Callback provided by someone else to process messages that arrive over
// the commissioner<->joiner DTLS connection.
void emHandleJoinDtlsMessage(EmberUdpConnectionData *connection,
                             uint8_t *packet,
                             uint16_t length,
                             Buffer buffer);

bool emStartJoinClient(const uint8_t *address,
                          uint16_t remotePort,
                          const uint8_t *key,
                          uint8_t keyLength);

extern EmberUdpConnectionHandle emParentConnectionHandle;
