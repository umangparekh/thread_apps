// Copyright 2015 Silicon Laboratories, Inc.

#include PLATFORM_HEADER
#include CONFIGURATION_HEADER
#include EMBER_AF_API_STACK
#include EMBER_AF_API_HAL
#include EMBER_AF_API_COMMAND_INTERPRETER2
#ifdef EMBER_AF_API_DEBUG_PRINT
  #include EMBER_AF_API_DEBUG_PRINT
#endif

// info
void networkManagementInfoCommand(void)
{
  EmberNetworkStatus status;

  status = emberNetworkStatus();
  emberAfAppPrintln("network status: 0x%x", status);

  emberAfAppPrint("eui64: ");
  emberAfAppDebugExec(emberAfPrintBigEndianEui64(emberEui64()));
  emberAfAppPrintln("");

  if (status != EMBER_NO_NETWORK) {
    EmberNetworkParameters parameters = {{0}};
    uint8_t networkId[EMBER_NETWORK_ID_SIZE + 1] = {0};
    uint8_t i;

    emberGetNetworkParameters(&parameters);

    MEMCOPY(networkId, parameters.networkId, EMBER_NETWORK_ID_SIZE);
    emberAfAppPrintln("network id: %s", networkId);
    emberAfAppPrintln("node type: 0x%x", parameters.nodeType);

    emberAfAppPrint("extended pan id: ");
    emberAfAppDebugExec(emberAfPrintExtendedPanId(parameters.extendedPanId));
    emberAfAppPrintln("");

    emberAfAppPrintln("pan id: 0x%2x", parameters.panId);
    emberAfAppPrintln("channel: %u", parameters.channel);
    emberAfAppPrintln("radio tx power: %d dBm ", parameters.radioTxPower);
    emberAfAppPrint("ula prefix: ");
    emberAfAppDebugExec(emberAfPrintIpv6Prefix(&parameters.ulaPrefix));
    emberAfAppPrintln("");

    for (i = 0; i < EMBER_MAX_IPV6_ADDRESS_COUNT; i++) {
      EmberIpv6Address address;
      if (emberGetLocalIpAddress(i, &address)) {
        emberAfAppPrint("local ip %u: ", i);
        emberAfAppDebugExec(emberAfPrintIpv6Address(&address));
        emberAfAppPrintln("");
      }
    }
  }
}

// network-management form <channel:1> <power:1> <node type:1> [<network id:0--16> [<ula prefix>]]
void networkManagementFormCommand(void)
{
  EmberNetworkParameters parameters = {{0}};
  uint16_t options = EMBER_USE_DEFAULTS;
  uint8_t channel;
  uint32_t channelMask;

  channel = (uint8_t)emberUnsignedCommandArgument(0);
  channelMask = (channel == 0
                 ? EMBER_ALL_802_15_4_CHANNELS_MASK
                 : BIT32(channel));

  parameters.radioTxPower = (int8_t)emberSignedCommandArgument(1);
  options |= EMBER_TX_POWER_OPTION;

  parameters.nodeType = (EmberNodeType)emberUnsignedCommandArgument(2);
  options |= EMBER_NODE_TYPE_OPTION;

  if (emberCommandArgumentCount() > 3) {
    uint8_t networkIdLength;
    uint8_t *networkId = emberStringCommandArgument(3, &networkIdLength);
    if (networkIdLength > EMBER_NETWORK_ID_SIZE) {
      emberAfAppPrintln("%p: %p", "ERR", "invalid network id");
      return;
    } else if (networkIdLength != 0) {
      MEMCOPY(parameters.networkId, networkId, networkIdLength);
      options |= EMBER_NETWORK_ID_OPTION;
    }
  }

  if (emberCommandArgumentCount() > 4) {
    if (!emberGetIpPrefixArgument(4, parameters.ulaPrefix.bytes)) {
      emberAfAppPrintln("%p: %p", "ERR", "invalid ula prefix");
      return;
    }
    options |= EMBER_ULA_PREFIX_OPTION;
  }

  emberFormNetwork(&parameters, options, channelMask);
}

// network-management join <channel:1> <power:1> <node type:1> <network id:0--16> <extended pan id:8> <pan id:2> <join key:0--16>
void networkManagementJoinCommand(void)
{
  EmberNetworkParameters parameters = {{0}};
  uint16_t options = EMBER_USE_DEFAULTS;
  uint8_t channel;
  uint32_t channelMask;
  uint8_t networkIdLength;
  uint8_t *networkId;
  uint8_t zeros[EXTENDED_PAN_ID_SIZE] = {0};
  uint8_t *joinKey;

  channel = (uint8_t)emberUnsignedCommandArgument(0);
  channelMask = (channel == 0
                 ? EMBER_ALL_802_15_4_CHANNELS_MASK
                 : BIT32(channel));

  parameters.radioTxPower = (int8_t)emberSignedCommandArgument(1);
  options |= EMBER_TX_POWER_OPTION;

  parameters.nodeType = (EmberNodeType)emberUnsignedCommandArgument(2);
  options |= EMBER_NODE_TYPE_OPTION;

  networkId = emberStringCommandArgument(3, &networkIdLength);
  if (networkIdLength > EMBER_NETWORK_ID_SIZE) {
    emberAfAppPrintln("%p: %p", "ERR", "invalid network id");
    return;
  } else if (networkIdLength != 0) {
    MEMCOPY(parameters.networkId, networkId, networkIdLength);
    options |= EMBER_NETWORK_ID_OPTION;
  }

  emberGetExtendedPanIdArgument(4, parameters.extendedPanId);
  if (MEMCOMPARE(zeros, parameters.extendedPanId, EXTENDED_PAN_ID_SIZE) != 0) {
    options |= EMBER_EXTENDED_PAN_ID_OPTION;
  }

  parameters.panId = (uint16_t)emberUnsignedCommandArgument(5);
  if (parameters.panId != 0xFFFF) {
    options |= EMBER_PAN_ID_OPTION;
  }

  joinKey = emberStringCommandArgument(6, &parameters.joinKeyLength);
  if (parameters.joinKeyLength > EMBER_ENCRYPTION_KEY_SIZE) {
    emberAfAppPrintln("%p: %p", "ERR", "invalid join key");
    return;
  } else if (parameters.joinKeyLength != 0) {
    MEMCOPY(parameters.joinKey, joinKey, parameters.joinKeyLength);
    options |= EMBER_JOIN_KEY_OPTION;
  }

  emberJoinNetwork(&parameters, options, channelMask);
}

// network-management commission <preferred channel:1> <fallback channel mask:4> <network id:0--16> <ula prefix> <extended pan id:8> <key:16> [<pan id:2> [<key sequence:4>]]
void networkManagementCommissionCommand(void)
{
  uint8_t preferredChannel;
  uint32_t fallbackChannelMask;
  uint8_t *networkId;
  uint8_t networkIdLength;
  EmberPanId panId;
  EmberIpv6Prefix ulaPrefix;
  uint8_t extendedPanId[EXTENDED_PAN_ID_SIZE];
  uint8_t contentsLength;
  uint8_t *contents;
  EmberKeyData key;
  uint32_t keySequence;

  preferredChannel = (uint8_t)emberUnsignedCommandArgument(0);
  fallbackChannelMask = emberUnsignedCommandArgument(1);

  networkId = emberStringCommandArgument(2, &networkIdLength);
  if (networkIdLength > EMBER_NETWORK_ID_SIZE) {
    emberAfAppPrintln("%p: %p", "ERR", "invalid network id");
    return;
  }

  if (!emberGetIpPrefixArgument(3, ulaPrefix.bytes)) {
    emberAfAppPrintln("%p: %p", "ERR", "invalid ula prefix");
    return;
  }

  emberGetExtendedPanIdArgument(4, extendedPanId);

  contents = emberStringCommandArgument(5, &contentsLength);
  if (contentsLength != EMBER_ENCRYPTION_KEY_SIZE) {
    emberAfAppPrintln("%p: %p", "ERR", "invalid key");
    return;
  }
  MEMCOPY(key.contents, contents, EMBER_ENCRYPTION_KEY_SIZE);

  panId = 0xFFFF;
  keySequence = 0;
  if (emberCommandArgumentCount() > 6) {
    panId = (EmberPanId)emberUnsignedCommandArgument(6);
    if (emberCommandArgumentCount() > 7) {
      keySequence = emberUnsignedCommandArgument(7);
    }
  }

  emberCommissionNetwork(preferredChannel,
                         fallbackChannelMask,
                         networkId,
                         networkIdLength,
                         panId,
                         ulaPrefix.bytes,
                         extendedPanId,
                         &key,
                         keySequence);
}

// network-management join-commissioned <power:1> <node type:1> [<require connectivity:1>]
void networkManagementJoinCommissionedCommand(void)
{
  int8_t radioTxPower;
  EmberNodeType nodeType;
  bool requireConnectivity;

  radioTxPower = (int8_t)emberSignedCommandArgument(0);
  nodeType = (EmberNodeType)emberUnsignedCommandArgument(1);
  requireConnectivity = (emberCommandArgumentCount() > 2
                         && (bool)emberUnsignedCommandArgument(2));

  emberJoinCommissioned(radioTxPower, nodeType, requireConnectivity);
}

// network-management set-master-key <master key:16>
void networkManagementSetMasterKeyCommand(void)
{
  EmberSecurityParameters parameters = {0};
  uint8_t contentsLength;
  uint8_t *contents;
  EmberKeyData masterKey;

  contents = emberStringCommandArgument(0, &contentsLength);
  if (contentsLength != EMBER_ENCRYPTION_KEY_SIZE) {
    emberAfAppPrintln("%p: %p", "ERR", "invalid master key");
    return;
  }
  MEMCOPY(masterKey.contents, contents, EMBER_ENCRYPTION_KEY_SIZE);
  parameters.networkKey = &masterKey;

  emberSetSecurityParameters(&parameters, EMBER_NETWORK_KEY_OPTION);
}

// network-management set-join-key <join key:1--16> [<eui64:8>]
void networkManagementSetJoinKeyCommand(void)
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
  }

  emberSetJoinKey((hasEui64 ? &eui64 : NULL), joinKey, joinKeyLength);
}

// network-management commissioning start <commissioner id>
void networkManagementCommissioningStartCommand(void)
{
  uint8_t commissionerIdLength;
  uint8_t *commissionerId;

  commissionerId = emberStringCommandArgument(0, &commissionerIdLength);

  emberBecomeCommissioner(commissionerId, commissionerIdLength);
}

// network-management set-joining-mode <mode:1> <length:1>
void networkManagementSetJoiningModeCommand(void)
{
  EmberJoiningMode mode;
  uint8_t length;

  mode = (EmberJoiningMode)emberUnsignedCommandArgument(0);

  length = (uint8_t)emberUnsignedCommandArgument(1);

  emberSetJoiningMode(mode, length);
}

// network-management steering add <eui64:8>
void networkManagementSteeringAddCommand(void)
{
  EmberEui64 eui64;

  emberGetEui64Argument(0, &eui64);

  emberAddSteeringEui64(&eui64);
}

// network-management scan active [<channel:1> [<duration:1>]]
// network-management scan energy [<channel:1> [<duration:1>]]
void networkManagementScanCommand(void)
{
  EmberNetworkScanType scanType;
  uint8_t channel;
  uint32_t channelMask;
  uint8_t duration;

  scanType = (emberStringCommandArgument(-1, NULL)[0] == 'a'
              ? EMBER_ACTIVE_SCAN
              : EMBER_ENERGY_SCAN);

  channel = (emberCommandArgumentCount() > 0
             ? (uint8_t)emberUnsignedCommandArgument(0)
             : 0);
  channelMask = (channel == 0
                 ? EMBER_ALL_802_15_4_CHANNELS_MASK
                 : BIT32(channel));

  duration = (emberCommandArgumentCount() > 1
              ? (uint8_t)emberUnsignedCommandArgument(1)
              : DEFAULT_SCAN_DURATION);

  emberStartScan(scanType, channelMask, duration);
}

// network-management gateway <border router flags:1> <prefix> <domain id:1> <preferred lifetime:4> <valid lifetime:4>
void networkManagementConfigureGatewayCommand(void)
{
  uint8_t borderRouterFlags;
  uint8_t prefixLength;
  EmberIpv6Prefix prefix;
  uint8_t domainId;
  uint32_t preferredLifetimeS;
  uint32_t validLifetimeS;

  borderRouterFlags = (uint8_t)emberUnsignedCommandArgument(0);

  emberStringCommandArgument(1, &prefixLength);
  if (prefixLength != 0 && !emberGetIpPrefixArgument(1, prefix.bytes)) {
    emberAfAppPrintln("%p: %p", "ERR", "invalid prefix");
    return;
  }

  domainId = (uint8_t)emberUnsignedCommandArgument(2);

  preferredLifetimeS = emberUnsignedCommandArgument(3);
  validLifetimeS = emberUnsignedCommandArgument(4);

  emberConfigureGateway(borderRouterFlags,
                        (prefixLength == 0 ? NULL : prefix.bytes),
                        prefixLength,
                        domainId,
                        preferredLifetimeS,
                        validLifetimeS);
}

// network-management global-addresses [prefix]
void networkManagementGetGlobalAddressesCommand(void)
{
  if (emberCommandArgumentCount() > 0) {
    EmberIpv6Prefix prefix = {{0}};
    if (!emberGetIpPrefixArgument(0, prefix.bytes)) {
      emberAfAppPrintln("%p: %p", "ERR", "invalid prefix");
      return;
    }
    emberGetGlobalAddresses(prefix.bytes, sizeof(EmberIpv6Prefix));
  } else {
    emberGetGlobalAddresses(NULL, 0);
  }
}
