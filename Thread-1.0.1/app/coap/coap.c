// File: coap.c
//
// Description: A CoAP server and client, based on draft-ietf-core-coap-12
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
#include "app/afv6/plugin/common/list.h"
#include "stack/framework/event-queue.h"

// for lose()
#include "stack/ip/tls/debug.h"

#define END_BYTE 0xFF
#define OPTION_LENGTH(byte) (byte & COAP_OPTION_LENGTH_MASK)
#define OPTION_DELTA(byte) ((byte & COAP_OPTION_DELTA_MASK) >> 4)

static uint16_t nextMessageId = 1;
static uint16_t nextToken = 1;

static CoapMessage *currentCoapMessage;

//----------------------------------------------------------------

static void retryEventMarker(CoapRetryEvent *event)
{
  emMarkBuffer(&event->packetHeader);
}

static void retryEventHandler(CoapRetryEvent *event);

EventActions emCoapRetryEventActions = {
  &emStackEventQueue,
  (void (*)(Event *)) retryEventHandler,
  (void (*)(Event *)) retryEventMarker,
  "coap retry"
};

// If the packet header is NULL we have already received an ack and are
// waiting for the actual response.

static bool messageIdPredicate(Event *event, void *messageIdLoc)
{
  CoapRetryEvent *retryEvent = (CoapRetryEvent *) event;
  return (retryEvent->packetHeader != NULL_BUFFER
          && retryEvent->messageId == (* ((uint16_t *) messageIdLoc)));
}

// Only those events that have response timeouts have token values.

static bool tokenPredicate(Event *event, void *tokenLoc)
{
  CoapRetryEvent *retryEvent = (CoapRetryEvent *) event;
  return (retryEvent->responseTimeoutMs != 0
          && retryEvent->token == (* ((uint16_t *) tokenLoc)));
}

static CoapRetryEvent *findRetryEvent(uint16_t value,
                                      EventPredicate predicate)
{
  return (CoapRetryEvent *)
    emberFindEvent(&emStackEventQueue,
                   &emCoapRetryEventActions,
                   predicate,
                   &value);
}

#define findRetryEventByMessageId(id) (findRetryEvent((id), &messageIdPredicate))
#define findRetryEventByToken(id)     (findRetryEvent((id), &tokenPredicate))

void emNoteCoapAck(uint16_t messageId)
{
  // Need to call the handler.
  findRetryEventByMessageId(messageId);
}

void emAddUnackedCoapMessage(CoapRetryEvent *event, CoapMessage *message)
{
  Buffer eventBuffer =
    emAllocateBuffer(sizeof(CoapRetryEvent) + message->responseAppDataLength);
  if (eventBuffer == NULL_BUFFER) {
    return;     // would be good to signal failure
  }

  CoapRetryEvent *retryEvent =
    (CoapRetryEvent *)(void *)emGetBufferPointer(eventBuffer);
  MEMCOPY(retryEvent, event, sizeof(CoapRetryEvent));
  MEMCOPY(((uint8_t *) retryEvent) + sizeof(CoapRetryEvent),
          message->responseAppData,
          message->responseAppDataLength);
  retryEvent->responseAppDataLength = message->responseAppDataLength;
  retryEvent->event.actions = &emCoapRetryEventActions;
  retryEvent->event.next = NULL;
  retryEvent->retryCount = 0;
  retryEvent->messageId = message->messageId;
  if (message->responseHandler != NULL) {
    retryEvent->handler = message->responseHandler;
    if (message->responseTimeoutMs != 0) {
      retryEvent->responseTimeoutMs = message->responseTimeoutMs;
      retryEvent->token = emberFetchHighLowInt16u(message->public.token);
    }
  }
  emberEventSetDelayMs((Event *) retryEvent, COAP_ACK_TIMEOUT_MS);
}

static void callResponseHandler(EmberCoapStatus status,
                                CoapMessage *message,
                                CoapRetryEvent *event)
{
  event->handler(status,
                 &message->public,
                 (event->responseAppDataLength == 0
                  ? NULL
                  : ((uint8_t *) event) + sizeof(CoapRetryEvent)),
                 event->responseAppDataLength == 0);
}

static void retryEventHandler(CoapRetryEvent *event)
{
  if (COAP_MAX_RETRANSMIT <= event->retryCount
      || event->packetHeader == NULL_BUFFER) {
    if (event->handler != NULL) {       // when could this happen?
      callResponseHandler(EMBER_COAP_MESSAGE_TIMED_OUT, NULL, event);
    }
  } else {
    emLogLine(COAP, "retry ID %u", event->messageId);
    event->retryCount += 1;
    emApiRetryCoapMessage(event);
    emberEventSetDelayMs((Event *) event,
                         (COAP_ACK_TIMEOUT_MS
                          << (event->retryCount - 1)));
  }
}

//----------------------------------------------------------------

typedef struct {
  EmberCoapCode type;
  const uint8_t * const string;
} CoapResponseMap;

static const CoapResponseMap responseMap[] = {
  {EMBER_COAP_CODE_EMPTY,                          "EMPTY"},
  {EMBER_COAP_CODE_GET,                            "GET"},
  {EMBER_COAP_CODE_POST,                           "POST"},
  {EMBER_COAP_CODE_PUT,                            "PUT"},
  {EMBER_COAP_CODE_DELETE,                         "DELETE"},
  {EMBER_COAP_CODE_201_CREATED,                    "2.01 Created"},
  {EMBER_COAP_CODE_202_DELETED,                    "2.02 Deleted"},
  {EMBER_COAP_CODE_203_VALID,                      "2.03 Valid"},
  {EMBER_COAP_CODE_204_CHANGED,                    "2.04 Changed"},
  {EMBER_COAP_CODE_205_CONTENT,                    "2.05 Content"},
  {EMBER_COAP_CODE_400_BAD_REQUEST,                "4.00 Bad Request"},
  {EMBER_COAP_CODE_401_UNAUTHORIZED,               "4.01 Unauthorized"},
  {EMBER_COAP_CODE_402_BAD_OPTION,                 "4.02 Bad Option"},
  {EMBER_COAP_CODE_403_FORBIDDEN,                  "4.03 Forbidden"},
  {EMBER_COAP_CODE_404_NOT_FOUND,                  "4.04 Not Found"},
  {EMBER_COAP_CODE_405_METHOD_NOT_ALLOWED,         "4.05 Method Not Allowed"},
  {EMBER_COAP_CODE_406_NOT_ACCEPTABLE,             "4.06 Not Acceptable"},
  {EMBER_COAP_CODE_412_PRECONDITION_FAILED,        "4.12 Precondition Failed"},
  {EMBER_COAP_CODE_413_REQUEST_ENTITY_TOO_LARGE,   "4.13 Request Entity Too Large"},
  {EMBER_COAP_CODE_415_UNSUPPORTED_CONTENT_FORMAT, "4.15 Unsupported Content-Format"},
  {EMBER_COAP_CODE_500_INTERNAL_SERVER_ERROR,      "5.00 Internal Server Error"},
  {EMBER_COAP_CODE_501_NOT_IMPLEMENTED,            "5.01 Not Implemented"},
  {EMBER_COAP_CODE_502_BAD_GATEWAY,                "5.02 Bad Gateway"},
  {EMBER_COAP_CODE_503_SERVICE_UNAVAILABLE,        "5.03 Service Unavailable"},
  {EMBER_COAP_CODE_504_GATEWAY_TIMEOUT,            "5.04 Gateway Timeout"},
  {EMBER_COAP_CODE_505_PROXYING_NOT_SUPPORTED,     "5.05 Proxying Not Supported"},
};

const uint8_t *emGetCoapCodeName(EmberCoapCode type)
{
  uint8_t i;

  for (i = 0; i < COUNTOF(responseMap); i++) {
    if (responseMap[i].type == type) {
      return responseMap[i].string;
    }
  }

  return "?????";
}

typedef struct {
  CoapType type;
  const char * const name;
} CoapTypeMap;

static const CoapTypeMap typeMap[] = {
  {COAP_TYPE_CONFIRMABLE,     "CON"},
  {COAP_TYPE_NON_CONFIRMABLE, "NON"},
  {COAP_TYPE_ACK,             "ACK"},
  {COAP_TYPE_RESET,           "RESET"}
};

const uint8_t *emGetCoapTypeName(CoapType type)
{
  uint8_t i;

  for (i = 0; i < COUNTOF(typeMap); i++) {
    if (typeMap[i].type == type) {
      return (const uint8_t *)typeMap[i].name;
    }
  }

  return "?????";
}

static uint8_t *lengthMemcopy(uint8_t *destination,
                            const uint8_t *source,
                            uint16_t length)
{
  assert(length < 255);
  MEMCOPY(destination, source, length);
  return destination + length;
}

#define STORE_OPTION_DELTA(byte, value) (byte |= value << 4)

HIDDEN uint8_t *storeOptionAndLength(uint8_t *finger,
                                   CoapOption option,
                                   CoapOption previousOption,
                                   uint16_t length)
{
  *finger = 0;
  uint8_t *start = finger;
  uint16_t optionDelta = option - previousOption;

  if (optionDelta < 13) {
    STORE_OPTION_DELTA(*finger, optionDelta);
    finger++;
  } else if (optionDelta <= 0xFF + 13) {
    STORE_OPTION_DELTA(*finger, 13);
    finger++;
    *finger++ = optionDelta - 13;
  } else {
    STORE_OPTION_DELTA(*finger, 14);
    finger++;
    emberStoreHighLowInt16u(finger, optionDelta - 14);
    finger += 2;
  }

  if (length < 13) {
    *start |= length;
  } else if (length <= 0xFF + 13) {
    *start |= 13;
    *finger++ = length - 13;
  } else {
    *start |= 14;
    emberStoreHighLowInt16u(finger, length - 14);
    finger += 2;
  }

  return finger;
}

static const uint8_t *fetchOptionAndLength(const uint8_t *finger,
                                         const CoapOption previousOption,
                                         CoapOption *option,
                                         uint16_t *length)
{
  uint8_t optionLength = OPTION_LENGTH(*finger);
  uint8_t optionDelta = OPTION_DELTA(*finger);

  finger++;

  if (optionDelta == 13) {
    optionDelta += *finger;
    finger++;
  } else if (optionDelta == 14) {
    optionDelta += emberFetchHighLowInt16u(finger);
    finger += 2;
  }

  *option = (CoapOption)(previousOption + optionDelta);

  if (optionLength == 13) {
    optionLength += *finger;
    finger++;
  } else if (optionLength == 14) {
    optionLength += emberFetchHighLowInt16u(finger);
    finger += 2;
  }

  *length = optionLength;
  return finger;
}

HIDDEN uint8_t *createContentFormatOption(CoapOption previousOption,
                                        uint8_t *finger)
{
  finger = storeOptionAndLength(finger,
                                COAP_OPTION_CONTENT_FORMAT,
                                previousOption,
                                1);
  *finger++ = COAP_CONTENT_FORMAT_OCTET_STREAM;
  return finger;
}

uint16_t emberCoapEncodeUri(const uint8_t *uri,
                            uint8_t *buffer,
                            uint16_t bufferLength)
{
  uint8_t *finger = buffer;
  uint8_t start = 0;
  uint8_t end = 0;
  uint16_t uriLength = emStrlen(uri);

  if (uriLength > 0) {
    // skip past an initial slash because we don't need it
    if (uri[0] == '/') {
      uri++;
    }

    for (end = 0; end <= uriLength; end++) {
      if (uri[end] == '/' || end == uriLength) {
        finger = storeOptionAndLength(finger,
                                      COAP_OPTION_URI_PATH,
                                      (start == 0
                                       ? COAP_NO_OPTION
                                       : COAP_OPTION_URI_PATH),
                                      end - start);
        finger = lengthMemcopy(finger, uri + start, end - start);
        start = end + 1;
        assert(finger < buffer + bufferLength);
      }
    }
  }

  return finger - buffer;
}

// We need some kind of limit.  64 bytes is a long URI to go over 802.15.4.
#define MAX_ENCODED_URI 64

// + 1 for the 0xFF separating the options from the payload
#define MAX_COAP_HEADER_SIZE \
 (COAP_BASE_SIZE + COAP_MAX_TOKEN_LENGTH + MAX_ENCODED_URI + 1)

// Payloads that fit we put in the header buffer; big payloads go in their
// own buffer.
#define COAP_MAX_FLAT_PAYLOAD 100

EmberStatus emSubmitCoapMessage(CoapMessage *message,
                                const uint8_t *uri,
                                Buffer payloadBuffer,
                                EmberCoapOptions options)
{
  // 1 for 0b11111111
  uint8_t coapHeader[MAX_COAP_HEADER_SIZE + COAP_MAX_FLAT_PAYLOAD];
  uint8_t *finger = coapHeader;

  if (emberCoapIsRequest(message->public.code)) {
    emberStoreHighLowInt16u(message->public.token, nextToken);
    nextToken += 1;
    message->public.tokenLength = 2;
  }

  if (message->public.localPort == 0) {
    message->public.localPort = ((options & EMBER_COAP_SECURED)
                                ? EMBER_COAP_SECURE_PORT
                                : EMBER_COAP_PORT);
  }
  if (message->public.remotePort == 0) {
    message->public.remotePort = ((options & EMBER_COAP_SECURED)
                                 ? EMBER_COAP_SECURE_PORT
                                 : EMBER_COAP_PORT);
  }

  // store the version
  *finger++ = (COAP_VERSION
               | ((uint8_t)message->type << COAP_TYPE_SHIFT_AMOUNT)
               | message->public.tokenLength);

  // store the code
  *finger++ = message->public.code;

  // increment the message ID?
  if (message->type != COAP_TYPE_ACK
      && message->type != COAP_TYPE_RESET) {
    message->messageId = nextMessageId;
    nextMessageId += 1;
  }

  emberStoreHighLowInt16u(finger, message->messageId);
  finger += 2;

  assert(message->public.tokenLength <= COAP_MAX_TOKEN_LENGTH);
  finger = lengthMemcopy(finger,
                         message->public.token,
                         message->public.tokenLength);

  if (uri != NULL) {
    uint8_t *limit = finger + MAX_ENCODED_URI;
    // We save the encoded path for debug printing by emFinishCoapMessage.
    message->public.encodedUri = finger;
    finger += emberCoapEncodeUri(uri, finger, limit - finger);
    message->public.encodedUriLength = finger - message->public.encodedUri;
    finger = createContentFormatOption(COAP_OPTION_URI_PATH, finger);
    assert(finger <= limit);
  } else if (message->public.encodedUri != NULL) {
    assert(message->public.encodedUriLength <= MAX_ENCODED_URI);
    emLogBytesLine(COAP, "encodedUri",
                   message->public.encodedUri,
                   message->public.encodedUriLength);
    finger = lengthMemcopy(finger,
                           message->public.encodedUri,
                           message->public.encodedUriLength);
  }

  // store the <payload>
  uint16_t payloadLength = emTotalPayloadLength(payloadBuffer);

  if (0 < message->public.payloadLength
      || (payloadBuffer != NULL_BUFFER
          && 0 < payloadLength)) {
    *finger++ = 0xFF;
  }

  if (0 < message->public.payloadLength) {
    MEMCOPY(finger, message->public.payload, message->public.payloadLength);
    finger += message->public.payloadLength;
  }
  // </payload>

  assert(finger - coapHeader <= sizeof(coapHeader));

  return emApiFinishCoapMessage(message,
                                coapHeader,
                                finger - coapHeader,
                                payloadBuffer,
                                options);
}

EmberStatus emberCoapSend(EmberCoapMessage *publicMessage,
                          uint32_t responseTimeoutMs,
                          EmberCoapResponseHandler responseHandler,
                          void *applicationData,
                          uint16_t applicationDataLength)
{
  CoapMessage message;
  MEMSET(&message, 0, sizeof(message));
  MEMCOPY(&message.public, publicMessage, sizeof(EmberCoapMessage));
  if (responseHandler == NULL) {
    message.type = COAP_TYPE_NON_CONFIRMABLE;
  } else {
    message.type = COAP_TYPE_CONFIRMABLE;
    message.responseTimeoutMs = responseTimeoutMs;
    message.responseHandler = responseHandler;
    message.responseAppData = applicationData;
    message.responseAppDataLength = applicationDataLength;
  }
  return emSubmitCoapMessage(&message,
                             NULL,
                             NULL_BUFFER,
                             message.public.options);
}

//
// History Table
//
typedef struct {
  uint16_t messageId;
  uint8_t type;
  EmberIpv6Address remoteAddress;
  uint16_t remotePort;
  uint16_t localPort;
} MessageHistoryEntry;

static Buffer historyTable = NULL_BUFFER;

static bool compareMessageHistoryEntry(const ListElement *first,
                                       const ListElement *second)
{
  return (MEMCOMPARE((MessageHistoryEntry *)first,
                     (MessageHistoryEntry *)second,
                     sizeof(MessageHistoryEntry))
          == 0);
}

static const ListType historyTableType = {
  sizeof(MessageHistoryEntry),
  5,
  LIST_REMOVE_TAIL_WHEN_FULL,
  NULL,
  &compareMessageHistoryEntry,
  NULL
};

static void copyMessageHistoryEntry(MessageHistoryEntry *entry,
                                    const CoapMessage *from)
{
  // safety
  MEMSET(entry, 0, sizeof(MessageHistoryEntry));

  MEMCOPY(&entry->remoteAddress,
          &from->public.remoteAddress,
          sizeof(EmberIpv6Address));
  entry->remotePort = from->public.remotePort;
  entry->localPort = from->public.localPort;
  entry->messageId = from->messageId;
  entry->type = from->type;
}

HIDDEN bool isInMessageHistory(const CoapMessage *message)
{
  MessageHistoryEntry dummy;
  copyMessageHistoryEntry(&dummy, message);

  return (emListGet(&historyTableType,
                    historyTable,
                    (ListElement *) &dummy,
                    NULL)
          != NULL);
}

HIDDEN void addMessageHistoryEntry(const CoapMessage *coapMessage)
{
  MessageHistoryEntry entry;
  copyMessageHistoryEntry(&entry, coapMessage);

  emListAdd(&historyTableType,
            &historyTable,
            (const ListElement *)&entry,
            NULL);
}

void emCoapMarkBuffers(void)
{
  emListMark(&historyTableType, &historyTable, emMarkBuffer);
}

EmberStatus emberCoapSendUri(EmberCoapCode code,
                             const EmberIpv6Address *destination,
                             const uint8_t *uri,
                             const uint8_t *body,
                             uint16_t bodyLength,
                             EmberCoapResponseHandler responseHandler)
{
  EmberCoapMessage message;
  MEMSET(&message, 0, sizeof(message));
  uint8_t coapOptions[256];
  message.code = code;
  message.encodedUri = coapOptions;
  message.encodedUriLength =
    emberCoapEncodeUri(uri, coapOptions, sizeof(coapOptions));
  MEMCOPY(message.remoteAddress.bytes, destination, 16);
  message.payload = body;
  message.payloadLength = bodyLength;
  return emberCoapSend(&message, 0, responseHandler, NULL, 0);
}

static const uint8_t *getOption(const uint8_t *data,
                              uint16_t dataLength,
                              CoapOption wantedOption,
                              uint16_t *optionLength)
{
  if (optionLength != NULL) {
    *optionLength = 0;
  }

  CoapOption previousOption = COAP_NO_OPTION;
  const uint8_t *result = NULL;
  const uint8_t *finger =
    data + COAP_BASE_SIZE + (data[0] & COAP_TOKEN_LENGTH_MASK);
  const uint8_t *limit = data + dataLength;

  while (finger < limit && *finger != END_BYTE) {
    CoapOption option;
    uint16_t length;
    const uint8_t *optionStart = finger;
    finger = fetchOptionAndLength(finger,
                                  previousOption,
                                  &option,
                                  &length);
    if (option > wantedOption) {
      break;
    } else if (option == wantedOption) {
      if (result == NULL) {
        result = optionStart;
      }

      if (optionLength != NULL) {
        *optionLength += (finger - optionStart) + length;
      }
    }

    previousOption = option;
    finger += length;
  }

  // if we're searching for END_BYTE and we have it, then return it
  if (finger < limit
      && *finger == END_BYTE
      && (int)wantedOption == END_BYTE) {
    return (finger + 1 < limit
            ? finger + 1
            : NULL);
  }

  return result;
}

// TODO: add failure cases for parsing
bool emParseCoapMessage(const uint8_t *data,
                        uint16_t dataLength,
                        CoapMessage *coapMessage)
{
  MEMSET(coapMessage, 0, sizeof(CoapMessage));
  coapMessage->public.tokenLength = data[0] & COAP_TOKEN_LENGTH_MASK;

  if ((data[0] & COAP_VERSION_MASK) != COAP_VERSION) {
    return false;
  }

  coapMessage->type = (CoapType)((data[0] & COAP_TYPE_MASK)
                                 >> COAP_TYPE_SHIFT_AMOUNT);
  coapMessage->public.code = (EmberCoapCode)data[1];
  coapMessage->messageId = emberFetchHighLowInt16u(data + 2);
  MEMCOPY(coapMessage->public.token,
          data + COAP_TOKEN_INDEX,
          coapMessage->public.tokenLength);
  coapMessage->public.encodedUri = getOption(data,
                                   dataLength,
                                   COAP_OPTION_URI_PATH,
                                   &coapMessage->public.encodedUriLength);
  const uint8_t *endPosition = getOption(data,
                                       dataLength,
                                       (CoapOption)END_BYTE,
                                       NULL);

  if (endPosition != NULL) {
    coapMessage->public.payloadLength = dataLength - (endPosition - data);
    coapMessage->public.payload = data + (endPosition - data);
  }

  return true;
}

// Resets and non-piggybacking acks

static void sendResponse(CoapMessage *coapMessage, CoapType type)
{
  CoapMessage reply;
  MEMSET(&reply, 0, sizeof(reply));
  reply.type = type;
  reply.messageId = coapMessage->messageId;
  MEMCOPY(&reply.public.localAddress,  &coapMessage->public.localAddress,  16);
  MEMCOPY(&reply.public.remoteAddress, &coapMessage->public.remoteAddress, 16);
  reply.public.localPort = coapMessage->public.localPort;
  reply.public.remotePort = coapMessage->public.remotePort;
  reply.dtlsHandle = coapMessage->dtlsHandle;
  
  emSubmitCoapMessage(&reply,
                      NULL,
                      NULL_BUFFER,
                      coapMessage->public.options & EMBER_COAP_SECURED);
}

void emProcessCoapMessage(CoapMessage *coapMessage,
                          CoapMessageHandler *messageHandler)
{
  

  switch (coapMessage->type) {

  case COAP_TYPE_RESET: {
    CoapRetryEvent *event = findRetryEventByMessageId(coapMessage->messageId);

    emLogLine(COAP,
              "Received RESET for message %u",
              coapMessage->messageId);

    if (event != NULL
        && event->handler != NULL) {
      callResponseHandler(EMBER_COAP_MESSAGE_RESET, coapMessage, event);
    }
    return;
  }

  case COAP_TYPE_ACK: {
    CoapRetryEvent *event = findRetryEventByMessageId(coapMessage->messageId);

    if (event == NULL) {
      // Do nothing.  It's a duplicate, or the sender is confused.
    } else if (event->handler == NULL) {
      if (coapMessage->public.code != EMBER_COAP_CODE_EMPTY) {
        // Temporary until we have handlers in place for everyone.
        currentCoapMessage = coapMessage;
        messageHandler(coapMessage);
        if (currentCoapMessage != NULL
            && coapMessage->type == COAP_TYPE_CONFIRMABLE) {
          sendResponse(coapMessage, COAP_TYPE_ACK);
        }
        currentCoapMessage = NULL;
      }
    } else if (coapMessage->public.code == EMBER_COAP_CODE_EMPTY) {
      if (event->responseTimeoutMs == 0) {
        callResponseHandler(EMBER_COAP_MESSAGE_ACKED, coapMessage, event);
      } else {
        event->packetHeader = NULL_BUFFER;
        emberEventSetDelayMs((Event *) event, event->responseTimeoutMs);
      }
    } else {    // have handler and non-empty message
      // TODO: check that the token matches
      callResponseHandler(EMBER_COAP_MESSAGE_RESPONSE, coapMessage, event);
    }
    return;
  }
    
  case COAP_TYPE_CONFIRMABLE:
  case COAP_TYPE_NON_CONFIRMABLE:
    if (coapMessage->public.code == EMBER_COAP_CODE_EMPTY) {
      // RFC says to send a reset in response to an empty CON or NON message.
      sendResponse(coapMessage, COAP_TYPE_RESET);
      return;
    } else if (! isInMessageHistory(coapMessage)) {
      addMessageHistoryEntry(coapMessage);
    } else {
      emLogLine(COAP,
                "Message %u is already in history",
                coapMessage->messageId);
      if (coapMessage->type == COAP_TYPE_CONFIRMABLE) {
        sendResponse(coapMessage, COAP_TYPE_ACK);
      }
      return;
    }

    currentCoapMessage = coapMessage;

    if (GET_COAP_CLASS(coapMessage->public.code) == EMBER_COAP_CLASS_REQUEST) {
      messageHandler(coapMessage);
    } else if (coapMessage->public.tokenLength == 2) {
      CoapRetryEvent *event =
        findRetryEventByToken(emberFetchHighLowInt16u(coapMessage->public.token));
      if (event != NULL) {
        callResponseHandler(EMBER_COAP_MESSAGE_RESPONSE, coapMessage, event);
      }
    } else {
      emLogLine(COAP,
                "Response with funny token length %d",
                coapMessage->public.tokenLength);
    }

    if (currentCoapMessage != NULL
        && coapMessage->type == COAP_TYPE_CONFIRMABLE) {
      sendResponse(coapMessage, COAP_TYPE_ACK);
    }
    currentCoapMessage = NULL;
    
    break;
  }
}

static bool coapRespond(EmberCoapCode responseCode,
                        const uint8_t *payload,
                        uint16_t payloadLength,
                        bool includeUri)
{
  if (currentCoapMessage == NULL) {
    return false;       // nothing to respond to
  }

  // Only send resets in response to confirmable messages.
  if (responseCode == EMBER_COAP_CODE_EMPTY) {
    if (currentCoapMessage->type == COAP_TYPE_CONFIRMABLE) {
      sendResponse(currentCoapMessage, COAP_TYPE_RESET);
    }
    return true;
  }

  CoapMessage reply;
  MEMSET(&reply, 0, sizeof(reply));
  
  if (currentCoapMessage->type == COAP_TYPE_CONFIRMABLE) {
    reply.type = COAP_TYPE_ACK;
    reply.messageId = currentCoapMessage->messageId;
  } else {
    reply.type = COAP_TYPE_NON_CONFIRMABLE;
  }

  reply.public.code = responseCode;
  MEMCOPY(&reply.public.remoteAddress, &currentCoapMessage->public.remoteAddress, 16);
  MEMCOPY(&reply.public.localAddress,  &currentCoapMessage->public.localAddress,  16);
  reply.public.localPort = currentCoapMessage->public.localPort;
  reply.public.remotePort = currentCoapMessage->public.remotePort;
  reply.dtlsHandle = currentCoapMessage->dtlsHandle;
  MEMCOPY(reply.public.token,
          currentCoapMessage->public.token,
          currentCoapMessage->public.tokenLength);
  reply.public.tokenLength = currentCoapMessage->public.tokenLength;
  reply.public.payload = payload;
  reply.public.payloadLength = payloadLength;
  if (includeUri) {
    reply.public.encodedUri = currentCoapMessage->public.encodedUri;
    reply.public.encodedUriLength = currentCoapMessage->public.encodedUriLength;
  }

  EmberCoapOptions options = (currentCoapMessage->public.options
                              & EMBER_COAP_SECURED);
  currentCoapMessage = NULL;
  return (emSubmitCoapMessage(&reply, NULL, NULL_BUFFER, options)
          == EMBER_SUCCESS);
}

bool emberCoapRespond(EmberCoapCode responseCode,
                      const uint8_t *payload,
                      uint16_t payloadLength)
{
  return coapRespond(responseCode, payload, payloadLength, false);
}

bool emCoapRespondWithUri(EmberCoapCode responseCode,
                          const uint8_t *payload,
                          uint16_t payloadLength)
{
  return coapRespond(responseCode, payload, payloadLength, true);
}

void emCoapInitialize(void)
{
  nextMessageId = halCommonGetRandom();
  nextToken = halCommonGetRandom();
  historyTable = NULL_BUFFER;
}

bool emberCoapDecodeUri(const uint8_t *rawUri,
                        uint16_t rawUriLength,
                        uint8_t *convertedUri,
                        uint16_t convertedUriLength)
{
  const uint8_t *rawFinger = rawUri;
  const uint8_t *rawLimit = rawFinger + rawUriLength;
  uint8_t *convertedFinger = convertedUri;
  const uint8_t *convertedLimit = convertedUri + convertedUriLength;
  CoapOption previousOption = COAP_NO_OPTION;

  while (rawFinger < rawLimit) {
    uint16_t length = 0;
    CoapOption option = COAP_NO_OPTION;
    rawFinger = fetchOptionAndLength(rawFinger, previousOption, &option, &length);

    if ((convertedFinger + length) >= convertedLimit) {
      return false;
    }

    MEMCOPY(convertedFinger, rawFinger, length);
    convertedFinger += length;
    rawFinger += length;

    if (rawFinger < rawLimit) {
      *convertedFinger++ = '/';
    }
  }

  if (convertedFinger >= convertedLimit) {
    return false;
  }

  *convertedFinger = 0;

  return true;
}

bool emberCoapIsRequest(EmberCoapCode code)
{
  // Codes 0.01--0.31 indicate a request.  Note that 0.00 indicates an empty
  // message and is not a request.
  return (code != 0 && GET_COAP_CLASS(code) == EMBER_COAP_CLASS_REQUEST);
}

bool emberCoapIsResponse(EmberCoapCode code)
{
  // Codes 2.00--5.31 indicate a response.
  uint8_t class = GET_COAP_CLASS(code);
  return (EMBER_COAP_CLASS_SUCCESS_RESPONSE <= class
          && class <= EMBER_COAP_CLASS_SERVER_ERROR_RESPONSE);
}
