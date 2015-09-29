// File: unix-listeners.c
//
// Description: Utilities for Unix listeners.
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

  // Linux support for SO_REUSEPORT is strange.  It seems to support the
  // semantics of SO_REUSEPORT, but doesn't have the macro defined.  We define
  // it manually if it doesn't exist and hope for the best.
  #ifndef SO_REUSEPORT
    #define SO_REUSEPORT_ASSUMED
    #define SO_REUSEPORT 15
  #endif
#endif

#include PLATFORM_HEADER
#include "stack/core/ember-stack.h"
#include "stack/ip/host/host-listener-table.h"
#include "stack/ip/host/unix-interface.h"
#include "stack/ip/tls/native-test-util.h"

int emberBindListener(uint16_t port,
                      const uint8_t *localAddress,
                      int type,
                      int protocol)
{
  int fd = socket(AF_INET6, type, protocol);

  if (fd == INVALID_SOCKET) {
    perror("socket");
    return INVALID_SOCKET;
  }

  int flags = fcntl(fd, F_GETFL);
  assert(fcntl(fd, F_SETFL, flags | O_NONBLOCK) != -1);

  int on = 1;
  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) != 0) {
    perror("set SO_REUSEADDR");
    return INVALID_SOCKET;
  }

#ifdef SO_REUSEPORT
  on = 1;
  if (setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on)) != 0) {
  // Don't error out if the system was correct in its statement that
  // SO_RESUSEPORT is unavailable.
#ifndef SO_REUSEPORT_ASSUMED    
    perror("set SO_REUSEPORT");
    return INVALID_SOCKET;
#endif
  }
#endif

  struct sockaddr_in6 address;
  MEMSET((char *) &address, 0, sizeof(address));
  address.sin6_family = AF_INET6;
  address.sin6_port = htons(port);
  char ipAddressString[INET6_ADDRSTRLEN];
  if (inet_ntop(AF_INET6, localAddress, ipAddressString, sizeof(ipAddressString)) == NULL) {
    return INVALID_SOCKET;
  }
  if (inet_pton(AF_INET6, ipAddressString, (void *)&address.sin6_addr.s6_addr) != 1) {
    return INVALID_SOCKET;
  }

  int interfaceIndex = if_nametoindex(emUnixInterface);
  if (interfaceIndex == 0) {
    fprintf(stderr, "invalid interface\n");
    return INVALID_SOCKET;
  }
  address.sin6_scope_id = interfaceIndex;

  // if we're reading a script, don't really perform the bind
  if (! emReadingTrace) {
    if (bind(fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
      perror("bind");
      return INVALID_SOCKET;
    }
  }

  return fd;
}

void emberCheckIncomingListener(const HostListener *listener)
{
  if (listener->socket == INVALID_SOCKET) {
    return;
  }
  uint8_t received[1000] = {0};
  struct sockaddr_storage peerAddress = {0};
  socklen_t addressLength = sizeof(peerAddress);

  int receivedLength;
  if (emReadingTrace) {
    // Scripted tests.
    receivedLength = nativeReadWithSender(listener->socket,
                                          received,
                                          sizeof(received),
                                          (struct sockaddr *) &peerAddress,
                                          &addressLength);
  } else {
    receivedLength = recvfrom(listener->socket,
                              received,
                              sizeof(received),
                              0,
                              (struct sockaddr *)&peerAddress,
                              &addressLength);
  }

  if (receivedLength < 0
      && errno != EAGAIN) {
    fprintf(stderr, "read on %d failed: %s\n", listener->socket, strerror(errno));
  }

  if (0 < receivedLength
      && peerAddress.ss_family == AF_INET6) {
    struct sockaddr_in6 *peerV6Address =
      (struct sockaddr_in6 *)&peerAddress;

    if (memcmp(peerV6Address->sin6_addr.s6_addr, listener->localAddress, 16) == 0
        && peerV6Address->sin6_port == listener->port) {
      // loopback, don't do anything
    } else {
      if (listener->type == SOCK_RAW
          && listener->protocol == IPPROTO_ICMPV6) {
        // ICMP callback
        Ipv6Header ipHeader;
        MEMSET(&ipHeader, 0, sizeof(Ipv6Header));
        ipHeader.transportProtocol = IPV6_NEXT_HEADER_ICMPV6;
        MEMCOPY(ipHeader.source, peerV6Address->sin6_addr.s6_addr, 16);
        MEMCOPY(ipHeader.destination, listener->localAddress, 16);
        int icmpHeaderLength = 1    // ICMP type
                               + 1  // ICMP code
                               + 2; // Checksum
        ipHeader.transportPayloadLength = receivedLength - icmpHeaderLength;
        ipHeader.transportPayload = received + icmpHeaderLength;
        ipHeader.icmpType = received[0];
        ipHeader.icmpCode = received[1];
        if (memcmp(peerV6Address->sin6_addr.s6_addr,
                   listener->localAddress,
                   8) == 0) {
          // match source and destination prefixes for testing purposes.
          emberIncomingIcmpHandler(&ipHeader);
        }
      } else if (listener->type == SOCK_DGRAM) {
        emberCounterHandler(EMBER_COUNTER_UDP_IN, 1);
        // UDP
        emberUdpHandler(listener->localAddress,
                        peerV6Address->sin6_addr.s6_addr,
                        listener->port,
                        ntohs(peerV6Address->sin6_port),
                        received, 
                        receivedLength);
      }
    }
  }
}

int emberCloseListener(const HostListener *listener)
{
  return close(listener->socket);
}

void emberListenerTick(void)
{
  uint8_t i;
  fd_set socks;
  int returned = 1;
  int highfd = INVALID_SOCKET;

  if (! emReadingTrace) {
    FD_ZERO(&socks);

    for (i = 0; i < EMBER_HOST_LISTENER_TABLE_SIZE; i++) {
      if (listeners[i].socket != INVALID_SOCKET) {
        FD_SET(listeners[i].socket, &socks);
        if (listeners[i].socket > highfd) {
          highfd = listeners[i].socket;
        }
      }
    }

    if (highfd > INVALID_SOCKET) {
      struct timeval timeout;
      timeout.tv_sec = 0;
      timeout.tv_usec = 100000;
      returned = select(highfd + 1, &socks, NULL, NULL, &timeout);
    }
  }

  if (returned) {
    for (i = 0; i < EMBER_HOST_LISTENER_TABLE_SIZE; i++) {
      if (! emReadingTrace) {
        if (highfd == INVALID_SOCKET) return;

        if (listeners[i].socket == INVALID_SOCKET
            || ! FD_ISSET(listeners[i].socket, &socks)) {
          continue;
        }
      }

      emberCheckIncomingListener(&listeners[i]);
    }
  }
}
