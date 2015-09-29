// File: unix-udp-wrapper.c
//
// Description: Simple UDP API implemented using POSIX sockets.
//
// Copyright 2015 by Silicon Laboratories. All rights reserved.                *80*

#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <netinet/in.h>
#include <net/if.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <time.h>

#include "stack/core/ember-stack.h"
#include "stack/ip/ip-address.h"
#include "stack/ip/host/unix-interface.h"
#include "stack/ip/tls/native-test-util.h"
#include "stack/ip/host/host-listener-table.h"

EmberStatus emberUdpListen(uint16_t port, const uint8_t *localAddress)
{
  if (emIsUnspecifiedAddress(localAddress)) {
    return EMBER_ERR_FATAL;
  }
  if (emberFindListener(port, localAddress) != NULL) {
    return EMBER_SUCCESS;
  }

  // The localAddress variable is used to glean the source address
  // associated with a particular listener.  For multicast listeners,
  // we use the mesh local address as source.
  bool isMulticast = emIsMulticastAddress(localAddress);
  EmberIpv6Address meshLocalAddress;
  emberGetLocalIpAddress(0, &meshLocalAddress);

  HostListener *listener = emberAddListener(port,
                                            (isMulticast
                                             ? meshLocalAddress.bytes
                                             : localAddress),
                                            SOCK_DGRAM, 0); // UDP

  if (listener == NULL) {
    return EMBER_TABLE_FULL;
  }

  if (listener->socket == INVALID_SOCKET) {
    return EMBER_ERR_FATAL;
  }

  int interfaceIndex = if_nametoindex(emUnixInterface);
  struct ipv6_mreq mreq6 = {0};
  MEMCOPY(&mreq6.ipv6mr_multiaddr.s6_addr, localAddress, 16);
  mreq6.ipv6mr_interface = interfaceIndex;
  int mcastTTL = 10;
  int loopBack = 1;

  if (setsockopt(listener->socket,
                 IPPROTO_IPV6,
                 IPV6_MULTICAST_IF,
                 &interfaceIndex,
                 sizeof(interfaceIndex))
      < 0) {
    perror("setsockopt:: IPV6_MULTICAST_IF:: ");
    return EMBER_ERR_FATAL;
  }

  if (setsockopt(listener->socket,
                 IPPROTO_IPV6,
                 IPV6_MULTICAST_LOOP,
                 &loopBack,
                 sizeof(loopBack))
      < 0) {
    perror("setsockopt:: IPV6_MULTICAST_LOOP:: ");
    return EMBER_ERR_FATAL;
  }

  if (setsockopt(listener->socket,
                 IPPROTO_IPV6,
                 IPV6_MULTICAST_HOPS,
                 &mcastTTL,
                 sizeof(mcastTTL))
      < 0) {
    perror("setsockopt:: IPV6_MULTICAST_HOPS::  ");
    return EMBER_ERR_FATAL;
  }

  if (isMulticast) {
    if (setsockopt(listener->socket,
                   IPPROTO_IPV6,
                   IPV6_JOIN_GROUP,
                   &mreq6,
                   sizeof(mreq6))
        < 0) {
      perror("setsockopt:: IPV6_JOIN_GROUP:: ");
      return EMBER_ERR_FATAL;
    }
  }

  return EMBER_SUCCESS;
}

EmberStatus emberSendUdp(uint8_t options,
                         const uint8_t *destination,
                         uint8_t hopLimit,
                         uint16_t sourcePort,
                         uint16_t destinationPort,
                         uint8_t *payload,
                         uint16_t payloadLength)
{
  uint8_t source[16];
  if (! emStoreIpSourceAddress(source, destination)) {
    return EMBER_BAD_ARGUMENT;
  }

  HostListener *listener = emberFindListener(sourcePort, source);
  if (listener != NULL) {
    struct sockaddr_in6 outSock = {0};
    outSock.sin6_family = AF_INET6;
    outSock.sin6_port = htons(destinationPort);
    MEMCOPY(outSock.sin6_addr.s6_addr, destination, 16);
    int interfaceIndex = if_nametoindex(emUnixInterface);
    outSock.sin6_scope_id = interfaceIndex;

    nativeWrite(listener->socket,
                payload,
                payloadLength,
                (struct sockaddr *)&outSock,
                sizeof(outSock));
    emberCounterHandler(EMBER_COUNTER_UDP_OUT, 1);

    return EMBER_SUCCESS;
  } else {
    fprintf(stderr, "No listener on sourcePort:%d\n",
            sourcePort);
    return EMBER_INVALID_CALL;
  }
}
