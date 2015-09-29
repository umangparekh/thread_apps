// File: ip-driver-app-unix-host.c
//
// Description: Unix Host code for the ip driver app
//
// Copyright 2013 by Silicon Laboratories. All rights reserved.             *80*

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
#include <signal.h>

#ifdef __linux__
  #include <linux/if_tun.h>
#endif

#include PLATFORM_HEADER
#include "stack/core/ember-stack.h"
#include "stack/core/log.h"
#include "hal/hal.h"
#include "phy/phy.h"
#include "app/util/serial/serial.h"
#include "uart-link-protocol.h"
#include "ip-driver-log.h"
#include "host-stream.h"
#include "data-client.h"
#include "ip-driver.h"
#include "app/tmsp/tmsp-enum.h"
#include "hal/micro/generic/ash-v3.h"
#include "app/ip-ncp/ncp-uart-interface.h"

#ifdef UNIX_HOST
  #define LOG(x) x
#else
  #define LOG(x)
#endif

extern int mgmtListenFd;
static int logging = 1;

#define ARG_LENGTH 40
#define STRINGIFY(x) #x
#define STRINGIFYX(x) STRINGIFY(x)

static void openManagementSocket(uint16_t port)
{
  mgmtListenFd = socket(AF_INET6, SOCK_STREAM, 0);

  if (mgmtListenFd < 0) {
    perror("socket creation failed");
    exit(1);
  }

  int on = 1;

  if (setsockopt(mgmtListenFd,
                 SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) != 0) {
    perror("set SO_REUSEADDR");
    exit(1);
  }

  int flags = fcntl(mgmtListenFd, F_GETFL);
  assert(fcntl(mgmtListenFd, F_SETFL, flags | O_NONBLOCK) != -1);
  struct sockaddr_in6 address;
  memset((char *) &address, 0, sizeof(address));
  address.sin6_family = AF_INET6;
  address.sin6_addr = in6addr_any;
  address.sin6_port = htons(port);

  if (bind(mgmtListenFd, (struct sockaddr *) &address, sizeof(address)) < 0) {
    perror("bind");
    exit(1);
  }

  if (listen(mgmtListenFd, 0) < 0) {
    perror("listen");
    exit(1);
  }
}

static void handleManagementConnect(void)
{
  struct sockaddr_in6 address = {0};
  socklen_t addressLength = sizeof(address);
  int acceptFd = accept(mgmtListenFd,
                        (struct sockaddr *) &address, &addressLength);
  if (acceptFd >= 0) {
    if (driverManagementFd >= 0) {
      close(driverManagementFd);
    }

    driverManagementFd = acceptFd;
    int flags = fcntl(driverManagementFd, F_GETFL);
    assert(fcntl(driverManagementFd, F_SETFL, flags | O_NONBLOCK) != -1);
  }
}

static int signalCaught = 0;

static void signalHandler(int signal)
{
  // save signal for DSR processing to avoid race condition of processing now
  signalCaught = signal;
}

static void cleanupAndExit(int exitCode)
{
  if (signalCaught) {
    // we test this here because some syscalls, notably select(), return an
    // error when interrupted by a signal and we want to still be able to
    // inform the user that the signal was the underlying cause
    LOG(ipDriverLogStatus("Caught signal %d, terminating", signalCaught);)
  }

  LOG(ipDriverLogFlush();)
  close(driverDataFd); // unsure if necessary; Ed had a signal handler for
                       // this that was never actually called AFAICT
  exit(exitCode);
}

//------------------------------------------------------------------------------
// Connection to NCP, which may be either an IPv6 socket or a serial port.
static void connectNcpSocket(uint16_t port)
{
  driverNcpFd = socket(AF_INET6, SOCK_STREAM, 0);

  if (driverNcpFd < 0) {
    perror("socket creation failed");
    exit(1);
  }

  struct sockaddr_in6 address = {0};
  address.sin6_family = AF_INET6;
  address.sin6_addr.s6_addr[15] = 1;
  address.sin6_port = htons(port);

  if (connect(driverNcpFd,
              (struct sockaddr *) &address,
              sizeof(address))
      != 0) {
    perror("connect failed");
    exit(1);
  }

  int flags = fcntl(driverNcpFd, F_GETFL);
  assert(fcntl(driverNcpFd, F_SETFL, flags | O_NONBLOCK) != -1);
}

bool initIpDriver(int argc, char **argv)
{
  if (argc < 4) {
    return false;
  }

  char uartArg[ARG_LENGTH + 1] = {0};
  char tunArg[ARG_LENGTH + 1] = {0};
  char mgmtArg[ARG_LENGTH + 1] = {0};
  char logArg[ARG_LENGTH + 1] = {0};

  bool ncpUsesSocket = false;

  while (true) {
    static struct option long_options[] = {
      { "nolog",  no_argument,       &logging,  0  },
      { "uart",   required_argument, 0,        'u' },
      { "tun",    required_argument, 0,        't' },
      { "mgmt",   required_argument, 0,        'm' },
      { "socket", no_argument,       0,        's' },
      { "log",    required_argument, 0,        'l' },
      { 0, 0, 0, 0 }
    };

    int option_index = 0;
    int c = getopt_long(argc, argv, "u:t:m:s", long_options, &option_index);
    if (c == -1) {
      if (option_index != argc && option_index != 0) {
        fprintf(stderr, "Unexpected argument %s\n", argv[option_index]);
        return false;
      }
      break;
    }

    switch (c) {
    case 0:
      break;
    case 'u':
      sscanf(optarg, "%" STRINGIFYX(ARG_LENGTH) "s", uartArg);
      break;
    case 't':
      sscanf(optarg, "%" STRINGIFYX(ARG_LENGTH) "s", tunArg);
      break;
    case 'm':
      sscanf(optarg, "%" STRINGIFYX(ARG_LENGTH) "s", mgmtArg);
      break;
    case 's':
      ncpUsesSocket = true;
      break;
    case 'l':
      sscanf(optarg, "%" STRINGIFYX(ARG_LENGTH) "s", logArg);
      assert(emLogConfigFromName(logArg, emStrlen(logArg), true, APP_SERIAL));
      break;
    default:
      return false;
    }
  }

  logEnabled = (bool) logging;
  LOG(ipDriverOpenLogFile("ip-driver-app.log");)

  if (ncpUsesSocket) {
    int ncpPort = atoi(uartArg);
    if (ncpPort <= 0 || ncpPort > 65535) {
      fprintf(stderr, "Invalid NCP port number %d\n", ncpPort);
      return false;
    }
    connectNcpSocket(ncpPort);
    ncpUartUseAsh = false;
  } else {
    ncpUartUseAsh = true;
    emOpenNcpUart(uartArg);
    LOG(ipDriverLogStatus("Opened UART device %s.", uartArg););
  }

  emOpenTunnel(tunArg);

  int mgmtPort = atoi(mgmtArg);
  if (mgmtPort <= 0 || mgmtPort > 65535) {
    fprintf(stderr, "Invalid management port number %d\n", mgmtPort);
    return false;
  }

  // configure signal handlers
  struct sigaction sigHandler;
  sigHandler.sa_handler = signalHandler;
  sigemptyset(&sigHandler.sa_mask);
  sigHandler.sa_flags = 0;
  sigaction(SIGINT, &sigHandler, NULL);
  sigaction(SIGTERM, &sigHandler, NULL);
  sigaction(SIGABRT, &sigHandler, NULL);

  openManagementSocket(mgmtPort);
  return true;
}

void ipDriverTick(void)
{
  fd_set input;
  FD_ZERO(&input);
  FD_SET(driverDataFd, &input);
  FD_SET(driverNcpFd,  &input);
  FD_SET(mgmtListenFd, &input);

  if (signalCaught) {
    cleanupAndExit(1);
  }

  if (driverManagementFd >= 0) {
    FD_SET(driverManagementFd, &input);
  }

  int maxFd = driverDataFd;
  if (maxFd < driverNcpFd)        maxFd = driverNcpFd;
  if (maxFd < mgmtListenFd)       maxFd = mgmtListenFd;
  if (maxFd < driverManagementFd) maxFd = driverManagementFd;

  struct timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = 100000;

  int n = select(maxFd + 1, &input, NULL, NULL, &timeout);

  if (n < 0) {
    perror("select failed");
    cleanupAndExit(1);
  } else if (n == 0) {
    // Timeout.
  } else {
    emCheckNcpUartInput(&input);

    if (FD_ISSET(mgmtListenFd, &input)) {
      handleManagementConnect();
    }

    if (0 <= driverManagementFd
        && FD_ISSET(driverManagementFd, &input)) {
      IpModemReadStatus status = 
        readIpModemInput(driverManagementFd,
                         &managementStream,
                         managementHandler);
      emTestIpModemReadStatusResult(status, &managementStream);
    }
  }
}
