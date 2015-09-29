// File: ip-driver-app.c
//
// Description: host-side driver application for the IP NCP.
//
// Copyright 2013 by Silicon Laboratories. All rights reserved.             *80*
//----------------------------------------------------------------

// This file is the main dispatch for the IP modem driver.  This file
// manages communication with three separate entities:
//   - serial connection to the IP modem
//   - connection to a management application
//   - connection to an IP stack, acting as IP interface
// The first two connections use the message format described in
// uart-link-protocol.h.  The connection with the IP stack send
// and receives IPv6 packets.
// 
// Utilities for reading and writing to these connections are in
// host-stream.c. All of the platform-specific code should be in
// either this file or that one.
// 
// The platform-independent message processing code is in ip-driver.c.

#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>

#ifdef __linux__
  #include <linux/if_tun.h>
#endif

#include PLATFORM_HEADER
#include "stack/core/ember-stack.h"
#include "stack/core/log.h"
#include "hal/hal.h"
#include "phy/phy.h"
#include "app/util/serial/serial.h"
#include "app/util/serial/command-interpreter2.h"
#include "app/util/serial/command-interpreter2-util.h"

#include "uart-link-protocol.h"
#include "host-stream.h"
#include "data-client.h"
#include "ip-driver.h"
#include "app/tmsp/tmsp-enum.h"
#include "hal/micro/generic/ash-v3.h"
#include "app/ip-ncp/ncp-uart-interface.h"
#include "stack/framework/event-queue.h"

#ifdef UNIX_HOST
  #define EMBER_READ   read
  #define EMBER_WRITE  write
  #define EMBER_SELECT select
  #include "ip-driver-log.h"
  #define LOG(x) x
#else
  // simulated I/O for testing
  #include "tool/simulator/child/posix-sim.h"
  #define LOG(x)
#endif

bool logEnabled = false;

//----------------------------------------------------------------
// Interface to ip-driver.c.

// connects to Host
int driverManagementFd = -1;

Stream dataStream = {{0}};
Stream managementStream = {{0}};

// What management clients connect to.
int mgmtListenFd = -1;   // listen for managment client

// Data messages from the IP stack are forwarded to the NCP.

#define IPV6_DESTINATION_ADDRESS_INDEX  24

void txBufferFullHandler(const uint8_t *packet,
                         uint16_t packetLength,
                         uint16_t lengthRemaining)
{
  assert(false);
}

void txFailedHandler(uint8_t fd,
                     const uint8_t *packet,
                     uint16_t packetLength,
                     uint16_t lengthRemaining)
{
  assert(false);
}

void dataHandler(const uint8_t *packet, uint16_t length)
{
  uint8_t type = UART_LINK_TYPE_THREAD_DATA;

  // Testing hack: dispatch to alarm network based on magic dest IP addresses.
  // This should really be done via separate dataStreams.
  uint8_t *destination = (uint8_t *)packet + IPV6_DESTINATION_ADDRESS_INDEX;
  if ((destination[0] == 0xFE
       && (destination[1] == 0x90
           || destination[1] == 0x91))
      || (destination[0] == 0xFF
          && destination[1] == 0x0A)) {
    type = UART_LINK_TYPE_ALT_DATA;
  }

  LOG(ipDriverLogEvent(LOG_IP_STACK_TO_NCP, packet, length, type);)
  emLogBytesLine(IP_MODEM, "DATA app->ncp ", packet, length);

  ipModemWrite(driverNcpFd, type, packet, length);
}

// Messages from the management client are forwarded to the NCP.

void managementHandler(SerialLinkMessageType type,
                       const uint8_t *message,
                       uint16_t length)
{
  LOG(ipDriverLogEvent(LOG_APP_TO_NCP, message, length, type);)
  emLogLine(IP_MODEM, "MGMT app->ncp '%s' (%d)", message, length);
  const uint8_t *body = message + 1;
  uint16_t identifier = emberFetchHighLowInt16u(body);
  if (identifier == EMBER_FF_WAKEUP_COMMAND_IDENTIFIER) {
    // 0xFF byte to wake the NCP
    uint8_t ffByte = 0xFF;
    assert(EMBER_WRITE(driverNcpFd, &ffByte, 1) == 1);
  } else if (identifier == EMBER_RESET_IP_DRIVER_ASH_COMMAND_IDENTIFIER
             && ncpUartUseAsh) {
    uartLinkReset();
  } else if (identifier == EMBER_RESET_NCP_GPIO_COMMAND_IDENTIFIER
             && ncpUartUseAsh) {
    resetNcp();
  } else if (identifier == EMBER_ENABLE_RESET_NCP_GPIO_COMMAND_IDENTIFIER
             && ncpUartUseAsh) {
    body += sizeof(identifier);
    enableResetNcp(*body);
  } else {
    // Everything else.
    ipModemWrite(driverNcpFd, type, message, length);
  }
}

bool halHostUartTxIdle(void)
{
#ifdef UNIX_HOST
  int value;
  ioctl(driverNcpFd, TIOCMGET, &value);
  return ((value & TIOCM_CTS) != 0);
#else
  // we're always idle in simulation
  return true;
#endif
}

void emNotifyTxComplete(void)
{
  // nothing to do
}

void halHostUartLinkTx(const uint8_t *data, uint16_t length)
{
  assert(EMBER_WRITE(driverNcpFd, data, length) == length);

  if (ncpUartUseAsh) {
    emAshReallyNotifyTxComplete(false);
  }
}

void emberResetSerialState(void)
{
}

//----------------------------------------------------------------
// Command interpreter

// No actual commands, but we are required to supply a default command table.
EmberCommandEntry emberCommandTable[] = {
  { NULL }
};

//----------------------------------------------------------------
// Main

const char * const emAppName = "ip-driver";

bool initIpDriver(int argc, char **argv);

#ifdef UNIX_HOST
  #define RETURN_TYPE int
#else
  #define RETURN_TYPE void
  #define argc 0
  #define argv NULL
#endif

void ipDriverTick(void);

EventQueue emApiAppEventQueue;

RETURN_TYPE main(MAIN_FUNCTION_PARAMETERS)
{
  halInit();
  INTERRUPTS_ON();

  emInitializeEventQueue(&emApiAppEventQueue);

  openNcpReset();
  resetNcp();

  emberSerialInit(1, BAUD_115200, PARITY_NONE, 1);
  assert(initIpDriver(argc, argv));

  if (ncpUartUseAsh) {
    uartLinkReset();
  }

  emberInit();

  while (true) {
    halResetWatchdog();
    emberTick();
    emberRunEventQueue(&emApiAppEventQueue);
    ipDriverTick();
  }

#ifdef UNIX_HOST
  return 0;
#endif
}

// stubs
EmberNodeId emberGetNodeId(void) { return EM_USE_LONG_ADDRESS; }
EmberPanId emApiGetPanId(void) { return EM_BROADCAST_PAN_ID; }

#ifndef EMBER_TEST
#define USE_STUB_emMarkBufferPointer
#endif
#define USE_STUB_emLocalEui64
#define USE_STUB_halButtonIsr
#include "stack/ip/stubs.c"
