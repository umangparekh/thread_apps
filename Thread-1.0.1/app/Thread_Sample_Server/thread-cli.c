// This file is generated by Ember Desktop.  Please do not edit manually.
//
//

// This file contains the mapping for the command line configuration

#include PLATFORM_HEADER
#include CONFIGURATION_HEADER
#include EMBER_AF_API_EMBER_TYPES
#include EMBER_AF_API_COMMAND_INTERPRETER2

void advertiseCommand(void);
void coapGetCommand(void);
void coapPostCommand(void);
static EmberCommandEntry coapTable[] = {
  emberCommandEntryAction("get", coapGetCommand, "bb", ""),
  emberCommandEntryAction("post", coapPostCommand, "bbb", ""),
  emberCommandEntryTerminator(),
};
void expectCommand(void);
void emberPrintCommandTable(void);
void icmpListenCommand(void);
void icmpPingCommand(void);
static EmberCommandEntry icmpTable[] = {
  emberCommandEntryAction("listen", icmpListenCommand, "b", ""),
  emberCommandEntryAction("ping", icmpPingCommand, "bv*", ""),
  emberCommandEntryTerminator(),
};
void networkManagementInfoCommand(void);
void emberAttachToNetwork(void);
void networkManagementCommissionCommand(void);
void networkManagementCommissioningStartCommand(void);
void emberStopCommissioning(void);
static EmberCommandEntry commissioningTable[] = {
  emberCommandEntryAction("start", networkManagementCommissioningStartCommand, "b", ""),
  emberCommandEntryAction("stop", emberStopCommissioning, "", ""),
  emberCommandEntryTerminator(),
};
void networkManagementFormCommand(void);
void networkManagementConfigureGatewayCommand(void);
void networkManagementGetGlobalAddressesCommand(void);
void emberGetGlobalPrefixes(void);
void networkManagementJoinCommand(void);
void networkManagementJoinCommissionedCommand(void);
void emberResetNetworkState(void);
void emberResumeNetwork(void);
void networkManagementScanCommand(void);
void networkManagementScanCommand(void);
void emberStopScan(void);
static EmberCommandEntry scanTable[] = {
  emberCommandEntryAction("active", networkManagementScanCommand, "u*", ""),
  emberCommandEntryAction("energy", networkManagementScanCommand, "u*", ""),
  emberCommandEntryAction("stop", emberStopScan, "", ""),
  emberCommandEntryTerminator(),
};
void networkManagementSetJoinKeyCommand(void);
void networkManagementSetJoiningModeCommand(void);
void networkManagementSetMasterKeyCommand(void);
void networkManagementSteeringAddCommand(void);
void emberSendSteeringData(void);
static EmberCommandEntry steeringTable[] = {
  emberCommandEntryAction("add", networkManagementSteeringAddCommand, "b", ""),
  emberCommandEntryAction("send", emberSendSteeringData, "", ""),
  emberCommandEntryTerminator(),
};
static EmberCommandEntry networkManagementTable[] = {
  emberCommandEntryAction("attach", emberAttachToNetwork, "", ""),
  emberCommandEntryAction("commission", networkManagementCommissionCommand, "uwbbbbv*", ""),
  emberCommandEntrySubMenu("commissioning", commissioningTable, ""),
  emberCommandEntryAction("form", networkManagementFormCommand, "usub*", ""),
  emberCommandEntryAction("gateway", networkManagementConfigureGatewayCommand, "ubuww", ""),
  emberCommandEntryAction("global-addresses", networkManagementGetGlobalAddressesCommand, "b*", ""),
  emberCommandEntryAction("global-prefixes", emberGetGlobalPrefixes, "", ""),
  emberCommandEntryAction("join", networkManagementJoinCommand, "usubbvb", ""),
  emberCommandEntryAction("join-commissioned", networkManagementJoinCommissionedCommand, "suu*", ""),
  emberCommandEntryAction("reset", emberResetNetworkState, "", ""),
  emberCommandEntryAction("resume", emberResumeNetwork, "", ""),
  emberCommandEntrySubMenu("scan", scanTable, ""),
  emberCommandEntryAction("set-join-key", networkManagementSetJoinKeyCommand, "bb*", ""),
  emberCommandEntryAction("set-joining-mode", networkManagementSetJoiningModeCommand, "uu", ""),
  emberCommandEntryAction("set-master-key", networkManagementSetMasterKeyCommand, "b", ""),
  emberCommandEntrySubMenu("steering", steeringTable, ""),
  emberCommandEntryTerminator(),
};
void emberResetMicro(void);
void udpListenCommand(void);
void udpSendCommand(void);
static EmberCommandEntry udpTable[] = {
  emberCommandEntryAction("listen", udpListenCommand, "vb", ""),
  emberCommandEntryAction("send", udpSendCommand, "ubuvvb", ""),
  emberCommandEntryTerminator(),
};
void emberGetVersions(void);
EmberCommandEntry emberCommandTable[] = {
  emberCommandEntryAction("advertise", advertiseCommand, "", ""),
  emberCommandEntrySubMenu("coap", coapTable, ""),
  emberCommandEntryAction("expect", expectCommand, "bb*", ""),
  emberCommandEntryAction("help", emberPrintCommandTable, "", ""),
  emberCommandEntrySubMenu("icmp", icmpTable, ""),
  emberCommandEntryAction("info", networkManagementInfoCommand, "", ""),
  emberCommandEntrySubMenu("network-management", networkManagementTable, ""),
  emberCommandEntryAction("reset", emberResetMicro, "", ""),
  emberCommandEntrySubMenu("udp", udpTable, ""),
  emberCommandEntryAction("versions", emberGetVersions, "", ""),
  emberCommandEntryTerminator(),
};