// Copyright 2015 Silicon Laboratories, Inc.

#include PLATFORM_HEADER
#include CONFIGURATION_HEADER
#include EMBER_AF_API_STACK
#include EMBER_AF_API_HAL
#include EMBER_AF_API_COMMAND_INTERPRETER2
#ifdef EMBER_AF_API_DEBUG_PRINT
  #include EMBER_AF_API_DEBUG_PRINT
#endif

// udp listen <port:2> <address>
void udpListenCommand(void)
{
  uint16_t port;
  EmberIpv6Address address;
  EmberStatus status;

  port = (uint16_t)emberUnsignedCommandArgument(0);
  if (!emberGetIpArgument(1, address.bytes)) {
    emberAfAppPrintln("%p: %p", "ERR", "invalid ip");
    return;
  }

  status = emberUdpListen(port, address.bytes);
  emberAfAppPrintln("%p 0x%x", "listen", status);
}

// udp send <options:1> <destination> <hop limit:1> <source port:2> <destination port:2> <payload>
void udpSendCommand(void)
{
  uint8_t options;
  EmberIpv6Address destination;
  uint8_t hopLimit;
  uint16_t sourcePort;
  uint16_t destinationPort;
  uint8_t *payload;
  uint8_t payloadLength;
  EmberStatus status;

  options = (uint8_t)emberUnsignedCommandArgument(0);

  if (!emberGetIpArgument(1, destination.bytes)) {
    emberAfAppPrintln("%p: %p", "ERR", "invalid ip");
    return;
  }

  hopLimit = (uint8_t)emberUnsignedCommandArgument(2);

  sourcePort = (uint16_t)emberUnsignedCommandArgument(3);
  destinationPort = (uint16_t)emberUnsignedCommandArgument(4);

  payload = emberStringCommandArgument(5, &payloadLength);

  status = emberSendUdp(options,
                        destination.bytes,
                        hopLimit,
                        sourcePort,
                        destinationPort,
                        payload,
                        payloadLength);
  emberAfAppPrintln("%p 0x%x", "send", status);
}
