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

void ALIAS(emberAddressConfigurationChange)(const EmberIpv6Address *address,
                                            uint32_t validLifetime,
                                            uint32_t preferredLifetime,
                                            uint8_t addressFlags)
{
  emberAfCorePrint("%p ",
                   (addressFlags == 0 || validLifetime != 0
                    ? "Bound to"
                    : "Unbound from"));
  emberAfCoreDebugExec(emberAfPrintIpv6Address(address));
  emberAfCorePrintln("");
}