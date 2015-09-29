// Copyright 2015 Silicon Laboratories, Inc.

#include PLATFORM_HEADER
#include CONFIGURATION_HEADER
#include EMBER_AF_API_STACK
#include EMBER_AF_API_HAL
#include EMBER_AF_API_COMMAND_INTERPRETER2
#ifdef EMBER_AF_API_DEBUG_PRINT
  #include EMBER_AF_API_DEBUG_PRINT
#endif

#include <arpa/inet.h>
#include <stdio.h>
#include <time.h>

#define ALIAS(x) x##Alias
#include "app/thread/plugin/address-configuration-debug/address-configuration-debug.c"
#include "app/thread/plugin/udp-debug/udp-debug.c"

// WARNING: By default, this sample application uses the well-know sensor/sink
// network key as the master key.  This is done for demonstration purposes, so
// that packets will decrypt automatically in Ember Desktop.  Using predefined
// keys is a significant security risk.  Real devices MUST NOT used fixed keys
// and MUST use random keys instead.  The stack automatically generates random
// keys if emberSetSecurityParameters is not called prior to forming.
//#define USE_RANDOM_MASTER_KEY
#ifdef USE_RANDOM_MASTER_KEY
  #define SET_SECURITY_PARAMETERS_OR_FORM_NETWORK FORM_NETWORK
#else
  static EmberKeyData masterKey = {
    {0x65, 0x6D, 0x62, 0x65, 0x72, 0x20, 0x45, 0x4D,
     0x32, 0x35, 0x30, 0x20, 0x63, 0x68, 0x69, 0x70,}
  };
  #define SET_SECURITY_PARAMETERS_OR_FORM_NETWORK SET_SECURITY_PARAMETERS
#endif

// The client/server sample applications use a fixed network id to simplify
// the join process.
static const uint8_t networkId[EMBER_NETWORK_ID_SIZE] = "client/server";

static const uint8_t commissionerId[] = "server";

static void resumeNetwork(void);
#ifndef USE_RANDOM_MASTER_KEY
static void setSecurityParameters(void);
#endif
static void formNetwork(void);
static void attachToNetwork(void);
static void getCommissioner(void);
static void becomeCommissioner(void);
static void advertise(void);
static void resetNetworkState(void);

#define ADVERTISEMENT_PERIOD_MS (60 * MILLISECOND_TICKS_PER_SECOND)
static const EmberIpv6Address allMeshNodes = {
  {0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,}
};

static const uint8_t clientReportUri[] = "client/report";
static const uint8_t serverAdvertiseUri[] = "server/advertise";

#ifdef REPORTS_CSV
static FILE *reports;
#endif

enum {
  INITIAL                 = 0,
  RESUME_NETWORK          = 1,
  SET_SECURITY_PARAMETERS = 2,
  FORM_NETWORK            = 3,
  ATTACH_TO_NETWORK       = 4,
  GET_COMMISSIONER        = 5,
  BECOME_COMMISSIONER     = 6,
  ADVERTISE               = 7,
  RESET_NETWORK_STATE     = 8,
};
static uint8_t state = INITIAL;
EmberEventControl stateEventControl;
static void setNextStateWithDelay(uint8_t nextState, uint32_t delayMs);
#define setNextState(nextState)       setNextStateWithDelay((nextState), 0)
#define repeatStateWithDelay(delayMs) setNextStateWithDelay(state, (delayMs))

void emCoapIncomingMessageHandler(const uint8_t *bytes,
                                  uint16_t bytesLength,
                                  const Ipv6Address *localAddress,
                                  uint16_t localPort,
                                  const Ipv6Address *remoteAddress,
                                  uint16_t remotePort);

void emberAfMainCallback(MAIN_FUNCTION_PARAMETERS)
{
#ifdef REPORTS_CSV
  reports = fopen(REPORTS_CSV, "a");
  if (reports == NULL) {
    perror(NULL);
  }
#endif
}

void emberAfInitCallback(void)
{
  // This callback is called after the stack has been initialized.  It is our
  // opporunity to do initialization, like resuming the network or starting a
  // new one.

  emberEventControlSetInactive(stateEventControl);

  switch (emberNetworkStatus()) {
  case EMBER_NO_NETWORK:
    setNextState(SET_SECURITY_PARAMETERS_OR_FORM_NETWORK);
    break;
  case EMBER_SAVED_NETWORK:
    setNextState(RESUME_NETWORK);
    break;
  case EMBER_JOINING_NETWORK:
    // TODO: What should happen in this case?
    state = RESUME_NETWORK;
    break;
  case EMBER_JOINED_NETWORK:
    setNextState(GET_COMMISSIONER);
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
  // If resuming succeeds, we can proceed to setting ourselves as the
  // commissioner.  If not, we need to form a new network, beginning with
  // setting our security parameters.  See the note below about resuming
  // sometimes being reported via a join status.

  assert(state == RESUME_NETWORK);

  if (status == EMBER_SUCCESS) {
    EmberNetworkParameters parameters = {{0}};
    uint8_t networkId[EMBER_NETWORK_ID_SIZE + 1] = {0};
    emberGetNetworkParameters(&parameters);
    MEMCOPY(networkId, parameters.networkId, EMBER_NETWORK_ID_SIZE);
    emberAfCorePrintln("Resumed operation on network \"%s\"", networkId);
    setNextState(GET_COMMISSIONER);
  } else if (status == EMBER_ATTACH_FAILED
             && emberNetworkStatus() == EMBER_JOINED_NETWORK_NO_PARENT) {
    emberAfCorePrintln("ERR: No connection to network");
    setNextState(ATTACH_TO_NETWORK);
  } else if (status != EMBER_RESUME_FAILED_NEW_NETWORK) {
    emberAfCorePrintln("ERR: Resuming failed: 0x%x", status);
    setNextState(SET_SECURITY_PARAMETERS_OR_FORM_NETWORK);
  }
}

#ifndef USE_RANDOM_MASTER_KEY
static void setSecurityParameters(void)
{
  // Setting a fixed master key is a security risk, but is useful for
  // demonstration purposes.  See the warning above.

  EmberSecurityParameters parameters = {0};

  assert(state == SET_SECURITY_PARAMETERS);

  emberAfCorePrint("Setting master key to ");
  emberAfCoreDebugExec(emberAfPrintZigbeeKey(masterKey.contents));

  parameters.networkKey = &masterKey;
  emberSetSecurityParameters(&parameters, EMBER_NETWORK_KEY_OPTION);
}
#endif

void emberSetSecurityParametersReturn(EmberStatus status)
{
  // After setting our security parameters, we can move on to actually forming
  // the network.

  assert(state == SET_SECURITY_PARAMETERS);

  if (status == EMBER_SUCCESS) {
    setNextState(FORM_NETWORK);
  } else {
    emberAfCorePrint("ERR: Setting master key to ");
    emberAfCoreDebugExec(emberAfPrintZigbeeKey(masterKey.contents));
    emberAfCorePrintln(" failed:  0x%x", status);
    setNextState(SET_SECURITY_PARAMETERS);
  }
}

static void formNetwork(void)
{
  EmberNetworkParameters parameters = {{0}};

  assert(state == FORM_NETWORK);

  emberAfCorePrintln("Forming network \"%s\"", networkId);

  MEMCOPY(parameters.networkId, networkId, sizeof(networkId));
  parameters.nodeType = EMBER_ROUTER;
  parameters.radioTxPower = 3;

  emberFormNetwork(&parameters,
                   (EMBER_NETWORK_ID_OPTION
                    | EMBER_NODE_TYPE_OPTION
                    | EMBER_TX_POWER_OPTION),
                   EMBER_ALL_802_15_4_CHANNELS_MASK);
}

void emberFormNetworkReturn(EmberStatus status)
{
  // With the network formed, we now want to establish ourself as the
  // commissioner.

  assert(state == FORM_NETWORK);

  if (status == EMBER_SUCCESS) {
    EmberNetworkParameters parameters = {{0}};
    uint8_t networkId[EMBER_NETWORK_ID_SIZE + 1] = {0};
    emberGetNetworkParameters(&parameters);
    MEMCOPY(networkId, parameters.networkId, EMBER_NETWORK_ID_SIZE);
    emberAfCorePrintln("Formed network \"%s\"", networkId);
    setNextState(GET_COMMISSIONER);
  } else {
    emberAfCorePrintln("ERR: Forming failed: 0x%x", status);
    setNextState(SET_SECURITY_PARAMETERS_OR_FORM_NETWORK);
  }
}

void emberJoinNetworkReturn(EmberStatus status)
{
  // Note that emberJoinNetworkReturn is also a generic stack status handler,
  // and is called at various times throughout the lifetime of the network.  In
  // particular, it notifies us if we have lost our connection to the network,
  // in which case we will try to rejoin.

  if (state == RESUME_NETWORK) {
    emberResumeNetworkReturn(status);
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
  // will trigger a fresh form attempt.

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

static void getCommissioner(void)
{
  assert(state == GET_COMMISSIONER);

  emberGetCommissioner();
}

void emberCommissionerStatusHandler(uint16_t flags,
                                    const uint8_t *commissionerName,
                                    uint8_t commissionerNameLength)
{
  if (state == GET_COMMISSIONER) {
    if (flags == EMBER_NO_COMMISSIONER) {
      setNextState(BECOME_COMMISSIONER);
    } else {
      if (!READBITS(flags, EMBER_AM_COMMISSIONER)) {
        emberAfCorePrint("ERR: Network already has a commissioner");
        if (commissionerName != NULL) {
          emberAfCorePrint(": ");
          uint8_t i;
          for (i = 0; i < commissionerNameLength; i++) {
            emberAfCorePrint("%c", commissionerName[i]);
          }
        }
        emberAfCorePrintln("");
      }
      setNextState(ADVERTISE);
    }
  }
}

static void becomeCommissioner(void)
{
  // We want to establishing ourselves as the commissioner so that we are in
  // charge of bringing new devices into the network.

  assert(state == BECOME_COMMISSIONER);

  emberAfCorePrintln("Becoming commissioner \"%s\"", commissionerId);

  emberBecomeCommissioner(commissionerId, sizeof(commissionerId));
}

void emberBecomeCommissionerReturn(EmberStatus status)
{
  // Once we are established as the commissioner, we move on to periodically
  // advertising our presence to other routers in the mesh.

  assert(state == BECOME_COMMISSIONER);

  if (status == EMBER_SUCCESS) {
    emberAfCorePrintln("Became commissioner");
    setNextState(ADVERTISE);
  } else {
    emberAfCorePrintln("ERR: Becoming commissioner failed: 0x%x", status);
    setNextState(GET_COMMISSIONER);
  }
}

// expect <join key:1--16> [<eui64:8>]
void expectCommand(void)
{
  uint8_t joinKeyLength;
  uint8_t *joinKey;
  bool hasEui64;
  EmberEui64 eui64;

  joinKey = emberStringCommandArgument(0, &joinKeyLength);
  if (joinKeyLength > EMBER_ENCRYPTION_KEY_SIZE) {
    emberAfAppPrintln("%p: %p", "ERR", "invalid join key");
    return;
  }

  hasEui64 = (emberCommandArgumentCount() > 1);
  if (hasEui64) {
    emberGetEui64Argument(1, &eui64);
    emberSetJoinKey(&eui64, joinKey, joinKeyLength);
    emberSetJoiningMode(EMBER_JOINING_CLEAR_STEERING, 16);
    emberAddSteeringEui64(&eui64);
  } else {
    emberSetJoinKey(NULL, joinKey, joinKeyLength);
    emberSetJoiningMode(EMBER_JOINING_NO_STEERING, 1);
  }
  emberSendSteeringData();
}

void emberSendSteeringDataReturn(EmberStatus status)
{
  // The steering data helps bring new devices into our network.

  if (status == EMBER_SUCCESS) {
    emberAfCorePrintln("Sent steering data");
  } else {
    emberAfCorePrintln("ERR: Sending steering data failed: 0x%x", status);
  }
}

void emberAddressConfigurationChange(const EmberIpv6Address *address,
                                     uint32_t validLifetime,
                                     uint32_t preferredLifetime,
                                     uint8_t addressFlags)
{
  ALIAS(emberAddressConfigurationChange)(address,
                                         validLifetime,
                                         preferredLifetime,
                                         addressFlags);

  if (addressFlags == 0 || validLifetime != 0) {
    EmberStatus status;

    status = emberIcmpListen(address->bytes);
    if (status != EMBER_SUCCESS) {
      emberAfCorePrintln("ERR: Listening for ICMP failed: 0x%x", status);
    }

    status = emberUdpListen(EMBER_COAP_PORT, address->bytes);
    if (status != EMBER_SUCCESS) {
      emberAfCorePrintln("ERR: Listening for CoAP failed: 0x%x", status);
    }
  }
}

static void advertise(void)
{
  // We peridocally send advertisements to all routers in the mesh.  Unattached
  // clients that hear these advertisements will begin sending data to us.

  EmberStatus status;

  assert(state == ADVERTISE);

  emberAfCorePrint("Advertising to ");
  emberAfCoreDebugExec(emberAfPrintIpv6Address(&allMeshNodes));
  emberAfCorePrintln("");

  status = emberCoapPostNonconfirmable(&allMeshNodes,
                                       serverAdvertiseUri,
                                       NULL, // body
                                       0);   // body length
  if (status != EMBER_SUCCESS) {
    emberAfCorePrintln("ERR: Advertising failed: 0x%x", status);
  }

  repeatStateWithDelay(ADVERTISEMENT_PERIOD_MS);
}

// advertise
void advertiseCommand(void)
{
  // If we are already advertising, we can manually send a new advertisement
  // using a CLI command.

  if (state == ADVERTISE) {
    advertise();
  }
}

void emberUdpHandler(const uint8_t *destination,
                     const uint8_t *source,
                     uint16_t localPort,
                     uint16_t remotePort,
                     const uint8_t *payload,
                     uint16_t payloadLength)
{
  // UDP packets for CoAP are passed through.  Everything else is simply logged
  // and ignored.

  if (localPort == EMBER_COAP_PORT) {
    emCoapIncomingMessageHandler(payload,
                                 payloadLength,
                                 (const Ipv6Address *)destination,
                                 localPort,
                                 (const Ipv6Address *)source,
                                 remotePort);
  } else {
    ALIAS(emberUdpHandler)(destination,
                           source,
                           localPort,
                           remotePort,
                           payload,
                           payloadLength);
  }
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
  // Reports from clients are sent as CoAP POST requests to the "client/report"
  // URI.

  if (emberCoapIsRequest(code)) {
    EmberCoapCode responseCode;
    uint8_t convertedUri[sizeof(clientReportUri)] = {0};

    if (code != EMBER_COAP_CODE_POST) {
      responseCode = EMBER_COAP_CODE_405_METHOD_NOT_ALLOWED;
    } else if (!emberCoapConvertUri(uriPath,
                                    uriPathLength,
                                    convertedUri,
                                    sizeof(convertedUri))
               && strncmp((const char *)convertedUri,
                          (const char *)clientReportUri,
                          sizeof(clientReportUri)) != 0) {
      responseCode = EMBER_COAP_CODE_404_NOT_FOUND;
    } else if (state != ADVERTISE) {
      responseCode = EMBER_COAP_CODE_503_SERVICE_UNAVAILABLE;
    } else if (payloadLength != sizeof(int32_t)) {
      responseCode = EMBER_COAP_CODE_400_BAD_REQUEST;
    } else {
      uint32_t data = (int32_t)emberFetchHighLowInt32u(payload);
      emberAfCorePrint("Received %ld from client at ", data);
      emberAfCoreDebugExec(emberAfPrintIpv6Address(source));
      emberAfCorePrintln("");

#ifdef REPORTS_CSV
      if (reports != NULL) {
        char dst[INET6_ADDRSTRLEN];
        if (fprintf(reports,
                    "%lli,%s,%ld\n",
                    (long long int)time(NULL),
                    inet_ntop(AF_INET6, source, dst, sizeof(dst)),
                    (long int)data) == -1
            || fflush(reports) != 0) {
          perror(NULL);
        }
      }
#endif

      responseCode = EMBER_COAP_CODE_204_CHANGED;
    }

    emberCoapRespond(responseCode,
                     NULL, // payload
                     0);   // payload length
  }
}

static void resetNetworkState(void)
{
  emberAfCorePrintln("Resetting network state");
  emberResetNetworkState();
}

void emberResetNetworkStateReturn(EmberStatus status)
{
  // If we ever leave the network, we go right back to forming again.  This
  // could be triggered by an external CLI command.

  if (status == EMBER_SUCCESS) {
    emberAfCorePrintln("Reset network state");
    setNextState(SET_SECURITY_PARAMETERS_OR_FORM_NETWORK);
  }
}

void stateEventHandler(void)
{
  emberEventControlSetInactive(stateEventControl);

  switch (state) {
  case RESUME_NETWORK:
    resumeNetwork();
    break;
#ifndef USE_RANDOM_MASTER_KEY
  case SET_SECURITY_PARAMETERS:
    setSecurityParameters();
    break;
#endif
  case FORM_NETWORK:
    formNetwork();
    break;
  case ATTACH_TO_NETWORK:
    attachToNetwork();
    break;
  case GET_COMMISSIONER:
    getCommissioner();
    break;
  case BECOME_COMMISSIONER:
    becomeCommissioner();
    break;
  case ADVERTISE:
    advertise();
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
