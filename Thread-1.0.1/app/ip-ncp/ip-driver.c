// File: ip-driver.c
//
// Description: host-side driver for the IP NCP.
//
// Copyright 2013 by Silicon Laboratories. All rights reserved.             *80*

#include <stdlib.h>
#include <unistd.h>

#include PLATFORM_HEADER
#include "stack/core/ember-stack.h"
#include "stack/core/log.h"
#include "app/ip-ncp/uart-link-protocol.h"
#include "app/ip-ncp/host-stream.h"
#include "app/ip-ncp/ip-driver.h"

#ifdef UNIX_HOST
  #include "ip-driver-log.h"
  #define LOG(x) x
#else
  #define LOG(x)
#endif

void emProcessNcpManagementCommand(SerialLinkMessageType type,
                                   const uint8_t *message,
                                   uint16_t length)
{
  uint8_t managementType = message[0];
  const uint8_t *body = message + 1;
  uint16_t identifier = emberFetchHighLowInt16u(body);

  // forwarded to the host
  LOG(ipDriverLogEvent(LOG_DRIVER_TO_APP, message, length, type);)
  ipModemWrite(driverManagementFd, type, message, length);
}
