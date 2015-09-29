// Copyright 2015 Silicon Laboratories, Inc.

#include PLATFORM_HEADER
#include CONFIGURATION_HEADER
#include EMBER_AF_API_STACK
#include EMBER_AF_API_HAL
#ifdef EMBER_AF_API_DEBUG_PRINT
  #include EMBER_AF_API_DEBUG_PRINT
#endif

#ifndef ALIAS
  #define ALIAS(x) x
#endif

void ALIAS(emberSlaacServerChange)(const uint8_t *prefix,
                                   uint8_t prefixLength,
                                   bool available)
{
  if (available) {
    emberAfCorePrint("Requesting SLAAC address in ");
    emberAfCoreDebugExec(emberAfPrintIpv6Fields(prefix, prefixLength / 2));
    emberAfCorePrintln(" prefix");
    emberRequestSlaacAddress(prefix, prefixLength);
  }
}

void ALIAS(emberRequestSlaacAddressReturn)(EmberStatus status,
                                           const uint8_t *prefix,
                                           uint8_t prefixLength)
{
  if (status != EMBER_SUCCESS) {
    emberAfCorePrint("ERR: Requesting SLAAC address in ");
    emberAfCoreDebugExec(emberAfPrintIpv6Fields(prefix, prefixLength / 2));
    emberAfCorePrintln(" prefix failed: 0x%x", status);
  }
}

