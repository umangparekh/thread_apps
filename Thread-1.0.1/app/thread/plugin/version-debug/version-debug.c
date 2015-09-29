// Copyright 2015 Silicon Laboratories, Inc.

#include PLATFORM_HEADER
#include CONFIGURATION_HEADER
#include EMBER_AF_API_STACK
#include EMBER_AF_API_HAL
#ifdef EMBER_AF_API_DEBUG_PRINT
  #include EMBER_AF_API_DEBUG_PRINT
#endif
#include "stack/config/config.h"

#ifndef ALIAS
  #define ALIAS(x) x
#endif

static const char * const versionTypeNames[] = {
  EMBER_VERSION_TYPE_NAMES
};

void ALIAS(emberGetVersionsReturn)(const uint8_t *versionName,
                                   uint16_t managementVersionNumber,
                                   uint16_t stackVersionNumber,
                                   uint16_t stackBuildNumber,
                                   EmberVersionType versionType,
                                   const uint8_t *buildTimestamp)
{
#ifdef EMBER_HOST
  emberAfCorePrint("Host: ");
  emberAfCorePrint("%s %u.%u.%u.%u",
                   EMBER_VERSION_NAME,
                   EMBER_MAJOR_VERSION,
                   EMBER_MINOR_VERSION,
                   EMBER_PATCH_VERSION,
                   EMBER_SPECIAL_VERSION);
  if (EMBER_VERSION_TYPE <= EMBER_VERSION_TYPE_MAX) {
    emberAfCorePrint(" %s", versionTypeNames[EMBER_VERSION_TYPE]);
  }
  emberAfCorePrint(" build %u", EMBER_BUILD_NUMBER);
  emberAfCorePrintln(" management %u", EMBER_MANAGEMENT_VERSION);
  emberAfCorePrint("NCP: ");
#endif

  emberAfCorePrint("%s %u.%u.%u.%u",
                   versionName,
                   (stackVersionNumber & 0xF000) >> 12,
                   (stackVersionNumber & 0x0F00) >> 8,
                   (stackVersionNumber & 0x00F0) >> 4,
                   (stackVersionNumber & 0x000F));
  if (versionType <= EMBER_VERSION_TYPE_MAX) {
    emberAfCorePrint(" %s", versionTypeNames[versionType]);
  }
  emberAfCorePrint(" build %u", stackBuildNumber);
#ifdef EMBER_HOST
  emberAfCorePrint(" management %u", managementVersionNumber);
#endif
  emberAfCorePrintln("");
}
