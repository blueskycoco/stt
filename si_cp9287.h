//***************************************************************************
//!file     si_cp9287.h
//!brief    CP 9287 Starter Kit firmware main module.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2008-2009, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#ifndef __SI_9287_H__
#define __SI_9287_H__
#include <stdio.h>        
#include <si_hal.h>
#include <si_edid.h>

//------------------------------------------------------------------------------
//
//  Compile-time build options
//
//------------------------------------------------------------------------------

#define FPGA_BUILD              0           // 1 == FPGA, 0 == Silicon
#define PRODUCTION_TEST         0

#define ENABLE_SERVICE_MODE     1           // Set to 1 to enable NVRAM update through DDC protocol
#define SERVICE_MODE_PORT       1           // Port number (1-3) to be used for data transfer in service mode (cannot be 0)

//------------------------------------------------------------------------------
//
//  CP 9287 Starter Kit Manifest Constants
//
//------------------------------------------------------------------------------

#define DEM_POLLING_DELAY   100         // Main Loop Polling interval (ms)

#define LEDS_MUST_RESET     0x0F        // 1111 - User must reset MCU
#define LEDS_CHOOSE_PORT    0x0A        // 1010 - User must select RSW position 0-3
#define LEDS_OFF            0x00        // 0000 - All off
#define LEDS_SIMON_INUSE    0x05        // 0101-1010 - Simon is in use - these flash once a second

    // GPIO assigments

#define JP15_1                          P1^2        // rev x02 only

#define JP15_2                          P1^1        //
#define JP15_3                          P1^0        //
#define JP15_4                          P0^7        //
#define JP15_5                          P0^6        //

#define JP15_6                          P1^2        // rev x01 only
#define JP15_7                          P2^5        // rev x01 only
#define JP15_8                          P2^6        // rev x01 only
#define JP15_9                          P2^7        // rev x01 only

#define DEVICE_POWER_PIN                P0^5        //
#define HDMI_TX_HPD                     P2^2
#define DEV_INT                         P3^2        // 9287 Interrupt pin

#define GPIO_MHL_EN0                    P2^6        // rev x04 board and up
#define GPIO_MHL_EN1                    P2^7        // rev x04 board and up
#define GPIO_MHL_EN2                    P0^1        // rev x04 board and up
#define GPIO_MHL_EN3                    P3^3        // rev x04 board and up

#define PORTLED_0                       P3^4        //
#define PORTLED_1                       P3^5        //
#define PORTLED_2                       P3^6        //
#define PORTLED_3                       P3^7        //

#define IR_PCA0                         P1^3

#define HDMI_TX_HPD                     P2^2

typedef enum
{
    CT_9287     = 0,
    CT_9285,
    CT_9187
};

    /* CP Test enable bits (for g_data.testEnable)  */

#define CPT_PORTSTATUS      0x01

//-------------------------------------------------------------------------------
//  Starter Kit EDID Read/write extensions
//-------------------------------------------------------------------------------

enum
{
    EDID_TV             = (EDID_BUFFER + 1),
    EDID_VGA,
    EDID_FLASH,
    EDID_EEPROM,
    EDID_EEPROM_BOOT,
    EDID_FLASH_VGA,
    EDID0_EEPROM,
    EDID1_EEPROM,
    EDID2_EEPROM,
    EDID3_EEPROM,
    EDID4_EEPROM,
    EDID_EEPROM_VGA,
    EDID_FILE
};

//-------------------------------------------------------------------------------
//  Macros
//-------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  typedefs
//------------------------------------------------------------------------------

typedef struct
{
    uint8_t chipType;
    uint8_t chipRev;

    uint8_t portSelect;
    uint8_t standbyMode;
    uint8_t useFixedEQ;
    uint8_t cecEnabled;
    uint8_t loadTvEdid;
    uint8_t autoSwitch;
    uint8_t spareJumper0;
    uint8_t spareJumper1;
    uint8_t portLedsState;

        // EEPROM settings

    uint8_t irEnabled;
    uint8_t edidLoad;
    uint8_t dontUseNvram;
    uint8_t testEnable;

} SI_CP9287Config_t;

//------------------------------------------------------------------------------
//  Global Data
//------------------------------------------------------------------------------

extern SI_CP9287Config_t   g_data;

extern BOOL                 g_deviceInterrupt;
extern uint8_t              g_dbgPrinted;

extern uint8_t XDATA        g_tempData [EDID_TABLE_LEN];

extern char g_signonMsg [];
extern char g_buildStr [];

//-------------------------------------------------------------------------------
// Board-specific sbit assigments
//-------------------------------------------------------------------------------

//sbit gpioJp15_1     = JP15_1;           // Rev x02 and above boards only.

//sbit gpioJp15_2     = JP15_2;
//sbit gpioJp15_3     = JP15_3;
//sbit gpioJp15_4     = JP15_4;
//sbit gpioJp15_5     = JP15_5;

//sbit gpioJp15_6     = JP15_6;           // Rev x01 board only
//sbit gpioJp15_7     = JP15_7;           // Rev x01 board only
//sbit gpioJp15_8     = JP15_8;           // Rev x01 board only
//sbit gpioJp15_9     = JP15_9;           // Rev x01 board only

//sbit gpioDevPower   = DEVICE_POWER_PIN;

//sbit gpioMhlPort0   = GPIO_MHL_EN0;
//sbit gpioMhlPort1   = GPIO_MHL_EN1;
//sbit gpioMhlPort2   = GPIO_MHL_EN2;
//sbit gpioMhlPort3   = GPIO_MHL_EN3;

//sbit gpioPortLED0   = PORTLED_0;
//sbit gpioPortLED1   = PORTLED_1;
//sbit gpioPortLED2   = PORTLED_2;
//sbit gpioPortLED3   = PORTLED_3;

//-------------------------------------------------------------------------------
//  Function Prototypes
//-------------------------------------------------------------------------------

BOOL CpInitEEPROM( void );
void CpInvalidateEEPROM( void );
BOOL CpCheckExternalRequests( void );
void CpUpdateBoardSettings( void );
void CpBoardStartup( void );
void CpSetPortLEDs( uint8_t newState );
void CpBlinkTilReset( uint8_t leds );
void CpDisplayChipInfo( int index );
void CpPowerControl( uint8_t newPowerState );
uint8_t CpReadRotarySwitch( uint8_t iWantItNow );
void CpDisplayData ( uint8_t msgLevel, uint8_t *pData, uint16_t length );

BOOL CpEdidRead( uint8_t source, uint8_t *pDest );
BOOL CpEdidWrite( uint8_t target, uint8_t *pSource );


BOOL IrCheckCommand( void );

#if ( ENABLE_SERVICE_MODE == 1 )
void CpServiceModeCheck( void );
#endif


#endif  // __SI_9287_H__
