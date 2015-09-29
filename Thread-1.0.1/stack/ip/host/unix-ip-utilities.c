// File: unix-ip-utilities.c
//
// Description: Utilities and stubs for running ip on a Unix host.
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
#include "stack/ip/host/unix-interface.h"
#include "stack/ip/ip-address.h"

const char *emUnixInterface = "tun0";

#if (defined (UNIX_HOST) && (!defined(__APPLE__)) && !defined(__CYGWIN__))

struct in6_ifreq {
  struct in6_addr ifr6_addr;
  uint32_t ifr6_prefixlen;
  unsigned int ifr6_ifindex;
};

static void addAddress(int sockfd, const EmberIpv6Address *address)
{
    struct ifreq ifr;
    struct sockaddr_in6 sai;
    struct in6_ifreq ifr6;
    bool already_assigned = false;
    char ipAddressString[INET6_ADDRSTRLEN];

    // get interface name
    strncpy(ifr.ifr_name, emUnixInterface, 16);

    memset(&sai, 0, sizeof(struct sockaddr));
    sai.sin6_family = AF_INET6;
    sai.sin6_port = 0;

    // configure address
    if (inet_ntop(AF_INET6, address->bytes, ipAddressString, sizeof(ipAddressString)) == NULL) {
      perror("Bad address");
      exit(-1);
    }
    if (inet_pton(AF_INET6, ipAddressString, (void *) &sai.sin6_addr) <= 0) {
      perror("Bad address");
      exit(-1);
    }

    memcpy((char *) &ifr6.ifr6_addr, (char *) &sai.sin6_addr, sizeof(struct in6_addr));

    //------------------------------------------------------------------------
    // The following ioctls do the equivalent of the following shell commands:
    // ip -6 addr add <address>/64 dev tun0
    // ifconfig tun0 up
    // route -A inet6 add <prefix>/64 dev tun0
    //------------------------------------------------------------------------

    if (ioctl(sockfd, SIOGIFINDEX, &ifr) < 0) {
      perror("SIOGIFINDEX");
      exit(-1);
    }

    // add route
    ifr6.ifr6_ifindex = ifr.ifr_ifindex;
    ifr6.ifr6_prefixlen = 64;
    if (ioctl(sockfd, SIOCSIFADDR, &ifr6) < 0) {
      if (errno != EEXIST) {
        perror("SIOCSIFADDR");
        exit(-1);
      } else {
        already_assigned = true;
      }
    }

    if (! already_assigned) {
      // interface up
      ifr.ifr_flags |= IFF_UP | IFF_RUNNING;

      if (ioctl(sockfd, SIOCSIFFLAGS, &ifr) < 0) {
        printf("set host address failed. errno: %d\n", errno);
        exit(-1);
      }
    }

    emberAddressConfigurationChange(address, 0, 0, 0);
}

// Configure IP addresses on the host.
void emberConfigureDefaultHostAddress(const EmberIpv6Address *ulAddress)
{
  //------------------------------------------------------------------------
  // Disable ND (RA, RS, etc.) messages from the Linux IPv6 stack
  // Documentation:
  // . https://www.kernel.org/doc/Documentation/networking/ip-sysctl.txt
  //   (for accept_ra, forwarding and router_solicitations)
  // . http://linux.die.net/man/8/radvd
  // . http://www.freebsd.org/cgi/man.cgi?query=rtsol
  // . sysctl interface: http://www.linux.it/~rubini/docs/sysctl/
  //------------------------------------------------------------------------
  // There exists a sysctl syscall to do the following steps, by defining
  // arguments matching the /proc/sys file corresponding to the setting (see
  // http://www.linux.it/~rubini/docs/sysctl/), but using it is strongly
  // discouraged as it is likely to be deprecated in future Linux versions
  // (see man 2 sysctl). Use the /proc/sys/ interface instead.
  //------------------------------------------------------------------------

  // set accept_ra, forwarding and router_solicitations to 0 on the tun0
  // interface
  FILE* procfile = NULL;

  const char * tun0_accept_ra = "/proc/sys/net/ipv6/conf/tun0/accept_ra";
  const char * tun0_forwarding = "/proc/sys/net/ipv6/conf/tun0/forwarding";
  const char * tun0_solicits = "/proc/sys/net/ipv6/conf/tun0/router_solicitations";

  int tun0_accept_ra_value = 0;
  int tun0_forwarding_value = 0;
  int tun0_solicits_value = 0;

  if ((procfile = fopen(tun0_accept_ra, "wb")) >= 0) {
    fprintf(procfile, "%d", tun0_accept_ra_value);
    fclose(procfile);
  }

  if ((procfile = fopen(tun0_forwarding, "wb")) >= 0) {
    fprintf(procfile, "%d", tun0_forwarding_value);
    fclose(procfile);
  }

  if ((procfile = fopen(tun0_solicits, "wb")) >= 0) {
    fprintf(procfile, "%d", tun0_solicits_value);
    fclose(procfile);
  }

  // kill the radvd and rtsol processes if they're running
  char line[10];
  FILE *cmd;
  pid_t pid;
  if ((cmd = popen("pidof -s radvd", "r")) != NULL) {
    memset(line, 0, 10);
    fgets(line, 10, cmd);
    if ((pid = strtoul(line, NULL, 10)) != 0) {
      if (kill(pid, SIGTERM) < 0) {
        kill(pid, SIGKILL);
      }
    }
    pclose(cmd);
  }
  if ((cmd = popen("pidof -s rtsol", "r")) != NULL) {
    memset(line, 0, 10);
    fgets(line, 10, cmd);
    if ((pid = strtoul(line, NULL, 10)) != 0) {
      if (kill(pid, SIGTERM) < 0) {
        kill(pid, SIGKILL);
      }
    }
    pclose(cmd);
  }

  if (ulAddress == NULL) {
    return;
  }

  int sockfd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_IP);
  if (sockfd == -1) {
    perror("Bad fd");
    exit(-1);
  }

  // FE80::
  EmberIpv6Address linkLocalAddress;
  emStoreLongFe8Address(emLinkLocalIdentifier, linkLocalAddress.bytes);
  addAddress(sockfd, &linkLocalAddress);

  // FE90::
  linkLocalAddress.bytes[1] = 0x90;
  addAddress(sockfd, &linkLocalAddress);

  // FE91::
  linkLocalAddress.bytes[1] = 0x91;
  addAddress(sockfd, &linkLocalAddress);

  // ULA
  addAddress(sockfd, ulAddress);

  // Legacy ULA (if required)
  EmberNetworkParameters network;
  emberGetNetworkParameters(&network);
  uint8_t type = network.nodeType;
  if (type & 0x80 || type == EMBER_LURKER) {
    EmberIpv6Address legacyAddress;
    MEMCOPY(legacyAddress.bytes, network.legacyUla.bytes, 8);
    emStoreLongInterfaceId(emMeshLocalIdentifier, legacyAddress.bytes + 8);
    addAddress(sockfd, &legacyAddress);
  }
  close(sockfd);
}

void emberConfigureGlobalHostAddress(const EmberIpv6Address *address)
{
  if (address == NULL) {
    return;
  }

  int sockfd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_IP);
  if (sockfd == -1) {
    perror("Bad fd");
    exit(-1);
  }

  // GUA
  addAddress(sockfd, address);
}

#else
void emberConfigureDefaultHostAddress(const EmberIpv6Address *ulAddress)
{
}

void emberConfigureGlobalHostAddress(const EmberIpv6Address *address)
{
}
#endif // UNIX_HOST
