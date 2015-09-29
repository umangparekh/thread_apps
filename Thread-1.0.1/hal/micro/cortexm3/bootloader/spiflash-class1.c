/*
 * File: spiflash-class1.c
 * Description: SPIFlash driver that supports a variety of SPI flash parts
 *   Including: Winbond W25X20BV (2Mbit), W25Q80BV (8Mbit)
 *              Macronix MX25L2006E (2Mbit), MX25L8006E (8Mbit)
 *                       MX25L1606E (16Mbit), MX25U1635E (16Mbit 2Volt)
 *              Atmel/Adesto AT25DF041A (4Mbit), AT25DF081A (8Mbit)
 *              Numonyx/Micron M25P20 (2Mbit), M25P40 (4Mbit),
 *                             M25P80 (8Mbit), M25P16 (16Mbit)
 *              ISSI IS25LQ025B (256Kbit), IS25LQ512B (512Kbit),
 *                   IS25LQ010B (1Mbit), IS25LQ020B (2Mbit), IS25LQ040B (4Mbit)
 *   It could be easily extended to support other parts that talk the
 *    same protocol as these.
 *
 *   This driver does not support any write protection functionality
 *
 * Copyright 2013 Silicon Laboratories, Inc.                                *80*
 *
 */

/*
 * When SPI_FLASH_SC1 is defined, serial controller one will be used to
 * communicate with the external flash.
 * - This must be enabled in the case of app bootloading over USB, since USB
 *   uses SC2. Also ensure that the board jumpers are in the right place and
 *   the debugger is not connected to any SC1 pins.
 * - This must be enabled in the case of PRO2+-connected devices since SC2
 *   is used to interact with the PRO2+ radio.
 */

#include PLATFORM_HEADER
#include "hal/micro/bootloader-eeprom.h"
#include "bootloader-common.h"
#include "bootloader-serial.h"
#include "hal/micro/micro.h"
#include BOARD_HEADER
#include "hal/micro/cortexm3/memmap.h"

// abstract away which pins and serial controller are being used
#include "external-flash-gpio-select.h"

#define THIS_DRIVER_VERSION (0x0109)

#if BAT_VERSION != THIS_DRIVER_VERSION
  #error External Flash Driver must be updated to support new API requirements
#endif

/* Debug
#define LEDON()  GPIO_PACLR = BIT(6)
#define LEDOFF() GPIO_PASET = BIT(6)
*/

//
// ~~~~~~~~~~~~~~~~~~~~~~~~~~ Generic SPI Routines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//

// WARNING! hardware FIFO is only 4 bytes, so don't push more than 4 before popping!
static void halSpiPush8(uint8_t txData)
{
  while( (EXTERNAL_FLASH_SCx_SPISTAT&SC_SPITXFREE) != SC_SPITXFREE ) {
    // wait for space available
  }
  EXTERNAL_FLASH_SCx_DATA = txData;
}

static uint8_t halSpiPop8(void)
{
  // WARNING!  spiPop8 must be matched 1:1 with spiPush8 calls made first
  //  or else this could spin forever!!
  while( (EXTERNAL_FLASH_SCx_SPISTAT&SC_SPIRXVAL) != SC_SPIRXVAL ) {
    // wait for byte to be avail
  }
  return EXTERNAL_FLASH_SCx_DATA;
}

static void halSpiPushN8(uint8_t n) {
  while(n--) {
    halSpiPush8(0xFF);
  }
}

static void halSpiPopN8(uint8_t n) {
  while(n--) {
    halSpiPop8();
  }
}

/* These spi funcs are not currently used
static void halSpiPush16MSB(uint16_t txData)
{
  halSpiPush8((txData >> 8) & 0xFF);
  halSpiPush8(txData & 0xFF);
}
*/

static void halSpiPush24MSB(uint32_t txData)
{
  halSpiPush8((txData >> 16) & 0xFF);
  halSpiPush8((txData >> 8) & 0xFF);
  halSpiPush8(txData & 0xFF);
}

/* These spi funcs are not currently used
static void halSpiPush32MSB(uint32_t txData)
{
  halSpiPush16MSB((txData >> 16) & 0xFFFF);
  halSpiPush16MSB(txData & 0xFFFF);
}
*/

static uint16_t halSpiPop16MSB(void)
{
  uint16_t result;
  result = ((uint16_t)halSpiPop8()) << 8;
  result |= ((uint16_t)halSpiPop8());
  return result;
}

/* These spi funcs are not currently used
static uint32_t halSpiPop24MSB(void)
{
  uint32_t result;
  result = ((uint32_t)halSpiPop8()) << 16;
  result |= ((uint32_t)halSpiPop8()) << 8;
  result |= ((uint32_t)halSpiPop8());
  return result;
}

static uint32_t halSpiPop32MSB(void)
{
  uint32_t result;
  result = ((uint32_t)halSpiPop16MSB()) << 16;
  result |= ((uint32_t)halSpiPop16MSB());
  return result;
}
*/

static void halSpiWrite8(uint8_t txData)
{
  halSpiPush8(txData);
  halSpiPop8();
}

/* These spi funcs are not currently used
static void halSpiWrite16MSB(uint16_t txData)
{
  halSpiPush16MSB(txData);
  halSpiPopN8(2);
}

static void halSpiWrite24MSB(uint32_t txData)
{
  halSpiPush24MSB(txData);
  halSpiPopN8(3);
}

static void halSpiWrite32MSB(uint32_t txData)
{
  halSpiPush32MSB(txData);
  halSpiPopN8(4);
}
*/

static uint8_t halSpiRead8(void)
{
  halSpiPush8(0xFF);
  return halSpiPop8();
}

/* These spi funcs are not currently used
static uint16_t halSpiRead16MSB(void)
{
  halSpiPushN8(2);
  return halSpiPop16MSB();
}

static uint32_t halSpiRead24MSB(void)
{
  halSpiPushN8(3);
  return halSpiPop24MSB();
}

static uint32_t halSpiRead32MSB(void)
{
  halSpiPushN8(4);
  return halSpiPop32MSB();
}
*/

//
// ~~~~~~~~~~~~~~~~~~~~~~~~ Device Specific Interface ~~~~~~~~~~~~~~~~~~~~~~~~~~
//

// Note: This driver does not support block write protection features

#define DEVICE_SIZE_256K                    (32768L)
#define DEVICE_SIZE_512K                    (65536L)
#define DEVICE_SIZE_1M                      (131072L)
#define DEVICE_SIZE_2M                      (262144L)
#define DEVICE_SIZE_4M                      (524288L)
#define DEVICE_SIZE_8M                      (1048576L)
#define DEVICE_SIZE_16M                     (2097152L)

// Pages are the write buffer granularity
#define DEVICE_PAGE_SIZE                    (256)
#define DEVICE_PAGE_MASK                    (255)
// Sectors are the erase granularity
// *except* for Numonyx parts which only support BLOCK erase granularity
#define DEVICE_SECTOR_SIZE                  (4096)
#define DEVICE_SECTOR_MASK                  (4095)
// Blocks define a larger erase granularity
#define DEVICE_BLOCK_SIZE_64K               (65536L)
#define DEVICE_BLOCK_MASK_64K               (65535L)
#define DEVICE_BLOCK_SIZE_32K               (32768L)
#define DEVICE_BLOCK_MASK_32K               (32767L)
// The flash word size in bytes
#define DEVICE_WORD_SIZE                    (1)

// JEDEC Manufacturer IDs
#define MFG_ID_WINBOND                      (0xEF)
#define MFG_ID_MACRONIX                     (0xC2)
#define MFG_ID_ATMEL                        (0x1F)
#define MFG_ID_NUMONYX                      (0x20)
#define MFG_ID_ISSI                         (0x9D)

// JEDEC Device IDs
#define DEVICE_ID_WINBOND_2M                (0x3012)
#define DEVICE_ID_WINBOND_4M                (0x3013)
#define DEVICE_ID_WINBOND_8M                (0x4014)
#define DEVICE_ID_MACRONIX_2M               (0x2012)
#define DEVICE_ID_MACRONIX_8M               (0x2014)
#define DEVICE_ID_MACRONIX_16M              (0x2015)
#define DEVICE_ID_MACRONIX_16M_2V           (0x2535)
#define DEVICE_ID_ATMEL_4M                  (0x4401)
#define DEVICE_ID_ATMEL_8M                  (0x4501)
#define DEVICE_ID_NUMONYX_2M                (0x2012)
#define DEVICE_ID_NUMONYX_4M                (0x2013)
#define DEVICE_ID_NUMONYX_8M                (0x2014)
#define DEVICE_ID_NUMONYX_16M               (0x2015)
#define DEVICE_ID_ISSI_256K                 (0x4009)
#define DEVICE_ID_ISSI_512K                 (0x4010)
#define DEVICE_ID_ISSI_1M                   (0x4011)
#define DEVICE_ID_ISSI_2M                   (0x4012)
#define DEVICE_ID_ISSI_4M                   (0x4013)

// Protocol commands
#define CMD_WRITE_ENABLE                    (0x06)
#define CMD_WRITE_DISABLE                   (0x04)
#define CMD_READ_STATUS                     (0x05)
#define CMD_WRITE_STATUS                    (0x01)
#define CMD_READ_DATA                       (0x03)
#define CMD_PAGE_PROG                       (0x02)
#define CMD_ERASE_SECTOR                    (0x20)
#define CMD_ERASE_BLOCK                     (0xD8)
#define CMD_ERASE_CHIP                      (0xC7)
#define CMD_POWER_DOWN                      (0xB9)
#define CMD_POWER_UP                        (0xAB)
#define CMD_JEDEC_ID                        (0x9F)
#define CMD_UNIQUE_ID                       (0x4B)

// Bitmasks for status register fields
#define STATUS_BUSY_MASK                    (0x01)
#define STATUS_WEL_MASK                     (0x02)

// These timings represent the worst case out of all chips supported by this
//  driver.  Some chips may perform faster.
// (in general Winbond is faster than Macronix is faster than Numonyx)
#define TIMING_POWERON_MAX_US               (10000)
#define TIMING_SLEEP_MAX_US                 (10)
#define TIMING_WAKEUP_MAX_US                (30)
#define TIMING_PROG_MAX_US                  (5000)
#define TIMING_WRITE_STATUS_MAX_US          (40000)
// (MS units are 1024Hz based)
#define TIMING_ERASE_SECTOR_MAX_MS          (410)
#define TIMING_ERASE_BLOCK_MAX_MS           (3072)
#define TIMING_ERASE_WINBOND_2M_MAX_MS      (2048)
#define TIMING_ERASE_WINBOND_4M_MAX_MS      (4096)
#define TIMING_ERASE_WINBOND_8M_MAX_MS      (6144)
#define TIMING_ERASE_MACRONIX_2M_MAX_MS     (3892)
#define TIMING_ERASE_MACRONIX_8M_MAX_MS     (15360)
#define TIMING_ERASE_MACRONIX_16M_MAX_MS    (30720)
#define TIMING_ERASE_MACRONIX_16M_2V_MAX_MS (20480)
#define TIMING_ERASE_ATMEL_4M_MAX_MS        (7168)
#define TIMING_ERASE_ATMEL_8M_MAX_MS        (28672)
#define TIMING_ERASE_NUMONYX_2M_MAX_MS      (6144)
#define TIMING_ERASE_NUMONYX_4M_MAX_MS      (10240)
#define TIMING_ERASE_NUMONYX_8M_MAX_MS      (20480)
#define TIMING_ERASE_NUMONYX_16M_MAX_MS     (40960)
#define TIMING_ERASE_ISSI_256K_MAX_MS       (512)
#define TIMING_ERASE_ISSI_512K_MAX_MS       (1024)
#define TIMING_ERASE_ISSI_1M_MAX_MS         (1536)
#define TIMING_ERASE_ISSI_2M_MAX_MS         (2048)
#define TIMING_ERASE_ISSI_4M_MAX_MS         (3072)


static const HalEepromInformationType windbond2MInfo = {
  EEPROM_INFO_VERSION,
  EEPROM_CAPABILITIES_ERASE_SUPPORTED | EEPROM_CAPABILITIES_PAGE_ERASE_REQD,
  TIMING_ERASE_SECTOR_MAX_MS,
  TIMING_ERASE_WINBOND_2M_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_2M,
  "W25X20BV",
  DEVICE_WORD_SIZE // word size in bytes
};

static const HalEepromInformationType windbond8MInfo = {
  EEPROM_INFO_VERSION,
  EEPROM_CAPABILITIES_ERASE_SUPPORTED | EEPROM_CAPABILITIES_PAGE_ERASE_REQD,
  TIMING_ERASE_SECTOR_MAX_MS,
  TIMING_ERASE_WINBOND_8M_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_8M,
  "W25Q80BV",
  DEVICE_WORD_SIZE // word size in bytes
};

static const HalEepromInformationType macronix2MInfo = {
  EEPROM_INFO_VERSION,
  EEPROM_CAPABILITIES_ERASE_SUPPORTED | EEPROM_CAPABILITIES_PAGE_ERASE_REQD,
  TIMING_ERASE_SECTOR_MAX_MS,
  TIMING_ERASE_MACRONIX_2M_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_2M,
  "MX25L2006E",
  DEVICE_WORD_SIZE // word size in bytes
};

static const HalEepromInformationType macronix8MInfo = {
  EEPROM_INFO_VERSION,
  EEPROM_CAPABILITIES_ERASE_SUPPORTED | EEPROM_CAPABILITIES_PAGE_ERASE_REQD,
  TIMING_ERASE_SECTOR_MAX_MS,
  TIMING_ERASE_MACRONIX_8M_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_8M,
  "MX25L8006E",
  DEVICE_WORD_SIZE // word size in bytes
};

static const HalEepromInformationType macronix16MInfo = {
  EEPROM_INFO_VERSION,
  EEPROM_CAPABILITIES_ERASE_SUPPORTED | EEPROM_CAPABILITIES_PAGE_ERASE_REQD,
  TIMING_ERASE_SECTOR_MAX_MS,
  TIMING_ERASE_MACRONIX_16M_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_16M,
  "MX25L1606E",
  DEVICE_WORD_SIZE // word size in bytes
};

static const HalEepromInformationType macronix16M2VInfo = {
  EEPROM_INFO_VERSION,
  EEPROM_CAPABILITIES_ERASE_SUPPORTED | EEPROM_CAPABILITIES_PAGE_ERASE_REQD,
  TIMING_ERASE_SECTOR_MAX_MS,
  TIMING_ERASE_MACRONIX_16M_2V_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_16M,
  "MX25U1635E",
  DEVICE_WORD_SIZE // word size in bytes
};

static const HalEepromInformationType atmel4MInfo = {
  EEPROM_INFO_VERSION,
  EEPROM_CAPABILITIES_ERASE_SUPPORTED | EEPROM_CAPABILITIES_PAGE_ERASE_REQD,
  TIMING_ERASE_SECTOR_MAX_MS,
  TIMING_ERASE_ATMEL_4M_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_4M,
  "AT25DF041A",
  DEVICE_WORD_SIZE // word size in bytes
};

static const HalEepromInformationType atmel8MInfo = {
  EEPROM_INFO_VERSION,
  EEPROM_CAPABILITIES_ERASE_SUPPORTED | EEPROM_CAPABILITIES_PAGE_ERASE_REQD,
  TIMING_ERASE_SECTOR_MAX_MS,
  TIMING_ERASE_ATMEL_8M_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_8M,
  "AT25DF081A",
  DEVICE_WORD_SIZE // word size in bytes
};

static const HalEepromInformationType numonyx2MInfo = {
  EEPROM_INFO_VERSION,
  EEPROM_CAPABILITIES_ERASE_SUPPORTED | EEPROM_CAPABILITIES_PAGE_ERASE_REQD,
  TIMING_ERASE_BLOCK_MAX_MS,
  TIMING_ERASE_NUMONYX_2M_MAX_MS,
  DEVICE_BLOCK_SIZE_64K, // Numonyx does not support smaller sector erase
  DEVICE_SIZE_2M,
  "M25P20",
  DEVICE_WORD_SIZE // word size in bytes
};

static const HalEepromInformationType numonyx4MInfo = {
  EEPROM_INFO_VERSION,
  EEPROM_CAPABILITIES_ERASE_SUPPORTED | EEPROM_CAPABILITIES_PAGE_ERASE_REQD,
  TIMING_ERASE_BLOCK_MAX_MS,
  TIMING_ERASE_NUMONYX_4M_MAX_MS,
  DEVICE_BLOCK_SIZE_64K, // Numonyx does not support smaller sector erase
  DEVICE_SIZE_4M,
  "M25P40",
  DEVICE_WORD_SIZE // word size in bytes
};

static const HalEepromInformationType numonyx8MInfo = {
  EEPROM_INFO_VERSION,
  EEPROM_CAPABILITIES_ERASE_SUPPORTED | EEPROM_CAPABILITIES_PAGE_ERASE_REQD,
  TIMING_ERASE_BLOCK_MAX_MS,
  TIMING_ERASE_NUMONYX_8M_MAX_MS,
  DEVICE_BLOCK_SIZE_64K, // Numonyx does not support smaller sector erase
  DEVICE_SIZE_8M,
  "M25P80",
  DEVICE_WORD_SIZE // word size in bytes
};

static const HalEepromInformationType numonyx16MInfo = {
  EEPROM_INFO_VERSION,
  EEPROM_CAPABILITIES_ERASE_SUPPORTED | EEPROM_CAPABILITIES_PAGE_ERASE_REQD,
  TIMING_ERASE_BLOCK_MAX_MS,
  TIMING_ERASE_NUMONYX_16M_MAX_MS,
  DEVICE_BLOCK_SIZE_64K, // Numonyx does not support smaller sector erase
  DEVICE_SIZE_16M,
  "M25P16",
  DEVICE_WORD_SIZE // word size in bytes
};

static const HalEepromInformationType issi256KInfo = {
  EEPROM_INFO_VERSION,
  EEPROM_CAPABILITIES_ERASE_SUPPORTED | EEPROM_CAPABILITIES_PAGE_ERASE_REQD,
  TIMING_ERASE_SECTOR_MAX_MS,
  TIMING_ERASE_ISSI_256K_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_256K,
  "IS25LQ025B",
  DEVICE_WORD_SIZE // word size in bytes
};

static const HalEepromInformationType issi512KInfo = {
  EEPROM_INFO_VERSION,
  EEPROM_CAPABILITIES_ERASE_SUPPORTED | EEPROM_CAPABILITIES_PAGE_ERASE_REQD,
  TIMING_ERASE_SECTOR_MAX_MS,
  TIMING_ERASE_ISSI_512K_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_512K,
  "IS25LQ512B",
  DEVICE_WORD_SIZE // word size in bytes
};

static const HalEepromInformationType issi1MInfo = {
  EEPROM_INFO_VERSION,
  EEPROM_CAPABILITIES_ERASE_SUPPORTED | EEPROM_CAPABILITIES_PAGE_ERASE_REQD,
  TIMING_ERASE_SECTOR_MAX_MS,
  TIMING_ERASE_ISSI_1M_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_1M,
  "IS25LQ010B",
  DEVICE_WORD_SIZE // word size in bytes
};

static const HalEepromInformationType issi2MInfo = {
  EEPROM_INFO_VERSION,
  EEPROM_CAPABILITIES_ERASE_SUPPORTED | EEPROM_CAPABILITIES_PAGE_ERASE_REQD,
  TIMING_ERASE_SECTOR_MAX_MS,
  TIMING_ERASE_ISSI_2M_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_2M,
  "IS25LQ020B",
  DEVICE_WORD_SIZE // word size in bytes
};

static const HalEepromInformationType issi4MInfo = {
  EEPROM_INFO_VERSION,
  EEPROM_CAPABILITIES_ERASE_SUPPORTED | EEPROM_CAPABILITIES_PAGE_ERASE_REQD,
  TIMING_ERASE_SECTOR_MAX_MS,
  TIMING_ERASE_ISSI_4M_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_4M,
  "IS25LQ040B",
  DEVICE_WORD_SIZE // word size in bytes
};

typedef enum {
  UNKNOWN_DEVICE,
  WINBOND_2M_DEVICE,
  WINBOND_8M_DEVICE,
  MACRONIX_2M_DEVICE,
  MACRONIX_8M_DEVICE,
  MACRONIX_16M_DEVICE,
  MACRONIX_16M_2V_DEVICE,
  ATMEL_4M_DEVICE,
  ATMEL_8M_DEVICE,
  //N.B. If add more ATMEL_ devices, update halEepromInit() accordingly
  NUMONYX_2M_DEVICE,
  NUMONYX_4M_DEVICE,
  NUMONYX_8M_DEVICE,
  NUMONYX_16M_DEVICE,
  //N.B. If add more NUMONYX_ devices, update halEepromErase() accordingly
  ISSI_256K_DEVICE,
  ISSI_512K_DEVICE,
  ISSI_1M_DEVICE,
  ISSI_2M_DEVICE,
  ISSI_4M_DEVICE,
} DeviceType;

// Initialization constants.  For more detail on the resulting waveforms,
// see the EM35x datasheet.
#define SPI_ORD_MSB_FIRST (0<<SC_SPIORD_BIT) // Send the MSB first
#define SPI_ORD_LSB_FIRST (1<<SC_SPIORD_BIT) // Send the LSB first

#define SPI_PHA_FIRST_EDGE (0<<SC_SPIPHA_BIT)  // Sample on first edge
#define SPI_PHA_SECOND_EDGE (1<<SC_SPIPHA_BIT) // Sample on second edge

#define SPI_POL_RISING_LEAD  (0<<SC_SPIPOL_BIT) // Leading edge is rising
#define SPI_POL_FALLING_LEAD (1<<SC_SPIPOL_BIT) // Leading edge is falling

#if    !defined(EXTERNAL_FLASH_RATE_LINEAR)       \
    || !defined(EXTERNAL_FLASH_RATE_EXPONENTIAL)

  #if    defined(EXTERNAL_FLASH_RATE_LINEAR)      \
      || defined(EXTERNAL_FLASH_RATE_EXPONENTIAL)

    #error Partial Flash serial rate definition. Please define both \
           EXTERNAL_FLASH_RATE_LINEAR and EXTERNAL_FLASH_RATE_EXPONENTIAL when \
           specifying a custom rate.

  #endif

  // configure for fastest allowable rate
  // rate = 12 MHz / ((LIN + 1) * (2^EXP))
  #if defined(CORTEXM3_EM350) || defined(CORTEXM3_STM32W108)
    #define EXTERNAL_FLASH_RATE_LINEAR  (1)     // limited to 6MHz
  #else
    #define EXTERNAL_FLASH_RATE_LINEAR  (0)     // 12Mhz - FOR EM35x
  #endif

  #define EXTERNAL_FLASH_RATE_EXPONENTIAL  (0)

#endif

#if EXTERNAL_FLASH_RATE_LINEAR < 0 || EXTERNAL_FLASH_RATE_LINEAR > 15
  #error EXTERNAL_FLASH_RATE_LINEAR must be between 0 and 15 (inclusive)
#endif

#if EXTERNAL_FLASH_RATE_EXPONENTIAL < 0 || EXTERNAL_FLASH_RATE_EXPONENTIAL > 15
  #error EXTERNAL_FLASH_RATE_EXPONENTIAL must be between 0 and 15 (inclusive)
#endif

static void waitNotBusy(void)
{
  while(halEepromBusy()) {
    // Do nothing
  }
}


static DeviceType getDeviceType(void)
{
  uint8_t mfgId;
  uint16_t deviceId;

  // cannot check for busy in this API since it is used by
  //  init.  Callers must verify not busy individually.
  EXTERNAL_FLASH_CS_ACTIVE();
  halSpiPush8(CMD_JEDEC_ID);
  halSpiPushN8(3);
  halSpiPop8();
  mfgId = halSpiPop8();
  deviceId = halSpiPop16MSB();
  EXTERNAL_FLASH_CS_INACTIVE();

  switch(mfgId) {
    case MFG_ID_WINBOND:
      switch(deviceId) {
        case DEVICE_ID_WINBOND_2M:
          return WINBOND_2M_DEVICE;
        case DEVICE_ID_WINBOND_8M:
          return WINBOND_8M_DEVICE;
        default:
          return UNKNOWN_DEVICE;
      }
    case MFG_ID_MACRONIX:
      switch(deviceId) {
        case DEVICE_ID_MACRONIX_2M:
          return MACRONIX_2M_DEVICE;
        case DEVICE_ID_MACRONIX_8M:
          return MACRONIX_8M_DEVICE;
        case DEVICE_ID_MACRONIX_16M:
          return MACRONIX_16M_DEVICE;
        case DEVICE_ID_MACRONIX_16M_2V:
          return MACRONIX_16M_2V_DEVICE;
        default:
          return UNKNOWN_DEVICE;
      }
    case MFG_ID_ATMEL:
      switch(deviceId)
      {
        case DEVICE_ID_ATMEL_4M:
          return ATMEL_4M_DEVICE;
        case DEVICE_ID_ATMEL_8M:
          return ATMEL_8M_DEVICE;
        default:
          return UNKNOWN_DEVICE;
      }
    case MFG_ID_NUMONYX:
      switch(deviceId) {
        case DEVICE_ID_NUMONYX_2M:
          return NUMONYX_2M_DEVICE;
        case DEVICE_ID_NUMONYX_4M:
          return NUMONYX_4M_DEVICE;
        case DEVICE_ID_NUMONYX_8M:
          return NUMONYX_8M_DEVICE;
        case DEVICE_ID_NUMONYX_16M:
          return NUMONYX_16M_DEVICE;
        default:
          return UNKNOWN_DEVICE;
      }
    case MFG_ID_ISSI:
      switch(deviceId) {
        case DEVICE_ID_ISSI_256K:
          return ISSI_256K_DEVICE;
        case DEVICE_ID_ISSI_512K:
          return ISSI_512K_DEVICE;
        case DEVICE_ID_ISSI_1M:
          return ISSI_1M_DEVICE;
        case DEVICE_ID_ISSI_2M:
          return ISSI_2M_DEVICE;
        case DEVICE_ID_ISSI_4M:
          return ISSI_4M_DEVICE;
        default:
          return UNKNOWN_DEVICE;
      }
    default:
      return UNKNOWN_DEVICE;
  }
}


static void setWEL(void)
{
  EXTERNAL_FLASH_CS_ACTIVE();
  halSpiWrite8(CMD_WRITE_ENABLE);
  EXTERNAL_FLASH_CS_INACTIVE();
}


uint8_t halEepromInit(void)
{
  DeviceType deviceType;

  //-----SPI Master GPIO configuration
  halGpioSetConfig(EXTERNAL_FLASH_MOSI_PIN, GPIOCFG_OUT_ALT);
  halGpioSetConfig(EXTERNAL_FLASH_MISO_PIN, GPIOCFG_IN);
  halGpioSetConfig(EXTERNAL_FLASH_SCLK_PIN, GPIOCFG_OUT_ALT);
  halGpioSetConfig(EXTERNAL_FLASH_nCS_PIN , GPIOCFG_OUT);

  //-----SPI Master SCx configuration
  EXTERNAL_FLASH_SCx_RATELIN = EXTERNAL_FLASH_RATE_LINEAR;
  EXTERNAL_FLASH_SCx_RATEEXP = EXTERNAL_FLASH_RATE_EXPONENTIAL;
  EXTERNAL_FLASH_SCx_SPICFG  = 0;
  EXTERNAL_FLASH_SCx_SPICFG  = (1 << SC_SPIMST_BIT)|  // 4; master control bit
                          (SPI_ORD_MSB_FIRST | SPI_PHA_FIRST_EDGE | SPI_POL_RISING_LEAD);
  EXTERNAL_FLASH_SCx_MODE    = EXTERNAL_FLASH_SCx_MODE_SPI;

  // Set EEPROM_POWER_PIN high as part of EEPROM init
  CONFIGURE_EEPROM_POWER_PIN();
  SET_EEPROM_POWER_PIN();

  // Ensure the device is ready to access after applying power
  // We delay even if shutdown control isn't used to play it safe
  // since we don't know how quickly init may be called after boot
  halCommonDelayMicroseconds(TIMING_POWERON_MAX_US);

  // Release the chip from powerdown mode
  EXTERNAL_FLASH_CS_ACTIVE();
  halSpiWrite8(CMD_POWER_UP);
  EXTERNAL_FLASH_CS_INACTIVE();
  halCommonDelayMicroseconds(TIMING_WAKEUP_MAX_US);

  deviceType = getDeviceType();
  if(deviceType == UNKNOWN_DEVICE) {
    return EEPROM_ERR_INVALID_CHIP;
  }
  // For Atmel devices, need to unprotect them because default is protected
  if(deviceType >= ATMEL_4M_DEVICE && deviceType <= ATMEL_8M_DEVICE) {
    setWEL();
    EXTERNAL_FLASH_CS_ACTIVE();
    halSpiWrite8(CMD_WRITE_STATUS);
    halSpiWrite8(0); // No protect bits set
    EXTERNAL_FLASH_CS_INACTIVE();
  }
  return EEPROM_SUCCESS;
}


void halEepromShutdown(void)
{
  // wait for any outstanding operations to complete before pulling the plug
  waitNotBusy();

  // always enter low power mode, even if using shutdown control
  //  since sometimes leakage prevents shutdown control from
  //  completely turning off the part.
  EXTERNAL_FLASH_CS_ACTIVE();
  halSpiWrite8(CMD_POWER_DOWN);
  EXTERNAL_FLASH_CS_INACTIVE();

  // pull power if using shutdown control
  CLR_EEPROM_POWER_PIN();
}


const HalEepromInformationType *halEepromInfo(void)
{
  waitNotBusy();
  switch(getDeviceType()) {
    case WINBOND_2M_DEVICE:
      return &windbond2MInfo;
    case WINBOND_8M_DEVICE:
      return &windbond8MInfo;
    case MACRONIX_2M_DEVICE:
      return &macronix2MInfo;
    case MACRONIX_8M_DEVICE:
      return &macronix8MInfo;
    case MACRONIX_16M_DEVICE:
      return &macronix16MInfo;
    case MACRONIX_16M_2V_DEVICE:
      return &macronix16M2VInfo;
    case ATMEL_4M_DEVICE:
      return &atmel4MInfo;
    case ATMEL_8M_DEVICE:
      return &atmel8MInfo;
    case NUMONYX_2M_DEVICE:
      return &numonyx2MInfo;
    case NUMONYX_4M_DEVICE:
      return &numonyx4MInfo;
    case NUMONYX_8M_DEVICE:
      return &numonyx8MInfo;
    case NUMONYX_16M_DEVICE:
      return &numonyx16MInfo;
    case ISSI_256K_DEVICE:
      return &issi256KInfo;
    case ISSI_512K_DEVICE:
      return &issi512KInfo;
    case ISSI_1M_DEVICE:
      return &issi1MInfo;
    case ISSI_2M_DEVICE:
      return &issi2MInfo;
    case ISSI_4M_DEVICE:
      return &issi4MInfo;
    default:
      return NULL;
  }
}

uint32_t halEepromSize(void)
{
  return halEepromInfo()->partSize;
}

bool halEepromBusy(void)
{
  uint8_t status;

  EXTERNAL_FLASH_CS_ACTIVE();
  halSpiPush8(CMD_READ_STATUS);
  halSpiPush8(0xFF);
  halSpiPop8();
  status = halSpiPop8();
  EXTERNAL_FLASH_CS_INACTIVE();
  if( (status & STATUS_BUSY_MASK) == STATUS_BUSY_MASK )
    return true;
  else
    return false;
}


static uint32_t getDeviceSize(DeviceType *pDeviceType)
{
  DeviceType deviceType;
  waitNotBusy();
  if(pDeviceType == NULL) {
    deviceType = getDeviceType();
  } else {
    deviceType = *pDeviceType;
    if(deviceType == UNKNOWN_DEVICE) {
      deviceType = getDeviceType();
      *pDeviceType = deviceType;
    }
  }
  switch(deviceType) {
    case ISSI_256K_DEVICE:
      return DEVICE_SIZE_256K;
    case ISSI_512K_DEVICE:
      return DEVICE_SIZE_512K;
    case ISSI_1M_DEVICE:
      return DEVICE_SIZE_1M;
    case WINBOND_2M_DEVICE:
    case NUMONYX_2M_DEVICE:
    case MACRONIX_2M_DEVICE:
    case ISSI_2M_DEVICE:
      return DEVICE_SIZE_2M;
    case ATMEL_4M_DEVICE:
    case NUMONYX_4M_DEVICE:
    case ISSI_4M_DEVICE:
      return DEVICE_SIZE_4M;
    case WINBOND_8M_DEVICE:
    case ATMEL_8M_DEVICE:
    case MACRONIX_8M_DEVICE:
    case NUMONYX_8M_DEVICE:
      return DEVICE_SIZE_8M;
    case MACRONIX_16M_DEVICE:
    case MACRONIX_16M_2V_DEVICE:
    case NUMONYX_16M_DEVICE:
      return DEVICE_SIZE_16M;
    default:
      return 0;
  }
}

static bool verifyAddressRange(uint32_t address, uint16_t length,
                                  DeviceType *pDeviceType)
{
  // all parts support addresses less than DEVICE_SIZE_2M
  if( (address + length) <= DEVICE_SIZE_2M )
    return true;

  // if address is greater than DEVICE_SIZE_2M, need to query the chip
  if( (address + length) <= getDeviceSize(pDeviceType) )
    return true;

  // out of range
  return false;
}


uint8_t halEepromRead(uint32_t address, uint8_t *data, uint16_t totalLength)
{
  if( !verifyAddressRange(address, totalLength, NULL) )
    return EEPROM_ERR_ADDR;

  waitNotBusy();

  EXTERNAL_FLASH_CS_ACTIVE();
  halSpiPush8(CMD_READ_DATA);
  halSpiPush24MSB(address);
  halSpiPopN8(4);

  while(totalLength--) {
    *data++ = halSpiRead8();
  }
  EXTERNAL_FLASH_CS_INACTIVE();

  return EEPROM_SUCCESS;
}


static void writePage(uint32_t address, const uint8_t *data, uint16_t len)
{
  waitNotBusy();
  setWEL();

  EXTERNAL_FLASH_CS_ACTIVE();
  halSpiPush8(CMD_PAGE_PROG);
  halSpiPush24MSB(address);
  halSpiPopN8(4);

  while(len--) {
    halSpiWrite8(*data++);
  }
  EXTERNAL_FLASH_CS_INACTIVE();
}


static bool verifyErased(uint32_t address, uint16_t len)
{
  waitNotBusy();

  EXTERNAL_FLASH_CS_ACTIVE();
  halSpiPush8(CMD_READ_DATA);
  halSpiPush24MSB(address);
  halSpiPopN8(4);

  while(len--) {
    if(halSpiRead8() != 0xFF) {
      return false;
    }
  }
  EXTERNAL_FLASH_CS_INACTIVE();
  return true;
}


uint8_t halEepromWrite(uint32_t address, const uint8_t *data, uint16_t totalLength)
{
  uint32_t nextPageAddr;
  uint16_t len;

  if( !verifyAddressRange(address, totalLength, NULL) )
    return EEPROM_ERR_ADDR;

  if(!verifyErased(address, totalLength)) {
    return EEPROM_ERR_ERASE_REQUIRED;
  }

  if( address & DEVICE_PAGE_MASK) {
    // handle unaligned first block
    nextPageAddr = (address & (~DEVICE_PAGE_MASK)) + DEVICE_PAGE_SIZE;
    if((address + totalLength) < nextPageAddr){
      // fits all within first block
      len = totalLength;
    } else {
      len = (uint16_t) (nextPageAddr - address);
    }
  } else {
    len = (totalLength>DEVICE_PAGE_SIZE)? DEVICE_PAGE_SIZE : totalLength;
  }
  while(totalLength) {
    writePage(address, data, len);
    totalLength -= len;
    address += len;
    data += len;
    len = (totalLength>DEVICE_PAGE_SIZE)? DEVICE_PAGE_SIZE : totalLength;
  }

  return EEPROM_SUCCESS;
}


static void doEraseCmd(uint8_t command, uint32_t address)
{
  waitNotBusy();
  setWEL();
  EXTERNAL_FLASH_CS_ACTIVE();
  halSpiPush8(command);
  halSpiPush24MSB(address);
  halSpiPopN8(4);
  EXTERNAL_FLASH_CS_INACTIVE();
}


uint8_t halEepromErase(uint32_t address, uint32_t totalLength)
{
  DeviceType deviceType = UNKNOWN_DEVICE;
  uint32_t sectorMask = DEVICE_SECTOR_MASK;
  uint32_t deviceSize = getDeviceSize(&deviceType);
  uint32_t deviceBlockSize = DEVICE_BLOCK_SIZE_64K;
  uint32_t deviceBlockMask = DEVICE_BLOCK_MASK_64K;
  // Numonyx/Micron parts only support block erase, not sector
  if( (deviceType >= NUMONYX_2M_DEVICE)
    &&(deviceType <= NUMONYX_16M_DEVICE) ) {
    sectorMask = DEVICE_BLOCK_MASK_64K;
  }
  else if ( (deviceType >= ISSI_256K_DEVICE)
          &&(deviceType <= ISSI_512K_DEVICE) ) {
    deviceBlockSize = DEVICE_BLOCK_SIZE_32K;
    deviceBlockMask = DEVICE_BLOCK_MASK_32K;
  }
  // Length must be a multiple of the sector size
  if( totalLength & sectorMask )
    return EEPROM_ERR_PG_SZ;
  // Address must be sector aligned
  if( address & sectorMask )
    return EEPROM_ERR_PG_BOUNDARY;
  // Address and length must be in range
  if( !verifyAddressRange(address, totalLength, &deviceType) )
    return EEPROM_ERR_ADDR;

  // Test for full chip erase
  if( (address == 0) && (totalLength == deviceSize) ) {
    waitNotBusy();
    setWEL();
    EXTERNAL_FLASH_CS_ACTIVE();
    halSpiWrite8(CMD_ERASE_CHIP);
    EXTERNAL_FLASH_CS_INACTIVE();
    return EEPROM_SUCCESS;
  }

  // first handle leading partial blocks
  while(totalLength && (address & deviceBlockMask)) {
    doEraseCmd(CMD_ERASE_SECTOR, address);
    address += DEVICE_SECTOR_SIZE;
    totalLength -= DEVICE_SECTOR_SIZE;
  }
  // handle any full blocks
  while(totalLength >= deviceBlockSize) {
    doEraseCmd(CMD_ERASE_BLOCK, address);
    address += deviceBlockSize;
    totalLength -= deviceBlockSize;
  }
  // finally handle any trailing partial blocks
  while(totalLength) {
    doEraseCmd(CMD_ERASE_SECTOR, address);
    address += DEVICE_SECTOR_SIZE;
    totalLength -= DEVICE_SECTOR_SIZE;
  }
  return EEPROM_SUCCESS;
}
