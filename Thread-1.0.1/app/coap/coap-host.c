// File: coap-host.c
//
// Description: CoAP Host functionality
//
// Copyright 2015 Silicon Laboratories, Inc.                                *80*

#include PLATFORM_HEADER
#include "stack/core/ember-stack.h"
#include "hal/hal.h"
#include "stack/include/error.h"
#include "stack/framework/buffer-management.h"
#include "stack/framework/ip-packet-header.h"
#include "stack/ip/6lowpan-header.h"
#include "stack/ip/dispatch.h"
#include "stack/ip/ip-header.h"
#include "stack/ip/ip-address.h"
#include "app/coap/coap.h"
#include "stack/framework/event-queue.h"
#include "stack/ip/zigbee/join.h"
#include "stack/ip/tls/dtls-join.h"

// for lose()
#include "stack/ip/tls/debug.h"

static EmberStatus submitMessage(const EmberIpv6Address *destination,
                                 uint16_t destinationPort,
                                 Buffer buffer)
{
  return emberSendUdp(0,   // options
                      destination->bytes,
                      255, // hop limit
                      destinationPort,
                      EMBER_COAP_PORT,
                      emGetBufferPointer(buffer),
                      emGetBufferLength(buffer));
}

EmberStatus emFinishCoapMessage(CoapMessage *coapMessage,
                                uint8_t *coapHeader,
                                uint16_t coapHeaderLength,
                                Buffer payloadBuffer,
                                EmberCoapOptions options)
{
  uint16_t payloadLength = emGetBufferLength(payloadBuffer);
  Buffer message = emAllocateBuffer(coapHeaderLength + payloadLength);
  uint8_t *bytes = emGetBufferPointer(message);
  MEMCOPY(bytes, coapHeader, coapHeaderLength);
  MEMCOPY(bytes + coapHeaderLength,
          emGetBufferPointer(payloadBuffer),
          payloadLength);

  EmberStatus result = submitMessage(&coapMessage->public.remoteAddress,
                                     coapMessage->public.remotePort,
                                     message);

  if (result == EMBER_SUCCESS) {
    emLogBytes(COAP, "CoAP TX to [",
               coapMessage->public.remoteAddress.bytes,
               16);
    emLogLine(COAP, "]");
  }

  if (result == EMBER_SUCCESS
      && coapMessage->type == COAP_TYPE_CONFIRMABLE) {
    CoapRetryEvent unackedMessage;
    MEMSET(&unackedMessage, 0, sizeof(CoapRetryEvent));
    unackedMessage.packetHeader = message;
    unackedMessage.options = options;
    MEMCOPY(&unackedMessage.remoteAddress,
            &coapMessage->public.remoteAddress,
            sizeof(EmberIpv6Address));
    unackedMessage.remotePort = coapMessage->public.remotePort;
    emAddUnackedCoapMessage(&unackedMessage, coapMessage);
  }

  return result;
}

EmberStatus emRetryCoapMessage(CoapRetryEvent *message)
{
  return submitMessage(&message->remoteAddress,
                       message->remotePort,
                       message->packetHeader);
}

static void hostCoapMessageHandler(CoapMessage *coapMessage)
{
  emApiCoapMessageHandler(&coapMessage->public.remoteAddress,
                          coapMessage->public.code,
                          coapMessage->public.token,
                          coapMessage->public.tokenLength,
                          coapMessage->public.encodedUri,
                          coapMessage->public.encodedUriLength,
                          coapMessage->public.payload,
                          coapMessage->public.payloadLength);
}

void emCoapIncomingMessageHandler(const uint8_t *bytes,
                                  uint16_t bytesLength,
                                  const Ipv6Address *localAddress,
                                  uint16_t localPort,
                                  const Ipv6Address *remoteAddress,
                                  uint16_t remotePort)
{
  CoapMessage coapMessage;
  if (emParseCoapMessage(bytes,
                         bytesLength,
                         &coapMessage)) {
    MEMCOPY(&coapMessage.public.localAddress,
            localAddress,
            sizeof(EmberIpv6Address));
    MEMCOPY(&coapMessage.public.remoteAddress,
            remoteAddress,
            sizeof(EmberIpv6Address));
    coapMessage.public.remotePort = remotePort;
    coapMessage.public.localPort = localPort;
    coapMessage.public.options = EMBER_NO_COAP_OPTIONS;
    coapMessage.message = emFillBuffer(bytes, bytesLength);
    emProcessCoapMessage(&coapMessage, hostCoapMessageHandler);
  } else {
    emLogBytesLine(COAP, "CoAP fail", bytes, bytesLength);
    loseVoid(COAP);
  }
}
