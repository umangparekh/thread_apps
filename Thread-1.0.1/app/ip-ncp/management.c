// File: management.c
//
// Description: Functions for sending management commands between the
// host and the NCP.
//
// Copyright 2012 by Silicon Laboratories. All rights reserved.             *80*

#include <stdio.h>

#include PLATFORM_HEADER
#include "stack/include/ember.h"
#include "hal/hal.h"
#include "uart-link-protocol.h"
#include "ip-modem-link.h"
#include "management.h"
#include "app/ip-ncp/serial-link.h"

void emSendManagementCommand(const char *command, const char *format, ...)
{
  va_list argumentList;
  va_start(argumentList, format);
  uint8_t buffer[256];            // uart-link-ip.c has max message size of 127.

  buffer[0] = '[';
  buffer[1] = UART_LINK_TYPE_MANAGEMENT;
  buffer[4] = MANAGEMENT_COMMAND;

  char *finger = (char *) buffer + 5;
  finger += sprintf(finger, "%s ", command);
  while (*format != 0) {
    switch (*format) {
    case 'u':
    case 's':
    case 'v':
    case 'w': {
      int value = va_arg(argumentList, int);
      finger += sprintf(finger, "%d ", value);
      break;
    }
    case 'b': {
      //
      // data bytes, the length is in the argument after it
      //
      const uint8_t *data = va_arg(argumentList, const uint8_t*);
      int dataSize = va_arg(argumentList, int);
      finger += sprintf(finger, "{");
      uint8_t i;
      for (i = 0; i < dataSize; i++) {
        finger += sprintf(finger, "%02x", data[i]);
      }
      finger += sprintf(finger, "} ");
      break;
    }
    case 'n':
      finger += sprintf(finger, "%s ", va_arg(argumentList, uint8_t *));
      break;
    case 'N':
      finger += sprintf(finger, "\"%s\" ", va_arg(argumentList, uint8_t *));
      break;
    default:
      // we shouldn't get here, an invalid input character was given
      assert(false);
      break;
    }
    format++;
  }
  va_end(argumentList);
  finger += sprintf(finger, "\r\n");
  uint16_t length = (uint8_t *) finger - buffer;
  // Length includes the MANAGEMENT_COMMAND, but not the rest of the header.
  emberStoreHighLowInt16u(buffer + 2, length - 4);
  emReallySendManagementCommand(buffer, length);
}
