// Copyright 2015 Silicon Laboratories, Inc.

#include PLATFORM_HEADER
#include CONFIGURATION_HEADER
#include EMBER_AF_API_STACK
#include EMBER_AF_API_HAL
#include EMBER_AF_API_COMMAND_INTERPRETER2
#ifdef EMBER_AF_API_DEBUG_PRINT
  #include EMBER_AF_API_DEBUG_PRINT
#endif

static void processResponse(EmberCoapStatus status,
                            EmberCoapMessage *coap,
                            void *appData,
                            uint16_t appDatalength)
{
}

// coap get <destination> <uri>
void coapGetCommand(void)
{
  EmberIpv6Address destination;
  uint8_t *uri;
  EmberStatus status;

  if (!emberGetIpArgument(0, destination.bytes)) {
    emberAfAppPrintln("%p: %p", "ERR", "invalid ip");
    return;
  }

  uri = emberStringCommandArgument(1, NULL);

  status = emberCoapGet(&destination, uri, processResponse);
  emberAfAppPrintln("%p 0x%x", "get", status);
}

// coap post <destination> <uri> <body>
void coapPostCommand(void)
{
  EmberIpv6Address destination;
  uint8_t *uri;
  uint8_t *body;
  uint8_t bodyLength;
  EmberStatus status;

  if (!emberGetIpArgument(0, destination.bytes)) {
    emberAfAppPrintln("%p: %p", "ERR", "invalid ip");
    return;
  }

  uri = emberStringCommandArgument(1, NULL);
  body = emberStringCommandArgument(2, &bodyLength);

  status = emberCoapPost(&destination,
                         uri,
                         body,
                         bodyLength,
                         processResponse);
  emberAfAppPrintln("%p 0x%x", "post", status);
}
