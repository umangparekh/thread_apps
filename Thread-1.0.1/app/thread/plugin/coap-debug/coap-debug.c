// Copyright 2015 Silicon Laboratories, Inc.

#include PLATFORM_HEADER
#include CONFIGURATION_HEADER
#include EMBER_AF_API_STACK
#include EMBER_AF_API_HAL
#ifdef EMBER_AF_API_DEBUG_PRINT
  #include EMBER_AF_API_DEBUG_PRINT
#endif

#ifndef ALIAS
  #define ALIAS(x) x
#endif

void ALIAS(emberCoapMessageHandler)(const EmberIpv6Address *source,
                                    EmberCoapCode code,
                                    const uint8_t *token,
                                    uint8_t tokenLength,
                                    const uint8_t *uriPath,
                                    uint16_t uriPathLength,
                                    const uint8_t *payload,
                                    uint16_t payloadLength)
{
  uint8_t convertedUri[100] = {0};
  bool converted;

  emberAfCorePrint("CoAP RX:");

  emberAfCorePrint(" s=");
  emberAfCoreDebugExec(emberAfPrintIpv6Address(source));

  emberAfCorePrint(" ");
  switch (code) {
  case EMBER_COAP_CODE_GET:
    emberAfCorePrint("GET");
    break;
  case EMBER_COAP_CODE_POST:
    emberAfCorePrint("POST");
    break;
  case EMBER_COAP_CODE_PUT:
    emberAfCorePrint("PUT");
    break;
  case EMBER_COAP_CODE_DELETE:
    emberAfCorePrint("DELETE");
    break;
  default:
    emberAfCorePrint("0x%x", code);
    break;
  }

  // When converting, always reserve an extra space at the end of our buffer
  // for a null terminator.  This allows us to safely print the converted URI,
  // even if our buffer was too small to hold the entire thing.
  converted = emberCoapConvertUri(uriPath,
                                  uriPathLength,
                                  convertedUri,
                                  sizeof(convertedUri) - 1);
  emberAfCorePrint(" u=%s", convertedUri);
  if (!converted) {
    emberAfCorePrint(" (truncated)");
  }

  if (tokenLength != 0) {
    emberAfCorePrint(" t=");
    emberAfCorePrintBuffer(token, tokenLength, false);
  }

  if (payloadLength != 0) {
    emberAfCorePrint(" p=");
    emberAfCorePrintBuffer(payload, payloadLength, false);
  }

  emberAfCorePrintln("");
}
