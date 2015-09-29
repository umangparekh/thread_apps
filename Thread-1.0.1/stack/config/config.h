// The 4 digit version: A.B.C.D
#define EMBER_MAJOR_VERSION 1
#define EMBER_MINOR_VERSION 0
#define EMBER_PATCH_VERSION 1
#define EMBER_SPECIAL_VERSION 1

// 2 bytes
#define EMBER_BUILD_NUMBER 272

#define EMBER_FULL_VERSION (((uint16_t)EMBER_MAJOR_VERSION << 12)      \
                            | ((uint16_t)EMBER_MINOR_VERSION << 8)     \
                            | ((uint16_t)EMBER_PATCH_VERSION << 4)     \
                            | ((uint16_t)EMBER_SPECIAL_VERSION))

// 4 bytes
#define EMBER_CHANGE_NUMBER 777

// 1 byte
#ifdef EMBER_WAKEUP_STACK
#define EMBER_VERSION_TYPE EMBER_VERSION_TYPE_LEGACY
#else
#define EMBER_VERSION_TYPE EMBER_VERSION_TYPE_GA
#endif

// 2 bytes
#define EMBER_MANAGEMENT_VERSION TMSP_VERSION

// alias used in the HAL
#define SOFTWARE_VERSION EMBER_FULL_VERSION

//COMMIT=e1a27af89f253ff1d83aa17cb5dfbf14fc8fde7b
