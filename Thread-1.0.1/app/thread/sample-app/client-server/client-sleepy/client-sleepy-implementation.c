// Copyright 2015 Silicon Laboratories, Inc.

#include PLATFORM_HEADER
#include CONFIGURATION_HEADER
#include EMBER_AF_API_STACK
#include EMBER_AF_API_HAL
#include EMBER_AF_API_COMMAND_INTERPRETER2
#ifdef EMBER_AF_API_DEBUG_PRINT
  #include EMBER_AF_API_DEBUG_PRINT
#endif
#include EMBER_AF_API_TFTP
#include EMBER_AF_API_TFTP_BOOTLOADER

#define ALIAS(x) x##Alias
#include "app/thread/plugin/udp-debug/udp-debug.c"

// WARNING: This sample application generates a human-readable (i.e., ASCII)
// join key based on a simple hash of the EUI64 of the node.  This method is
// used because it provides a predictable yet reasonably unique key suitable
// for a development kit node.  Real devices MUST NOT use join keys that are
// based on the EUI64.  Random join keys MUST be used for real devices, with
// the keys typically programmed during product manufacturing and printed on
// the devices themselves for access by the user.

// The client/server sample applications use a fixed network id to simplify
// the join process.
static const uint8_t networkId[EMBER_NETWORK_ID_SIZE] = "client/server";

static void resumeNetwork(void);
static void joinNetwork(void);
static void getJoinKey(uint8_t *joinKey, uint8_t *joinKeyLength);
static void attachToNetwork(void);
static void waitForServerAdvertisement(void);
static void attachToServer(const EmberIpv6Address *newServer);
static void reportDataToServer(void);
static void detachFromServer(void);
static void resetNetworkState(void);

static EmberIpv6Address server;
static uint8_t failedReports;
#define REPORT_FAILURE_LIMIT 3
#define WAIT_PERIOD_MS   (30 * MILLISECOND_TICKS_PER_SECOND)
#define REPORT_PERIOD_MS (10 * MILLISECOND_TICKS_PER_SECOND)

static const uint8_t clientReportUri[] = "client/report";
static const uint8_t serverAdvertiseUri[] = "server/advertise";

static bool okToSleep = true;

enum {
  INITIAL                       = 0,
  RESUME_NETWORK                = 1,
  JOIN_NETWORK                  = 2,
  ATTACH_TO_NETWORK             = 3,
  WAIT_FOR_SERVER_ADVERTISEMENT = 4,
  REPORT_DATA_TO_SERVER         = 5,
  WAIT_FOR_DATA_CONFIRMATION    = 6,
  RESET_NETWORK_STATE           = 7,
};
static uint8_t state = INITIAL;
EmberEventControl stateEventControl;
static void setNextStateWithDelay(uint8_t nextState, uint32_t delayMs);
#define setNextState(nextState)       setNextStateWithDelay((nextState), 0)
#define repeatState()                 setNextStateWithDelay(state, 0)
#define repeatStateWithDelay(delayMs) setNextStateWithDelay(state, (delayMs))

void emberAfInitCallback(void)
{
  // This callback is called after the stack has been initialized.  It is our
  // opporunity to do initialization, like resuming the network or joining a
  // new network.

  emberEventControlSetInactive(stateEventControl);

  switch (emberNetworkStatus()) {
  case EMBER_NO_NETWORK:
    setNextState(JOIN_NETWORK);
    break;
  case EMBER_SAVED_NETWORK:
    setNextState(RESUME_NETWORK);
    break;
  case EMBER_JOINING_NETWORK:
    state = JOIN_NETWORK;
    break;
  case EMBER_JOINED_NETWORK:
    setNextState(WAIT_FOR_SERVER_ADVERTISEMENT);
    break;
  case EMBER_JOINED_NETWORK_NO_PARENT:
    setNextState(ATTACH_TO_NETWORK);
    break;
  default:
    assert(false);
  }
}

static void resumeNetwork(void)
{
  EmberNetworkParameters parameters = {{0}};
  uint8_t networkId[EMBER_NETWORK_ID_SIZE + 1] = {0};

  assert(state == RESUME_NETWORK);

  emberGetNetworkParameters(&parameters);
  MEMCOPY(networkId, parameters.networkId, EMBER_NETWORK_ID_SIZE);

  emberAfCorePrintln("Resuming operation on network \"%s\"", networkId);

  emberResumeNetwork();
}

void emberResumeNetworkReturn(EmberStatus status)
{
  // If resuming succeeds, we can proceed to waiting for a server
  // advertisement.  If not, we need to join a network first.  See the note
  // below about resuming sometimes being reported via a join status.

  assert(state == RESUME_NETWORK);

  if (status == EMBER_SUCCESS) {
    EmberNetworkParameters parameters = {{0}};
    uint8_t networkId[EMBER_NETWORK_ID_SIZE + 1] = {0};
    emberGetNetworkParameters(&parameters);
    MEMCOPY(networkId, parameters.networkId, EMBER_NETWORK_ID_SIZE);
    emberAfCorePrintln("Resumed operation on network \"%s\"", networkId);
    setNextState(WAIT_FOR_SERVER_ADVERTISEMENT);
  } else if (status == EMBER_ATTACH_FAILED
             && emberNetworkStatus() == EMBER_JOINED_NETWORK_NO_PARENT) {
    emberAfCorePrintln("ERR: No connection to network");
    setNextState(ATTACH_TO_NETWORK);
  } else if (status != EMBER_RESUME_FAILED_NEW_NETWORK) {
    emberAfCorePrintln("ERR: Resuming failed: 0x%x", status);
    setNextState(JOIN_NETWORK);
  }
}

static void joinNetwork(void)
{
  // When joining a network, we look for one specifically with our network id.
  // The commissioner must have our join key for this to succeed.

  EmberNetworkParameters parameters = {{0}};
  uint8_t joinKey[EMBER_ENCRYPTION_KEY_SIZE + 1] = {0};

  assert(state == JOIN_NETWORK);

  MEMCOPY(parameters.networkId, networkId, sizeof(networkId));
  parameters.nodeType = EMBER_SLEEPY_END_DEVICE;
  parameters.radioTxPower = 3;
  getJoinKey(joinKey, &parameters.joinKeyLength);
  MEMCOPY(parameters.joinKey, joinKey, parameters.joinKeyLength);

  emberAfCorePrintln("Searching for network \"%s\" using join key \"%s\"",
                     networkId,
                     joinKey);

  emberJoinNetwork(&parameters,
                   (EMBER_NETWORK_ID_OPTION
                    | EMBER_NODE_TYPE_OPTION
                    | EMBER_TX_POWER_OPTION
                    | EMBER_JOIN_KEY_OPTION),
                   EMBER_ALL_802_15_4_CHANNELS_MASK);
}

static void getJoinKey(uint8_t *joinKey, uint8_t *joinKeyLength)
{
  // Using a join key generated from the EUI64 of the node is a security risk,
  // but is useful for demonstration purposes.  See the warning above.

  const EmberEui64 *eui64;

  eui64 = emberEui64();
  for (*joinKeyLength = 0; *joinKeyLength < EUI64_SIZE; (*joinKeyLength)++) {
    joinKey[*joinKeyLength] = ((eui64->bytes[*joinKeyLength] & 0x0F)
                               + (eui64->bytes[*joinKeyLength] >> 4));
    joinKey[*joinKeyLength] += (joinKey[*joinKeyLength] < 10 ? '0' : 'A' - 10);
  }
}

void emberJoinNetworkReturn(EmberStatus status)
{
  // After joining, we can proceed to waiting for a server advertisement.  If
  // joining fails, we simply try again.  Note that emberJoinNetworkReturn is
  // also a generic stack status handler, and is called at various times
  // throughout the lifetime of the network.  In particular, it notifies us if
  // we have lost our connection to the network, in which case we will try to
  // rejoin.

  if (state == RESUME_NETWORK) {
    emberResumeNetworkReturn(status);
  } else if (state == JOIN_NETWORK || state == ATTACH_TO_NETWORK) {
    if (status == EMBER_SUCCESS) {
      EmberNetworkParameters parameters = {{0}};
      uint8_t networkId[EMBER_NETWORK_ID_SIZE + 1] = {0};
      emberGetNetworkParameters(&parameters);
      MEMCOPY(networkId, parameters.networkId, EMBER_NETWORK_ID_SIZE);
      emberAfCorePrintln("%s network \"%s\"",
                         (state == JOIN_NETWORK ? "Joined" : "Rejoined"),
                         networkId);
      setNextState(WAIT_FOR_SERVER_ADVERTISEMENT);
    } else {
      emberAfCorePrintln("ERR: %s failed: 0x%x",
                         (state == JOIN_NETWORK ? "Joining" : "Rejoining"),
                         status);
      repeatState();
    }
  } else if (status == EMBER_ATTACH_FAILED
             && emberNetworkStatus() == EMBER_JOINED_NETWORK_NO_PARENT) {
    emberAfCorePrintln("ERR: Lost connection to network");
    setNextState(ATTACH_TO_NETWORK);
  }
}

static void attachToNetwork(void)
{
  assert(state == ATTACH_TO_NETWORK);
  emberAttachToNetwork();
}

void emberAttachToNetworkReturn(EmberStatus status)
{
  // This return indicates whether the stack is going to attempt to attach.
  // If so, the result is reported later in the form of a join.  If we cannot
  // even attempt to attach, we just give up and reset out network state, which
  // will trigger a fresh join attempt.

  assert(state == ATTACH_TO_NETWORK);

  if (status == EMBER_SUCCESS) {
    EmberNetworkParameters parameters = {{0}};
    uint8_t networkId[EMBER_NETWORK_ID_SIZE + 1] = {0};
    emberGetNetworkParameters(&parameters);
    MEMCOPY(networkId, parameters.networkId, EMBER_NETWORK_ID_SIZE);
    emberAfCorePrintln("Rejoining network \"%s\"", networkId);
  } else {
    emberAfCorePrintln("ERR: Unable to rejoin: 0x%x", status);
    setNextState(RESET_NETWORK_STATE);
  }
}

static void waitForServerAdvertisement(void)
{
  // Once on the network, we wait for a server to advertise itself.  We
  // periodically print a message while waiting to prove we are alive.

  assert(state == WAIT_FOR_SERVER_ADVERTISEMENT);

  emberAfCorePrintln("Waiting for an advertisement from a server");

  repeatStateWithDelay(WAIT_PERIOD_MS);
}

void emberCoapMessageHandler(const EmberIpv6Address *source,
                             EmberCoapCode code,
                             const uint8_t *token,
                             uint8_t tokenLength,
                             const uint8_t *uriPath,
                             uint16_t uriPathLength,
                             const uint8_t *payload,
                             uint16_t payloadLength)
{
  // Advertisements from servers are sent as CoAP POST requests to the
  // "server/advertise" URI.  When we receive an advertisement, we attach to
  // the that sent it and beginning sending reports.

  if (emberCoapIsRequest(code)) {
    EmberCoapCode responseCode;
    uint8_t convertedUri[sizeof(serverAdvertiseUri)] = {0};

    if (code != EMBER_COAP_CODE_POST) {
      responseCode = EMBER_COAP_CODE_405_METHOD_NOT_ALLOWED;
    } else if (!emberCoapConvertUri(uriPath,
                                    uriPathLength,
                                    convertedUri,
                                    sizeof(convertedUri))
               || strncmp((const char *)convertedUri,
                          (const char *)serverAdvertiseUri,
                          sizeof(serverAdvertiseUri)) != 0) {
      responseCode = EMBER_COAP_CODE_404_NOT_FOUND;
    } else if (state != WAIT_FOR_SERVER_ADVERTISEMENT) {
      responseCode = EMBER_COAP_CODE_503_SERVICE_UNAVAILABLE;
    } else {
      attachToServer(source);
      responseCode = EMBER_COAP_CODE_204_CHANGED;
    }

    emberCoapRespond(responseCode,
                     NULL, // payload
                     0);   // payload length
  }
}

static void attachToServer(const EmberIpv6Address *newServer)
{
  // We attach to a server in response to an advertisement (or a CLI command).
  // Once we have a server, we begin reporting periodically.  We start from a
  // clean state with regard to failed reports.

  assert(state == WAIT_FOR_SERVER_ADVERTISEMENT);

  MEMCOPY(&server, newServer, sizeof(EmberIpv6Address));
  failedReports = 0;

  emberAfCorePrint("Attached to server at ");
  emberAfCoreDebugExec(emberAfPrintIpv6Address(newServer));
  emberAfCorePrintln("");

  setNextState(REPORT_DATA_TO_SERVER);
}

// attach <server>
void attachCommand(void)
{
  // If we are waiting for a server, we can manually attach to one using a CLI
  // command.

  if (state == WAIT_FOR_SERVER_ADVERTISEMENT) {
    EmberIpv6Address newServer = {{0}};
    if (emberGetIpArgument(0, newServer.bytes)) {
      attachToServer(&newServer);
    } else {
      emberAfCorePrintln("ERR: Invalid ip");
    }
  }
}

static void processServerDataAck(EmberCoapStatus status,
                                 EmberCoapMessage *coap,
                                 void *appData,
                                 uint16_t appDatalength)
{
  // We track the success or failure of reports so that we can determine when
  // we have lost the server.  A series of consecutive failures is the trigger
  // to detach from the current server and find a new one.  Any successfully-
  // transmitted report clears past failures.

  if (state == WAIT_FOR_DATA_CONFIRMATION) {
    if (status == EMBER_COAP_MESSAGE_ACKED
        || status == EMBER_COAP_MESSAGE_RESPONSE) {
      failedReports = 0;
    } else {
      failedReports++;
      emberAfCorePrintln("ERR: Report timed out - failure %u of %u",
                         failedReports,
                         REPORT_FAILURE_LIMIT);
    }
    if (failedReports < REPORT_FAILURE_LIMIT) {
      setNextStateWithDelay(REPORT_DATA_TO_SERVER, REPORT_PERIOD_MS);
    } else {
      detachFromServer();
    }
  }
}

static void reportDataToServer(void)
{
  // We peridocally send data to the server.  The data is the temperature,
  // measured in 10^-3 degrees Celsius.  The success or failure of the reports
  // is tracked so we can determine if the server has disappeared and we should
  // find a new one.

  EmberStatus status;
  uint16_t value;
  int16_t volts;
  int32_t data;

  assert(state == REPORT_DATA_TO_SERVER);

  halStartAdcConversion(ADC_USER_APP,
                        ADC_REF_INT,
                        TEMP_SENSOR_ADC_CHANNEL,
                        ADC_CONVERSION_TIME_US_256);
  halReadAdcBlocking(ADC_USER_APP, &value);
  volts = halConvertValueToVolts(value / TEMP_SENSOR_SCALE_FACTOR);
  data = (1591887L - (171 * (int32_t)volts)) / 10;

  emberAfCorePrint("Reporting %ld to server at ", data);
  emberAfCoreDebugExec(emberAfPrintIpv6Address(&server));
  emberAfCorePrintln("");

  // Convert from host-order to network-order before sending so the data can be
  // reliably reconstructed by the server.
  data = HTONL(data);

  status = emberCoapPost(&server,
                         clientReportUri,
                         (const uint8_t *)&data,
                         sizeof(data),
                         processServerDataAck);
  if (status == EMBER_SUCCESS) {
    setNextState(WAIT_FOR_DATA_CONFIRMATION);
  } else {
    emberAfCorePrintln("ERR: Reporting failed: 0x%x", status);
    repeatStateWithDelay(REPORT_PERIOD_MS);
  }
}

// report
void reportCommand(void)
{
  // If we have a server and are reporting data, we can manually send a new
  // report using a CLI command.

  if (state == REPORT_DATA_TO_SERVER) {
    reportDataToServer();
  }
}

static void detachFromServer(void)
{
  // We detach from a server in response to failed reports (or a CLI command).
  // Once we detach, we wait for a new server to advertise itself.

  assert(state == REPORT_DATA_TO_SERVER
         || state == WAIT_FOR_DATA_CONFIRMATION);

  emberAfCorePrint("Detached from server at ");
  emberAfCoreDebugExec(emberAfPrintIpv6Address(&server));
  emberAfCorePrintln("");

  setNextState(WAIT_FOR_SERVER_ADVERTISEMENT);
}

// detach
void detachCommand(void)
{
  // If we have a server and are reporting data, we can manually detach and try
  // to find a new server by using a CLI command.

  if (state == REPORT_DATA_TO_SERVER
      || state == WAIT_FOR_DATA_CONFIRMATION) {
    detachFromServer();
  }
}

static void resetNetworkState(void)
{
  emberAfCorePrintln("Resetting network state");
  emberResetNetworkState();
}

void emberResetNetworkStateReturn(EmberStatus status)
{
  // If we ever leave the network, we go right back to joining again.  This
  // could be triggered by an external CLI command.

  if (status == EMBER_SUCCESS) {
    emberAfCorePrintln("Reset network state");
    setNextState(JOIN_NETWORK);
  }
}

void emberUdpHandler(const uint8_t *destination,
                     const uint8_t *source,
                     uint16_t localPort,
                     uint16_t remotePort,
                     const uint8_t *payload,
                     uint16_t payloadLength)
{
  // UDP packets for TFTP bootloading are passed through.  Everything else is
  // simply logged and ignored.

  if (localPort == emTftpLocalTid) {
    emProcessTftpPacket(source, remotePort, payload, payloadLength);
  } else if (localPort == TFTP_BOOTLOADER_PORT) {
    emProcessTftpBootloaderPacket(source, payload, payloadLength);
  } else {
    ALIAS(emberUdpHandler)(destination,
                           source,
                           localPort,
                           remotePort,
                           payload,
                           payloadLength);
  }
}

bool emberVerifyBootloadRequest(const TftpBootloaderBootloadRequest *request)
{
  // A real implementation should verify a bootload request to ensure it is
  // valid.  This sample application simply accepts any request.

  return true;
}

bool emberAfPluginIdleSleepOkToSleepCallback(uint32_t durationMs)
{
  // The application normally permits itself to go to sleep whenever possible,
  // but the buttons can be used to keep the device awake.

  return okToSleep;
}

void halButtonIsr(uint8_t button, uint8_t state)
{
  // The CLI is unusable during sleep, which makes it difficult to interact
  // with the node.  To address this, the buttons can be used to alter the
  // sleep characteristics of the node.  Pressing button0 will cause the node
  // to sleep when possible.  Pressing any other button will cause the node to
  // stay awake.

  if (state == BUTTON_PRESSED) {
    okToSleep = (button == BUTTON0);
  }
}

bool emberAfPluginPollingOkToLongPollCallback(void)
{
  // While waiting for a server advertisement or for the server to confirm our
  // data report, we need to short poll, so that we can actually receive the
  // messages via our parent.  At all other times, we can long poll.

  return (state != WAIT_FOR_SERVER_ADVERTISEMENT
          && state != WAIT_FOR_DATA_CONFIRMATION);
}

void stateEventHandler(void)
{
  emberEventControlSetInactive(stateEventControl);

  switch (state) {
  case RESUME_NETWORK:
    resumeNetwork();
    break;
  case JOIN_NETWORK:
    joinNetwork();
    break;
  case ATTACH_TO_NETWORK:
    attachToNetwork();
    break;
  case WAIT_FOR_SERVER_ADVERTISEMENT:
    waitForServerAdvertisement();
    break;
  case REPORT_DATA_TO_SERVER:
    reportDataToServer();
    break;
  case WAIT_FOR_DATA_CONFIRMATION:
    break;
  case RESET_NETWORK_STATE:
    resetNetworkState();
    break;
  default:
    assert(false);
  }
}

static void setNextStateWithDelay(uint8_t nextState, uint32_t delayMs)
{
  state = nextState;
  emberEventControlSetDelayMS(stateEventControl, delayMs);
}
