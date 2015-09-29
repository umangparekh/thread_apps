//Copyright 2015 Silicon Laboratories, Inc.

#include PLATFORM_HEADER
#include CONFIGURATION_HEADER
#include EMBER_AF_API_STACK
#ifdef EMBER_AF_API_DEBUG_PRINT
  #include EMBER_AF_API_DEBUG_PRINT
#endif
#include "thread-bookkeeping.h"
#include "thread-callbacks.h"
#include "stack/framework/buffer-management.h"
#include "stack/ip/mark-list.h"

void emberAfTftpBootloaderInit(void)
{
  emAddMarkingFunction(emMarkTftpBootloaderBuffers);
}
