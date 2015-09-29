// File: host-app-utilities.c
//
// Description: Utilities for a host.
//
// Copyright 2015 by Silicon Laboratories. All rights reserved.                *80*

#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stddef.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h>

#ifdef __linux__
  #include <sys/sysctl.h>
  #include <linux/sysctl.h>
  #include <sys/syscall.h>
#endif

#include PLATFORM_HEADER
#include "stack/core/ember-stack.h"
#include "hal/hal.h"
#include "phy/phy.h"
#include "stack/ip/ip-header.h"
#include "stack/ip/ip-address.h"
#include "stack/framework/ip-packet-header.h"
#include "stack/mac/802.15.4/mac.h"
#include "stack/ip/host/host-address-table.h"
#include "stack/ip/host/host-listener-table.h"
#include "app/ip-ncp/binary-management.h"
#include "app/tmsp/tmsp-enum.h"
#include "stack/framework/event-queue.h"

//------------------------------------------------------------------------------
// Tasks.

EventQueue emStackEventQueue;

//------------------------------------------------------------------------------
// Framework.

void emberInit(void)
{
  emInitializeBuffers();
  emInitializeEventQueue(&emStackEventQueue);
  emberInitializeListeners();
  emberInitializeHostAddressTable();
  emSendBinaryManagementCommand(EMBER_INIT_HOST_COMMAND_IDENTIFIER, "");
}

void markSimBuffers(void);

void markEventQueue(void)
{
  emberMarkEventQueue(&emStackEventQueue);
}

extern void emCoapMarkBuffers(void);

static BufferMarker const markers[] = {
  emberMarkApplicationBuffers,
  markEventQueue,
  emCoapMarkBuffers,
  #ifdef UNIX_HOST_SIM
    markSimBuffers,
  #endif
  NULL
};

void emberTick(void)
{
  emberListenerTick();
  emReclaimUnusedBuffers(markers);
  #ifndef UNIX_HOST_SIM
    usleep(100);
  #endif
}

EmberNodeId emberGetNodeId(void) { return EM_USE_LONG_ADDRESS; }
bool emIsNdNewNodeId(EmberNodeId shortId) { return false; }