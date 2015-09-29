/** @file hal/micro/cortexm3/efm32/token-manufacturing.h
 * @brief Definitions for manufacturing tokens.
 *
 * This file should not be included directly. It is accessed by the other
 * token files.
 * 
 * Please see stack/config/token-stack.h and hal/micro/token.h for a full
 * explanation of the tokens.
 * 
 * The tokens listed below are the manufacturing tokens.  This
 * token definitions file is included from the master definitions
 * file: stack/config/token-stack.h  Please see that file for more details.
 *
 * The user application can include its own manufacturing tokens in a header
 * file similar to this one. The macro ::APPLICATION_MFG_TOKEN_HEADER should
 * be defined to equal the name of the header file in which application
 * manufacturing tokens are defined.
 *
 * The macro DEFINE_MFG_TOKEN() should be used when instantiating a
 * manufacturing token.  Refer to the list of *_LOCATION defines to
 * see what memory is allocated and what memory is unused/available.
 *
 * REMEMBER: By definition, manufacturing tokens exist at fixed addresses.
 *           Tokens should not overlap.
 *
 * Here is a basic example of a manufacturing token header file:
 *
 * @code
 * #define CREATOR_MFG_EXAMPLE 0x4242
 * #ifdef DEFINETYPES
 * typedef uint8_t tokTypeMfgExample[8];
 * #endif //DEFINETYPES
 * #ifdef DEFINETOKENS
 * #define MFG_EXAMPLE_LOCATION 0x0980
 * DEFINE_MFG_TOKEN(MFG_EXAMPLE,
 *                  tokTypeMfgExample,
 *                  MFG_EXAMPLE_LOCATION,
 *                  {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF})
 * #endif //DEFINETOKENS
 * @endcode
 *
 * Since this file contains both the typedefs and the token defs, there are
 * two \#defines used to select which one is needed when this file is included.
 * \#define DEFINETYPES is used to select the type definitions and
 * \#define DEFINETOKENS is used to select the token definitions.
 * Refer to token.h and token.c to see how these are used.
 *
 * <!-- Copyright 2015 Silicon Laboratories, Inc.                        *80*-->
 */

/**
 * @name Convenience Macros
 * @brief The following convenience macros are used to simplify the definition
 * process for commonly specified parameters to the basic TOKEN_DEF macro.
 * Please see hal/micro/token.h for a more complete explanation.
 *@{
 */
#define DEFINE_MFG_TOKEN(name, type, address, ...)  \
  TOKEN_NEXT_ADDRESS(name,(address))                  \
  TOKEN_MFG(name, CREATOR_##name, 0, 0, type, 1,  __VA_ARGS__)
/** @} END Convenience Macros */

#ifndef TOKEN_NEXT_ADDRESS
  #define TOKEN_NEXT_ADDRESS(region, address)
#endif

//////////////////////////////////////////////////////////////////////////////
// MANUFACTURING DATA
// *the addresses of these tokens must not change*


// MANUFACTURING CREATORS
// The creator codes are here in one list instead of next to their token
// definitions so comparision of the codes is easier.  The only requirement
// on these creator definitions is that they all must be unique.  A favorite
// method for picking creator codes is to use two ASCII characters in order
// to make the codes more memorable.  Also, the msb of Stack and Manufacturing
// token creator codes is always 1, while the msb of Application token creator
// codes is always 0.  This distinction allows Application tokens
// to freely use the lower 15 bits for creator codes without the risk of
// duplicating a Stack or Manufacturing token creator code.
#define CREATOR_MFG_EMBER_EUI_64        0xE545 // msb+'e'+'E'
//--- User Data ---
#define CREATOR_MFG_CUSTOM_VERSION      0xC356
#define CREATOR_MFG_CUSTOM_EUI_64       0xE345
#define CREATOR_MFG_STRING              0xED73
#define CREATOR_MFG_BOARD_NAME          0xC24E // msb+'B'+'N' (Board Name)
#define CREATOR_MFG_EUI_64              0xB634
#define CREATOR_MFG_MANUF_ID            0xC944 // msb+'I'+'D' (Id)
#define CREATOR_MFG_PHY_CONFIG          0xD043 // msb+'P'+'C' (Phy Config)
#define CREATOR_MFG_EZSP_STORAGE        0xCD53
#define CREATOR_MFG_ASH_CONFIG          0xC143 // msb+'A'+'C' (ASH Config)
#define CREATOR_MFG_SYNTH_FREQ_OFFSET   0xD346 // msb+'S'+'F' (Synth Freq)
#define CREATOR_MFG_CCA_THRESHOLD       0xC343 // msb+'C'+'C' (Clear Channel)
//--- Lock Bits Data ---
#define CREATOR_MFG_BOOTLOAD_AES_KEY    0xC24B // msb+'B'+'K' (Bootloader Key)
#define CREATOR_MFG_CBKE_DATA           0xC342 // msb+'C'+'B' (CBke)
#define CREATOR_MFG_INSTALLATION_CODE   0xC943 // msb+'I'+'C' (Installation Code)
#define CREATOR_MFG_SECURITY_CONFIG     0xD343 // msb+'S'+'C' (Security Config)
#define CREATOR_MFG_SECURE_BOOTLOADER_KEY  0xD342 // msb+'S'+'B' (Secure Bootloader)
#define CREATOR_MFG_CBKE_283K1_DATA     0xC345 // msb+'C'+'B' (CBke) 283k1 ECC (a.k.a. SE 1.2)

// Defines indicating the verions number these definitions work with.
#define CURRENT_MFG_CUSTOM_VERSION 0xFFFFFFFE


#ifdef DEFINETYPES
typedef uint8_t tokTypeMfgEmberEui64[8];
//--- User Data ---
typedef uint16_t tokTypeMfgCustomVersion;
typedef uint8_t tokTypeMfgCustomEui64[8];
typedef uint8_t tokTypeMfgString[16];
typedef uint8_t tokTypeMfgBoardName[16];
typedef uint16_t tokTypeMfgManufId;
typedef uint16_t tokTypeMfgPhyConfig;
typedef uint8_t tokTypeMfgEui64[8];
typedef uint8_t tokTypeMfgEzspStorage[8];
typedef uint16_t tokTypeMfgAshConfig;
typedef uint16_t tokTypeMfgSynthFreqOffset;
typedef uint16_t tokTypeMfgCcaThreshold;

//--- Lock Bits Data ---
typedef uint8_t tokTypeMfgBootloadAesKey[16];
// Smart Energy 1.0 (ECC 163k1 based crypto - 80-bit symmetric equivalent)
typedef struct {
  uint8_t certificate[48];
  uint8_t caPublicKey[22];
  uint8_t privateKey[21];
  // The bottom flag bit is 1 for uninitialized, 0 for initialized.
  // The other flag bits should be set to 0 at initialization.
  uint8_t flags;
} tokTypeMfgCbkeData;
typedef struct {
  // The bottom flag bit is 1 for uninitialized, 0 for initialized.
  // Bits 1 and 2 give the size of the value string:
  // 0 = 6 bytes, 1 = 8 bytes, 2 = 12 bytes, 3 = 16 bytes.
  // The other flag bits should be set to 0 at initialization.
  // Special flags support.  Due to a bug in the way some customers
  // had programmed the flags field, we will also examine the upper
  // bits 9 and 10 for the size field.  Those bits are also reserved.
  uint16_t flags;
  uint8_t value[16];
  uint16_t crc;
} tokTypeMfgInstallationCode;
typedef uint16_t tokTypeMfgSecurityConfig;
typedef struct {
  uint8_t data[16];  // AES-128 key
} tokTypeMfgSecureBootloaderKey;
// Smart Energy 1.2 (ECC 283k1 based crypto - 128-bit symmetric equivalent)
typedef struct {
  uint8_t certificate[74];
  uint8_t caPublicKey[37];
  uint8_t privateKey[36];
  // The bottom flag bit is 1 for uninitialized, 0 for initialized.
  // The other flag bits should be set to 0 at initialization.
  uint8_t flags;
} tokTypeMfgCbke283k1Data;

#endif //DEFINETYPES


#ifdef DEFINETOKENS
//The Manufacturing tokens need to be stored at well-defined locations.
//None of these addresses should ever change without extremely great care.
//Any _LOCATION <256 will dispatch to SimEE by halInternalGetTokenData.

//NOTE: On the EFM32 platform the EMBER_EUI64_TOKEN is taken from the
//      DEVINFO info UNIQUEL and UNIQUEH register.
//      That means this MFG_EMBER_EUI_64_LOCATION is not used to offset
//      to any address, but is used as a distinct value
//      so the mfg-token.c code can check if the token is equal to it and
//      know to use the UNIQUE value registers.
#define MFG_EMBER_EUI_64_LOCATION        0x81F0  //   8 bytes

//--- User Data --- 
//User Data is mapped to 2kB at USERDATA_BASE (0x0FE00000-0x0FE007FF).
//Any _LOCATION <256 will dispatch to SimEE by halInternalGetTokenData.
//Overloading the location with 0x1000 will keep the token out of SimEE and
//indicate it is a token living in User Data space.
//0x1000 is stripped off by halInternalGetMfgTokenData() to get the correct
//UserData offset.
#define USERDATA_TOKENS                  0x1000
#define MFG_CUSTOM_VERSION_LOCATION      (USERDATA_TOKENS | 0x000)  //   2 bytes
#define MFG_CUSTOM_EUI_64_LOCATION       (USERDATA_TOKENS | 0x002)  //   8 bytes
#define MFG_STRING_LOCATION              (USERDATA_TOKENS | 0x01A)  //  16 bytes
#define MFG_BOARD_NAME_LOCATION          (USERDATA_TOKENS | 0x02A)  //  16 bytes
#define MFG_MANUF_ID_LOCATION            (USERDATA_TOKENS | 0x03A)  //   2 bytes
#define MFG_PHY_CONFIG_LOCATION          (USERDATA_TOKENS | 0x03C)  //   2 bytes
#define MFG_ASH_CONFIG_LOCATION          (USERDATA_TOKENS | 0x03E)  //  40 bytes
#define MFG_SYNTH_FREQ_OFFSET_LOCATION   (USERDATA_TOKENS | 0x0F0)  //   2 bytes
#define MFG_CCA_THRESHOLD_LOCATION       (USERDATA_TOKENS | 0x0F6)  //   2 bytes

//--- Lock Bits Data ---
//Lock Bits Data is mapped to 1636 bytes at LOCKBITS_BASE+0x200.
//The LOCKBITS_BASE page is physically mapped to 0x0FE04000-0x0FE047FF,
//but the lock words are fixed functionality in the first 512 bytes of the
//page so all data must exist above 0x0FE04200.
//Overloading the location with 0x2000 will keep the token out of SimEE and
//indicate it is a token living in Lock Bits data space.
//0x2000 is stripped off by halInternalGetMfgTokenData() to get the correct
//LockBits offset.
#define LOCKBITSDATA_TOKENS                0x2200
//Intentionally skipping the first four bytes.  The location might be
//best for a version or other type of information.
#define MFG_BOOTLOAD_AES_KEY_LOCATION      (LOCKBITSDATA_TOKENS | 0x04)  //  16 bytes
#define MFG_CBKE_DATA_LOCATION             (LOCKBITSDATA_TOKENS | 0x14)  //  92 bytes
#define MFG_INSTALLATION_CODE_LOCATION     (LOCKBITSDATA_TOKENS | 0x70)  //  20 bytes
#define MFG_SECURITY_CONFIG_LOCATION       (LOCKBITSDATA_TOKENS | 0x84)  //   2 bytes
#define MFG_SECURE_BOOTLOADER_KEY_LOCATION (LOCKBITSDATA_TOKENS | 0x86)  //  16 bytes
#define MFG_CBKE_283K1_DATA_LOCATION       (LOCKBITSDATA_TOKENS | 0x96)  // 148 bytes
#define MFG_EZSP_STORAGE_LOCATION          (LOCKBITSDATA_TOKENS | 0x12A)  //   8 bytes
//--- Virtual MFG Tokens ---
#define MFG_EUI_64_LOCATION              0x8000  // Special Trigger - see token.c

// Define the size of indexed token array
#define MFG_ASH_CONFIG_ARRAY_SIZE       20

TOKEN_NEXT_ADDRESS(MFG_EMBER_EUI_64_ADDR,MFG_EMBER_EUI_64_LOCATION)
TOKEN_MFG(MFG_EMBER_EUI_64, CREATOR_MFG_EMBER_EUI_64,
          0, 0, tokTypeMfgEmberEui64, 1,
          {3,0,0,0,0,0,0,3})
//--- User Data ---
TOKEN_NEXT_ADDRESS(MFG_CUSTOM_VERSION_ADDR,MFG_CUSTOM_VERSION_LOCATION)
TOKEN_MFG(MFG_CUSTOM_VERSION, CREATOR_MFG_CUSTOM_VERSION,
          0, 0, tokTypeMfgCustomVersion, 1,
          CURRENT_MFG_CUSTOM_VERSION)

TOKEN_NEXT_ADDRESS(MFG_CUSTOM_EUI_64_ADDR,MFG_CUSTOM_EUI_64_LOCATION)
TOKEN_MFG(MFG_CUSTOM_EUI_64, CREATOR_MFG_CUSTOM_EUI_64,
          0, 0, tokTypeMfgCustomEui64, 1,
          {0,3,3,3,3,3,3,0})

TOKEN_NEXT_ADDRESS(MFG_STRING_ADDR,MFG_STRING_LOCATION)
TOKEN_MFG(MFG_STRING, CREATOR_MFG_STRING,
          0, 0, tokTypeMfgString, 1,
          {0,})
          
TOKEN_NEXT_ADDRESS(MFG_BOARD_NAME_ADDR,MFG_BOARD_NAME_LOCATION)
TOKEN_MFG(MFG_BOARD_NAME, CREATOR_MFG_BOARD_NAME,
          0, 0, tokTypeMfgBoardName, 1,
          {0,})
          
TOKEN_NEXT_ADDRESS(MFG_MANUF_ID_ADDR,MFG_MANUF_ID_LOCATION)
TOKEN_MFG(MFG_MANUF_ID, CREATOR_MFG_MANUF_ID,
          0, 0, tokTypeMfgManufId, 1,
          {0x00,0x00,}) // default to 0 for ember
          
TOKEN_NEXT_ADDRESS(MFG_PHY_CONFIG_ADDR,MFG_PHY_CONFIG_LOCATION)
TOKEN_MFG(MFG_PHY_CONFIG, CREATOR_MFG_PHY_CONFIG,
          0, 0, tokTypeMfgPhyConfig, 1,
          {0x00,0x00,}) // default to non-boost mode, internal pa.

TOKEN_NEXT_ADDRESS(MFG_EZSP_STORAGE_ADDR,MFG_EZSP_STORAGE_LOCATION)
TOKEN_MFG(MFG_EZSP_STORAGE, CREATOR_MFG_EZSP_STORAGE,
          0, 0, tokTypeMfgEzspStorage, 1,
          { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF })

TOKEN_NEXT_ADDRESS(MFG_ASH_CONFIG_ADDR,MFG_ASH_CONFIG_LOCATION)
TOKEN_MFG(MFG_ASH_CONFIG, CREATOR_MFG_ASH_CONFIG,
          0, 1, tokTypeMfgAshConfig, MFG_ASH_CONFIG_ARRAY_SIZE,
          { 0xFFFF, })

TOKEN_NEXT_ADDRESS(MFG_BOOTLOAD_AES_KEY_ADDR,MFG_BOOTLOAD_AES_KEY_LOCATION)
TOKEN_MFG(MFG_BOOTLOAD_AES_KEY, CREATOR_MFG_BOOTLOAD_AES_KEY,
          0, 0, tokTypeMfgBootloadAesKey, 1,
          {0xFF,}) // default key is all f's
          
TOKEN_NEXT_ADDRESS(MFG_CBKE_DATA_ADDR,MFG_CBKE_DATA_LOCATION)
TOKEN_MFG(MFG_CBKE_DATA, CREATOR_MFG_CBKE_DATA,
          0, 0, tokTypeMfgCbkeData, 1,
          {0xFF,})

TOKEN_NEXT_ADDRESS(MFG_INSTALLATION_CODE_ADDR,MFG_INSTALLATION_CODE_LOCATION)
TOKEN_MFG(MFG_INSTALLATION_CODE, CREATOR_MFG_INSTALLATION_CODE,
          0, 0, tokTypeMfgInstallationCode, 1,
          {0xFF,})

TOKEN_NEXT_ADDRESS(MFG_SYNTH_FREQ_OFFSET_ADDR,MFG_SYNTH_FREQ_OFFSET_LOCATION)
TOKEN_MFG(MFG_SYNTH_FREQ_OFFSET, CREATOR_MFG_SYNTH_FREQ_OFFSET,
          0, 0, tokTypeMfgSynthFreqOffset, 1,
          {0xFF,0xFF,})

TOKEN_NEXT_ADDRESS(MFG_SECURITY_CONFIG_ADDR, MFG_SECURITY_CONFIG_LOCATION)
TOKEN_MFG(MFG_SECURITY_CONFIG, CREATOR_MFG_SECURITY_CONFIG,
          0, 0, tokTypeMfgSecurityConfig, 1,
          { 0xFF, 0xFF })

TOKEN_NEXT_ADDRESS(MFG_CCA_THRESHOLD_ADDR,MFG_CCA_THRESHOLD_LOCATION)
TOKEN_MFG(MFG_CCA_THRESHOLD, CREATOR_MFG_CCA_THRESHOLD,
          0, 0, tokTypeMfgCcaThreshold, 1,
          {0xFF, 0xFF,})

TOKEN_NEXT_ADDRESS(MFG_SECURE_BOOTLOADER_KEY_ADDR, MFG_SECURE_BOOTLOADER_KEY_LOCATION)
TOKEN_MFG(MFG_SECURE_BOOTLOADER_KEY, CREATOR_MFG_SECURE_BOOTLOADER_KEY,
          0, 0, tokTypeMfgSecureBootloaderKey, 1,
          {0xFF,}) // default key is all f's

TOKEN_NEXT_ADDRESS(MFG_CBKE_283K1_DATA_ADDR,MFG_CBKE_283K1_DATA_LOCATION)
TOKEN_MFG(MFG_CBKE_283K1_DATA, CREATOR_MFG_CBKE_283K1_DATA,
          0, 0, tokTypeMfgCbke283k1Data, 1,
          {0xFF,})
          
TOKEN_NEXT_ADDRESS(MFG_EUI_64_ADDR,MFG_EUI_64_LOCATION)
TOKEN_MFG(MFG_EUI_64, CREATOR_MFG_EUI_64,
          0, 0, tokTypeMfgEui64, 1,
          {3,3,3,3,0,0,0,0})

#endif //DEFINETOKENS


#ifdef APPLICATION_MFG_TOKEN_HEADER
  #include APPLICATION_MFG_TOKEN_HEADER
#endif

#undef TOKEN_NEXT_ADDRESS
